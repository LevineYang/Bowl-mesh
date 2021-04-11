#include "include/sample.hpp"
#include <iostream>
#include <thread>
#include <chrono>


using namespace std::chrono_literals;

static GLFWwindow* wnd;

using func_ptr = void (*)();
static func_ptr onEnd = nullptr;


Shader SVshader;
Camera cam_(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 1.f, 0.f));
float aspect_ratio = SRC_WIDTH / SRC_HEIGHT;
float deltaTime = 0.f;
float lastFrame = 0.f;
float lastX = SRC_WIDTH / 2.f; // last x pos cursor
float lastY = SRC_HEIGHT / 2.f; // last y pos cursor
bool firstMouse = true;


#ifdef GAMMA
extern bool gammaEnabled;
bool gammaKeyPressed = false;
#endif




int main(int argc, char* argv[])
{
	wnd = projectInit("Bowl");

	renderLoop(wnd);

	glfwTerminate();

	return 0;
}



void frame_buffer_size_callback(GLFWwindow* wnd, int width, int height)
{
	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	const float cameraSpeed = 2.5f * deltaTime; //2.5f
	glm::vec3 camPos = cam_.getCamPos();
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cam_.processKeyboard(Camera_Movement::FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cam_.processKeyboard(Camera_Movement::BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cam_.processKeyboard(Camera_Movement::LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cam_.processKeyboard(Camera_Movement::RIGHT, deltaTime);
	}


#ifdef GAMMA
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !gammaKeyPressed) {
		gammaEnabled = !gammaEnabled;
		gammaKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
		gammaKeyPressed = false;
#endif
}



void processMouse(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed: y ranges bottom to top
	lastX = xpos;
	lastY = ypos;

	cam_.processMouseMovement(xoffset, yoffset);
}

void processScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	cam_.processMouseScroll(yoffset);
}


static void glewProjectInit()
{
	glewExperimental = GL_TRUE; // for use graphic drivers function
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Error: " << glewGetErrorString(err) << "\n";
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
}


GLFWwindow* projectInit(const std::string& wndname, int width, int height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_SAMPLES, 4); // for MSAA
	GLFWwindow* window = glfwCreateWindow(width, height, wndname.c_str(), NULL, NULL);
	// GLFWwindow* window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, wndname.c_str(), NULL, NULL);
	if (window == nullptr) {
		std::cerr << "Failed create GLFW window\n";
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	glewProjectInit();

	glViewport(0, 0, width, height);
	// glViewport(0, 0, SRC_WIDTH, SRC_HEIGHT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))


	// reszie callback
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);

	// hide cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// set mouse pos callback
	glfwSetCursorPosCallback(window, processMouse);
	// set mouse scroll callback
	glfwSetScrollCallback(window, processScroll);

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind default framebuffer
	SVshader.initShader("shaders/svvert.glsl", "shaders/svfrag.glsl");
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	onEnd = destroyShapes;

	return window;

}


void renderLoop(GLFWwindow* window)
{
	// render loop
	while (!glfwWindowShouldClose(window)) {
		// input 
		processInput(window);

		// render command
		// ...
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //  | GL_STENCIL_BUFFER_BIT
		displayDEMO();


		// check and call events and swap the buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		std::this_thread::sleep_for(10ms);
	}

	if (onEnd)
		onEnd();

}
