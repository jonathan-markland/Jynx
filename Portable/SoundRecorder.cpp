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

#include "SoundRecorder.h"
#include "ZeroInitialiseMemory.h"


namespace Jynx
{
	SoundRecorder::SoundRecorder()
	{
	}



	SoundRecorder::~SoundRecorder()
	{
		Close();
	}



	void SoundRecorder::StartNewFile( IFileOpener *fileOpener )
	{
		fileOpener->OpenOutputStream( _outStream, std::ios::binary | std::ios::out );

		// Output a reservation for the file header.  We shall fill it in on close:
		char  wavFileHeaderReservation[0x2C];
		ZeroInitialiseMemory( wavFileHeaderReservation );
		_outStream.write( wavFileHeaderReservation, 0x2C );
	}



	bool SoundRecorder::IsOpen() const
	{
		return _outStream.is_open();
	}



	void SoundRecorder::Binary( void *baseAddress, uintptr_t blockSize )
	{
		// TODO: We could do with having a limit on the size!
		_outStream.write( (const char *) baseAddress, blockSize );
	}



	void SoundRecorder::Close()
	{
		if( _outStream.is_open() )
		{
			// Seek to start and fill in the WAVE FILE HEADER.

			auto endPos = _outStream.tellp();
			auto bytesOfSoundData = (uint32_t) endPos;

			_outStream.seekp( 0, std::ios_base::beg );

			// 00000000  52 49 46 46  RIFF
			// 00000004  8C CD 15 00  number of bytes in sound image + 36
			// 00000008  57 41 56 45  WAVE
			// 0000000C  66 6D 74 20  fmt

			// 00000010  10 00 00 00  (size)
			// 00000014  01 00 01 00  Audio format | Number of channels
			// 00000018  44 AC 00 00  SampleRate (44100Hz)
			// 0000001C  88 58 01 00  ByteRate   (44100Hz * 2) ???

			// 00000020  02 00 10 00  BlockAlign | Bits Per Sample
			// 00000024  64 61 74 61 data
			// 00000028  68 CD 15 00 number of bytes in sound image

			uint8_t  wavFileHeader[0x2C];
			ZeroInitialiseMemory( wavFileHeader );

			wavFileHeader[ 0] = 'R';
			wavFileHeader[ 1] = 'I';
			wavFileHeader[ 2] = 'F';
			wavFileHeader[ 3] = 'F';

			* (uint32_t *) (wavFileHeader + 4) = bytesOfSoundData + 36;

			wavFileHeader[ 8] = 'W';
			wavFileHeader[ 9] = 'A';
			wavFileHeader[10] = 'V';
			wavFileHeader[11] = 'E';

			wavFileHeader[12] = 'f';
			wavFileHeader[13] = 'm';
			wavFileHeader[14] = 't';
			wavFileHeader[15] = ' ';

			wavFileHeader[0x10] = 0x10;
			wavFileHeader[0x14] = 0x01;
			wavFileHeader[0x16] = 0x01;

			* (uint32_t *) (wavFileHeader + 0x18) = 44100;
			* (uint32_t *) (wavFileHeader + 0x1C) = 44100 * 2;

			wavFileHeader[0x20] = 0x02;
			wavFileHeader[0x22] = 0x10;

			wavFileHeader[0x24] = 'd';
			wavFileHeader[0x25] = 'a';
			wavFileHeader[0x26] = 't';
			wavFileHeader[0x27] = 'a';

			* (uint32_t *) (wavFileHeader + 0x28) = bytesOfSoundData;

			_outStream.write( (const char *) wavFileHeader, 0x2C );

			_outStream.close();
		}
	}

} // end namespace Jynx
