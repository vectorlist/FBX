
#include <util.h>
#include <animmodel.h>
#include <shadertool.h>

#define WIDTH		1280
#define HEIGHT		720

#define TAKE0

#ifdef TAKE0

int main(int argc, char* argv[])
{
	Window::setConsoleOutput(10, 10, 800, 800);
	SDL_Window* window;
	SDL_GLContext context;
	Window::initWindow(window, context, WIDTH, HEIGHT, 1);
	Renderer renderer;
	Timer timer;
	Camera camera;
	camera.setPerspective(45.f, WIDTH / (float)HEIGHT, 0.0001f, 1000.0f);

	//FBX
	GLuint shader = LOAD_SHADER("shader.vert", "shader.frag");
	
	//AnimModel animModel("worrior_idle.fbx");
	//AnimModel animModel("dia.fbx");
	AnimModel animModel("plane.fbx");
	
	//animRenderer.setRenderableMesh(&model.mMesh);
	
	while (renderer.isRunning)
	{
		timer.begin();
		camera.delta = timer.delta;
		Input::event(renderer, camera);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*ANIMATION MANAGER*/
		animModel.processAnimation(renderer.sampleIndex);

		glUseProgram(shader);
		ShaderTool::setUniformMatrix4f(shader, camera.proj(), "proj", true);
		ShaderTool::setUniformMatrix4f(shader, camera.view(), "view", true);
		Matrix4x4 trans;
		//trans.scale(0.5);
		ShaderTool::setUniformMatrix4f(shader, trans, "model", true);
		glUseProgram(0);
	
		animModel.render(shader);
	
		timer.end(window);
	}

	Window::release(window, context);
	
	return 0;
}

#else

#include <fbxtool.h>
#include <chrono>
struct martices
{
	Matrix4x4 mat0[10];
	Matrix4x4 mat1;
};

template <class T>
class SomeUBO
{
public:
	SomeUBO() { init(); };
	void init() {
		datasize = sizeof(T);
	}
	int datasize;
};

int main(int argc, char* argv[])
{
	Window::setConsoleOutput(10, 10, 700, 800);
	
	//martices mats;
	std::vector<martices> mats;
	for (int i; i < 10; ++i)
		mats.push_back(martices());

	for (auto i : mats)
		LOG << i << ENDN;

	system("pause");
	return 0;
}

#endif // TAKE0


