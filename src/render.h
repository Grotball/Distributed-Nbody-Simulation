#pragma once

#include <iostream>
#define GLFW_INCLUDE_NONE
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <filesystem>
#include <fstream>


void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* glInit(bool& success);

unsigned int createShader(const std::filesystem::path& vertPath, const std::filesystem::path& fragPath);