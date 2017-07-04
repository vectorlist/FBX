#include <config.h>
#include <eventhandler.h>
#include <renderer.h>

//Most important handler for esy to use native window function everywhere
HWND						EventHandler::NativeWindow = NULL;
EventFunc					EventHandler::CallBack = DefaultEvent;
EventKeyFunc				EventHandler::KeyCallBack = NULL;

void EventHandler::Event(Renderer *renderer)
{
	if (CallBack) {
		CallBack(renderer);
	}
	if (KeyCallBack) {
		KeyCallBack();
	}

}

void EventHandler::DefaultEvent(Renderer *renderer)
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			renderer->isRunning = false;
			break;
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_ESCAPE) {
				renderer->isRunning = false;
			}
		default:
			break;
		}
	}
}

void EventHandler::SetCallBackFunction(EventFunc func)
{
	CallBack = func;
}

void EventHandler::SetNativeWindow(HWND hwnd)
{
	NativeWindow = hwnd;
}
