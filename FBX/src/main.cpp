
#include <util.h>
#include <fbxmodel.h>
#include <shadertool.h>
#include <animationrenderer.h>

#define WIDTH		720
#define HEIGHT		640

#define TAKE0

#ifdef TAKE0

int main(int argc, char* argv[])
{
	Window::setConsoleOutput(10, 10, 800, 800);
	SDL_Window* window;
	SDL_GLContext context;
	Window::initWindow(window, context, WIDTH, HEIGHT, 20);
	Renderer renderer;
	Timer timer;
	Camera camera;
	camera.setPerspective(45.f, WIDTH / (float)HEIGHT, 0.0001f, 1000.0f);

	//FBX
	GLuint shader = LOAD_SHADER("shader.vert", "shader.frag");
	FBXModel model("ani.fbx", shader);

	//AnimationSample(TEST)
	AnimationRenderer animRenderer;
	animRenderer.setRootNode(&model.mNode);
	animRenderer.setRenderableMesh(&model.m_mesh);
	
	while (renderer.isRunning)
	{
		timer.begin();
		camera.delta = timer.delta;
		Input::event(renderer, camera);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*ANIMATION MANAGER*/
		animRenderer.startAnimation();

		glUseProgram(shader);
		ShaderTool::setUniformMatrix4f(shader, camera.proj(), "proj", true);
		ShaderTool::setUniformMatrix4f(shader, camera.view(), "view", true);
		Matrix4x4 trans;
		trans.scale(0.1);
		ShaderTool::setUniformMatrix4f(shader, trans, "model", true);
		glUseProgram(0);
	
		animRenderer.render(shader);
	
		timer.end(window);
	}

	Window::release(window, context);
	
	return 0;
}

#else

#include <fbxtool.h>
#include <chrono>



int main(int argc, char* argv[])
{
	Window::setConsoleOutput(10, 10, 700, 800);
	

	system("pause");
	return 0;
}

#endif // TAKE0

