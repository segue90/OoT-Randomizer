- Download the armips assembler: <https://github.com/Kingcom/armips>, build it if necessary, and put the executable in the `tools` directory, or somewhere in your PATH.
- Put the ROM you want to patch at `roms/base.z64`. This needs to be an uncompressed ROM; OoTRandomizer will produce one at ZOOTDEC.z64 when you run it with a compressed ROM.
- Run `python build.py`, which will:
  - create `roms/patched.z64`
  - update some `txt` files in `build/` and in `../data/generated/`. Check `git status` to see which ones have changed. Make sure you submit them all together!

To recompile the C modules, use the `--compile-c` option. This requires the N64 development tools to be installed: <https://github.com/glankk/n64>

To generate symbols for the Project 64 debugger, use the `--pj64sym` option:

    python build.py --pj64sym 'path_to_pj64/Saves/THE LEGEND OF ZELDA.sym'

--------------------------------------------------------------------------

How to use the Debug mode: 
- First put the DEBUG_MODE variable at 1 in debug.h.
- You will then have access to a hidden menu with the following options:
  - Instant warps to Dungeons, Bosses or Overworld locations
  - Item inventory edits
  - Instant age switch
  - Bunny Hood applied on
The menu will appear if you press either L+R or Dpad-Up according to what you set for the menu_not_on_dup variable in debug.c.
Use Dpad-Left/Dpad-Right and A/B to navigate it.
The L Button will also allow you to levitate.
The warps and items are easily customizable with the code at the top of debug.c.
- Additionally, you can call functions to print numbers on screen, to help you debug new features.
Call either draw_debug_int or draw_debug_float in your code, with the first argument being the number wanted to be displayed, and the 
second argument its place on the screen (up to 10 values).
- An input display is also available, but the icons are currently bugged on Dolphin.