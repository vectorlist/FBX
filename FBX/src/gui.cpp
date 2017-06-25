#include "gui.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <log.h>
#include <node.h>
#include <animhandle.h>
#include <animsample.h>
#include <animlayer.h>
#include <animmodel.h>

#define COLOR_WINDOW	ImVec4(0.4,0.42,0.44,1);
#define COLOR_FRAME		ImVec4(0.2,0.2,0.2,1);
#define COLOR_BG		ImVec4(0.22,0.25,0.27,1);
#define COLOR_LBG		ImVec4(0.1, 0.43, 0.73, 1);
#define COLOR_LBGD		ImVec4(0.07, 0.3, 0.53, 1);			//disabled active light bs color


Gui::Gui(SDL_Window* window) 
	: mWindow(window)
{

	ImGui_ImplSdlGL3_Init(mWindow);
	initialize();
}


Gui::~Gui()
{
	ImGui_ImplSdlGL3_Shutdown();
}

void Gui::initialize()
{
	//WIDGET
	m_widget.isOpen = true;
	m_widget.name = "FilmBox Animation";

	//FONT
	mIO = &ImGui::GetIO();
	mIO->Fonts->AddFontFromFileTTF("../data/font/consola.ttf", 13.5);

	//STYLE
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding = ImVec2(15, 15);
	style.WindowRounding = 0.0f;
	style.FramePadding = ImVec2(5, 5);
	style.FrameRounding = 1.5f;
	style.ItemSpacing = ImVec2(12, 8);
	style.ItemInnerSpacing = ImVec2(8, 6);
	style.IndentSpacing = 25.0f;
	style.ScrollbarSize = 10.0f;
	style.ScrollbarRounding = 1.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 1.0f;
	style.ChildWindowRounding = 1.0f;
	
	style.Colors[ImGuiCol_WindowBg] = COLOR_WINDOW;
	style.Colors[ImGuiCol_TitleBg] = COLOR_LBG;
	style.Colors[ImGuiCol_TitleBgActive] = COLOR_LBG;
	style.Colors[ImGuiCol_TitleBgCollapsed] = COLOR_LBG;
	style.Colors[ImGuiCol_FrameBg] = COLOR_FRAME;
	style.Colors[ImGuiCol_FrameBgActive] = COLOR_FRAME;
	style.Colors[ImGuiCol_FrameBgHovered] = COLOR_FRAME;
	style.Colors[ImGuiCol_Header] = COLOR_LBG;
	style.Colors[ImGuiCol_HeaderActive] = COLOR_LBG;
	style.Colors[ImGuiCol_HeaderHovered] = COLOR_LBG;

	style.Colors[ImGuiCol_ResizeGrip] = COLOR_LBG;
	
}

void Gui::process(AnimModel* model)
{
	SDL_Event e;
	ImGui_ImplSdlGL3_ProcessEvent(&e);
	ImGui_ImplSdlGL3_NewFrame(mWindow);

	//WIDGET
	ImGui::Begin(m_widget.name.c_str(), &m_widget.isOpen, ImGuiWindowFlags_Modal);
	ImGui::SetWindowSize(ImVec2(380, 700));

	auto node = model->getNode();
	auto handle = model->getHandle();
	
	//NODE
	if (ImGui::CollapsingHeader("Node", ImGuiTreeNodeFlags_DefaultOpen))
	{
		SubGui::evalMeshTree(node->getMeshNodeRoot());
		SubGui::evalBoneTree(node->getBoneNodeRoot());
	}
	
	//ANIMATION
	if (ImGui::CollapsingHeader("Animation Layer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		SubGui::evalAnimSamples(model);
		SubGui::evalAnimHandle(handle);
	}
	

	ImGui::CollapsingHeader("node");
	ImGui::End();
}

void Gui::render()
{
	glViewport(0, 0, (int)mIO->DisplaySize.x, (int)(mIO->DisplaySize.y));
	glClearColor(0.2,0.2,0.2,1);
	//glClear(GL_COLOR_BUFFER_BIT);
	ImGui::Render();
}

//SUBGUI

namespace SubGui
{
	void evalBoneTree(BoneNode* boneNode)
		/**Create Bone Node*/
	{
		if (ImGui::TreeNode("Bone Node"))
		{
			evalBoneNodeTree(boneNode);
			ImGui::TreePop();
		}
	}

	void evalMeshTree(MeshNode* meshNode)
	{
		if (ImGui::TreeNode("Mesh Node"))
		{
			evalMeshNodeTree(meshNode);
			ImGui::TreePop();
		}
	}

	void evalMeshNodeTree(MeshNode* meshNode)
	{
		for (MeshNode* node = meshNode; node != NULL; node = node->mNext)
		{
			const char* nodeName = node->getName().c_str();
			std::string bits;
			int faceNum = node->getFaces().size();
			int pointNum = node->getPoints().size();
					
			if (ImGui::TreeNode(nodeName)) {

				bits = STR_FORMAT("faces : %d points : %d", faceNum, pointNum);
				ImGui::SameLine();
				ImGui::Text(bits.c_str());
				
				if (node->mFirstChild)
				{
					evalMeshNodeTree(node->mFirstChild);
				}

				ImGui::TreePop();
			}
		}
	}

	void evalBoneNodeTree(BoneNode* boneNode)
	{
		for (BoneNode* node = boneNode; node != NULL; node = node->mNext)
		{
			const char* nodeName = node->getName().c_str();
			
			if (ImGui::TreeNode(nodeName)) {
				if (node->mFirstChild)
				{
					evalBoneNodeTree(node->mFirstChild);
				}

				ImGui::TreePop();
			}
		}
	}

	void evalAnimSamples(AnimModel* model)
	{
		//ImGui::Separator();
		Node* node = model->getNode();
		AnimLayer* layer = node->getAnimationLayer();
	
		if(ImGui::Combo("Samples", &layer->mIndex, layer->GetSamplesNames().data(),
			layer->mNumSamples))
		{
			//TODO : replace to more simple callback function
			LOG << layer->mIndex << ENDN;
			node->setCurrentSample(layer->getSample(layer->mIndex));
			model->hasChanged = true;
		}
		//get info
		AnimSample* currentSample = node->getCurrentSample();
		if (!currentSample) return;
		ImGui::Separator();
		evalAnimSampleInfo(currentSample);
		ImGui::Separator();
	}

	void evalAnimSampleInfo(AnimSample *sample)
	{
		SubGui::evalText("Current Sample : " + sample->mName + " (%.1f fps)", sample->mFps);
		ImGui::Text("Frame Start    : %d", sample->mFrameStart);
		ImGui::Text("Frame End      : %d", sample->mFrameEnd);
		ImGui::Text("Frame Length   : %d", sample->mFrameCount);
	}

	void evalAnimHandle(AnimHandle* handle)
	{
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
		
		/*ImGui::Text("Local Time : %d", handle->getLocalTime());
		ImGui::Text("Local Frame : %d",handle->getLocalFrame());
		ImGui::Text("Global Frame : %d", handle->getGlobalFrame());*/

		ImGui::PopItemWidth();
	}

	template<typename ... Args>
	void evalText(const std::string &code, Args ... args)
	{
		ImGui::Text(code.c_str(), args ...);
	}

}


// i have 3-4 renderer for entity, frame buffer object, terrain, water.. animation model... etc

//each render has vector of entity(200-300 models more)

// i want to render for insight(whithin 50m (30-40 models)) only

// so i need to batch them (FIFO, QUEUE...etc) each frame

//so convert to 300-400 model to 30-40 lol

