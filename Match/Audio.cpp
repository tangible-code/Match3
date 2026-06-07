#include "Audio.h"

ALCdevice* Audio::device = NULL;
ALCcontext* Audio::context = NULL;
bool Audio::Initialized = false;
float Audio::Volume;
size_t Audio::SourceCount = 0;

std::map<std::string, unsigned int> Audio::buffers;

std::map<std::string, glm::vec3> Audio::queue;

std::vector<Source> Audio::Sources;
std::vector<Source> Audio::RSource;

bool Audio::Init()
{

	device = alcOpenDevice(NULL);
	if (!device) {
		printf("could not open AL device\n");
		return false;
	}

	context = alcCreateContext(device, NULL);
	if (!context) {
		printf("could not create Al Context\n");
		return false;
	}

	alcMakeContextCurrent(context);
	

	float position[] = { 0.0f,0.0f,0.0f };
	float velocity[] = { 0.0f,0.0f,0.0f };
	float orintation[] = { 0.0f,0.0f,-1.0f, 0.0f,1.0f,0.0f };

	alListenerfv(AL_POSITION, position);
	alListenerfv(AL_VELOCITY, velocity);
	alListenerfv(AL_ORIENTATION, orintation);

	Initialized = true;
	return Initialized;
}



void Audio::SetVolume(float value)
{
	if (!Initialized) {
		return;
	}
	Volume = std::max(0.0f, std::min(1.0f, value));
	alListenerf(AL_GAIN, Volume);
}

float Audio::GetVolume()
{
	return Volume;
}

bool Audio::LoadSound(std::string id, ALenum format, ALvoid* data, ALsizei size, ALsizei freq)
{
	if (!Initialized) {
		return false;
	}
	auto check = buffers.find(id);
	if (check != buffers.end()) {
		printf("the given id:%s is duplicate\n", id.c_str());
		return false;
	}

	unsigned int sound = 0;
	alGenBuffers(1, &sound);
	alBufferData(sound, format, data, size, freq);
	buffers.insert({ id,sound });
	return true;
}

void Audio::Play(std::string sound, glm::vec3 pos)
{
	//TODO check if sound exist
	if (!Initialized) {
		return;
	}
	queue.emplace(sound,pos);
}

void Audio::Stop(std::string sound)
{
	for (auto it = Sources.begin(); it != Sources.end(); it++) {
		if (it->currentSound == sound)
		{
			ALint state;
			alGetSourcei(it->id, AL_SOURCE_STATE, &state);

			if (state == AL_PLAYING) {
				alSourceStop(it->id);
			}
			
		}
	}
}

int Audio::GetSourceCount()
{
	return (int)SourceCount;
}

void Audio::Quit()
{
	if (!Initialized) {
		return;
	}
	for (Source& s : Sources) {
		alSourcei(s.id, AL_BUFFER, 0);
		alDeleteSources(1, &s.id);
	}
	Sources.clear();
	for (Source& s : RSource) {
		alSourcei(s.id, AL_BUFFER, 0);
		alDeleteSources(1, &s.id);
	}
	RSource.clear();
	queue.clear();
	for (auto& b : buffers) {
		alDeleteBuffers(1, &b.second);
	}
	buffers.clear();
	alcDestroyContext(context);
	alcCloseDevice(device);
}


void Audio::Step()
{
	if (!Initialized) {
		return;
	}


	//cheking source status
	//if they are playing continue if not move them to recycle source

	std::vector<Source> tSource;
	for (auto it = Sources.begin(); it != Sources.end(); it++) {
		ALint state;
		alGetSourcei(it->id, AL_SOURCE_STATE, &state);

		if (state == AL_PLAYING) {
			tSource.push_back(*it);
		}
		else {
			alSourcei(it->id, AL_BUFFER, 0);
			RSource.push_back(*it);
		}
	}
	tSource.swap(Sources);
	//ERRORR dont know but the for loop vector is modified inside the loop wich result in crash maybe ranged for loop might work but how to erasw the source hmm maybe using the source code design creating a new vector to store playing source then swaping the content with swap

	for (auto it = queue.begin(); it != queue.end(); it++) {
		unsigned int source = 0;
		if (RSource.empty()) {
			alGenSources(1, &source);
			SourceCount++;
		}
		else {
			source = RSource.back().id;
			RSource.pop_back();
		}
		alSource3f(source, AL_POSITION, it->second.x, it->second.y, it->second.z);
		alSourcei(source, AL_BUFFER, buffers[it->first]);
		Sources.push_back(Source(it->first, source));
		alSourcePlay(source);
	}
	//ERRORR queue was not cleared multiple duplicates
	queue.clear();
	//check if there are free source if not make new one
	//if queue is there then move the source to the request location load the buffer then play

	//TODO: vector to store all the source for memory reasons
}


