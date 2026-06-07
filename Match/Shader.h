#pragma once
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

class Shader
{
public:
	Shader();
	~Shader();
	void Compile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	void setName(std::string pname);
	std::string GetName();
	void Use();
	void SetUniformMatrix4f(const std::string& uniform, glm::mat4& matrix);
	void SetUniformVec3(const std::string& uniform, glm::vec3 vec);
	void SetUniformVec2(const std::string& uniform, glm::vec2 vec);
	void SetUniformFloat(const std::string& uniform, float value);
	void SetUniformInt(const std::string& uniform, int value);
	unsigned int ID;
private:
	std::string name;
	std::map<std::string, unsigned int> locations;
private:
	std::string LoadCStringFromFile(const std::string& path);
};

