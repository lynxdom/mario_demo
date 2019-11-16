/*
 * ShaderTemplate.cpp
 *
 *  Created on: Dec 9, 2017
 *      Author: lynx
 */

#include "ShaderTemplate.h"

#include <regex>

auto Compile(GLenum shaderType_,
			 GLuint &name_,
			 const char *shaderCode_,
			 const GLint iLength_)->bool
{
	GLuint shaderName = 0;

    // create and compiler vertex shader
	shaderName = glCreateShader(shaderType_);
    glShaderSource(shaderName, 1, &shaderCode_, &iLength_);
    glCompileShader(shaderName);

    GLint status;
    glGetShaderiv(shaderName, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetShaderiv(shaderName, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetShaderInfoLog(shaderName, length, &length, &log[0]);
        std::cerr << &log[0];
        return false;
    }

    name_ = shaderName;
    return true;
}

auto GetUniformVariables(UniformVariables &vars_, const char* ShaderSource_, GLint programID_)->bool
{
	std::regex regUniformPatern("uniform [A-Za-z0-9_]+ [A-Za-z0-9_]+;");
	std::cmatch matches;

	std::string source(ShaderSource_);

	std::sregex_iterator it(std::begin(source), std::end(source), regUniformPatern);
	std::sregex_iterator end;

	while( it != end)
	{
		std::string line((*it).str(0));
		std::string varName(line.begin() + line.find_last_of(" ") + 1, line.begin() + line.rfind(";"));

		GLint varID = glGetUniformLocation(programID_, varName.c_str());
		if((GL_INVALID_VALUE != varID) && (GL_INVALID_OPERATION != varID))
		{
			vars_[varName.c_str()] = varID;
		}

		it++;
	}

	return false;
}


