#include "PrimitiveRenderer.h"
#include <glm/gtc/type_ptr.hpp>

PrimitiveRenderer::PrimitiveRenderer()
{
	const char* vertexSource = "#version 330 core\n"
		"layout(location = 0) in vec2 aPos;\n"

		"uniform mat4 model;\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"gl_Position = projection * model * vec4(aPos, 0.0, 1.0);\n"
		"}\n";

	const char* fragmentSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"uniform vec3 Color;\n"

		"void main()\n"
		"{\n"

		"FragColor = vec4(Color, 1.0);\n"
		"}\n";

	primitiveShader = glCreateProgram();

	unsigned int vertexShader, fragmentShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char log[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, log);
		printf("Vertex Shader:%s\n", log);
	}

	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, log);
		printf("Fragment Shader:%s\n", log);
	}

	glAttachShader(primitiveShader, vertexShader);
	glAttachShader(primitiveShader, fragmentShader);
	glLinkProgram(primitiveShader);

	glGetProgramiv(primitiveShader, GL_LINK_STATUS, &success);


	if (!success) {
		glGetShaderInfoLog(primitiveShader, 512, NULL, log);
		printf("Shader Program:%s\n", log);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	matrixSet = false;
	Init();
}

PrimitiveRenderer::~PrimitiveRenderer()
{
	glDeleteProgram(primitiveShader);
	glDeleteBuffers(1, &primitiveVBO);
	glDeleteVertexArrays(1, &primitiveVAO);
}

void PrimitiveRenderer::Init()
{
	float primitivevertices[] = {
		1.0f, 0.0f,
		1.0f,0.0f
	};
	glGenBuffers(1, &primitiveVBO);
	glGenVertexArrays(1, &this->primitiveVAO);

	glBindVertexArray(this->primitiveVAO);
	glBindBuffer(GL_ARRAY_BUFFER, primitiveVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(primitivevertices), primitivevertices, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void PrimitiveRenderer::DrawLine(glm::vec2 start, glm::vec2 end, glm::vec3 color)
{
	if (!matrixSet)
	{
		printf("Primitive Renderer matrix not set\n");
		return;
	}

	glUseProgram(primitiveShader);
	glm::mat4 primitivemodel = glm::mat4(1.0f);
	float primitivevertices[] = {
		start.x,start.y,
		end.x,end.y
	};

	glUniformMatrix4fv(glad_glGetUniformLocation(primitiveShader, "model"), 1, GL_FALSE, glm::value_ptr(primitivemodel));
	glUniform3f(glGetUniformLocation(primitiveShader, "Color"), color.x, color.y, color.z);
	glBindVertexArray(this->primitiveVAO);
	glBindBuffer(GL_ARRAY_BUFFER, primitiveVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(primitivevertices), primitivevertices);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

void PrimitiveRenderer::PutPixel(glm::vec2 position, glm::vec3 color)
{
	if (!matrixSet)
	{
		printf("Primitive Renderer matrix not set\n");
		return;
	}

	glUseProgram(primitiveShader);

	glm::mat4 primitivemodel = glm::mat4(1.0f);
	float primitivevertices[] = {
		position.x,position.y,
	};

	glUniformMatrix4fv(glad_glGetUniformLocation(primitiveShader, "model"), 1, GL_FALSE, glm::value_ptr(primitivemodel));
	glUniform3f(glGetUniformLocation(primitiveShader, "Color"), color.x, color.y, color.z);
	glBindVertexArray(this->primitiveVAO);
	glBindBuffer(GL_ARRAY_BUFFER, primitiveVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(primitivevertices), primitivevertices);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}

void PrimitiveRenderer::SetProjectionMat(glm::mat4& proj)
{
	glUseProgram(primitiveShader);
	glUniformMatrix4fv(glad_glGetUniformLocation(primitiveShader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
	matrixSet = true;
}
