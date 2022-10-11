#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <vector>

void load_obj(const char* fileName, std::vector<glm::vec4>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, std::vector<GLushort>& elements);

bool loadPngImage(char* name, int& outWidth, int& outHeight, bool& outHasAlpha, GLubyte** outData);