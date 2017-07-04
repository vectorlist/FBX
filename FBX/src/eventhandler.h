#pragma once

class Renderer;
typedef void(*EventFunc)(Renderer* renderer);
typedef void(*EventKeyFunc)(void);

class EventHandler
{
public:

	static void Event(Renderer* renderer);
	static void DefaultEvent(Renderer* renderer);
	static void SetCallBackFunction(EventFunc func);
	static void SetNativeWindow(HWND hwnd);

	//Static Callback Event Func
	static EventFunc			CallBack;
	static EventKeyFunc			KeyCallBack;
	static HWND					NativeWindow;
};