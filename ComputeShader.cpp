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
        if (line.find(GShaderIncludeWildCard) != std::string::npos)
        {
            static char delimiters[] = " \n\t";
            std::string utilShader = "";

            std::string::size_type start = 0;
            std::string::size_type end = 0;

            start = line.find_first_of(" ", start) + 1;
            end = line.find_first_of(delimiters, end);

            utilShader = line.substr(start, (end - start));

            std::string outUtilShaderSource;
            ParseSourceFromFilepath((GShaderPath + utilShader + GShaderUtilExt).c_str(), outUtilShaderSource);

            if (!outUtilShaderSource.empty())
            {
                ss << outUtilShaderSource;
            }

            continue;
        }

        ss << line << '\n';
    }

    OutSourceString = ss.str();
}

ProcessState ComputeShader::CompileShader(const char* computePath)
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
        return ProcessState::NOT_OKAY;
    }

    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        printf("Failed to link compute shader: %s\n", infoLog);
        return ProcessState::NOT_OKAY;
    }

    glDeleteShader(compute);
    return ProcessState::OKAY;
}
