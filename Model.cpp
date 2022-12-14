#include "Model.h"

model::model()
{
	modelMat = glm::mat4(1.0f);
}

model::~model()
{
	for (const auto& vbo : vbo_)
	{
		glDeleteBuffers(1, &vbo);
	}
}


void model::load_obj(const char* file_name)
{
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	std::string line;
	std::ifstream file(file_name);

	mesh current_object = mesh();
	bool first_object = true;

	if(file.is_open())
	{
		while(std::getline(file, line))
		{
			std::string header = line.substr(0, 2);
			
			if(header == "o ") //suboject
			{
				if(first_object)
				{
					first_object = false;
					continue;
				}

				//new Subobject
				std::vector<vertex> vertices = std::vector<vertex>(vertexIndices.size());
				
				for (unsigned int i = 0; i < vertexIndices.size(); i++) {
					unsigned int vertexIndex = vertexIndices[i];
					vertices[i].position = temp_vertices[vertexIndex - 1];
				}

				for (unsigned int i = 0; i < uvIndices.size(); i++) {
					unsigned int uvIndex = uvIndices[i];
					vertices[i].uv = temp_uvs[uvIndex - 1];
				}
				
				for (unsigned int i = 0; i < normalIndices.size(); i++) {
					unsigned int normalIndex = normalIndices[i];
					vertices[i].normal = temp_normals[normalIndex - 1];
				}
				


				current_object.vertices = vertices;
				//add the mesh and reset the current_object
				meshes_.push_back(current_object);
				current_object = mesh();

				//reset vector temp
				vertexIndices.clear();
				uvIndices.clear();
				normalIndices.clear();

			}
			else if(header == "v ") // VERTEX
			{
				//Split line into 3 x y z
				std::vector<std::string> result;
				boost::split(result, line, boost::is_any_of(" "));
				auto vertex = glm::vec3(std::stof(result[1]), std::stof(result[2]), std::stof(result[3]));
				temp_vertices.push_back(vertex);
			}
			else if(header == "vt") // UV
			{
				//Split line into 2 u v
				std::vector<std::string> result;
				boost::split(result, line, boost::is_any_of(" "));
				auto uv = glm::vec2(std::stof(result[1]), -std::stof(result[2]));
				temp_uvs.push_back(uv);
			}
			else if (header == "vn") // NORMAL
			{
				//Split line into 3 nx ny nz
				std::vector<std::string> result;
				boost::split(result, line, boost::is_any_of(" "));
				auto normal = glm::vec3(std::stof(result[1]), std::stof(result[2]), std::stof(result[3]));
				temp_normals.push_back(normal);
			}
			else if (header == "f ") //Face
			{
				//split into 9 x u nx | y v ny | z w nz
				std::vector<std::string> result, result1, result2, result3;
				boost::split(result, line, boost::is_any_of(" "));
				boost::split(result1, result[1], boost::is_any_of("/"));
				boost::split(result2, result[2], boost::is_any_of("/"));
				boost::split(result3, result[3], boost::is_any_of("/"));

				vertexIndices.push_back(std::stoi(result1[0]));
				vertexIndices.push_back(std::stoi(result2[0]));
				vertexIndices.push_back(std::stoi(result3[0]));

				uvIndices.push_back(std::stoi(result1[1]));
				uvIndices.push_back(std::stoi(result2[1]));
				uvIndices.push_back(std::stoi(result3[1]));

				normalIndices.push_back(std::stoi(result1[2]));
				normalIndices.push_back(std::stoi(result2[2]));
				normalIndices.push_back(std::stoi(result3[2]));
				
			}
		}
		file.close();

		std::vector<vertex> vertices = std::vector<vertex>(vertexIndices.size());
		for (unsigned int i = 0; i < vertexIndices.size(); i++) {
			unsigned int vertexIndex = vertexIndices[i];
			vertices[i].position = temp_vertices[vertexIndex - 1];
		}

		for (unsigned int i = 0; i < uvIndices.size(); i++) {
			unsigned int uvIndex = uvIndices[i];
			vertices[i].uv = temp_uvs[uvIndex - 1];
		}

		for (unsigned int i = 0; i < normalIndices.size(); i++) {
			unsigned int normalIndex = normalIndices[i];
			vertices[i].normal = temp_normals[normalIndex - 1];
		}
		current_object.vertices = vertices;

		//add the mesh
		meshes_.push_back(current_object);
		
	}
	else
	{
		std::cout << "Cannot open file : " << file_name << std::endl;
	}

}

void model::show_all_vertices()
{
	for(int i = 0; i < meshes_.size(); ++i)
	{
		std::cout << i << " SOBJ" << std::endl;

		for(const auto & vertex : meshes_[i].vertices)
		{
			std::cout << vertex.position.x << " | " << vertex.position.y << " | " << vertex.position.z << std::endl;
		}

	}
}

std::vector < std::vector < glm::vec3 >> model::return_vertices()
{
	std::vector < std::vector < glm::vec3 >> result;

	for(const auto & mesh : meshes_)
	{
		std::vector<glm::vec3> result_int;
		for(const auto & vertex : mesh.vertices)
		{
			result_int.push_back(vertex.position);
		}
		result.push_back(result_int);
	}

	return result;
}

void model::add_texture(const std::string path)
{
	textures_.push_back(std::make_unique<Texture>(path));
}

void model::add_texture(Texture& texture) {
	textures_.push_back(std::make_unique<Texture>(texture));
}

void model::setup()
{
	vbo_ = std::vector<GLuint>(meshes_.size());

	for(int i = 0; i < meshes_.size(); ++i)
	{
		// Create the vbo
		glGenBuffers(1, &vbo_[i]);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[i]);
		glBufferData(GL_ARRAY_BUFFER, meshes_[i].vertices.size() * sizeof(vertex), &meshes_[i].vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void model::draw()
{
	for (int i = 0; i < meshes_.size(); ++i)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_[i]);

		
		if(textures_.size() == 1)
			textures_[0]->bind();
		else {

			textures_[i]->bind();
		}
		

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, uv));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
		glEnableVertexAttribArray(2);

		glDrawArrays(GL_TRIANGLES, 0, meshes_[i].vertices.size());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void model::reset() {
	modelMat = glm::mat4(1.0f);
}

void model::translate(float x, float y, float z) {
	modelMat = glm::translate(modelMat, glm::vec3(x, y, z));
}

void model::scale(float x, float y, float z) {
	modelMat = glm::scale(modelMat, glm::vec3(x, y, z));
}
void model::rotate(float angle, float x, float y, float z) {
	modelMat = glm::rotate(modelMat, angle, glm::vec3(x, y, z));
}