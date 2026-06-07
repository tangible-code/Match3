#include "ResourceManager.h"
#include <filesystem>
#include <fstream>
#include "Audio.h"

std::map<std::string, Shader*> ResourceManager::Shaders;
std::map<std::string, Texture*> ResourceManager::Textures;
std::map<std::string, std::map<char, F_Character>*>  ResourceManager::Fonts;
std::map<std::string, std::string> ResourceManager::ConfigFile;
bool  ResourceManager::FontInit = false;
FT_Library ResourceManager::ft;

void ResourceManager::LoadShader(std::string id, const std::string& vertex, const std::string& fragment)
{
	if (!is_FileExist(vertex) && !is_FileExist(fragment)) {
		printf("Shader file not found\n");
		return;
	}
	Shader* shader = new Shader();
	shader->Compile(vertex, fragment);
	Shaders.insert({ id,shader });
}

Shader* ResourceManager::GetShader(std::string shader)
{
	if (Shaders.find(shader) == Shaders.end())
	{
		printf("Shader: %s not Found\n", shader.c_str());
		return NULL;
	}

	return Shaders[shader];
}

void ResourceManager::LoadTexture(std::string id, const std::string& path)
{
	if (!is_FileExist(path)) {
		printf("Texture not found\n");
		return;
	}

	if (Textures.find(id) != Textures.end())
	{
		return;
	}

	stbi_set_flip_vertically_on_load(true);
	int width, height, channel;
	unsigned char* textureData = stbi_load(path.c_str(), &width, &height, &channel, 0);
	GLenum format;

	switch (channel) {
	case STBI_rgb:
		format = GL_RGB;
		break;
	case STBI_rgb_alpha:
		format = GL_RGBA;
		break;
	}

	unsigned int texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(textureData);

	Textures.insert({ id,new Texture(texture,width,height)});
}

Texture* ResourceManager::GetTexture(std::string texture)
{
	if (Textures.find(texture) == Textures.end())
	{
		printf("Texture: %s not Found\n", texture.c_str());
		return NULL;
	}

	return Textures[texture];
}

void ResourceManager::LoadFont(std::string id, std::string path, int size)
{
	if (!is_FileExist(path)) {
		printf("font file not found\n");
		return;
	}

	if (!FontInit) {
		if (FT_Init_FreeType(&ft)) {
			printf("could not untualize ftt\n");
		}
		FontInit = true;
	}

	FT_Face face;

	if (FT_New_Face(ft, path.c_str(), 0, &face)) {
		printf("could not create face\n");
	}

	std::map<char, F_Character>* characters = new std::map<char, F_Character>();

	FT_Set_Pixel_Sizes(face, 0, size);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (int c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			printf("cant load given character:%d\n", c);
		}

		unsigned int texture = 0;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		F_Character character = { texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x };

		characters->insert({ c,character });
	}
	FT_Done_Face(face);

	Fonts.insert({ id,characters });
}

std::map<char, F_Character>* ResourceManager::GetFont(std::string id)
{
	return Fonts[id];
}

void ResourceManager::LoadSound(std::string id, std::string path)
{
	FILE* wave;
	wave = fopen(path.c_str(), "rb");
	char Id[5];
	Id[4] = '\0';

	fread(Id, 4, 1, wave);

	unsigned long size;
	fread(&size, sizeof(long), 1, wave);


	fread(Id, 4, 1, wave);

	bool fmtCk = false;
	do {
		fread(Id, 4, 1, wave);
		if (strcmp(Id, "fmt ") == 0)
		{
			fmtCk = true;
		}
		else
		{
			unsigned int ckSize;
			fread(&ckSize, sizeof(long), 1, wave);
			fseek(wave, ckSize, SEEK_CUR);
		}

	} while (!fmtCk);

	unsigned long fmtsize;
	fread(&fmtsize, sizeof(long), 1, wave);


	unsigned short formatTag;
	fread(&formatTag, sizeof(short), 1, wave);


	unsigned short channels;
	fread(&channels, sizeof(short), 1, wave);



	unsigned long sampleRate;
	fread(&sampleRate, sizeof(long), 1, wave);



	unsigned long byteRate;
	fread(&byteRate, sizeof(long), 1, wave);



	unsigned short blockAlign;
	fread(&blockAlign, sizeof(short), 1, wave);


	unsigned short bitPerSample;
	fread(&bitPerSample, sizeof(short), 1, wave);


	bool dataCk = false;
	do {
		fread(Id, 4, 1, wave);

		if (strcmp(Id, "data") == 0)
		{
			dataCk = true;
		}
		else
		{
			unsigned int ckSize;
			fread(&ckSize, sizeof(long), 1, wave);
			fseek(wave, ckSize, SEEK_CUR);
		}

	} while (!dataCk);

	unsigned long dataSize;
	fread(&dataSize, sizeof(long), 1, wave);


	void* data = malloc(dataSize);

	if (!data)
	{
		printf("memory not allocated\n");
	}

	fread(data, dataSize, 1, wave);
	fclose(wave);

	ALenum format;

	if (channels == 1 && bitPerSample == 8) {
		format = AL_FORMAT_MONO8;
	}
	else if (channels == 2 && bitPerSample == 8) {
		format = AL_FORMAT_STEREO8;
	}
	else if (channels == 1 && bitPerSample == 16) {
		format = AL_FORMAT_MONO16;
	}
	else if (channels == 2 && bitPerSample == 16) {
		format = AL_FORMAT_STEREO16;
	}
	else {
		printf("unknown format\n");
	}

	Audio::LoadSound(id, format, data, dataSize, sampleRate);
}

void ResourceManager::LoadConfigPath(std::string id, std::string path)
{
	if (!is_FileExist(path))
	{
		printf("Config file does not Exist\n");
		return;
	}

	if (ConfigFile.find(id) != ConfigFile.end())
	{
		printf("Config already from this id\n");
		return;
	}

	ConfigFile.insert({ id,path });
}

std::string ResourceManager::GetConfigPath(std::string id)
{
	if (ConfigFile.find(id) == ConfigFile.end())
	{
		printf("Config does not exist id\n");
		return "";
	}
	return ConfigFile[id];
}

void ResourceManager::ReleaseTextures()
{
	for (auto& value : Textures) {
		glDeleteTextures(1, &value.second->id);
		delete value.second;
	}

	Textures.clear();
}

void ResourceManager::ReleaseShaders()
{
	for (auto& value : Shaders) {
		glDeleteProgram(value.second->ID);
		delete value.second;
	}
	Shaders.clear();
}

void ResourceManager::ReleaseFonts()
{
	for (auto& f : Fonts) {
		for (auto b = f.second->begin(); b != f.second->end(); b++)
		{
			glDeleteTextures(1, &b->second.texture);
		}
	}
	Fonts.clear();
}

void ResourceManager::Clear()
{
	if (FontInit) {
		FT_Done_FreeType(ft);
	}
	ReleaseFonts();
	ReleaseTextures();
	ReleaseShaders();
}

bool ResourceManager::is_FileExist(std::string path)
{
	std::filesystem::path Path(path);
	return std::filesystem::exists(Path);
}
