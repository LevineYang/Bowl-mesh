#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <string>

#include "shaders.h"
#include "camera.h"


using uint = unsigned int;
using uchar = unsigned char;
constexpr uint SRC_WIDTH = 1280;
constexpr uint SRC_HEIGHT = 720;
constexpr uint num_ch = 3;


void frame_buffer_size_callback(GLFWwindow* wnd, int width, int height);


GLFWwindow* projectInit(const std::string& wndname, int width = SRC_WIDTH, int height = SRC_HEIGHT);
void renderLoop(GLFWwindow* window);
void processInput(GLFWwindow* window);

void displayDEMO(GLFWwindow* window = nullptr);
void destroyShapes();
void saveScreenshot(const std::string& fname);