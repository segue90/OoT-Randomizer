Advanced modifications to the Randomzier source require a bit more software than what is needed for running it.

## Assembly: armips
- Download the armips assembler: <https://github.com/Kingcom/armips>, build it if necessary, and put the executable in the `tools` directory, or somewhere in your PATH.
- Put the ROM you want to patch at `roms/base.z64`. This needs to be an uncompressed ROM; OoTRandomizer will produce one at ZOOTDEC.z64 when you run it with a compressed ROM.
- Run `python build.py`, which will:
  - create `roms/patched.z64`
  - update some `txt` files in `build/` and in `../data/generated/`. Check `git status` to see which ones have changed. Make sure you submit them all together!

## C: n64 toolchain
To recompile the C modules, use the `--compile-c` option. This requires the N64 development tools to be installed: <https://github.com/glankk/n64>.
There are several ways to do this depending on your platform.
- **Windows**:
  - **Without WSL**: [Download this zip archive](https://discord.com/channels/274180765816848384/442752384834469908/1085678948614144081)
  and extract the `n64` folder into the `tools` directory alongside armips.
  - **Using WSL**: Install the latest Debian Linux from the Windows Store and follow the below instructions for Debian.
- **Debian**: [Follow this how-to](https://practicerom.com/public/packages/debian/howto.txt) on adding the toolchain's 
package repository and installing the pre-built binaries.
- **Any platform with a gcc compiler**: Build from source from the [glankk/n64](https://github.com/glankk/n64) 
repository. Simply follow the readme.
  - It is easiest if you use `--prefix=/the/path/to/OoT-Randomizer/ASM/tools` for the `./configure` step. This will 
  install all the toolchain in a way the build script can use, however this is inconvenient if you plan to use the 
  toolchain for other projects as well.
  - If you are trying to update the toolchain this way, it is easiest to just delete your local copy of the repository 
  and clone it again to ensure all the packages get updated and are compatible.

You can substitute using the `tools` folder with adding the `n64/bin` folder to your environment PATH if you need an 
advanced setup.

## Debugging Symbols for Project64
To generate symbols for the Project64 debugger, use the `--pj64sym` option:

    python build.py --pj64sym 'path_to_pj64/Saves/THE LEGEND OF ZELDA.sym'

You'll need to disable `Unique Game Save Directory` in Project64 for these to work without copying them into each 
unique save folder. Remember that some changes in code will not be reflected in an existing save, and they need to be 
deleted and a new save created with this setting disabled.
