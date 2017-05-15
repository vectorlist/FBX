
#pragma once

#include <GL/glew.h>
#include <string>
#include <vec2f.h>
#include <vec3f.h>
#include <matrix4x4.h>
#include <log.h>
#include <fstream>
#include <sstream>

#define LOAD_SHADER(x,xx) ShaderTool::load(x,xx)

namespace ShaderTool
{
	
	GLuint load(const char* vertfile, const char* fragfile);
	std::string codeFromFile(const std::string &filename);
	GLuint compile(const GLchar* source, const GLenum type, const char *code);

	void setUniform1i(GLuint &program, uint32_t data, const char* name);
	void setUniform1f(GLuint &program, float data, const char* name);
	void setUniForm2i(GLuint &program, const vec2i &data, const char* name);
	void setUniForm2f(GLuint &program, const vec2f &data, const char* name);
	void setUniForm3f(GLuint &program, const vec3f &data, const char* name);
	void setUniformMatrix4f(
		GLuint &program,
		const Matrix4x4 &mat,
		const char* name,
		bool transpose = false);
}

inline GLuint ShaderTool::load(const char * vertpath, const char * fragpath)
{
	std::string vertCode = codeFromFile(vertpath);
	std::string fragCode = codeFromFile(fragpath);

	GLuint vert = compile(vertCode.c_str(), GL_VERTEX_SHADER, vertpath);
	GLuint frag = compile(fragCode.c_str(), GL_FRAGMENT_SHADER, fragpath);

	GLuint program = glCreateProgram();

	glAttachShader(program, vert);
	glAttachShader(program, frag);

	glLinkProgram(program);

	GLint success;
	GLchar info_log[512];

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, info_log);
		LOG_ERROR(info_log);
	}

	//we dont need shader anymore
	glDeleteShader(vert);
	glDeleteShader(frag);

	glUseProgram(program);

	return program;
}

inline std::string ShaderTool::codeFromFile(const std::string &filename)
{
	std::string bits;
	std::ifstream filestream(filename, std::ios::in);
	if (!filestream.is_open())
	{
		std::string err = "failed to load file : ";
		err.append(filename);
		LOG_ERROR(err);
	}

	std::string line = "";
	while (!filestream.eof())
	{
		std::getline(filestream, line);
		bits.append(line + "\n");
	}
	filestream.close();
	return bits;

}

inline GLuint ShaderTool::compile(const GLchar *code, const GLenum type, const char* path)
{
	GLint success;
	GLchar info_log[512];

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		LOG_SHADER_ERROR(path, info_log);
	}

	return shader;
}

inline void ShaderTool::setUniform1i(GLuint &program, uint32_t data, const char* name)
{
	glUniform1i(glGetUniformLocation(program, name), data);
}

inline void ShaderTool::setUniform1f(GLuint &program, float data, const char* name)
{
	glUniform1f(glGetUniformLocation(program, name), data);
}

inline void ShaderTool::setUniForm2i(GLuint &program, const vec2i &data, const char* name)
{
	glUniform2i(glGetUniformLocation(program, name), data.x, data.y);
}

inline void ShaderTool::setUniForm2f(GLuint &program, const vec2f &data, const char* name)
{
	glUniform2f(glGetUniformLocation(program, name), data.x, data.y);
}

inline void ShaderTool::setUniForm3f(GLuint &program, const vec3f &data, const char* name)
{
	glUniform3f(glGetUniformLocation(program, name), data.x, data.y, data.z);
}

inline void ShaderTool::setUniformMatrix4f(GLuint &program, const Matrix4x4 &mat, const char* name, bool transpose)
{
	glUniformMatrix4fv(glGetUniformLocation(program, name), 1, transpose, mat.constData());
}