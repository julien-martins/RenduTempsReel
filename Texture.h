#pragma once

#include <string>
#include <glad/glad.h>

class Texture
{

	public:
		Texture(const std::string& file_name);
		~Texture();

		void bind() const;

		GLuint get_texture() const { return texture_; };

	private:
		GLuint texture_;
};

