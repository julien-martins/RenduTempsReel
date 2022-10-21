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
	
	float quadVertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	unsigned int screen_VAO;
	glGenVertexArrays(1, &screen_VAO);

	unsigned int screen_VBO;
	glGenBuffers(1, &screen_VBO);

	glBindVertexArray(screen_VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, screen_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//MODEL SHADER
	
	const auto vertexShader = MakeShader(GL_VERTEX_SHADER, "resources/shaders/shader.vert");
	const auto fragmentShader = MakeShader(GL_FRAGMENT_SHADER, "resources/shaders/shader.frag");

	const auto shaderProgram = AttachAndLink({ vertexShader, fragmentShader });

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	

	//SCREEN SHADER
	const auto screen_vertexShader = MakeShader(GL_VERTEX_SHADER, "resources/shaders/screen.vert");
	const auto screen_fragmentShader = MakeShader(GL_FRAGMENT_SHADER, "resources/shaders/screen.frag");

	const auto screenShaderProgram = AttachAndLink({ screen_vertexShader, screen_fragmentShader });

	glDeleteShader(screen_vertexShader); 
	glDeleteShader(screen_fragmentShader);

	Light light1 = { glm::vec3(10, 4, 10), glm::vec3(10000, 10000, 10000) };
	
	
	Texture texText("resources/textures/tex3.png");

	model garfield_obj;
	garfield_obj.load_obj("resources/models/garfield.obj");

	
	garfield_obj.add_texture("resources/textures/tex3.png");
	garfield_obj.add_texture("resources/textures/tex1.png");
	garfield_obj.add_texture("resources/textures/tex2.png");
	

	model cube;
	cube.load_obj("resources/models/cube.obj");
	cube.add_texture("resources/textures/tex3.png");

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	garfield_obj.setup();
	cube.setup();

	
	const auto locTrans = glGetUniformLocation(shaderProgram, "transformation");
	const auto locProj = glGetUniformLocation(shaderProgram, "projection");

	const auto locLightPos = glGetUniformLocation(shaderProgram, "lightPos");
	const auto locLightEmission = glGetUniformLocation(shaderProgram, "lightEmission");
	
	//Configuration of framebuffer

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	unsigned int fbo_texture;
	glGenTextures(1, &fbo_texture);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
	
	//unsigned int rbo;
	//glGenRenderbuffers(1, &rbo);

	//glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "FBO : COMPLETED" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	while(!glfwWindowShouldClose(window))
	{
		processInput(window);

		//first pass
		
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.2f, 0.7f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glUniform1i(glGetUniformLocation(shaderProgram, "texDiffuse"), 0);

		glUniform3f(locLightPos, light1.position.x, light1.position.y, light1.position.z);
		glUniform3f(locLightEmission, light1.emission.x, light1.emission.y, light1.emission.z);

		//Projection Transformation
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
		 
		cube.reset();
		cube.translate(0, 0, -450);

		garfield_obj.reset();
		garfield_obj.translate(0, -50, -150);
		garfield_obj.scale(0.5, 0.5, 0.5);
		garfield_obj.rotate((float)(glfwGetTime()), 0.0, 1.0, 0.0);

		glUniformMatrix4fv(locTrans, 1, GL_FALSE, glm::value_ptr(garfield_obj.modelMat));
		glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(projection));

		glUseProgram(shaderProgram);
		glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(VAO);
		garfield_obj.draw();
		//cube.draw();
		glUseProgram(0);
		glBindVertexArray(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable (GL_DEPTH_TEST);
		
		//Draw framebuffer on quad
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(screenShaderProgram);

		glBindVertexArray(screen_VAO);

		//glBindTexture(GL_TEXTURE_2D, texText.get_texture());
		glBindTexture(GL_TEXTURE_2D, fbo_texture);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//glDeleteVertexArrays(1, &VAO);
	//glDeleteProgram(shaderProgram);

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