//
// Jynx - Jonathan's Lynx Emulator (Camputers Lynx 48K/96K models).
// Copyright (C) 2014  Jonathan Markland
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//		jynx_emulator {at} yahoo {dot} com
//

#include <stdexcept>
#include <assert.h>
#include "TapFileSplitter.h"
#include "SignalWriter.h"
#include "FileLoader.h"



namespace Jynx
{
    class TapFileLexerException: public std::runtime_error
    {
    public:
        TapFileLexerException( const char *message ) : std::runtime_error(message) {}
    };



	class TapFileLexer
	{
	public:

		TapFileLexer( const std::vector<uint8_t> &tapFileImage );
            // NOTE: fileImage must have 0 terminator added.
            // Throws TapFileLexerException if parse fails.

		bool End() const;
		std::string ExpectFileName();
		std::vector<uint8_t> ExpectFileBody();

	private:

		const uint8_t *_position;
		const uint8_t *_endPosition;

		void RaiseError() const;

		size_t SpaceRemaining() const;

	};



	TapFileLexer::TapFileLexer( const std::vector<uint8_t> &tapFileImage )
	{
		assert( tapFileImage.size() >= 1 );  // must be
		assert( tapFileImage.back() == 0 );  // must be a 0 terminator at the end (eases parsing)
		auto startPosition = &tapFileImage.front();
		_position    = startPosition;
		_endPosition = startPosition + (tapFileImage.size() - 1);  // subtract one to stop our 0 terminator being consumed by block readers.
	}



	bool TapFileLexer::End() const
	{
		if( _position > _endPosition ) RaiseError();
		return _position == _endPosition;
	}



	size_t TapFileLexer::SpaceRemaining() const
	{
		return _endPosition - _position;
	}



	std::string TapFileLexer::ExpectFileName()
	{
		// Some TAPs have a spurious A5 at the start:
		if( _position[0] == 0xA5 )
		{
			++_position;  // consume the 0xA5 only.  These sort of redundant A5s will be re-constituted by the replay, and do not need to be part of the data.
		}

		// Check for type 'A' files (no file name portion):
		if( _position[0] == 'A' )
		{
			return std::string(); // There is no name string with an 'A' format TAP (as used by Level-9 adventures)
		}

		if( _position[0] == '\"' )
		{
			++_position;
			std::string fileName;
			while(true)
			{
				auto byte = *_position;
				if( byte == 0 ) RaiseError();
				if( byte == '\"' ) break;
				fileName.push_back( (char) byte );
				++_position;
			}
			++_position; // skip second "
			return fileName;
		}

		RaiseError();
		return std::string(); // never executed
	}



	std::vector<uint8_t> TapFileLexer::ExpectFileBody()
	{
		auto positionOfFileStart = _position;

		// Read file type letter:
		if( SpaceRemaining() < 1 ) RaiseError();
		auto fileTypeLetter = _position[0];

		// Read header now we know the type:
		uint8_t  lengthLow  = 0;
		uint8_t  lengthHigh = 0;
		if( fileTypeLetter == 'A' ) // 'A' type files used by Level 9 games, possibly others?
		{
			if( SpaceRemaining() < 5 ) RaiseError();
			lengthLow  = _position[3];
			lengthHigh = _position[4];
			_position += 5;
		}
		else if( fileTypeLetter == 'B' || fileTypeLetter == 'M' )  // Lynx basic or machine code.
		{
			if( SpaceRemaining() < 3 ) RaiseError();
			lengthLow  = _position[1];
			lengthHigh = _position[2];
			_position += 3;
		}
		else RaiseError();

		// Determine the image length (as recorded in the file):
		auto payloadLength = (uint32_t) ((lengthHigh << 8) | lengthLow);

		// Increase the length to include other known data:
		if( fileTypeLetter == 'B' ) payloadLength += 3;   // Basic files have 3 extra bytes after the payload
		if( fileTypeLetter == 'M' ) payloadLength += 7;   // "Machine code" files have 7 extra bytes after the payload
		if( fileTypeLetter == 'A' ) payloadLength += 12;  // "A" files have 12 extra bytes after the payload

		// Check it fits the file:
		auto spaceRemaining = SpaceRemaining();
		if( payloadLength > spaceRemaining ) RaiseError();

		// Copy out the raw file data, starting at the file type byte:
		_position += payloadLength;
		auto resultVector = std::vector<uint8_t>( positionOfFileStart, _position );

		// Unfortunately, a minority of TAPs, particularly BASIC ones, have a spurious zero
		// at the end.  However, we can skip this, if found, because TAP files always begin
		// with 0x22 0x41 (or the also-spurious 0xA5), never 0x00.
		while( SpaceRemaining() > 0 )  // Must do this test, so we don't mistake our own NUL terminator for a spurious zero!
		{
			if( *_position != 0 ) break;
			++_position;  // move past spurious zero
		}

		return resultVector;
	}



	void TapFileLexer::RaiseError() const
	{
		throw TapFileLexerException( "Failed to parse TAP file." );
	}





}




namespace Jynx
{
	TapFileSplitter::TapFileSplitter()
	{
		// The constructor for an empty tape.
	}



	TapFileSplitter::TapFileSplitter( IFileOpener *tapFile )
	{
		LoadAndParseTapFile( tapFile );
	}



	size_t TapFileSplitter::GetNumberOfFiles() const
	{
		assert( _fileNames.size() == _contentImages.size() ); // should always be so.
		return _fileNames.size();
	}



	std::vector<uint16_t>  TapFileSplitter::GenerateWaveformForFile( size_t fileIndex, uint32_t bitsPerSecond )
	{
		std::vector<uint16_t>  resultWave;

		// Seed constants deduced from analysis of the signal tape files.  (At Lynx TAPE 0, 600bps).
		// "Bits per second" is from Camputers documentation, it may or may not have ever been accurate, I don't know.
		// We don't need to care - everything is calculated in Z80 cycles anyway, and I'm only interested in RATIOs.

		uint32_t zeroSeed = (0x80C * 600) / bitsPerSecond;
		uint32_t oneSeed  = (0xFB1 * 600) / bitsPerSecond;

		SignalWriter  signalWriter( resultWave,
			SignalLengthInfo( zeroSeed,  zeroSeed + 0x57,  zeroSeed + 0x11F ),    // Signal length information (in Z80 cycles) for a ZERO  (at "TAPE 0" speed).
			SignalLengthInfo( oneSeed,   oneSeed + 0x121,  oneSeed + 0x1F7) );    // Signal length information (in Z80 cycles) for a ONE   (at "TAPE 0" speed).

			//SignalLengthInfo( 0x80C,  0x863,  0x92B ),    // Signal length information (in Z80 cycles) for a ZERO  (at "TAPE 0" speed).
			//SignalLengthInfo( 0xFB1, 0x10D2, 0x11A8 ) );  // Signal length information (in Z80 cycles) for a ONE   (at "TAPE 0" speed).

		auto &thisFileName    = _fileNames[fileIndex];
		auto &thisFileContent = _contentImages[fileIndex];

		// Initial SYNC + A5 applies to all file types:
		signalWriter.WriteSyncAndA5();

		// Types 'B' and 'M' require the file name portion and a second SYNC + A5:
		if( ! thisFileContent.empty() )  // avoid library assert on address-take if vector empty.
		{
			auto fileTypeLetter = thisFileContent[0];
			if( fileTypeLetter == 'B' || fileTypeLetter == 'M' )
			{
				signalWriter.WriteByte( 0x22 );
				signalWriter.WriteBytes( (const uint8_t *) (thisFileName.c_str()), thisFileName.size() );
				signalWriter.WriteByte( 0x22 );
				signalWriter.WriteExtraHighCycles( 0x1F1 );
				signalWriter.WriteSyncAndA5();
			}
		}

		// All types emit the file payload:
		if( ! thisFileContent.empty() )  // avoid library assert on address-take if vector empty.
		{
			signalWriter.WriteBytes( &thisFileContent.front(), thisFileContent.size() );
		}

		return resultWave;
	}



	void TapFileSplitter::LoadAndParseTapFile( IFileOpener *tapFileOpener )
	{
		// Load file:

		std::vector<uint8_t>  fileImage;
		LoadFileIntoVector( tapFileOpener, fileImage ); // throws
		fileImage.push_back(0); // A kind of NUL terminator byte, because having a terminator makes lexical analysis code easy -- there is always at least 1 byte to read!

		// Containers for the load (we ditch these if fail):

		std::vector<std::string>  fileNames;
		std::vector< std::vector<uint8_t> >  contentImages;

		// Initialise parsing pointer:
		TapFileLexer  lexer( fileImage );

		// Files loop:
		while( ! lexer.End() )
		{
			auto fileName = lexer.ExpectFileName();
			auto fileBody = lexer.ExpectFileBody();
			fileNames.push_back( fileName );
			contentImages.push_back( fileBody );
		}

		// Success:  Transfer results to member variables:

		_fileNames.swap( fileNames );
		_contentImages.swap( contentImages );
	}



	std::string  TapFileSplitter::GetTapeDirectory( TapeDirectoryStyle::Enum styleRequired ) const
	{
		// Retrieves text giving tape content and file types (LOAD / MLOAD).

		std::string  directoryString;

		assert( _contentImages.size() == _fileNames.size() ); // should always be from the LoadAndParseTapFile() result.

		if( _fileNames.empty() )
		{
			directoryString += "REM No files on tape.\r";
		}
		else if( styleRequired == TapeDirectoryStyle::REMCommandListing )
		{
			for( size_t  i=0; i<_fileNames.size(); i++ )
			{
				directoryString += "REM "; // so is suitable for automated "typing" into the Lynx.
				AppendLynxLoadCommandForFile( i, directoryString );
				directoryString += "\"\r"; // Lynx compatible line ending.
			}
		}
		else if( styleRequired == TapeDirectoryStyle::LoadCommands )
		{
			directoryString += "TAPE 5\r";
			AppendLynxLoadCommandForFile( 0, directoryString );
			directoryString += "\"\r"; // Lynx compatible line ending.
		}
		else assert(false);

		return directoryString;
	}



	void TapFileSplitter::AppendLynxLoadCommandForFile( size_t fileIndex, std::string &directoryString ) const
	{
		assert( _contentImages[fileIndex].size() > 0 ); // should always be from the LoadAndParseTapFile() result.

		auto fileType = _contentImages[fileIndex][0];
		if( fileType == 'B' )
		{
			directoryString += "LOAD \"";
		}
		else if( fileType == 'M' )
		{
			directoryString += "MLOAD \"";
		}
		else
		{
			directoryString += "UNKNOWN FILE TYPE \"";  // should never happen.
		}

		directoryString += _fileNames[fileIndex];
	}



} // end namespace Jynx
