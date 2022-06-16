#include <GLFW/glfw3.h>

#include "player.h"
#include "simulation.h"
#include "shader.h"
#include "input.h"
#include "common.h"

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

typedef struct {
    Vec2 dimensions;
    GLFWwindow * handle;
} Window;

static Vec2 camera = {0.0f, 0.0f};
static simulation_t * sim;
static Window window;

void pan_camera(Vec2 delta) {
    camera.x += delta.x;
    camera.y += delta.y;
}

void gl_clear_error() {
    while(glGetError() != GL_NO_ERROR);
}

void gl_check_error() {
    unsigned int error;
    while((error = glGetError())) {
        printf("[opengl error]: %d\n", error);
    }
}

void resize(GLFWwindow * handle, int w, int h) {
	if(h == 0)
		h = 1;
    
    Vec2 new_dimensions = {(float) w, (float) h};
    window.dimensions = new_dimensions;

    glViewport(0, 0, w, h);
}

void error(int err, const char * description) {
    fputs(description, stderr);
}

void update() {
    if(!sim->paused)
        next_frame(sim);

    Keyboard keyboard = get_keyboard();
    float speed = (keyboard.lshift_down ? PAN_SPEED_SHIFT : PAN_SPEED) / sim->framerate,
          dx = ((float) (keyboard.d_down - keyboard.a_down)) * speed / normalize_zoom(),
          dy = ((float) (keyboard.w_down - keyboard.s_down)) * speed / normalize_zoom();
    if(keyboard.r_down) {dx = -camera.x; dy = -camera.y; reset_zoom();};
    if(keyboard.f_down) {sim->frame = 0;};
    Vec2 delta = {dx, dy};
    pan_camera(delta);

    if(keyboard.space_down) sim->paused = 1 - sim->paused;
}

void render(unsigned int point_shader) {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    const float u_dims[2] = {(float) window.dimensions.x, (float) window.dimensions.y};
    glUniform2fv(glGetUniformLocation(point_shader, "window"), 1, u_dims);

    const float u_camera[2] = {(float) camera.x, (float) camera.y};
    glUniform2fv(glGetUniformLocation(point_shader, "camera"), 1, u_camera);

    const float u_zoom = normalize_zoom();
    glUniform1f(glGetUniformLocation(point_shader, "zoom"), u_zoom);

    glBufferData(GL_ARRAY_BUFFER, 5 * sim->settings->n * sizeof(float), sim->particles + sim->settings->n * sim->frame, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_POINTS, 0, sim->settings->n);

    glfwSwapBuffers(window.handle);
}

int main(int argc, char * argv[]) {
    if((sim = load_simulation(argc, argv)) == NULL)
        return -1;

    if (!glfwInit())
        return 1;
        
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwSetErrorCallback(error);

    Window w = {{WIDTH, HEIGHT}, glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL)};
    window = w;

    if(window.handle == NULL) {
        glfwTerminate();
        return 1;
    }

    glfwSetKeyCallback(window.handle, handle_keyboard);
    glfwSetScrollCallback(window.handle, handle_scroll);
    glfwSetWindowSizeCallback(window.handle, resize);

    glfwMakeContextCurrent(window.handle);
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

    unsigned int shader = create_shader(v_shader, f_shader);

    use_shader(shader);

    
    uint64_t delta = 1000000/(sim->framerate),
             acc = 0;

    struct timespec start, end;
    while(!glfwWindowShouldClose(window.handle)) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        while(acc >= delta) {
            update();
            acc -= delta;
        }

        render(shader);
        
        glfwPollEvents();

        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        acc += (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    }

    glfwDestroyWindow(window.handle);
    glfwTerminate();

    unload_simulation(sim);

    return 0;
}