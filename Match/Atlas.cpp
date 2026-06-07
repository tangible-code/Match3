#include "Atlas.h"

SpriteAtlas::SpriteAtlas(std::string name)
	: name(name)
{
}

SpriteAtlas::~SpriteAtlas()
{
	for (auto& value : Animations) {
		delete value.second;
	}
	Animations.clear();
}

void SpriteAtlas::Insert(std::string animation, std::string sheet, int framewidth, int frameheight, int maxframe, bool vertical)
{
	Animations.insert({ animation,new Sprite(sheet, float(ResourceManager::GetTexture(sheet)->width), (float)ResourceManager::GetTexture(sheet)->height, (float)framewidth, (float)frameheight, maxframe, vertical) });
}

Sprite* SpriteAtlas::GetAnimation(std::string id)
{
	if (Animations.find(id) == Animations.end()) {
		printf("warning: %s does not exist\n",id.c_str());
		return NULL;
	}
	return Animations[id];
}

TileAtlas::TileAtlas(std::string name, std::string texture, float width, float height)
	: name(name), fWidth(width), fHeight(height)
{
	this->texture = ResourceManager::GetTexture(texture);
	nWidth = fWidth / float(this->texture->width);
	nHeight = fHeight / float(this->texture->height);
}


TileAtlas::~TileAtlas()
{
	for (auto& value : Offsets) {
		delete value.second;
	}
	Offsets.clear();
}

void TileAtlas::Insert(std::string Id, glm::vec2 Offset)
{
	if (Offsets.find(Id) == Offsets.end()) {
		glm::vec2 normalizePosition;
		normalizePosition = glm::vec2(Offset.x / float(texture->width), (float(texture->height) - (Offset.y + fHeight)) / float(texture->height));
		Quad* quad = new Quad();
		quad->Up_left = glm::vec2(normalizePosition.x, normalizePosition.y);
		quad->Down_right = glm::vec2(normalizePosition.x + nWidth, normalizePosition.y + nHeight);
		quad->Down_left = glm::vec2(normalizePosition.x, normalizePosition.y + nHeight);
		quad->Up_right = glm::vec2(normalizePosition.x + nWidth, normalizePosition.y);
		Offsets.insert({ Id,quad });
	}
	else {
		printf("TileAtlas Id:%s already exist\n", Id.c_str());
	}
}

void TileAtlas::Insert(std::string Id, glm::vec2 Offset, glm::vec2 size)
{
	fWidth = size.x;
	fHeight = size.y;
	nWidth = fWidth / float(this->texture->width);
	nHeight = fHeight / float(this->texture->height);
	if (Offsets.find(Id) == Offsets.end()) {
		glm::vec2 normalizePosition;
		normalizePosition = glm::vec2(Offset.x / float(texture->width), (float(texture->height) - (Offset.y + fHeight)) / float(texture->height));
		Quad* quad = new Quad();
		quad->Up_left = glm::vec2(normalizePosition.x, normalizePosition.y);
		quad->Down_right = glm::vec2(normalizePosition.x + nWidth, normalizePosition.y + nHeight);
		quad->Down_left = glm::vec2(normalizePosition.x, normalizePosition.y + nHeight);
		quad->Up_right = glm::vec2(normalizePosition.x + nWidth, normalizePosition.y);
		quad->size = size;
		Offsets.insert({ Id,quad });
	}
	else {
		printf("TileAtlas Id:%s already exist\n", Id.c_str());
		return;
	}
}

Quad* TileAtlas::GetOffset(std::string Id)
{
	return Offsets[Id];
}
