VFDump
======
A GBA multiboot program for dumping GBA carts with support for Vast Fame protection

Loosely based on [SendSave by chishm](https://www.chishm.com/SendSave/index.html)

Requirements
------------
YOU WILL NEED...

* **A Game Boy Advance (regular or SP)**  
* **Xboo Communicator**  
  The PC-side program. Archived download links:
  [Windows](http://web.archive.org/web/20091218212855/http://www.devkitpro.org/xcomms_win32.tar.bz2) /
  [Linux](http://web.archive.org/web/20091218212855/http://www.devkitpro.org/xcomms_linux.tar.bz2)
* **A PC with an onboard parallel port**  
  As with GBlinkDX I'm not sure if PCI or USB adapters will work but probably not
* **Some OS that will allow direct access to said parallel port**  
  Linux in general should be OK if run as superuser.  
  32-bit versions of Windows may work but also may need the UserPort driver.  
  64-bit Windows probably nah  
  Unfortunately the source for Xboo Communicator doesn't seem to be available so I was unable to do anything to
  improve Windows compatibility
* **An Xboo cable**  
  Schematic available in the Xboo Communicator archive

Usage
-----
* Download the VFDump GBA ROM from the latest release
* Insert the cartridge you want to dump into your GBA
* Connect GBA to PC
* Turn on the GBA, then press Select+Start when the Nintendo logo appears - this should stop the game from booting
* Transfer the VFDump ROM to the GBA using Xboo Communicator and follow the on-screen instructions.

Supported actions:
* **Normal dump** - dump inserted cartridge as a standard GBA ROM (will always dump max 32mb)
* **VF dump** - dump game with Vast Fame protection
* **Get value reordering** - determine SRAM address scrambling (for emulation, can differ between games)
* **Get address reordering** - determine SRAM value scrambling (for emulation, can differ between games)

Development
-----------
Needs devkitARM and libgba to build, currently being built against devkitARM r65-1

To build: `make build`  
to clean up everything it generates: `make clean`

You may need to change the value of DEVKITPRO_BASE_DIR in the makefile depending on where you have it installed
