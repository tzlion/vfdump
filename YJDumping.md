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
      See section "Circumventing header protection"
4. Select the "skip" option and then "Dump YJencrypted", it will begin to dump.
    * If your cartridge is one of the weaker protected ones, the dump will complete, no further action needed.
    * If your cartridge has the stronger protection, at some point the message "Protection trip" will appear.
      This means you've hit a "trap address" which will lock out the cartridge from further reads.
      See the section "Skipping trap addresses" for how to deal with this.

TODO What you need to do to restore the header section and make it boot

Skipping trap addresses
-----------------------

YJencrypted carts with stronger protection have "trap addresses" which will lock the cartridge from further reading if
they are read after it has been initially unlocked by the boot sequence. These differ per cartridge so can't be 
pre-empted, you will need to determine them yourself as part of the dumping process. Typically a cartridge may have
about 12 trap addresses and then a larger trap "block" of about 1000-2000 bytes where any read within it will trip the
protection.

VFDump in YJ dumping mode will display a message "Protection trip XXXXXXXX" when it hits a trap address, where XXXXXXXX
is the 32-bit trap address found. Now you need to make it skip that address the next time it dumps the game. To do that,
use a hex editor to enter the address into skips.bin, a 72-byte binary file which is provided in the repository, or you
can create it yourself.

An "empty" skips.bin looks like this (# are comments)

```
FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF # 16 possible skip addresses
FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF
FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF
FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF
FFFFFFFF FFFFFFFF                   # Start and end addresses of skip block (Start is inclusive and end is exclusive)
```

So if you populate it like this

```
00001234 0090A0B0 FFFFFFFF FFFFFFFF # Will skip individual 32-bit addresses 00001234 and 0090A0B0
FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF
FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF
FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF
00102000 0010C000                   # Will skip all addresses between 00102000-0010BFFF
```

Every time you find a new trap address and add it to skips.bin, you will have to fully power off the GBA and start the 
dumping process again from step 1. If you find multiple consecutive trap addresses, it is likely you've found the start
of the "trap block" and can use some trial and error to find the end of it. So to fully populate skips.bin you might
need ~20 cycles of the whole process (and if there is a trap address in the Nintendo logo, you will need to repeat the
process below every time as well).

Once skips.bin is populated with every trap address/block in the cartridge, the dump should be able to complete to the
end.

VFDump will replace any skipped data in the dumped ROM with the string "SKIP" in ASCII. Generally the skipped data is
repeated elsewhere in the ROM, so you should be able to infer what it should be, using a hex editor, by searching for
other locations of the same data before and after it, and then patch in the missing data from the other location in
order to recreate what the original ROM should have looked like. Note that ROMs dumped and recreated through this
process should not be treated as 100% confirmed dumps due to the necessity of inferring unreadable data like this.
TODO this is badly written

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
