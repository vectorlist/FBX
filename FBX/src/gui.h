#pragma once

#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>
#include <string>
#include <stdlib.h>
#include <vector>

namespace UI = ImGui;

//TODO get Animation Layers
class AnimHandle;
class AnimSample;
class Node;
class BoneNode;
class MeshNode;
class AnimModel;
struct AnimationInfo
{
	AnimationInfo() : pHandle(NULL), pNode(NULL){}
	AnimHandle* pHandle;
	Node* pNode;
};

struct Widget
{
	Widget() : isOpen(true),name("untitled"){}
	bool isOpen;
	std::string name;
};

class SDL_Window;
class Gui
{
public:
	Gui(SDL_Window* window);
	~Gui();

	void initialize();
	void process(AnimModel* pModel);
	void render();
	
	
private:
	SDL_Window* m_window;
	ImGuiIO* m_io;

	Widget m_widget;
};

namespace SubGui
{
	//Node
	void evalBoneTree(BoneNode* boneNode);
	void evalMeshTree(MeshNode* meshNode);
	void evalMeshNodeTree(MeshNode* meshnode);
	void evalBoneNodeTree(BoneNode* boneNode);
	//Animation
	void evalAnimSamples(AnimModel* model);
	void evalAnimSampleInfo(AnimSample* sample);
	void evalAnimHandle(AnimHandle* handle);

	template<typename ... Args>
	void evalText(const std::string& code, Args ... args);
}

struct SampleItem
{
	SampleItem() : index(0){}
	int index;
	std::vector<const char*> name;
};

