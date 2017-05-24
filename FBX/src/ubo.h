#pragma once

#include <GL/glew.h>
#include <stdlib.h>
#include <memory>

template <class T>
class UBO
{
public:
	UBO(int bufferIndex);
	~UBO();

	void updateBuffer(const GLvoid* data);
private:
	GLuint ubo;
	GLuint mBufferIndex;
	GLuint mBufferSize;
};


template<class T>
inline UBO<T>::UBO(int bufferIndex)
	: mBufferIndex(bufferIndex), mBufferSize(sizeof(T))
{
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, mBufferSize, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

template<class T>
inline UBO<T>::~UBO()
{
	if (ubo) {
		glDeleteBuffers(1, &ubo);
	}
	ubo = NULL;
}
template<class T>
inline void UBO<T>::updateBuffer(const GLvoid* data)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, mBufferIndex, ubo);
	GLvoid* uboData = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	std::memcpy(uboData, data, mBufferSize);
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}
