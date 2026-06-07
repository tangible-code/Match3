#pragma once
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include "ResourceManager.h"
#include <string>

struct Sprite {
	int maxframe;
	std::string spritesheet;
	float width, height;
	float framewidth, frameheight;
	bool vertical;
	Sprite(std::string sheet, float width, float height, float framewidth, float frameheight, int maxframe,bool vertical)
		: spritesheet(sheet), width(width), height(height), framewidth(framewidth), frameheight(frameheight), maxframe(maxframe), vertical(vertical)
	{
	}
};


class SpriteAtlas
{
public:
	SpriteAtlas(std::string name);
	~SpriteAtlas();

	void Insert(std::string animation, std::string sheet, int framewidth, int frameheight, int maxframe,bool vertical = false);
	Sprite* GetAnimation(std::string id);
private:
	std::string name;
	std::map<std::string, Sprite*> Animations;
};

struct Quad {
	glm::vec2 Up_left, Up_right;
	glm::vec2 Down_left, Down_right;
	glm::vec2 size;
};
class TileAtlas
{
public:
	TileAtlas(std::string name, std::string texture,float width, float height);
	//TileAtlas(std::string name, std::string texture,float width, float height,float marginx,float marginy);
	~TileAtlas();

	void Insert(std::string Id, glm::vec2 Offset);
	void Insert(std::string Id, glm::vec2 Offset,glm::vec2 size);
	Quad* GetOffset(std::string Id);

	Texture* texture;
	float nWidth, nHeight;
	float fWidth, fHeight;
private:
	std::map<std::string, Quad*> Offsets;
	std::string name;
};
