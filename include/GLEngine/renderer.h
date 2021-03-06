#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <sstream>
class Renderer
{
public:
	Renderer(){}

	bool isRunning = true;
	//TEST
	int sampleIndex = 0;
};

class Timer
{
public:
	float delta = 0.0f;
	int frameCount = 0;
	float last = 0.0f;
	float current = 0.0f;
	float lastSecond = 0.0;

	void begin();
	void end(SDL_Window* window);
};

inline void Timer::begin()
{
	current = (float)SDL_GetTicks();
	delta = (current - last) * 0.001f;
	last = current;
}

inline void Timer::end(SDL_Window* window)
{
	frameCount++;
	if (current - lastSecond > 1000)
	{
		const float fps = static_cast<float>(frameCount) /
			(current - lastSecond) * 1000.0f;

		std::ostringstream os;
		os << "OpenGL Code Sample - " << " [fps: " << fps << "]";
		SDL_SetWindowTitle(window, os.str().c_str());

		lastSecond = current;
		frameCount = 0;

	}
	SDL_Delay(1);
	SDL_GL_SwapWindow(window);
}