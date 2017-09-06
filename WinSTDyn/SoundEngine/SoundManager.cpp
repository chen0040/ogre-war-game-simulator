#include "stdafx.h"
#include "SoundManager.h"
#include <iostream>
#include <Ogre.h>
#include <sstream>
#include "../OSEnvironment/OSEnvironment.h"
#include "../tinyxml/tinyxml.h"

SoundManager* SoundManager::mSingleton=NULL;

SoundManager::SoundManager()
: m_theme_music(NULL)
{
#ifdef _SM_FMOD
	if(!FSOUND_Init (44100, 32, FSOUND_INIT_USEDEFAULTMIDISYNTH))
	{
		throw Ogre::Exception(42, "Failed to initialize FMOD", "SoundManager::SoundManager()");
	}
#else
	mDevice=OpenDevice();
	if(!mDevice) 
	{
		throw Ogre::Exception(42, "Failed to initialize Audiere", "SoundManager::SoundManager()");
	}
#endif
	

	TiXmlDocument doc;
	if(!doc.LoadFile(OSEnvironment::getFullPath("..\\config\\SoundEngine.xml").c_str()))
	{
		throw Ogre::Exception(42, "Failed to load SoundEngine.xml", "SoundManager::SoundManager()");
	}

	TiXmlElement* xmlRoot=doc.RootElement();
	for(TiXmlElement* xmlLevel1=xmlRoot->FirstChildElement(); xmlLevel1 != NULL; xmlLevel1=xmlLevel1->NextSiblingElement())
	{
		if(strcmp(xmlLevel1->Value(), "sound")==0)
		{
			std::string name=xmlLevel1->Attribute("name");
			std::string src=OSEnvironment::getFullPath(xmlLevel1->Attribute("src"));

			if(!OSEnvironment::exists(src))
			{
				std::ostringstream osse;
				osse << "Failed to load \"" << src << "\"";
				throw Ogre::Exception(42, osse.str().c_str(), "SoundManager::SoundManager()");
			}

			mSoundFiles[name]=src;
		}
		else if(strcmp(xmlLevel1->Value(), "music")==0)
		{
			std::string name=xmlLevel1->Attribute("name");
			std::string src=OSEnvironment::getFullPath(xmlLevel1->Attribute("src"));

			if(!OSEnvironment::exists(src))
			{
				std::ostringstream osse;
				osse << "Failed to load \"" << src << "\"";
				throw Ogre::Exception(42, osse.str().c_str(), "SoundManager::SoundManager()");
			}

			mSongFiles[name]=src;
		}
	}
}

SoundManager::~SoundManager()
{
	m_theme_music=NULL;
#ifdef _SM_FMOD
	FMUSIC_StopAllSongs();

	std::map<std::string, FSOUND_SAMPLE*>::iterator pos;
	for(pos=m_samples.begin(); pos != m_samples.end(); ++pos)
	{
		FSOUND_Sample_Free(pos->second);
	}
	std::map<std::string, FMUSIC_MODULE*>::iterator posm;
	for(posm=m_songs.begin(); posm != m_songs.end(); ++posm)
	{
		FMUSIC_FreeSong(posm->second);
	}
	FSOUND_Close();
#else
#endif
}

SoundManager* SoundManager::getSingletonPtr()
{
	if(mSingleton == NULL)
	{
		mSingleton=new SoundManager();
	}
	return mSingleton;
}

SoundManager& SoundManager::getSingleton()
{
	if(mSingleton == NULL)
	{
		mSingleton=new SoundManager();
	}
	return *mSingleton;
}

void SoundManager::play(const std::string& sound)
{
	std::map<std::string, std::string>::iterator fnd=mSoundFiles.find(sound);
	if(fnd==mSoundFiles.end())
	{
		std::ostringstream osse;
		osse << "Sound (key: " << sound << ") is not found";
		throw Ogre::Exception(42, osse.str().c_str(), "play(const std::string& sound)");
	}

	const std::string& sound_file=fnd->second;

#ifdef _SM_FMOD
	FSOUND_PlaySound(FSOUND_FREE, get_sample(sound_file));
#else
	get_sample(sound_file)->play();
#endif
}

#ifdef _SM_FMOD
FSOUND_SAMPLE* SoundManager::get_sample(const std::string& sound_file)
{
	std::map<std::string, FSOUND_SAMPLE*>::const_iterator pos=m_samples.find(sound_file);
	if(pos!=m_samples.end())
	{
		return pos->second;
	}

	FSOUND_SAMPLE* pSample=FSOUND_Sample_Load(FSOUND_UNMANAGED, sound_file.c_str(), FSOUND_NORMAL, 0, 0);  
	if(pSample==NULL)
	{
		throw Ogre::Exception(42, "pSample==NULL", "SoundManager::play(const std::string& sound_file)");
	}

	m_samples[sound_file]=pSample;

	return pSample;
}
#else
OutputStreamPtr SoundManager::get_sample(const std::string& sound_file)
{
	OutputStreamPtr sound(OpenSound(mDevice, sound_file.c_str(), false));
	if(!sound)
	{
		throw Ogre::Exception(42, "!sound", "SoundManager::get_sample(const std::string& sound_file)");
	}

	return sound;
}
#endif

#ifdef _SM_FMOD
FMUSIC_MODULE* SoundManager::get_music(const std::string& music_file)
{
	std::map<std::string, FMUSIC_MODULE*>::const_iterator pos=m_songs.find(music_file);
	if(pos!=m_songs.end())
	{
		return pos->second;
	}

	FMUSIC_MODULE* pSong=FMUSIC_LoadSong(music_file.c_str());
	m_songs[music_file]=pSong;

	return pSong;
}
#else
OutputStreamPtr SoundManager::get_music(const std::string& music_file)
{
	OutputStreamPtr stream(OpenSound(mDevice, music_file.c_str(), true));
	if(!stream)
	{
		throw Ogre::Exception(42, "!stream", "SoundManager::get_music(const std::string& music_file)");
	}

	return stream;
}
#endif

void SoundManager::playThemeMusic(const std::string& song, bool looped)
{
#ifdef _SM_FMOD
	std::map<std::string, std::string>::iterator fnd=mSongFiles.find(song);
	if(fnd==mSongFiles.end())
	{
		std::ostringstream osse;
		osse << "Song (key: " << song << ") is not found";
		throw Ogre::Exception(42, osse.str().c_str(), "playSong(const std::string& song)");
	}

	const std::string& song_file=fnd->second;

	FMUSIC_MODULE* music=get_music(song_file);
	if(m_theme_music==music)
	{
		return;
	}

	if(m_theme_music != NULL)
	{
		FMUSIC_StopSong(m_theme_music);
	}

	m_theme_music=get_music(song_file);

	FMUSIC_SetLooping(m_theme_music, looped);
	FMUSIC_PlaySong(m_theme_music);
#else
	if(m_theme_music==NULL)
	{
		OutputStreamPtr stream(OpenSound(mDevice, song.c_str(), true));
		if(!stream)
		{
			throw Ogre::Exception(42, "!sound", "SoundManager::playThemeMusic(const std::string& sound_file, bool looped)");
		}
		m_theme_music=stream;
	}
	m_theme_music->play();

#endif
}

void SoundManager::pauseThemeMusic()
{
#ifdef _SM_FMOD
	if(m_theme_music != NULL)
	{
		FMUSIC_SetPaused(m_theme_music, true);
	}
#else
#endif
}

bool SoundManager::isThemeMusic(const std::string& song)
{
#ifdef _SM_FMOD
	std::map<std::string, std::string>::const_iterator fnd=mSongFiles.find(song);
	if(fnd==mSongFiles.end())
	{
		std::ostringstream osse;
		osse << "Song (key: " << song << ") is not found";
		throw Ogre::Exception(42, osse.str().c_str(), "playSong(const std::string& song)");
	}

	const std::string& song_file=fnd->second;

	FMUSIC_MODULE* music=get_music(song_file);

	return m_theme_music==music;
#else
	return false;
#endif
}

void SoundManager::unpauseThemeMusic()
{
#ifdef _SM_FMOD
	if(m_theme_music != NULL)
	{
		FMUSIC_SetPaused(m_theme_music, false);
	}
#else
#endif
}

void SoundManager::stopThemeMusic()
{
#ifdef _SM_FMOD
	if(m_theme_music != NULL)
	{
		FMUSIC_StopSong(m_theme_music);
		m_theme_music=NULL;
	}
#else
#endif
}
