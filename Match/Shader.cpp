#include "Shader.h"

Shader::Shader()
{
	ID = glCreateProgram();
}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

void Shader::Compile(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	std::string vertexShaderSource;
	std::string fragmentShaderSource;

	vertexShaderSource = LoadCStringFromFile(vertexShaderPath);
	fragmentShaderSource = LoadCStringFromFile(fragmentShaderPath);

	unsigned int vertexShader, fragmentShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vertexSource = vertexShaderSource.c_str();
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char log[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, log);
		printf("Vertex Shader:%s\n", log);
	}

	const char* fragmentSource = fragmentShaderSource.c_str();
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, log);
		printf("Fragment Shader:%s\n", log);
	}

	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);


	if (!success) {
		glGetShaderInfoLog(ID, 512, NULL, log);
		printf("Shader Program:%s\n", log);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

void Shader::setName(std::string pname)
{
	name = pname;
}

std::string Shader::GetName()
{
	return name;
}

void Shader::Use()
{
	glUseProgram(ID);
}

void Shader::SetUniformMatrix4f(const std::string& uniform, glm::mat4& matrix)
{
	unsigned int location;
	glUseProgram(ID);
	if (locations.find(uniform) == locations.end()) {
		location = glGetUniformLocation(ID, uniform.c_str());
		locations.insert({ uniform,location });
	}
	else {
		location = locations[uniform];
	}

	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetUniformVec3(const std::string& uniform, glm::vec3 vec)
{
	unsigned int location;
	glUseProgram(ID);
	if (locations.find(uniform) == locations.end()) {
		location = glGetUniformLocation(ID, uniform.c_str());
		locations.insert({ uniform,location });
	}
	else {
		location = locations[uniform];
	}

	glUniform3f(location, vec.x, vec.y, vec.z);
}

void Shader::SetUniformVec2(const std::string& uniform, glm::vec2 vec)
{
	unsigned int location;
	glUseProgram(ID);
	if (locations.find(uniform) == locations.end()) {
		location = glGetUniformLocation(ID, uniform.c_str());
		locations.insert({ uniform,location });
	}
	else {
		location = locations[uniform];
	}

	glUniform2f(location, vec.x, vec.y);
}

void Shader::SetUniformFloat(const std::string& uniform, float value)
{
	unsigned int location;
	glUseProgram(ID);
	if (locations.find(uniform) == locations.end()) {
		location = glGetUniformLocation(ID, uniform.c_str());
		locations.insert({ uniform,location });
	}
	else {
		location = locations[uniform];
	}

	glUniform1f(location, value);
}

void Shader::SetUniformInt(const std::string& uniform, int value)
{
	unsigned int location;
	glUseProgram(ID);
	if (locations.find(uniform) == locations.end()) {
		location = glGetUniformLocation(ID, uniform.c_str());
		locations.insert({ uniform,location });
	}
	else {
		location = locations[uniform];
	}


	glUniform1i(location, value);
}

std::string Shader::LoadCStringFromFile(const std::string& path)
{
	std::ifstream file;

	file.open(path);

	std::stringstream str;

	str << file.rdbuf();

	file.close();
	return str.str();
}
