
Jynx
====

Emulator for the 1983 Camputers Lynx Microcomputer, written by Jonathan Markland.

Please refer to the readme.htm for installation instructions.

    - Scroll down for pictures!
    - My wordpress blog:  https://jynxemulator.wordpress.com/
    - Camputers Lynx (Wikipedia):  https://en.wikipedia.org/wiki/Camputers_Lynx

	
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

	
Pictures!
---------

![Brushes](/Images/lynxmainscreen.gif)

![Brushes](/Images/airraid2.gif)

![Brushes](/Images/airraid1.gif)

![Brushes](/Images/battlebrick.gif)

![Brushes](/Images/lynxinvaders1.gif)

![Brushes](/Images/lynxinvaders2.gif)

![Brushes](/Images/panik.jpg)

![Brushes](/Images/twinkle3.jpg)

![Brushes](/Images/dungeon2.jpg)

![Brushes](/Images/deflektor7.jpg)

![Brushes](/Images/penrose1.gif)

![Brushes](/Images/penrose2.gif)

![Brushes](/Images/planets4.gif)

![Brushes](/Images/planets1.gif)

![Brushes](/Images/planets5.gif)

![Brushes](/Images/disasm.gif)

![Brushes](/Images/jam1.gif)

![Brushes](/Images/jamming3.gif)

![Brushes](/Images/jynx-on-the-pi-2.jpg)

