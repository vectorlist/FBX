#include "ubo.h"


UBO::UBO(int bufferIndex, int bufferSize)
	: mBufferIndex(bufferIndex), mBufferSize(bufferSize)
{
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, bufferSize, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


UBO::~UBO()
{
	if (ubo) {
		glDeleteBuffers(1, &ubo);
	}
	ubo = NULL;
}

void UBO::updateBuffer(const GLvoid* data)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, mBufferIndex, ubo);
	GLvoid* uboData = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	std::memcpy(uboData, data, mBufferSize);
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}
