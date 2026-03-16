<div align="center">

## Crimsonite
A modding framework for [Coromon](https://www.coromon.com/)!

</div>

 > **Heads-up!**
 > Any issues caused by modding your game should not be reported to the developers. Modding is *not* something supported by the game itself, therefore anything you do is at your own risk! Always make a backup before installing any mods.

<hr />

## Platform support
<div align="center">

| Platform        | Supported? |
| --------------- | ---------- |
| Windows (Steam) | Yes        |
| Windows (Epic)  | Yes*       |
| Windows (GOG)   | Yes*       |
| MacOS (Steam)   | No         |
| MacOS (Epic)    | No         |
| MacOS (GOG)     | No         |
| Linux (Steam)   | No         |
| Linux (Epic)    | No         |
| Linux (GOG)     | No         |

</div>

> \* The framework is made primarly with Windows in mind for the moment. Currently, I'm only testing it with the Steam version, so don't be surprised if some things don't work on the other storefronts.

## Installation
**Crimsonite** is currently *very* early in development. This repository is purely public for those tech-savvy and interested, as a way to monitor the progress I'm making. For the moment, everything is still very much in early testing stages, including the way the framework is installed and mods are handled.

### Building
For those who still want to check the project out and want to build it themselves, here's how to currently build it.

1. Download the source (either a `.zip` or `git clone` works fine)
2. From the root directory, run `cmake -B build -A Win32 && cmake --build build --config Release`
3. Grab `build/launcher/Release/crimsonite.exe` and `build/modloader/Release/crimsonite.dll`, and copy it to your game's installation directory
4. Run the launcher (`crimsonite.exe`), which will launch the game with mods!

## Mod Creation
*To be written.*