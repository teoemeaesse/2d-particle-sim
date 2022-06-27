#ifndef __SHADER_H__
#define __SHADER_H__

/* Compiles the shader at src and returns a handle to it
*/
unsigned int compile_shader(const char * src, unsigned int type);

/* Creates a program from the given shader sources and returns a handle to it
*/
unsigned int create_render_shader(const char * vertex, const char * fragment);

/* Links the given program and returns 0 on success or -1 on failure
*/
int link_program(unsigned int program);

/* Clear opengl errors
*/
void gl_clear_errors();

/* Check for and print opengl errors
*/
void gl_check_error();

/* Log work group and invocations max sizes for compute shaders
*/
void log_wg_sizes();


#endif