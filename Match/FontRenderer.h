#pragma once
#include <string>
#include "ResourceManager.h"
class FontRenderer
{
public:
	FontRenderer();
	~FontRenderer();
	void RenderText(glm::vec2 position, std::string str, std::map<char, F_Character>* font, glm::vec3 color, float scale = 1.0f);
	glm::vec2 GetSize(std::string str, std::map<char, F_Character>* font, float scale = 1.0f);
	void SetProjectionMat(glm::mat4& proj);
private:
	//ading shader class instance
	void Init();
	unsigned int fontShader;
	unsigned int fontVAO, fontVBO;
	bool matrixSet;
};

