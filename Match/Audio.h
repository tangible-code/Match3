#pragma once
#include <map>
#include <string>
#include <al.h>
#include <alc.h>
#include<vector>
#include <glm/glm.hpp>


struct Source {

	Source(std::string sound,unsigned int sourceId)
	: currentSound(sound),id(sourceId)
	{
	}
	unsigned int id;
	std::string currentSound;
};


class Audio
{
public:
	static bool Init();

	static void SetVolume(float value);
	static float GetVolume();
	static bool LoadSound(std::string id, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);
	static void Play(std::string sound, glm::vec3 pos);
	static void Stop(std::string sound);
	static int GetSourceCount();
	static void Quit();
	static void Step();


private:
	static ALCdevice* device;
	static ALCcontext* context;

	static bool Initialized;
	static float Volume;

	static size_t SourceCount;

	static std::vector<Source> Sources;
	static std::vector<Source> RSource;

	static std::map<std::string, unsigned int> buffers;

	static std::map<std::string, glm::vec3> queue;
};

