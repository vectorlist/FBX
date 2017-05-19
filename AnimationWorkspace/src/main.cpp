
#include <util.h>
#include <animationrenderer.h>

#define WIDTH		480
#define HEIGHT		360

void subEvent(AnimationRenderer &renderer);
int main(int argc, char* argv[])
{
	Window::setConsoleOutput(10, 10, 700, 900);
	SDL_Window *window;
	SDL_GLContext context;
	Window::initWindow(window, context, WIDTH, HEIGHT, 1);
	

	Renderer renderer;
	Timer timer;
	AnimationRenderer animRenderer;
	while (renderer.isRunning)
	{
		timer.begin();
		Input::simpleEvent(renderer);

		subEvent(animRenderer);
		animRenderer.playAnimation();

		timer.end(window);
	}

	Window::release(window, context);
	return 0;
}

void subEvent(AnimationRenderer &renderer)
{
	const Uint8* state = SDL_GetKeyboardState(NULL);
	
	if (state[SDL_SCANCODE_1]) {
		
		renderer.changeSampleID();
		Sleep(100);
	}
}