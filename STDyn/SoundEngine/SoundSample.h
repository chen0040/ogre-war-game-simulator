#ifndef _H_ML_LAB_SOUND_SAMPLE_H
#define _H_ML_LAB_SOUND_SAMPLE_H

#include "../tinyxml/tinyxml.h"

class SoundSample
{
public:
	SoundSample(TiXmlElement* xmlElement);
	virtual ~SoundSample();
};
#endif