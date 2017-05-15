
#pragma once

#include <iostream>
#include <Windows.h>
#include <assert.h>
#include <GL/glew.h>
#include <string>

#define LOG						std::cout
#define ENDL					std::endl
#define ENDN					"\n"

#define LOG_ERROR(x)				Log::log_error(x)
#define LOG_ASSERT(x)				Log::log_error(x)
#define LOG_SHADER_ERROR(x,xx)		Log::log_shader_error(x,xx)

namespace Log
{
	inline void log_error(const std::string &msg)
	{
		MessageBox(NULL, msg.c_str(), "Error", MB_OK);
	}

	inline void log_assert(const std::string &msg)
	{
		MessageBox(NULL, msg.c_str(), "Error", MB_OK);
		assert(0 && msg.c_str());
	}

	inline void log_shader_error(const std::string &path, const std::string &msg)
	{
		std::string err = path;
		err.append(" : ").append(msg);
		LOG << err.c_str() << ENDL;
		//MessageBox(NULL, err.c_str(), "Error", MB_OK);
		//assert(0 && msg.c_str());
	}

	/*extern int debugIndex;
	int debugIndex = 0;*/
	inline void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
	{
		//return warnings
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

		std::cout << "OpenGL debug message" << std::endl;
		std::cout << "    Source: ";

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:
			std::cout << "API";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			std::cout << "APPLICATION";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			std::cout << "OTHER";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			std::cout << "SHADER COMPILER";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			std::cout << "THIRD PARTY";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			std::cout << "WINDOW SYSTEM";
			break;
		}

		std::cout << std::endl;
		std::cout << "    Type: ";

		switch (type)
		{
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			std::cout << "DEPRECATED BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_ERROR:
			std::cout << "ERROR";
			break;
		case GL_DEBUG_TYPE_MARKER:
			std::cout << "MARKER";
			break;
		case GL_DEBUG_TYPE_OTHER:
			std::cout << "OTHER";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			std::cout << "PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			std::cout << "POP GROUP";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			std::cout << "PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			std::cout << "PUSH GROUP";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			std::cout << "UNDEFINED BEHAVIOR";
			break;
		}

		std::cout << std::endl;
		std::cout << "    ID: " << id << std::endl;
		std::cout << "    Severity: ";

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			std::cout << "HIGH";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			std::cout << "LOW";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			std::cout << "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			std::cout << "NOTIFICATION";
			break;
		}

		std::cout << std::endl;
		std::cout << "    Message: " << message << std::endl;

		std::string debugMsg("Debug Index : ");
		/*debugMsg += std::to_string(debugIndex) + "\n";*/
		debugMsg += message;

		const int msgID =
			MessageBox(NULL, debugMsg.c_str(),
				"GLEW_KHR_debug", MB_OKCANCEL);

		switch (msgID)
		{
		case IDOK:
			/*debugIndex++;*/
			break;
		case IDCANCEL:
			exit(0);
			break;
		default:
			break;
		}
	}

}
