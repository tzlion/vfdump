VFDump
======
A GBA multiboot program for dumping GBA carts with support for Vast Fame protection

Loosely based on [SendSave by chishm](https://www.chishm.com/SendSave/index.html)

Requirements
------------
YOU WILL NEED...

* **A Game Boy Advance (regular or SP)**
* **A PC with a parallel port**  
  You will need either an onboard parallel port or a PCI/PCIe parallel card. USB to parallel adapters will not work.
* **A PC-side client program**  
  Current options are:
  * **Xboo Communicator**, the original full-featured program, but no longer maintained. Can be used with onboard
    parallel ports only, under either Linux or 32-bit Windows. Doesn't work under 64-bit Windows.  
    Archived download links:
    [Windows](http://web.archive.org/web/20091218212855/http://www.devkitpro.org/xcomms_win32.tar.bz2) /
    [Linux](http://web.archive.org/web/20091218212855/http://www.devkitpro.org/xcomms_linux.tar.bz2)
  * **[Xboop](https://github.com/tzlion/xboop)**, a simplified command-line based program implementing enough
    functionality for VFDump to work. This works under any modern version of Windows or Linux and can support ports on
    expansion cards as well as onboard ports.
* **An Xboo cable**  
  Schematic available in the Xboo Communicator archive. If you're using Xboop you can also optionally use a GBlink
  cable which is differently wired.

Usage
-----
* Download the VFDump GBA ROM from the latest release
* Insert the cartridge you want to dump into your GBA
* Connect GBA to PC
* Turn on the GBA, then press Select+Start when the Nintendo logo appears - this should stop the game from booting
* Transfer the VFDump ROM to the GBA using Xboo Communicator or Xboop and follow the on-screen instructions.
  * Note: If you are using Xboop, ignore the instructions to press keyboard keys on the PC, this only works in Xboo
    Communicator. You will need to control VFDump using the buttons on the GBA only.

When the option to "read header/skip/start game" appears, typically you should choose "read header" to display the
game's title from the header and ensure the cartridge is being read properly. The "skip" option should be used when
dumping YJencrypted games, as reading the header may trip their protection. "Start game" just boots the game.

After this, the main menu will appear.

Supported actions:
* **Dump normal ROM** - dump inserted cartridge as a standard GBA ROM (will always dump max 32mb)
* **Dump VF ROM** - dump game with Vast Fame protection
* **Dump YJencrypted** - dump game with YJencrypted protection, see separate file **YJDumping.md**
* **Get value reordering** - determine Vast Fame SRAM address scrambling (for emulation, can differ between games)
* **Get address reordering** - determine Vast Fame SRAM value scrambling (for emulation, can differ between games)

Development
-----------
Needs devkitARM and libgba to build, currently being built against devkitARM r65-1

To build: `make build`  
to clean up everything it generates: `make clean`

You may need to change the value of DEVKITPRO_BASE_DIR in the makefile depending on where you have it installed
