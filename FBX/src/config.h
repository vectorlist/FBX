#pragma once

#include <stdlib.h>
#include <vector>
#include <memory>

#define BONE_MAX				64
#define BONE_COMPONENT_NUM		4
#define FACE_COMPONENT_NUM		3
#define DEFAULT_FPS				30

class Node;
typedef std::shared_ptr<Node> node_ptr;

//fbx
class FBXDevice;
typedef std::shared_ptr<FBXDevice> fbxdevice_ptr;
class AnimLayer;
typedef std::shared_ptr<AnimLayer> animlayer_ptr;