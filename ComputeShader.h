#pragma once
#include <glad.h> 
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

class ComputeShader
{
public:

	ComputeShader(const char* computePath);

	void Use() { glUseProgram(ID); }

	void SetBool(const std::string& name, bool value) const
	{
		const int boolLoc = glGetUniformLocation(ID, name.c_str());
		glUniform1i(boolLoc, (int)value);
	}
	void SetInt(const std::string& name, int value) const
	{
		const int intLoc = glGetUniformLocation(ID, name.c_str());
		glUniform1i(intLoc, value);
	}
	void SetFloat(const std::string& name, float value) const
	{
		const int floatLoc = glGetUniformLocation(ID, name.c_str());
		glUniform1f(floatLoc, value);
	}
	void SetVec3(const std::string& name, const glm::vec3& value)
	{
		const int vec3Loc = glGetUniformLocation(ID, name.c_str());
		glUniform3fv(vec3Loc, 1, glm::value_ptr(value));
	}
	void SetTexture(const std::string& name, int value)
	{
		const int texLoc = glGetUniformLocation(ID, name.c_str());
		glUniform1i(texLoc, value);
	}
	void SetMatrix(const std::string& name, const glm::mat4& mat)
	{
		const int matLoc = glGetUniformLocation(ID, name.c_str());
		glUniformMatrix4fv(matLoc, 1, GL_FALSE, glm::value_ptr(mat));
	}

private:

	unsigned int ID = -1;

};

