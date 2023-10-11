#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/alut.h>
#include <Miracle/Miracle.hpp>

using namespace Miracle;

bool SoundInitialized = false;

void InitSound() {
	if (SoundInitialized)
		return;

	// Initialize OpenAL
	ALboolean success = alutInit(NULL, NULL);
	if (success == AL_FALSE) {
		Logger::error("Error initializing OpenAL");
		exit(-1);
	}

	SoundInitialized = true;
}

void DeinitSound() {
	alutExit();
}

class Sound {
private:
	ALuint m_buffer = 0;
	ALuint m_source = 0;

public:
	Sound(const char* file) {
		InitSound();

		// Create a new OpenAL buffer and source
		ALuint buffer;
		ALuint source;
		alGenBuffers(1, &buffer);
		alGenSources(1, &source);

		ALenum format;
		void* data;
		ALsizei size;
		ALsizei freq;
		ALboolean loop;

		// Load the audio file into the buffer
		alutLoadWAVFile((ALbyte*)file, &format, &data, &size, &freq, &loop);

		// Fill the buffer with audio data
		alBufferData(buffer, format, data, size, freq);

		// Set the source properties
		alSourcei(source, AL_BUFFER, buffer);
		m_buffer = buffer;
		m_source = source;
	}

	~Sound() {
		alDeleteSources(1, &m_source);
		alDeleteBuffers(1, &m_buffer);
	}

	void SetLoop(bool loop) {
		alSourcei(m_source, AL_LOOPING, (ALboolean)loop);
	}

	void Play() {
		alSourcePlay(m_source);
	}

	void Stop() {
		alSourceStop(m_source);
	}
};