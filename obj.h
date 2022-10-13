#pragma once

#include <glm/glm.hpp>

#include <vector>

void load_obj(const char* fileName, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals);