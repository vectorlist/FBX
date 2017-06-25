
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
	
	//AnimModel animModel("demon.fbx");
	AnimModel animModel("demon_idle.fbx");
	//AnimModel animModel("plane.fbx");
	//AnimModel animModel("worrior_idle.fbx");
	
	while (renderer.isRunning)
	{
		timer.begin();
		camera.delta = timer.delta;
		Input::event(renderer, camera);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*ANIMATION MANAGER*/
		animModel.Update(timer.delta);

		glUseProgram(shader);
		ShaderTool::setUniformMatrix4f(shader, camera.proj(), "proj", true);
		ShaderTool::setUniformMatrix4f(shader, camera.view(), "view", true);
		Matrix4x4 trans;
		//trans.scale(0.5);
		ShaderTool::setUniformMatrix4f(shader, trans, "model", true);
		glUseProgram(0);
		
		
		animModel.render(shader);

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

};


int main(int argc, char* argv[])
{
	Window::setConsoleOutput(10, 10, 700, 800);
	
	float start = 0.02f;
	float end = 0.08f;

	float global = 0.0f;
	float current = 0.0f;
	for (global; global < 10.f; global += 0.01f) {
		Sleep(100);
		current = fmod(global, end) + start;
		LOG << current << ENDN;
	}

	

	system("pause");
	return 0;
}

#endif // TAKE0


