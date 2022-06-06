#include <GLFW/glfw3.h>

#include "player.h"
#include "simulation.h"
#include "shader.h"
#include "common.h"

#include <stdio.h>
#include <math.h>
#include <time.h>

typedef struct {
    int x, y;
} Vec2;

typedef struct {
    Vec2 dimensions;
    GLFWwindow * handle;
} Window;

static Vec2 camera = {0, 0};
static simulation_t * sim;
static Window window;

void pan_camera(Vec2 delta) {
    camera.x += delta.x;
    camera.y += delta.y;
}

void draw_point(float x, float y, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glVertex2f(x, y);
}

void update() {
    next_frame(sim);
}

void render(unsigned int point_shader) {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    const float u_dims[2] = {(float) window.dimensions.x, (float) window.dimensions.y};
    glUniform2fv(glGetUniformLocation(point_shader, "window"), 1, u_dims);

    glBufferData(GL_ARRAY_BUFFER, 5 * sim->settings->particle_count * sizeof(float), sim->particles + sim->settings->particle_count * sim->frame, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_POINTS, 0, sim->settings->particle_count);

    glfwSwapBuffers(window.handle);
    glfwPollEvents();
}

void resize(GLFWwindow * handle, int w, int h) {
	if(h == 0)
		h = 1;
    
    Vec2 new_dimensions = {w, h};
    window.dimensions = new_dimensions;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    
    glViewport(0, 0, w, h);
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	
    glMatrixMode(GL_MODELVIEW);
}

void error(int err, const char * description) {
    fputs(description, stderr);
}

int main(int argc, char * argv[]) {
    if((sim = load_simulation(argc, argv)) == NULL)
        return -1;
    sim->frame = 0;

    if (!glfwInit())
        return 1;
    
    glfwSetErrorCallback(error);
    
    Window w = {{WIDTH, HEIGHT}, glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL)};
    window = w;

    if(window.handle == NULL) {
        glfwTerminate();
        return 1;
    }

    glfwSetWindowSizeCallback(window.handle, resize);
    glfwMakeContextCurrent(window.handle);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    resize(window.handle, WIDTH, HEIGHT);

    glfwSwapInterval(0);

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

    char * v_shader = read_file_as_string("shaders/point.vert"),
         * f_shader = read_file_as_string("shaders/point.frag");

    unsigned int shader = create_shader(v_shader, f_shader);
    glUseProgram(shader);
    
    uint64_t delta = 1000000/144/4,
             acc = 0;

    struct timespec start, end;
    while(!glfwWindowShouldClose(window.handle)) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);

        while(acc >= delta) {
            update();
            acc -= delta;
        }

        render(shader);

        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        acc += (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    }

    glfwDestroyWindow(window.handle);
    glfwTerminate();

    unload_simulation(sim);

    return 0;
}