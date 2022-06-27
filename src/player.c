#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "player.h"
#include "io.h"
#include "common.h"
#include "shader.h"

static window_t * window;

void resize(GLFWwindow * handle, int w, int h) {
	if(h == 0)
		h = 1;
    
    window->dimensions[0] = w;
    window->dimensions[1] = h;

    glViewport(0, 0, w, h);
}

void error(int err, const char * description) {
    fputs(description, stderr);
}

void update(simulation_t * sim) {
    if(!sim->paused)
        next_frame(sim);

    keyboard_t * keyboard = get_keyboard();
    camera_t * camera = get_camera();
    float speed = (keyboard->lshift_down ? PAN_SPEED_SHIFT : PAN_SPEED) / sim->framerate,
          dx = ((float) (keyboard->d_down - keyboard->a_down)) * speed / normalize_zoom(camera->zoom),
          dy = ((float) (keyboard->w_down - keyboard->s_down)) * speed / normalize_zoom(camera->zoom);
    if(keyboard->r_down) {dx = -camera->offset[0]; dy = -camera->offset[1]; camera->zoom = 1.0f;};
    if(keyboard->f_down) {sim->frame = 0;};

    float delta[2] = {dx, dy};
    //pan_camera(delta);
    printf("frame: %d\n", sim->frame);

    if(keyboard->space_down) sim->paused = 1 - sim->paused;
}

void render(unsigned int render_shader, simulation_t * sim) {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    const float u_dims[2] = {window->dimensions[0], window->dimensions[1]};
    glUniform2fv(glGetUniformLocation(render_shader, "window"), 1, u_dims);

    camera_t * camera = get_camera();
    const float u_camera[2] = {camera->offset[0], camera->offset[1]};
    glUniform2fv(glGetUniformLocation(render_shader, "camera"), 1, u_camera);

    const float u_zoom = normalize_zoom(camera->zoom);
    glUniform1f(glGetUniformLocation(render_shader, "zoom"), u_zoom);

    glBufferData(GL_ARRAY_BUFFER, 5 * sim->settings->n * sizeof(float), sim->particles + sim->settings->n * sim->frame, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_POINTS, 0, sim->settings->n);

    glfwSwapBuffers(window->handle);
}

int play(int argc, char * argv[], window_t * w) {
    simulation_t * sim;

    if((sim = load_simulation(argc, argv)) == NULL)
        return EXIT_FAILURE;
    
    window = w;
    
    glfwSetErrorCallback(error);
    glfwSetKeyCallback(window->handle, handle_keyboard);
    glfwSetScrollCallback(window->handle, handle_scroll);
    glfwSetWindowSizeCallback(window->handle, resize);

    glfwMakeContextCurrent(window->handle);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glfwSwapInterval(0);

    GLuint vao_name;
    glGenVertexArrays(1, &vao_name);
    glBindVertexArray(vao_name);

    unsigned int buffer;
    glGenBuffers(1, &buffer);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

    char * v_shader = read_file_as_string("shaders/point.vert"),
         * f_shader = read_file_as_string("shaders/point.frag");

    unsigned int shader = create_render_shader(v_shader, f_shader);

    link_program(shader);
    glUseProgram(shader);

    uint64_t delta = 1000000/(sim->framerate),
             acc = 0;

    struct timespec start, end;
    while(!glfwWindowShouldClose(window->handle)) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        while(acc >= delta) {
            update(sim);
            acc -= delta;
        }

        render(shader, sim);
        
        glfwPollEvents();

        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        acc += (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    }

    free_simulation(sim);

    return EXIT_SUCCESS;
}