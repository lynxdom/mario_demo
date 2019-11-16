/*
 * main.cpp
 *
 *  Created on: Dec 26, 2017
 *      Author: lynx
 */

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <GLM\glm.hpp>
#include <GLM\GTX\transform.hpp>

#include "ImageFileReader.h"
#include <vector>

#include <chrono>
#include <ctime>

#include <iomanip>

#include <thread>

#define MAXIMUM_FRAME_RATE 55  //maximum frame rate in ms

static const char *VertexShader = R"(
#version 400

// x, y
in layout(location = 0) vec2 position;

// r, g, b
in layout(location = 1) vec3 vertextColor;

out vec3 vertColor;

uniform mat4 MVP;

void main(void)
{
	gl_Position = MVP * vec4(position, 0.0, 1.0);
	vertColor = vertextColor;
}

)";

static const char *FragmentShader = R"(
#version 400

out vec4 outColor;
in vec3 vertColor;

void main(void)
{
	// r, g, b, a
	outColor = vec4(vertColor, 1.0);
}

)";

auto CompileShaders()->GLuint
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	const char* shader[1];

	shader[0] = VertexShader;

	glShaderSource(vertexShader, 1, shader, 0);
	glCompileShader(vertexShader);

	GLint glCompile = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &glCompile);

	if(glCompile != GL_TRUE)
	{
		std::cerr << "Vertex shader failed to compile" << std::endl;

		GLint iLogLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &iLogLength);

		auto buffer = std::unique_ptr<GLchar[]>(new GLchar[iLogLength]);
		glGetShaderInfoLog(vertexShader, iLogLength, &iLogLength, static_cast<GLchar*>(buffer.get()));

		std::cerr << buffer.get();

		return 0;
	}

	shader[0] = FragmentShader;
	glShaderSource(fragmentShader, 1, shader, 0);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &glCompile);

	if(glCompile != GL_TRUE)
	{
		std::cerr << "Fragment shader failed to compile" << std::endl;


		GLint iLogLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &iLogLength);

		auto buffer = std::unique_ptr<GLchar[]>(new GLchar[iLogLength]);
		glGetShaderInfoLog(fragmentShader, iLogLength, &iLogLength, static_cast<GLchar*>(buffer.get()));

		std::cerr << buffer.get();

		return 0;
	}

	GLuint Program = glCreateProgram();

	glAttachShader(Program, vertexShader);
	glAttachShader(Program, fragmentShader);

	glLinkProgram(Program);

	return Program;
}

auto CreateImageBuffer(const char *sFileName_, unsigned int &pointCount_)->GLuint
{
	int iHeight = 0,
		iWidth = 0;

	auto pImage = Lynxdom::ImageFileReader::ReadBMP(sFileName_, iWidth, iHeight);
	std::vector<GLfloat> vecBuffer;

	pointCount_ = 0;
	int i = 0;

	for(int y = 0; y < iHeight; ++y)
	{
		for(int x = 0; x < iWidth; ++x)
		{
			auto oPixel = pImage[i++];

			if((oPixel.R == 0xFF) &&
			   (oPixel.G == 0xFF) &&
			   (oPixel.B == 0xFF))
			{
				continue;
			}

			// x, y
			vecBuffer.emplace_back(x);
			vecBuffer.emplace_back(y);

			auto fR = static_cast<float>(oPixel.R) / 255.0f;
			auto fG = static_cast<float>(oPixel.G) / 255.0f;
			auto fB = static_cast<float>(oPixel.B) / 255.0f;

			// r, g, b
			vecBuffer.emplace_back(fB);
			vecBuffer.emplace_back(fG);
			vecBuffer.emplace_back(fR);

			++pointCount_;
		}
	}

	int iBufferSize = vecBuffer.size() * sizeof(GLfloat);

	GLuint iBufferName = 0;

	GLuint iVertexArrayName = 0;
	glGenVertexArrays(1, &iVertexArrayName);
	glBindVertexArray(iVertexArrayName);

		GLfloat *buffer = &vecBuffer[0];
		glGenBuffers(1, &iBufferName);
		glBindBuffer(GL_ARRAY_BUFFER, iBufferName);
		glBufferData(GL_ARRAY_BUFFER, iBufferSize, buffer, GL_STATIC_DRAW);

		// Position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
							   0,                  // Layout 0 in the shader
							   2,                  // size
							   GL_FLOAT,           // type
							   GL_FALSE,           // normalized
							   sizeof(float) * 5,   // stride
							   (void*)0            // array buffer offset
							);

		// Color
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
							   1,                  							// Layout 1 in the shader
							   3,                  							// size
							   GL_FLOAT,          							// type
							   GL_FALSE,           							// normalized?
							   sizeof(float) * 5,   							// stride
							   reinterpret_cast<void*>(sizeof(float) * 2)	// array buffer offset
							);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	return iVertexArrayName;
}

void error_callback(int error_, const char* desc_)
{
	std::cerr << "Error:" << desc_ << std::endl;
}

struct BufferObject
{
	GLuint buffer;
	unsigned int points;
};

auto main()->int
{

	if(!glfwInit())
	{
		std::cerr << "Failed to initialize glfw!" << std::endl;
		return -1;
	}

	glfwSetErrorCallback(error_callback);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	GLFWwindow* win = glfwCreateWindow(512, 512, "First Window", nullptr, nullptr);

	int width, height;
	glfwGetFramebufferSize(win, &width, &height);

	glfwMakeContextCurrent(win);

	glewExperimental = GL_TRUE;

	if( glewInit() != GLEW_OK )
	{
		std::cerr << "Failed to initialize glew!" << std::endl;
		return -1;
	}

	// Build Data
	BufferObject bufferNames[3];
	bufferNames[0].buffer = CreateImageBuffer("../res/Mario1.bmp", bufferNames[0].points);
	bufferNames[1].buffer = CreateImageBuffer("../res/Mario2.bmp", bufferNames[1].points);
	bufferNames[2].buffer = CreateImageBuffer("../res/Mario3.bmp", bufferNames[2].points);

	glBindVertexArray(0);

	// Build Shaders
	GLuint glProgram = CompileShaders();
	GLint uniformMVP = glGetUniformLocation(glProgram, "MVP");

	// Set up Matrix Math
	glm::mat4 P = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, -1.0f, 1.0f);
	glm::mat4 V = glm::mat4(1.0f);
	glm::mat4 M = glm::mat4(1.0f);

	M = glm::scale(M, glm::vec3(0.5f, 0.5f, 0.0f));

	unsigned int iBuffer = 0;

	while(!glfwWindowShouldClose(win))
	{
		// Frame regulation
		auto frameStart = std::chrono::steady_clock::now();
		
		if(iBuffer == 3)
		{
			iBuffer = 0;
		}
		
        glfwPollEvents();

        // clear first
        glClearColor(0.4f, 0.4f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		GLenum glError = glGetError();

		glm::mat4 MVP = P * V * M;

		glBindVertexArray(bufferNames[iBuffer].buffer);
		glUseProgram(glProgram);

		glEnable( GL_PROGRAM_POINT_SIZE );
		glPointSize(5.5);

		glUniformMatrix4fv(uniformMVP, 1, GL_FALSE, &MVP[0][0]);
		glDrawArrays(GL_POINTS, 0, bufferNames[iBuffer].points);

		glError = glGetError();

		if(glError != 0)
		{
			std::cerr << "An Error occurred in the main loop" << std::endl;
			break;
		}

		// release buffers
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

        glfwSwapBuffers(win);
		iBuffer++;
		
		// Frame regulation
		auto frameStop = std::chrono::steady_clock::now();
		auto frameDuration = std::chrono::duration<double>( frameStop - frameStart );
		long durationMS = static_cast<long>(frameDuration.count() * 1000);
		
		std::cout << frameDuration.count() * 1000 << std::endl;
		std::cout << durationMS << std::endl;
		
		if( durationMS < MAXIMUM_FRAME_RATE )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( MAXIMUM_FRAME_RATE - durationMS ) );
		}
	}

	// delete window
	glfwDestroyWindow(win);

	// Release GLFW
	glfwTerminate();

	return 0;
}


