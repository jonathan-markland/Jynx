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

#pragma once

#include "ISerialiser.h"
#include "Lexer.h"
#include "IFileOpener.h"
#include <iostream>
#include <fstream>
#include <vector>

namespace Jynx
{

	class OutputFileSerialiser: public ISerialiser
	{
	public:
		OutputFileSerialiser( IFileOpener *fileOpener, const std::string endOfLineSequence );
		virtual void OpenTag( const char *tagName ) override;
		virtual void CloseTag( const char *tagName ) override;
		virtual void Field( const char *tagName, unsigned char &field ) override;
		virtual void Field( const char *tagName, unsigned short &field ) override;
		virtual void Field( const char *tagName, int32_t &field ) override;
		virtual void Field( const char *tagName, bool &field ) override;
		virtual void Binary( void *baseAddress, uintptr_t blockSize ) override;
		void Close();
	private:
		void EndLine();
		std::ofstream  _outStream;
		std::string    _endOfLineSequence;
	};



	class InputFileSerialiser: public ISerialiser
	{
	public:
		InputFileSerialiser( IFileOpener *fileOpener );
		virtual void OpenTag( const char *tagName ) override;
		virtual void CloseTag( const char *tagName ) override;
		virtual void Field( const char *tagName, unsigned char &field ) override;
		virtual void Field( const char *tagName, unsigned short &field ) override;
		virtual void Field( const char *tagName, int32_t &field ) override;
		virtual void Field( const char *tagName, bool &field ) override;
		virtual void Binary( void *baseAddress, uintptr_t blockSize ) override;
		void Close();
	private:
		std::ifstream  _inStream;
		std::vector<char>  _fileImage;
		Lexer _lexer;
	};



} // end namespace Jynx
