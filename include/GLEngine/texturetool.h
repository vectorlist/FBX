
#pragma once

#include <GL/glew.h>
#include <SDL2/SDL_image.h>
#include <log.h>
#include <vector>

#define LOAD_TEXTURE(x,xx)	TextureTool::loadTexture(x,xx)
#define LOAD_CUBEMAP(x)		TextureTool::loadCubeMap(x)

namespace TextureTool
{
	inline GLuint loadTexture(const char *path, bool clamp = true)
	{
		SDL_Surface* image = IMG_Load(path);

		if (image == NULL) {
			std::string err = "failed to load image : ";
			err.append(path);
			LOG_ERROR(err);
		}
		GLuint texture;

		glGenTextures(1, &texture);		
		glBindTexture(GL_TEXTURE_2D, texture);

		if (clamp) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		if (image->format->BytesPerPixel == 4)
		{
			GLenum format = (image->format->Rmask == 255) ? GL_RGBA : GL_BGRA;

			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				image->w, image->h,
				0,
				format,
				GL_UNSIGNED_BYTE,
				(GLvoid*)image->pixels);
		}
		else {
			GLenum format = (image->format->Rmask == 255) ? GL_RGB : GL_BGR;

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0,
				format,
				GL_UNSIGNED_BYTE,
				(GLvoid*)image->pixels);
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		SDL_FreeSurface(image);
		return texture;
	}

	inline GLuint loadCubeMap(std::vector<std::string> faces)
	{
		GLuint texture;
		glGenTextures(1, &texture);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

		for (int i = 0; i < faces.size(); ++i)
		{
			SDL_Surface* image = IMG_Load(faces[i].c_str());

			if (image == NULL) {
				std::string err = "failed to load image : ";
				err.append(faces[i].c_str());
				LOG_ERROR(err);
			}
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
				image->w,
				image->h,
				0,
				GL_RGBA, GL_UNSIGNED_INT, image->pixels);
			SDL_FreeSurface(image);
		}

		glTextureParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_CUBE_MAP,0);
		return texture;
	}
}

