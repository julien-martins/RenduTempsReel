#pragma once

#include <string>
#include <glad/glad.h>

#include "stb_image.h"

class Texture
{

	public:
		Texture(const std::string& file_name);
		~Texture();

		void bind() const;

	private:
		GLuint texture_;
};

