#include <GLFW/glfw3.h>

#include "shader.h"
#include "common.h"

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
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        char * message = (char *) malloc(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        LOG(message);
        free(message);
        
        glDeleteShader(id);

        ERROR(ERR_SHADER_COMPILE(type), 0);
    }

    return id;
}

unsigned int create_shader(const char * vertex, const char * fragment) {
    unsigned int program = glCreateProgram();
    unsigned int v_shader = compile_shader(vertex, GL_VERTEX_SHADER);
    unsigned int f_shader = compile_shader(fragment, GL_FRAGMENT_SHADER);

    glAttachShader(program, v_shader);
    glAttachShader(program, f_shader);
    glValidateProgram(program);

    glDeleteShader(v_shader);
    glDeleteShader(f_shader);

    return program;
}

int use_shader(unsigned int shader) {
    glLinkProgram(shader);

    int result;
    glGetProgramiv(shader, GL_LINK_STATUS, &result);
    if(!result) {
        LOG(ERR_SHADER_LINK);

        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        char * message = (char *) malloc(length * sizeof(char));
        glGetShaderInfoLog(shader, length, &length, message);
        LOG(message);
        free(message);

        return -1;
    }
    
    glUseProgram(shader);

    return 0;
}