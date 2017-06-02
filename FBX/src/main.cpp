
#include <util.h>
#include <animmodel.h>
#include <shadertool.h>
#include <gui.h>

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
	Gui gui(window);
	Timer timer;
	Camera camera;
	camera.setPerspective(45.f, WIDTH / (float)HEIGHT, 0.0001f, 1000.0f);

	//FBX
	GLuint shader = LOAD_SHADER("shader.vert", "shader.frag");
	
	AnimModel animModel("worrior.fbx");
	//AnimModel animModel("plane.fbx");
	AnimationInfo info;
	
	while (renderer.isRunning)
	{
		timer.begin();
		camera.delta = timer.delta;
		Input::event(renderer, camera);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*ANIMATION MANAGER*/
		animModel.processAnimation();

		glUseProgram(shader);
		ShaderTool::setUniformMatrix4f(shader, camera.proj(), "proj", true);
		ShaderTool::setUniformMatrix4f(shader, camera.view(), "view", true);
		Matrix4x4 trans;
		//trans.scale(0.5);
		ShaderTool::setUniformMatrix4f(shader, trans, "model", true);
		glUseProgram(0);
		
		
		animModel.render(shader);

		
		//info.pNode = animModel.getNode();
		//info.pHandle = animModel.getHandle();

		gui.process(&animModel);
		gui.render();
		
		timer.end(window);
	}

	Window::release(window, context);
	
	return 0;
}

#else

struct SomeSample
{
	//i need that char** array of name 
	char* name;
	int index;
	std::string somedata;
	float length;
	//many more members.....
};


int main(int argc, char* argv[])
{
	Window::setConsoleOutput(10, 10, 700, 800);
	
	std::vector<SomeSample> samples;

	for (int i = 0; i < 3; ++i)
	{
		SomeSample sample;
		sample.name = "abc";
		sample.index = i;
		samples.push_back(sample);
	}
	//added some datas 

	//how to get char data array
	/*i want char* name array from SomeSample arrays*/
	char** names = samples.data();

	system("pause");
	return 0;
}

#endif // TAKE0


