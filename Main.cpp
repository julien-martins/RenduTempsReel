#include <iostream>

#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

#include "stl.h"
#include "obj.h"
#include "Texture.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
	//Initialize GLFW
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Create the window
	GLFWwindow* window = glfwCreateWindow(800, 600, "Rendu Temps Reel", NULL, NULL);
	if(window == NULL)
	{
		std::cerr << "Failed to create GLFW winow" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Callback to have the possibility to resize the window
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glViewport(0, 0, 800, 600);

	const char *vertexShaderSource = "#version 330 core\n"
		"in vec3 position;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(position * 0.01, 1.0f);\n"
		"}\0";

	const char *fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n\0";

	
	//Generate VBO
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	unsigned int uvBuffer;
	glGenBuffers(1, &uvBuffer);

	//Generate VAO
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	//Compile Vertex Shader
	const auto vertexShader = MakeShader(GL_VERTEX_SHADER, "resources/shaders/shader.vert");
	const auto fragmentShader = MakeShader(GL_FRAGMENT_SHADER, "resources/shaders/shader.frag");

	const auto shaderProgram = AttachAndLink({ vertexShader, fragmentShader });

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//Load model STL
	const auto tris = ReadStl("resources/models/garfield.stl");
	std::cout << tris.size() << std::endl;
	const auto nTriangles = tris.size();

	//Load Garfield Texture
	Texture garfieldTex1("resources/textures/tex1.png");
	Texture garfieldTex2("resources/textures/tex2.png");
	Texture garfieldTex3("resources/textures/tex3.png");

	//Load OBJ Garfield
	std::vector<glm::vec3> garfield_vertices;
	std::vector<glm::vec2> garfield_uvs;
	std::vector<glm::vec3> garfield_normals;

	load_obj("resources/models/garfield.obj", garfield_vertices, garfield_uvs, garfield_normals);

	const auto index = glGetAttribLocation(shaderProgram, "position");
	const auto indexUV = glGetAttribLocation(shaderProgram, "vertexUV");

	//Copy all vertices in the VAO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, garfield_vertices.size() * sizeof(glm::vec3), &garfield_vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(index);

	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, garfield_uvs.size() * sizeof(glm::vec2), &garfield_uvs[0], GL_STATIC_DRAW);

	glVertexAttribPointer(indexUV, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(indexUV);

	glEnable(GL_DEPTH_TEST);

	const auto locTrans = glGetUniformLocation(shaderProgram, "transformation");
	const auto locProj = glGetUniformLocation(shaderProgram, "projection");
	while(!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		glUniform1i(glGetUniformLocation(shaderProgram, "tex1"), 0);
		glUniform1i(glGetUniformLocation(shaderProgram, "tex2"), 1);
		glUniform1i(glGetUniformLocation(shaderProgram, "tex3"), 2);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, garfieldTex1.get_texture());

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, garfieldTex2.get_texture());

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, garfieldTex3.get_texture());


		//Projection Transformation
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);

		//model Transformation
		glm::mat4 transMat = glm::mat4(1.0f);
		transMat = glm::translate(transMat, glm::vec3(0, -50, -150));
		transMat = glm::scale(transMat, glm::vec3(0.5, 0.5, 0.5));
		transMat = glm::rotate(transMat, (float)(glfwGetTime()), glm::vec3(0.0, 1.0, 0.0));

		glUniformMatrix4fv(locTrans, 1, GL_FALSE, glm::value_ptr(transMat));
		glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(projection));

		glDrawArrays(GL_TRIANGLES, 0, nTriangles * 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}