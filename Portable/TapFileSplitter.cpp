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


#include "TapFileSplitter.h"
#include <assert.h>
#include "SignalWriter.h"
#include "FileLoader.h"


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

		signalWriter.WriteSyncAndA5();
		signalWriter.WriteByte( 0x22 );
		signalWriter.WriteBytes( (const uint8_t *) (thisFileName.c_str()), thisFileName.size() );
		signalWriter.WriteByte( 0x22 );
		signalWriter.WriteExtraHighCycles( 0x1F1 );
		signalWriter.WriteSyncAndA5();
		if( ! thisFileContent.empty() )  // avoid library assert on address-take if vector empty.
		{
			signalWriter.WriteBytes( &thisFileContent.front(), thisFileContent.size() );
		}

		return resultWave;
	}



	class TapFileLexer
	{
	public:
	
		TapFileLexer( const std::vector<uint8_t> &tapFileImage );  // fileImage must have 0 terminator added.

		bool End() const;
		std::string ExpectFileName();
		std::vector<uint8_t> ExpectFileBody();

	private:

		const uint8_t *_position;
		const uint8_t *_endPosition;

		void RaiseError();

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
		return _position[0] == 0;
	}



	size_t TapFileLexer::SpaceRemaining() const 
	{ 
		return _endPosition - _position; 
	}



	std::string TapFileLexer::ExpectFileName()
	{
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
		if( SpaceRemaining() >= 3 )  // The type, and the two bytes of the length.
		{
			auto byte = _position[0];
			if( byte == 'B' || byte == 'M' )  // Lynx basic or machine code.
			{
				auto lengthLow  = _position[1];
				auto lengthHigh = _position[2];
				_position += 3;
				auto length = (uint32_t) ((lengthHigh << 8) | lengthLow);
				if( byte == 'B' ) length += 3;  // Basic files have 3 extra bytes NOT accounted in the length.
				if( byte == 'M' ) length += 7;  // "Machine code" files have 7 extra bytes NOT accounted in the length.
				if( length <= SpaceRemaining() )
				{
					// Lift out the file data, including the 3 bytes we've just processed, and return that:
					auto resultVector = std::vector<uint8_t>( _position - 3, _position + length );
					_position += length;
					return resultVector;
				}
			}
		}
		RaiseError();
		return std::vector<uint8_t>(); // never executed
	}



	void TapFileLexer::RaiseError()
	{
		throw std::runtime_error( "Failed to parse TAP file." );
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

} // end namespace Jynx
