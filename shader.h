#pragma once

#include <glad/glad.h>

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

GLuint MakeShader(GLuint t, std::string path);
GLuint AttachAndLink(std::vector<GLuint> shaders);