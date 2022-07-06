#ifndef __IO_H__
#define __IO_H__

#include <GLFW/glfw3.h>

#include "settings.h"

typedef struct {
    settings_t * settings;
    int frame, framerate, paused;
    float * particles;
} simulation_t;

typedef struct {
    int w_down, a_down, s_down, d_down, r_down, f_down, space_down, lshift_down, plus_down, minus_down;
} keyboard_t;

typedef struct {
    float offset[2];
    float zoom;
} camera_t;


/* Open the file and return the file pointer
*/
FILE * open_file(char * filename, const char * mode);

/* Closes fp
*/
int close_file(char * filename, FILE * fp);

/* Creates output file and writes the simulation settings to it
*/
FILE * init_output_file(settings_t * settings);

/* Loads simulation from file
*/
simulation_t * load_simulation(int argc, char * argv[]);

/* Frees up memory allocated for simulation
*/
void free_simulation(simulation_t * sim);

/* Forwards to next frame and resets it to 0 if it exceeds sim->frames
*/
void next_frame(simulation_t * sim);


/*  -----   KEYBOARD & MOUSE INPUT  -----   */


/* Keyboard callback function
*/
void handle_keyboard(GLFWwindow * window, int key, int scancode, int action, int mods);

/* Mouse wheel callback function
*/
void handle_scroll(GLFWwindow * window, double x, double y);

/* Scale zoom to the open interval (0; infinity)
*/
float normalize_zoom(float zoom);

/* Pan camera by delta
*/
void pan_camera(float delta[2]);

/* Returns global keyboard context
*/
keyboard_t * get_keyboard();

/* Returns global camera context
*/
camera_t * get_camera();

#endif