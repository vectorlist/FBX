#pragma once

#include <GL/glew.h>
#include <stdlib.h>
#include <memory>

class UBO
{
public:
	UBO(int bufferIndex, int bufferSize);
	~UBO();

	void updateBuffer(const GLvoid* data);
private:
	GLuint ubo;
	GLuint mBufferIndex;
	GLuint mBufferSize;
};

