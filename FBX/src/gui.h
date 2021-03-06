#pragma once

#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>
#include <string>
#include <stdlib.h>
#include <vector>

class SDL_Window;
class AnimHandle;
class AnimSample;
class Node;
class BoneNode;
class MeshNode;
class AnimModel;

struct Widget
{
	Widget() : isOpen(true),name("untitled"){}
	bool isOpen;
	std::string name;
};

class Gui
{
public:
	Gui(SDL_Window* window);
	~Gui();

	void initialize();
	void process(AnimModel* model);
	void render();
private:
	SDL_Window* mWindow;
	ImGuiIO* mIO;
	Widget m_widget;
};

namespace SubGui
{
	//Node
	void evalBoneTree(BoneNode* boneNode);		/**Create Bone Node*/
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
