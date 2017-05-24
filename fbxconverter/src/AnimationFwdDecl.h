#pragma once

#include <fbxsdk.h>
#include <boost/shared_ptr.hpp>
#include <vector>


class AnimationController;
class VectorKey;
class QuaternionKey;

typedef std::vector<QuaternionKey> QuaternionKeyArray;
typedef std::vector<VectorKey> VectorKeyArray;

typedef boost::shared_ptr<AnimationController> AnimationControllerPtr;
