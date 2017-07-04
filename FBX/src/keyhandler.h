#pragma once

#define KEY_A	'A'
#define KEY_B	'B'
#define KEY_C	'C'
#define KEY_D	'D'
#define KEY_E	'E'
#define KEY_F	'F'
#define KEY_G	'G'
#define KEY_H	'H'
#define KEY_I	'I'
#define KEY_K	'K'
#define KEY_L	'L'
#define KEY_M	'M'
#define KEY_N	'N'
#define KEY_O	'O'
#define KEY_P	'P'
#define KEY_Q	'Q'
#define KEY_R	'R'
#define KEY_S	'S'
#define KEY_T	'T'
#define KEY_U	'U'
#define KEY_V	'V'
#define KEY_W	'W'
#define KEY_X	'X'
#define KEY_Y	'Y'
#define KEY_Z	'Z'

#define KEY_SPACE		VK_SPACE
#define KEY_LBUTTON		VK_LBUTTON
#define KEY_RBUTTON		VK_RBUTTON
#define KEY_SHIFT		VK_SHIFT
#define KEY_LSHIFT		VK_LSHIFT
#define KEY_RSHIFT		VK_RSHIFT

#define KEY_NUM_MAX		256

//TODO Singleton template class

class KeyEventHandler
{
public:
	static bool isKeyPress(int key);
	static bool isKeyDown(int key);
	static bool isKeyUp(int key);

private:
	static bool mDownKeys[KEY_NUM_MAX];
	static bool mUpKeys[KEY_NUM_MAX];
};

typedef KeyEventHandler KeyHandler;