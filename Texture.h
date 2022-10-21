#pragma once

#include <string>
#include <glad/glad.h>

class Texture
{

	public:
		Texture(const std::string& file_name);
		Texture(int width, int height);
		~Texture();

		Texture& operator=(const Texture& other)
		{
			texture_ = other.texture_;
			return *this;
		}

		void bind() const;

		GLuint get_texture() const { return texture_; };

	private:
		GLuint texture_;
};

