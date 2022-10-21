#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

#include "Texture.h"

struct vertex {
	glm::vec3 position, normal;
	glm::vec2 uv;
};

struct mesh {
	std::vector<vertex> vertices;
	/*std::vector<texture> textures;*/
	/*std::vector<int> indices; */
};

class model
{
	public:
		model();
		~model();

		void reset();
		void translate(float x, float y, float z);
		void scale(float x, float y, float z);
		void rotate(float angle, float x, float y, float z);

		void load_obj(const char* file_name);

		void show_all_vertices();

		void add_texture(const std::string path);
		void add_texture(Texture& texture);

		void setup();
		void draw();

		std::vector < std::vector < glm::vec3 >> return_vertices();

		std::vector<std::unique_ptr<Texture>> textures_;

		std::vector<mesh> meshes_;
		std::vector<GLuint> vbo_;

		glm::mat4 modelMat;
	private:
};

