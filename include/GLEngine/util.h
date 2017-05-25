#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <Windows.h>
#include <camera.h>
#include <log.h>
#include <renderer.h>

namespace Input
{
	inline void event(Renderer &renderer,Camera &camera)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				renderer.isRunning = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (e.button.button == SDL_BUTTON_LEFT)
				{
					SDL_SetRelativeMouseMode(SDL_TRUE);
					camera.mousePos.x = (float)e.motion.x;
					camera.mousePos.y = (float)e.motion.y;
					camera.motion = true;
					/*SDL_SetRelativeMouseMode(SDL_TRUE);*/
				}
				break;
			case SDL_MOUSEMOTION:
				if (camera.motion)
				{
					float xpos = (float)e.motion.x;
					float ypos = (float)e.motion.y;
					//LOG << "moving" << e.motion.x << " " << e.motion.y << ENDL;
					float xoffset = xpos - camera.mousePos.x;
					float yoffset = camera.mousePos.y - ypos;

					camera.mousePos.x = xpos;
					camera.mousePos.y = ypos;
					camera.ProcessMouseMovement(xoffset, yoffset);
				}	
				break;
			case SDL_MOUSEBUTTONUP:
				camera.motion = false;
				SDL_SetRelativeMouseMode(SDL_FALSE);
				break;
			case SDL_MOUSEWHEEL:
				
				break;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym)
				{
				case SDLK_1:
				{
					if (renderer.sampleIndex != 0) {
						renderer.sampleIndex = 0;
					}
					else {
						renderer.sampleIndex = 1;
					}
					break;
				}
				case SDLK_ESCAPE:
					renderer.isRunning = false;
					break;
				}
				break;
			
			}
		}
		const Uint8* state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_W]) {
			camera.ProcessKeyboard(FORWARD);
		}
		if (state[SDL_SCANCODE_S]) {
			camera.ProcessKeyboard(BACKWARD);
		}
		if (state[SDL_SCANCODE_A]) {
			camera.ProcessKeyboard(LEFT);
		}
		if (state[SDL_SCANCODE_D]) {
			camera.ProcessKeyboard(RIGHT);
		}
		if (state[SDL_SCANCODE_SPACE]) {
			camera.ProcessKeyboard(UP);
		}
	}

	inline void simpleEvent(Renderer &renderer)
	{
		/*bool firstClick = true;*/
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				renderer.isRunning = false;
				break;
			
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					renderer.isRunning = false;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}
	}
}

namespace Window
{
	inline void initWindow(SDL_Window *&window, SDL_GLContext &context, int width, int height, int swapInterval = 1)
	{
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

		window = SDL_CreateWindow("FBX - OpenGL AnimationSample",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			width,
			height,
			SDL_WINDOW_OPENGL);
		

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		context = SDL_GL_CreateContext(window);
		SDL_GL_SetSwapInterval(swapInterval);
		SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE);
		glewExperimental = GL_TRUE;
		
		if (glewInit() != GLEW_OK) {
			LOG_ASSERT("failed to init glew");
		}
		else {
			LOG << "Graphic Device : " << glGetString(GL_RENDERER) << ENDN;
			LOG << "GL Version     : " << glGetString(GL_VERSION) << ENDN;
			LOG << "GLEW Version   : " << glewGetString(GLEW_VERSION) << ENDN;

			/*int extensions;
			glGetIntegerv(GL_NUM_EXTENSIONS, &extensions);
			for (int i = 0; i < extensions; ++i)
			{
				LOG << glGetStringi(GL_EXTENSIONS, i) << ENDL;
			}*/
		}

		if (GLEW_KHR_debug) 
		{
			LOG << "Enable KHR OpenGL Debug" << ENDL;
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(Log::glDebugCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_ERROR,
				GL_DONT_CARE, 0, nullptr, GL_TRUE);
			/*glDebugMessageControl(
				GL_DEBUG_SOURCE_API,
				GL_DEBUG_TYPE_ERROR,
				GL_DEBUG_SEVERITY_HIGH,
				0, nullptr, GL_TRUE);*/
		}
	
		glViewport(0, 0, width, height);
		glEnable(GL_DEPTH_TEST);

		glClearColor(0.09f, 0.09f, 0.1f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SDL_GL_SwapWindow(window);
	}

	inline void release(SDL_Window *&window, SDL_GLContext &context)
	{
		SDL_DestroyWindow(window);
		SDL_GL_DeleteContext(context);
		SDL_Quit();
	}

	inline void setConsoleOutput(int x, int y, int w, int h)
	{
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		FILE *stream;
		freopen_s(&stream, "CONOUT$", "w", stdout);
		HWND console = GetConsoleWindow();
		MoveWindow(console, x, y, w, h, TRUE);
	}
}
