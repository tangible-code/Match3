#pragma once
#include "Atlas.h"
#include <SDL.h>
#include "ResourceManager.h"
class SpriteRenderer
{
public:
	SpriteRenderer();
	~SpriteRenderer();
	void changeTileAtlas(TileAtlas* atlas);
	void SetProjectionMat(glm::mat4& proj);
	void Animate(glm::vec2 position, glm::vec2 size, Sprite* animation,int duration,float rotate = 0.0f);
	void Tiled(glm::vec2 position, glm::vec2 size, std::string Id, float rotate = 0.0f, glm::vec3 axis = glm::vec3(0.0f,0.0f,1.0f));
	void Tiled(glm::vec2 position, glm::vec2 size, std::string Id, std::vector<std::pair<float,glm::vec3>> rotation);
private:
	Texture* current;
	std::string textureName;
	unsigned int texCoordinates;
	unsigned int AtlasVAO;
	unsigned int AtlasShader;
	TileAtlas* tileAtlas;
	bool matrixSet;
	void InitVBO();
};
