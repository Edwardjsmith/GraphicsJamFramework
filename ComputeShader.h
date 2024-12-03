#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <glad.h>
#include "Globals.h"

static std::string GShaderPath = "Shaders/";
static std::string GShaderExt = ".shader";
static std::string GShaderUtilExt = ".shaderutil";
static std::string GShaderIncludeWildCard = "#include";

struct VertexInput
{
	alignas(16) glm::vec4 Pos;
	alignas(16) glm::vec4 Normal;
	alignas(16) glm::vec2 UV;
};

struct TriangleData
{
	VertexInput data[3];
	bool bInitialized = false;
};

class ComputeShader
{
public:

	virtual ProcessState CompileShader(const char* computePath);

	void Use() { glUseProgram(ID); }

	virtual void Dispatch(void* OutData, int dispatchCount) = 0;
	virtual void GenerateBuffers() = 0;

	void CheckUniform(int index, const std::string& name) const
	{
		if (index == -1)
		{
			printf("Unable to find uniform variable: %s\n", name.c_str());
		}
	}

	void SetBool(const std::string& name, bool value) const
	{
		const int boolLoc = glGetUniformLocation(ID, name.c_str());
		CheckUniform(boolLoc, name);
		glUniform1i(boolLoc, (int)value);
	}
	void SetInt(const std::string& name, int value) const
	{
		const int intLoc = glGetUniformLocation(ID, name.c_str());
		CheckUniform(intLoc, name);
		glUniform1i(intLoc, value);
	}
	void SetFloat(const std::string& name, float value) const
	{
		const int floatLoc = glGetUniformLocation(ID, name.c_str());
		CheckUniform(floatLoc, name);
		glUniform1f(floatLoc, value);
	}
	void SetVec3(const std::string& name, const glm::vec3& value)
	{
		const int vec3Loc = glGetUniformLocation(ID, name.c_str());
		CheckUniform(vec3Loc, name);
		glUniform3fv(vec3Loc, 1, glm::value_ptr(value));
	}
	void SetTexture(const std::string& name, int value)
	{
		const int texLoc = glGetUniformLocation(ID, name.c_str());
		CheckUniform(texLoc, name);
		glUniform1i(texLoc, value);
	}
	void SetMatrix(const std::string& name, const glm::mat4& mat)
	{
		const int matLoc = glGetUniformLocation(ID, name.c_str());
		CheckUniform(matLoc, name);
		glUniformMatrix4fv(matLoc, 1, GL_FALSE, glm::value_ptr(mat));
	}

	std::vector<GLint>& GetShaderBuffers() { return ShaderBuffers; }

	unsigned int GetID() const { return ID; }

private:

	unsigned int ID = 0;
	std::vector<GLint> ShaderBuffers;
};

