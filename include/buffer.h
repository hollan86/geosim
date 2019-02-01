#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "glad/glad.h"
#include <GLFW/glfw3.h>

class buffer
{

public:
buffer();
void allocateBufferData(unsigned int size, void* data, GLenum usage);
void generateBuffer(GLenum bufferType);
void bindBuffer();
void setVertexAttribPointer(unsigned int index, int size, GLenum type, bool normalized, int stride, void* pointer);
void cleanup();

private:

GLuint Buffer;

// buffer type
GLenum BufferType;
};
#endif