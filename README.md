== How the Game Works ==

Snake is a retro arcade game where the player controls a line that moves around the screen, eating food and growing longer. The game ends when the snake collides with itself or the edges of the screen, or becomes so long that it takes up every pixel. 

In our OS’s version of snake, you can use the WAXD keys to maneuver the snake around the screen. Food will appear at random locations on the screen, and maneuvering the snake up to the food will allow it to “eat” the food and grow longer. The game ends and is reset when the snake collides with the boundaries of the screen, or reaches its max size (limited by the number of small sprites available to us). 

-----------------------

== How to run games ==

Our OS is located in a folder named “firmware”, and has a makefile in said folder that will build the binaries to use our OS. Additionally, we have included the binaries of group 0 and group 2 and put them in folders named “firmware_0” and “firmware_2” respectively. The game cartridges for our OS, Group 0’s OS, and Group 2’s OS can be found in the folders “demo-cart”, “zero-cartridge”, and “implementation_for_group_2” respectively. They also contain makefiles to build their respective binaries.

NOTE: Use the .strip version for Group 2’s OS
