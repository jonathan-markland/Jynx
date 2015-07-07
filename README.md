
Jynx
====

Emulator for the 1983 Camputers Lynx Microcomputer, written by Jonathan Markland.

Please refer to the readme.htm for installation instructions.



For Windows
-----------
The source code can be built using Microsoft Visual Studio 2012.




For Linux/X86-32 or X86-64
--------------------------
The source code has been developed and built using g++ 4.8.2 on Linux Mint 17.1 Rebecca.
The Code::Blocks CBP file is provided for this configuration. 
Also tested on Elementary OS Freya.
Since these are based on Ubuntu, I expect this binary should work on Ubuntu (untested).



For Linux/ARM  (Raspberry PI 2)
-------------------------------
The source code has been built using g++ 4.9 on Raspbian "Wheezy" edition, using:

	make -f makefile_ARM

This requires:

	sudo apt-get install libgtk2.0-dev
	sudo apt-get install libasound2-dev

A hack is required to obtain g++ from the "Jessie" edition:

	sudo apt-get update
	sudo apt-get upgrade
	sudo vim /etc/apt/sources.list
	sudo apt-get update
	sudo apt-get install gcc-4.9 g++-4.9
	sudo vim /etc/apt/sources.list
	sudo apt-get update


