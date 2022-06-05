#include <GLFW/glfw3.h>

#include "player.h"
#include "simulation.h"

#include <stdio.h>

typedef struct {
    int x, y;
} Vec2;

static Vec2 camera = {0, 0};
static simulation_t * sim;

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

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glBegin(GL_POINTS);

    int offset = sim->frame * sim->settings->particle_count;
    for(int i = 0; i < sim->settings->particle_count; i++) {
        float * particle = sim->particles + offset + i * 5;
        
        draw_point(particle[0] + camera.x, particle[1] + camera.y, 1.0, 0.5, 0.2, 0.1);
    }

    glEnd();
}

void resize(GLFWwindow * window, int w, int h) {
	if(h == 0)
		h = 1;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    
    glViewport(0, 0, w, h);
    glOrtho(0, w, h, 0, -1, 1);
	
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
    
    GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
    if(window == NULL) {
        glfwTerminate();
        return 1;
    }

    glfwSetWindowSizeCallback(window, resize);
    glfwMakeContextCurrent(window);

    resize(window, WIDTH, HEIGHT);
    while(!glfwWindowShouldClose(window)) {
        update();
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    unload_simulation(sim);

    return 0;
}