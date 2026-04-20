 > **Heads-up!**
 > Any issues caused by modding your game should not be reported to the developers. Modding is *not* something supported by the game itself, therefore anything you do is at your own risk! Always make a backup before installing any mods.

## Examples
Since a modloader with no example mods is hard to get into for new modders, I decided to have some example mods in the repository as well. Note that to run these example mods, you must adhere to the standard of how **Crimsonite** loads mods -- in this case, this means simply copying an example mod's folder to the `mods/` folder works just fine!

The rest of this document will serve as a short explanation for each of the example mods that you'll find in here. Any important key ideas will be shortly highlighted as well! For aspiring modders reading this: feel free to take inspiration from how I have structured these mods, but since the loader injects Lua directly, anything is possible! Lastly, make sure to consult the documentation if you're unsure about anything.

### `barebones`
Introduces the `init.lua` and `mod.lua` files and their structure. These serve as the *required* files for any mod. `init.lua` contains a required `init()` method which will be called by **Crimsonite** upon loading it; `mod.lua` contains information about the mod itself, which is displayed on the mod list page in-game.