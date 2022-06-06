#include <GLFW/glfw3.h>

#include "shader.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


unsigned int compile_shader(const char * src, unsigned int type) {
    unsigned int id = glCreateShader(type);
    
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if(!result) {
        printf("Error in %s shader. Log:\n", type == GL_VERTEX_SHADER ? "vertex" : "fragment");
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        char * message = (char *) malloc(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        printf("%s\n", message);
        free(message);
        
        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int create_shader(const char * vertex, const char * fragment) {
    unsigned int program = glCreateProgram();
    unsigned int v_shader = compile_shader(vertex, GL_VERTEX_SHADER);
    unsigned int f_shader = compile_shader(fragment, GL_FRAGMENT_SHADER);

    glAttachShader(program, v_shader);
    glAttachShader(program, f_shader);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(v_shader);
    glDeleteShader(f_shader);

    return program;
}