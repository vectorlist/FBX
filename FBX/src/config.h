#pragma once

#include <stdlib.h>
#include <vector>
#include <memory>

#define BONE_MAX				64
#define BONE_COMPONENT_NUM		4
#define FACE_COMPONENT_NUM		3
#define DEFAULT_FPS				30

class AnimationLayers;
class LayerInfo;
typedef std::shared_ptr<AnimationLayers> AnimationLayersPtr;
