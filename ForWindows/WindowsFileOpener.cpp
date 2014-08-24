//
// Camputers 48K/96K Lynx emulator.
// Software designed and developed by Jonathan Markland.
// (C) 2014 Jonathan Markland
//


#include "WindowsFileOpener.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//     FILE PATH FORMAT ABSTRACTION ON WINDOWS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

WindowsFileOpener::WindowsFileOpener( const wchar_t *filePath )
	: _filePath( filePath )
{
}



WindowsFileOpener::WindowsFileOpener( const std::wstring &filePath )
	: _filePath( filePath )
{
}



void WindowsFileOpener::OpenOutputStream( std::ofstream &stream, std::ios_base::openmode openModeRequired )
{
	stream.exceptions( std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit );
	stream.open( _filePath.c_str(), openModeRequired );
}



void WindowsFileOpener::OpenInputStream(  std::ifstream &stream, std::ios_base::openmode openModeRequired )
{
	stream.exceptions( std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit );
	stream.open( _filePath.c_str(), openModeRequired );
}
