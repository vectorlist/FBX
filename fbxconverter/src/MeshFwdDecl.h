#pragma once

#include <memory>
#include <vector>
#include <fbxsdk.h>
#include <cmath>

#include <boost/shared_array.hpp>

#define MAX_BONES 64
#define MAX_INFLUENCES 4 


class Mesh;
class AnimationInfo;

class Vertex;
class Face;

class BoneNode;
class MeshNode;

//typedef boost::shared_array<Vertex> MeshVertexArrayPtr;
//typedef boost::shared_array<Face> MeshTriangleArrayPtr;

typedef boost::shared_array<Vertex> MeshVertexArrayPtr;
typedef boost::shared_array<Face> MeshTriangleArrayPtr;

typedef boost::shared_ptr<Mesh> MeshPtr;
typedef boost::shared_ptr<AnimationInfo> AnimationInfoPtr;
