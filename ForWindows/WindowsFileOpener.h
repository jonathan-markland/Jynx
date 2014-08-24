//
// Camputers 48K/96K Lynx emulator.
// Software designed and developed by Jonathan Markland.
// (C) 2014 Jonathan Markland
//


#pragma once

#include <string>
#include "../Portable/IFileOpener.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     FILE PATH FORMAT ABSTRACTION ON WINDOWS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

class WindowsFileOpener: public Jynx::IFileOpener
{
public:

	// Encapsulate the nature and representation of a file path.
	// Assist opening or creating a file with the hidden path.

	// See IFileOpener

	WindowsFileOpener( const wchar_t *filePath );
	WindowsFileOpener( const std::wstring &filePath );
	virtual void OpenOutputStream( std::ofstream &stream, std::ios_base::openmode openModeRequired ) override;
	virtual void OpenInputStream(  std::ifstream &stream, std::ios_base::openmode openModeRequired ) override;

private:

	std::wstring  _filePath;

};
