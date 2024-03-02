#include "ComputeShader.h"
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>

void ParseSourceFromFilepath(const char* InFilePath, std::string& OutSourceString)
{
    std::ifstream stream(InFilePath);

    std::string line;
    std::stringstream ss;

    while (getline(stream, line))
    {
        ss << line << '\n';
    }

    OutSourceString = ss.str();
}

ComputeShader::ComputeShader(const char* computePath)
{
	unsigned int compute = glCreateShader(GL_COMPUTE_SHADER);

    std::string sourceString;
    ParseSourceFromFilepath(computePath, sourceString);

    const char* sourceChar = sourceString.c_str();

	glShaderSource(compute, 1, &sourceChar, NULL);
	glCompileShader(compute);

    int success = 0;
    glGetShaderiv(compute, GL_COMPILE_STATUS, &success);

    char infoLog[512];

    if (!success)
    {
        glGetShaderInfoLog(compute, 512, NULL, infoLog);
        printf("Unable to compile compute shader: %s\n", infoLog);
    }
	
    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        printf("Failed to link compute shader: %s\n", infoLog);
    }

    glDeleteShader(compute);
}
