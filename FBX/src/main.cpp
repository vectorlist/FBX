#include <config.h>
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

	//Get Native Window Info
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(window, &info);

	HWND handle = info.info.win.window;
	EventHandler::SetNativeWindow(handle);

	Renderer renderer;
	Gui gui(window);
	Timer timer;
	Camera camera;
	camera.setPerspective(45.f, WIDTH / (float)HEIGHT, 0.0001f, 1000.0f);
	camera.setLookAt(vec3f(0, 3, 10), vec3f(0,2,0), vec3f(0, 1, 0));
	//FBX
	GLuint shader = LOAD_SHADER("shader.vert", "shader.frag");
	
	//AnimModel animModel("demon.fbx");
	//AnimModel animModel("demon_idle.fbx");
	//AnimModel animModel("demon_walk.fbx");
	AnimModel animModel("nd.fbx");

	
	while (renderer.isRunning)
	{
		timer.begin();
		camera.delta = timer.delta;
		//Input::event(renderer, camera);
		EventHandler::Event(&renderer);
		
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

#include <vec3f.h>
struct SomeMatrix
{
	SomeMatrix(){}
	union
	{
		struct 
		{
			float x, y, z;
		};
		struct
		{
			float data[3];
		};
	};
};


int main(int argc, char* argv[])
{
	Window::setConsoleOutput(10, 10, 700, 800);
	
	
	SomeMatrix mat;

	mat.x = 100;
	mat.y = 200;
	LOG << mat.data[0] << " " << mat.data[1] << ENDN;
	

	system("pause");
	return 0;
}

#endif // TAKE0


