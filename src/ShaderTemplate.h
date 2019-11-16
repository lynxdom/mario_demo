/*
 * ShaderTemplate.h
 *
 *  Created on: Dec 9, 2017
 *      Author: lynx
 */

#ifndef SHADERTEMPLATE_H_
#define SHADERTEMPLATE_H_

#include "GL\glew.h"
#include <map>
#include <vector>
#include <iostream>
#include <string>

struct TextureShader
{
	static const char* GetVertexShader(GLint &length)
	{
		static const char VertexShader[] = R"(#version 450
		// Vertex position
		layout(location = 0) in vec3 vertexCoords;

		// Texture position
		layout(location = 1) in vec2 uvCoords;
		
		// Pass the UV coords to the FragmentShader
		out vec2 fragUVCoords;

		// Model View Perspective Matrix
		uniform mat4 MVP;

		void main() 
		{
			// Pass the texture coordinates through
			// to the Fragment Shader
			fragUVCoords = uvCoords;

			// process the vertex and pass its final 
			// position to the card.
			gl_Position = MVP * vec4(vertexCoords, 1);
		})";

		length = sizeof(VertexShader);
		return VertexShader;
	}

	static const char* GetFragmentShader(GLint &length)
	{
		static const char FragmentShader[] = R"(#version 450
		
		// The texture map source
		uniform sampler2D textureStore;

		in vec2 fragUVCoords;
		out vec4 outColor;

		void main(){
			outColor = texture(textureStore, fragUVCoords);
		})";

		length = sizeof(FragmentShader);
		return FragmentShader;
	}
};

typedef std::map<const char*, GLint> UniformVariables;

bool Compile(GLenum shaderType_,
		 	 GLuint &name_,
			 const char *shaderCode_,
			 const GLint iLength_);

bool GetUniformVariables(UniformVariables &vars_, const char* ShaderSource_, GLint programID_);



template<class _Shaders>
class ShaderTemplate {
private:
	GLuint iProgramName;
	UniformVariables mapVariables;

public:
	ShaderTemplate()
	{
		GLuint iVertextShader = 0,
			   iFragementShader = 0;


		GLint iLength = 0;
		const char* vertexShader = _Shaders::GetVertexShader(iLength);
		if(!Compile(GL_VERTEX_SHADER,
					iVertextShader,
					vertexShader,
					iLength))
		{
			throw -1;
		}

		iLength = 0;
		const char* fragmentShader = _Shaders::GetFragmentShader(iLength);
		if(!Compile(GL_FRAGMENT_SHADER,
					iFragementShader,
					fragmentShader,
					iLength))
		{
			throw -1;
		}

		iProgramName = glCreateProgram();

	    // attach Shaders
	    glAttachShader(iProgramName, iVertextShader);
	    glAttachShader(iProgramName, iFragementShader);

	    // link the program and check for errors
	    glLinkProgram(iProgramName);
	    GLint status;
	    glGetProgramiv(iProgramName, GL_LINK_STATUS, &status);
	    if(status == GL_FALSE) {
	        GLint length;
	        glGetProgramiv(iProgramName, GL_INFO_LOG_LENGTH, &length);
	        std::vector<char> log(length);
	        glGetProgramInfoLog(iProgramName, length, &length, &log[0]);
	        std::cerr << &log[0];

	        throw -1;
	    }

	    GetUniformVariables(mapVariables, vertexShader, iProgramName);
	    GetUniformVariables(mapVariables, fragmentShader, iProgramName);

	    glDetachShader(iProgramName, iVertextShader);
	    glDetachShader(iProgramName, iFragementShader);
	    glDeleteShader(iVertextShader);
	    glDeleteShader(iFragementShader);
	}

	virtual ~ShaderTemplate()
	{
		glDeleteProgram(iProgramName);
	}

	GLuint GetUniformVar(const char* sVerName_)
	{
		return glGetUniformLocation(iProgramName, sVerName_);
	}

	GLuint getProgram()
	{
		return iProgramName;
	}
};

#endif /* SHADERTEMPLATE_H_ */
