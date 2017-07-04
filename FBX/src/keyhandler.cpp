
#include <config.h>
#include <keyhandler.h>

//above c11++
bool KeyEventHandler::mDownKeys[KEY_NUM_MAX] = {0};
bool KeyEventHandler::mUpKeys[KEY_NUM_MAX] = {0};

/*Keep Pressed Key
*/
bool KeyEventHandler::isKeyPress(int key)
{
	return (GetAsyncKeyState(key) & 0x8000);
}

/*Presse Key at once
*/
bool KeyEventHandler::isKeyDown(int key)
{
	if (GetAsyncKeyState(key) & 0x8000) {
		//if down key member are false
		if (!mDownKeys[key]) {
			mDownKeys[key] = true;
			return true;
		}
	}
	else {
		mDownKeys[key] = false;
	}
	return false;
}

/*Key Up at once
*/
bool KeyEventHandler::isKeyUp(int key)
{
	if (GetAsyncKeyState(key) & 0x8000)
	{
		if (!mUpKeys[key]) {
			mUpKeys[key] = true;
		}
	}
	else {
		if (mUpKeys[key]) {
			mUpKeys[key] = false;
			return true;
		}
	}
	return false;
}
