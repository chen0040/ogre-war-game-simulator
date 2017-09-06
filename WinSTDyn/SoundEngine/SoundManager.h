#ifndef _ML_LAB_SOUNDMANAGER_H__
#define _ML_LAB_SOUNDMANAGER_H__

#include <map>
#include <string>

#ifdef _SM_FMOD
#include "fmod/fmod.h"
#else
#include "audiere/audiere.h"
using namespace audiere;
#endif

class SoundManager
{
public:
	virtual ~SoundManager();

private:
	SoundManager();
	SoundManager(const SoundManager& rhs) { }
	SoundManager& operator=(const SoundManager& rhs) { return *this; }

public:
	static SoundManager* getSingletonPtr();
	static SoundManager& getSingleton();
	virtual void play(const std::string& sound);
	virtual void playThemeMusic(const std::string& song, bool looped=true);
	virtual void stopThemeMusic();
	virtual void unpauseThemeMusic();
	virtual void pauseThemeMusic();
	virtual bool isThemeMusic(const std::string& song);

protected:
#ifdef _SM_FMOD
	FSOUND_SAMPLE* get_sample(const std::string& sound_file);
	FMUSIC_MODULE* get_music(const std::string& music_file);
#else
	OutputStreamPtr get_sample(const std::string& sound_file);
	OutputStreamPtr get_music(const std::string& music_file);
#endif

protected:
	static SoundManager* mSingleton;

public:
#ifdef _SM_FMOD
	std::map<std::string, FSOUND_SAMPLE*> m_samples;
	std::map<std::string, FMUSIC_MODULE*> m_songs;
#endif

	std::map<std::string, std::string> mSoundFiles;
	std::map<std::string, std::string> mSongFiles;

#ifdef _SM_FMOD
	FMUSIC_MODULE* m_theme_music;
#else
	AudioDevicePtr mDevice;
	OutputStreamPtr m_theme_music;
#endif
};

#endif 