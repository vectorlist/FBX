
#include <util.h>
#include <fbxmodel.h>
#include <shadertool.h>
#define WIDTH		1024
#define HEIGHT		720

#define TAKE0

#ifdef TAKE0

int main(int argc, char* argv[])
{
	Window::setConsoleOutput(10, 10, 600, 800);
	SDL_Window* window;
	SDL_GLContext context;
	Window::initWindow(window, context, WIDTH, HEIGHT);
	Renderer renderer;
	Timer timer;
	Camera camera;
	camera.setPerspective(45.f, WIDTH / (float)HEIGHT, 0.0001f, 1000.0f);

	//FBX
	GLuint shader = LOAD_SHADER("shader.vert", "shader.frag");
	FBXModel model("ani.fbx", shader);
	
	while (renderer.isRunning)
	{
		timer.begin();
		camera.delta = timer.delta;
		Input::event(renderer, camera);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader);
		ShaderTool::setUniformMatrix4f(shader, camera.proj(), "proj", true);
		ShaderTool::setUniformMatrix4f(shader, camera.view(), "view", true);
		Matrix4x4 trans;
		trans.scale(0.1);
		ShaderTool::setUniformMatrix4f(shader, trans, "model", true);
		glUseProgram(0);
		model.render(shader);
		//model.m_mesh.renderBoundingBox();
		timer.end(window);
	}

	Window::release(window, context);
	
	return 0;
}

#else

#include <Logger.h>

int main(int argc, char* argv[])
{
	std::cout << "awdda" << std::endl;
	LOG_W << "hello" << "logical" << ENDN;
	LOG_W << vec3f(1, 2, 3) << ENDN << Matrix4x4() <<ENDN;
	//system("pause");
	return 0;
}

#endif // TAKE0