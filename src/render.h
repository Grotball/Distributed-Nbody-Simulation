#pragma once

#ifdef USE_GLES
#define ENABLE_OPENGL
#endif

#ifdef ENABLE_OPENGL

#include <iostream>
#define GLFW_INCLUDE_NONE
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <cmath>
#include "camera.h"


void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* glInit(bool& success);

unsigned int createShader(const std::filesystem::path& vertPath, const std::filesystem::path& fragPath);

#endif