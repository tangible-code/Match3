#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

class PrimitiveRenderer
{
public:
	PrimitiveRenderer();
	~PrimitiveRenderer();

	void DrawLine(glm::vec2 start, glm::vec2 end, glm::vec3 color);
	void PutPixel(glm::vec2 position, glm::vec3 color);
	void SetProjectionMat(glm::mat4& proj);

private:
	unsigned int primitiveVAO, primitiveVBO;
	unsigned int primitiveShader;
	bool matrixSet;

	void Init();
};

