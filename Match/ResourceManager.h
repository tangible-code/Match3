#pragma once
#include "Shader.h"
#include <map>
#include <stb_image.h>
#include <al.h>
#include <SDL_Audio.h>
#include <ft2build.h>
#include <freetype/freetype.h>


struct Texture {
	int width, height;
	unsigned int id;
	Texture(unsigned int Id, int width, int height) 
		: id(Id), width(width), height(height)
	{}
};

struct F_Character {
	unsigned int texture;
	glm::ivec2 size;
	glm::ivec2 bearing;
	int advance;

};

class ResourceManager
{
public:
	static void LoadShader(std::string id, const std::string& vertex, const std::string& fragment);
	static Shader* GetShader(std::string shader);

	static void LoadTexture(std::string id, const std::string& path);
	static Texture* GetTexture(std::string texture);

	static void LoadFont(std::string id, std::string path, int size);
	static std::map<char, F_Character>* GetFont(std::string id);

	static void LoadSound(std::string id, std::string path);

	static void LoadConfigPath(std::string id, std::string path);
	static std::string GetConfigPath(std::string id);


	static void ReleaseTextures();
	static void ReleaseShaders();
	static void ReleaseFonts();

	static void Clear();

	static bool is_FileExist(std::string path);

private:
	static std::map<std::string, Shader*> Shaders;
	static std::map<std::string, Texture*> Textures;
	static std::map<std::string, std::map<char, F_Character>*> Fonts;
	static std::map<std::string, std::string> ConfigFile;

	static bool FontInit;
	static FT_Library ft;
};

