# 2d-particle-sim
2D N-body particle simulator and player in C using OpenGL and GLFW.
The goal of this project is to create a fast, easy to use and fully customizable N-body particle simulator in 2D for linux.

## System Requirements
To compile and run you need the following:
- A GPU with **OpenGL 4.5** (or higher) support;
- **G++** and [**GLFW**](https://stackoverflow.com/questions/17768008/how-to-build-install-glfw-3-and-use-it-in-a-linux-project) installed on your machine.

## Compile && Run
Do `make` in each the sim/ subdirectory to compile the simulator or in the player/ subdirectory to compile the simulation player.
- sim: `./simulator [filename] [particle_count] [frames]`
- player: `./player [output file] [config file]`
