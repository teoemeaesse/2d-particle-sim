#ifndef __SHADER_H__
#define __SHADER_H__

unsigned int compile_shader(const char * src, unsigned int type);
unsigned int create_shader(const char * vertex, const char * fragment);

#endif