
Jynx
====

Emulator for the 1983 Camputers Lynx Microcomputer, written by Jonathan Markland.

Please refer to the readme.htm for installation instructions.

    - Scroll down for pictures!

[My wordpress blog](https://jynxemulator.wordpress.com/)

[Camputers Lynx (Wikipedia)](https://en.wikipedia.org/wiki/Camputers_Lynx)


Notes to programmers
--------------------

- Tag v1.0.8 is the most recent *tested and released* Windows version.
- Tag v1.0.9 is the most recent *tested and released* Linux version.

v1.0.9 didn't build on Windows - I broke the build, sorry.  Branch 'master' 
fixes this, but NO v1.0.9 changes have been regression-tested on Windows!
When I get around to re-testing, I will apply a new version tag, but since
it takes about 3-4 hours to run the manual test plan, I think I will
leave this until I do further work on Jynx.

	
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

Lynx main screen:

![Lynx main screen](/Images/lynxmainscreen.gif)

Air Raid:

![Air raid](/Images/airraid2.gif)

![Air raid instructions](/Images/airraid1.gif)

Battle brick:

![Battle brick](/Images/battlebrick.gif)

Lynx Invaders

![Invaders](/Images/lynxinvaders1.gif)

![Invaders](/Images/lynxinvaders2.gif)

PANIK

![Panik](/Images/panik.jpg)

Twinkle

![Twinkle](/Images/twinkle3.gif)

Level 9's Dungeon Adventure

![Dungeon Adventure](/Images/dungeon2.gif)

Jonathan's 'Deflektor' demo that never got finished into a game:

![Jonathan's 'Deflektor' demo](/Images/deflektor7.jpg)

Penrose demo:

![Penrose demo](/Images/penrose1.gif)

![Penrose demo](/Images/penrose2.gif)

Cytera's Planets demo.  You can transfer Lynx BASIC text
between your favourite editor and the emulator:

![Cytera Planets Demo](/Images/planets4.gif)

![Cytera Planets Demo](/Images/planets1.gif)

![Cytera Planets Demo](/Images/planets5.gif)

I added a built-in disassembler:

![Built-in disassembler](/Images/disasm.gif)

Just me doing some basic jamming:

![Jonathan jamming](/Images/jam1.gif)

![Jonathan jamming](/Images/jamming3.jpg)

Raspberry PI version of the software:

![Raspberry PI image](/Images/jynx-on-the-pi-2.jpg)

