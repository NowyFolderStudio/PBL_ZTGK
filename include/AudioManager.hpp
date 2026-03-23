#pragma once

#include <fmod.hpp>
#include <string>
#include <iostream>

class AudioManager {
public:
	AudioManager();
	~AudioManager();

	bool Init();
	void Update();
	void LoopSound(const std::string& soundFile, float volume);
	void Cleanup();

private:
	FMOD::System* fmodSystem;
};