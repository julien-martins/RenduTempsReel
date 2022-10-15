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
#include "Model.h"

struct Light {
	glm::vec3 position;
	glm::vec3 emission;
};

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
	
	//Generate VBO
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	unsigned int VBO_head;
	glGenBuffers(1, &VBO_head);

	unsigned int VBO_eye;
	glGenBuffers(1, &VBO_eye);

	//Generate VAO
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	unsigned int VAO_head;
	glGenVertexArrays(1, &VAO_head);

	unsigned int VAO_eye;
	glGenVertexArrays(1, &VAO_eye);

	//Compile Vertex Shader
	const auto vertexShader = MakeShader(GL_VERTEX_SHADER, "resources/shaders/shader.vert");
	const auto fragmentShader = MakeShader(GL_FRAGMENT_SHADER, "resources/shaders/shader.frag");

	const auto shaderProgram = AttachAndLink({ vertexShader, fragmentShader });

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	Light light1 = { glm::vec3(10, 4, 10), glm::vec3(10000, 10000, 10000) };

	//Load Garfield Texture
	Texture garfieldTex1("resources/textures/tex1.png");
	Texture garfieldTex2("resources/textures/tex2.png");
	Texture garfieldTex3("resources/textures/tex3.png");
	
	model garfield_obj;
	garfield_obj.load_obj("resources/models/garfield.obj");
	
	const auto garfield_obj_vertices = garfield_obj.return_vertices();
	
	const auto index = glGetAttribLocation(shaderProgram, "position");
	const auto indexUV = glGetAttribLocation(shaderProgram, "vertexUV");
	const auto indexNormal = glGetAttribLocation(shaderProgram, "normal");
	
	//Copy all vertices in the VAO
	//CORPS
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, garfield_obj.meshes_[0].vertices.size() * sizeof(vertex), &garfield_obj.meshes_[0].vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
	glEnableVertexAttribArray(index);

	glVertexAttribPointer(indexUV, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, uv));
	glEnableVertexAttribArray(indexUV);

	glVertexAttribPointer(indexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
	glEnableVertexAttribArray(indexNormal);

	//HEAD
	glBindVertexArray(VAO_head);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_head);
	glBufferData(GL_ARRAY_BUFFER, garfield_obj.meshes_[1].vertices.size() * sizeof(vertex), &garfield_obj.meshes_[1].vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
	glEnableVertexAttribArray(index);

	glVertexAttribPointer(indexUV, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, uv));
	glEnableVertexAttribArray(indexUV);

	glVertexAttribPointer(indexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
	glEnableVertexAttribArray(indexNormal);

	//EYE
	glBindVertexArray(VAO_eye);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_eye);
	glBufferData(GL_ARRAY_BUFFER, garfield_obj.meshes_[2].vertices.size() * sizeof(vertex), &garfield_obj.meshes_[2].vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
	glEnableVertexAttribArray(index);

	glVertexAttribPointer(indexUV, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, uv));
	glEnableVertexAttribArray(indexUV);

	glVertexAttribPointer(indexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
	glEnableVertexAttribArray(indexNormal);
	

	glEnable(GL_DEPTH_TEST);

	const auto locTrans = glGetUniformLocation(shaderProgram, "transformation");
	const auto locProj = glGetUniformLocation(shaderProgram, "projection");

	const auto locLightPos = glGetUniformLocation(shaderProgram, "lightPos");
	const auto locLightEmission = glGetUniformLocation(shaderProgram, "lightEmission");

	
	while(!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniform1i(glGetUniformLocation(shaderProgram, "texDiffuse"), 0);

		glUniform3f(locLightPos, light1.position.x, light1.position.y, light1.position.z);
		glUniform3f(locLightEmission, light1.emission.x, light1.emission.y, light1.emission.z);

		//Projection Transformation
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);

		//model Transformation
		glm::mat4 transMat = glm::mat4(1.0f);
		transMat = glm::translate(transMat, glm::vec3(0, -50, -150));
		transMat = glm::scale(transMat, glm::vec3(0.5, 0.5, 0.5));
		transMat = glm::rotate(transMat, (float)(glfwGetTime()), glm::vec3(0.0, 1.0, 0.0));

		glUniformMatrix4fv(locTrans, 1, GL_FALSE, glm::value_ptr(transMat));
		glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(projection));

		glUseProgram(shaderProgram);

		//CORPS
		glBindVertexArray(VAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, garfieldTex3.get_texture());
		
		glDrawArrays(GL_TRIANGLES, 0, garfield_obj.meshes_[0].vertices.size());

		//HEAD

		glBindVertexArray(VAO_head);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, garfieldTex1.get_texture());
		
		glDrawArrays(GL_TRIANGLES, 0, garfield_obj.meshes_[1].vertices.size());

		//EYE
		glBindVertexArray(VAO_eye);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, garfieldTex2.get_texture());

		glDrawArrays(GL_TRIANGLES, 0, garfield_obj.meshes_[2].vertices.size());

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