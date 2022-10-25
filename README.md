# Author is jasonks2
# Special thanks to NCSA AVL and NASA SVS

# SpacePilot-PRO-for-Houdini-Engine
Features scripts for the Houdini Engine to pipe and interpret SpacePilot PRO inputs

#Requirements:


- A physical SpacePilot 3D Mouse by 3DConnexion (SpacePilot PRO is used for this prototype)
- A legitimate version of the Houdini Engine >18.0.5
- The spacenavd daemon (https://github.com/FreeSpacenav/spacenavd)
- The libspnav extension library (https://github.com/FreeSpacenav/libspnav)
- X11 library (for testing purposes) (https://github.com/mirror/libX11) # Maybe not needed?


# Step-by-step instructions

** 1. Plug in Space Mouse **

await the "Initializing.." message

** 2. Build environment **

Compile spacenavd by using "--disable-x11" flag (see link above)

Configure libspnav (see 2nd link above)

** 3. Build PipeOut applications **

The Houdini Engine natively reads AF_Unix pipes via CHOP Networks

Within the directory of ../../CHOP_Example/

Run -> make -f Makefile.PPDOUT
       cd test
       make -f Makefile.pipeout

** 4. Start Pipeout process **

Run -> ./pipeout /tmp/chop.pipe

Await "Looking for reader"


** 5. Start Houdini Engine **

Load and execute Python file space_mouse.py


** 6. Load and execute Python file **

As opposed to an .HDA. space_mouse.py will:
	- Create a Pipe In Node
	- Create a Camera obj linked to the viewport
	- Register a callback to read the space mouse at clock rate

** 7. Interact and give feedback! **
       


Thanks



