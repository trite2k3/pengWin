# pengWin
pengWin is a fully external hack for Counter-Strike\: Global Offensive.  
In comparison to internal cheats (e.g. AimTux) it does not inject any code into the game.  
Unlike other external hacks it does not write to the games memory at all.

# Current Features:
- External Radar
- Triggerbot
- BunnyHop
- Aimbot
- Offset Dumper
- Loading settings from file
- Defining Hotkeys

# Todo:
- Sound ESP
- BlockBot
- Visibility check (BSP parsing)
- Switch visu to using the pipe

# Building
Use the included Makefile to build.
Then run `chmod 700 pengWin` and `chown root pengWin` to make the hack unaccessible for VAC.

# Usage
`sudo ./pengWin [options]`
Possible options are:  
-aimbot  
-bhop  
-debug  
-radar  
-trigger  
