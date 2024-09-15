YJencrypted dumping
===================

YJencrypted cartridges use heavy read protection which cannot be easily circumvented by software alone. The YJencrypted
dumping mode in VFDump is designed to be used as part of a process as follows:

1. Boot the game on the GBA normally, allow it to reach the title screen. This means the cartridge is now unlocked for
   reading.
2. Reset the GBA _without_ losing power to the cartridge. On original model GBAs this can usually be achieved by rapidly
   power cycling. On a GBA SP it is more difficult and may require modification to the console.
3. At this point the console should begin to boot up with a corrupted Nintendo logo, press Select+Start and load VFDump.
    * If the Nintendo logo appears normally, it may mean the cartridge was not protected, OR it may mean power was lost
      to the cartridge in step 2. You can find out which by trying to dump it.
    * If the Nintendo logo appears BLANK, this means the protection tripped while reading the header.
      See section "circumventing header protection"
4. Select the "skip" option and then "Dump YJencrypted", it will begin to dump.
    * If your cartridge is one of the weaker protected ones, the dump will complete, no further action needed.
    * If your cartridge has the stronger protection, at some point the message "Protection trip" will appear.
      This means you've hit a "trap address" which will lock out the cartridge from further reads.
      TBCTBC on dealing with this

Skipping trap addresses
-----------------------

YJencrypted carts with stronger protection have "trap addresses" which will lock the cartridge from further reading if
they are read after it has been initially unlocked by the boot sequence. These differ per cartridge so can't be 
pre-empted, you will need to determine them yourself as part of the dumping process. Typically a cartridge may have
X number of trap addresses and then a larger trap "block" where any read within it will trip the protection.

VFDump in YJ dumping mode will display a message "Protection trip XXXXXXXX" when it hits a trap address, where XXXXXXXX
is the actual address. Now you need to skip the address. skips.bin ... tbc

Circumventing header protection
-------------------------------

Some cartridges have "trap addresses" in the Nintendo logo area of the header, meaning their protection is tripped by
re-reading the Nintendo logo after resetting the GBA. These are particularly difficult to dump using the normal method,
as the logo is read by the console's BIOS before vfdump has loaded, so we have no way to stop it from happening in
software. Instead we need a way to _maintain power to the cartridge while rebooting_ BUT _prevent the GBA from reading
the logo from it_.

The method I found is using a device called a "Game Changer" which allows 3 cartridges to be connected to a GBA SP
simultaneously. Using this device, steps 1-3 of the dumping process are replaced by the following:
1. Insert your protected cartridge into one slot of the Game Changer. Optionally insert another cartridge into
   another slot (it should still work if the other slot is empty)
2. Switch the Game Changer to the slot with the protected cartridge
3. Power on the GBA, allow the game to boot to the title screen
4. (With the GBA's power still on) Switch the Game Changer to another slot. The game will crash at this point but it
   doesn't matter.
5. Reset the GBA and load VFDump as per step 2-3 of the original process. You should either see a Nintendo logo (if a
   cartridge is connected in the other slot) or a blank logo (if nothing is connected).
6. Once VFDump has loaded, switch the Game Changer back to the slot with the protected cartridge, and proceed with
   dumping as per the original process from step 4 onwards.

There are 2 hardware issues you will need to circumvent if using a Game Changer, depending on the console model you are
dumping with.

* For an **original GBA**, because the Game Changer is designed to fit the SP only, you will need some way of extending
  the GBA's cartridge port to allow the Game Changer to be used. I used an Xploder Advance cheat cartridge for this
  purpose, with the front half of its shell removed.
* For a **GBA SP**, the Game Changer has a pass-through power switch and a hardware interlock that prevents the game
  from being switched while the power switch is on. You will need to open the Game Changer and remove the interlock in
  order to switch games with the power on.
