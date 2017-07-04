#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdlib.h>
#include <vector>
#include <memory>

//DEFINES
#define BONE_MAX				64
#define BONE_COMPONENT_NUM		4
#define FACE_COMPONENT_NUM		3
#define DEFAULT_FPS				30

#define SAFE_DELETE(x)			{if(x){delete (x); (x) = NULL;}}
#define SAFE_DELETE_ARRAY(x)	{if(x){delete[] (x); (x) = NULL;}}

class Node;
typedef std::shared_ptr<Node> node_ptr;

//fbx
class FBXDevice;
typedef std::shared_ptr<FBXDevice> fbxdevice_ptr;
class AnimLayer;
typedef std::shared_ptr<AnimLayer> animlayer_ptr;

//SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <log.h>

#include <keyhandler.h>
#include <eventhandler.h>