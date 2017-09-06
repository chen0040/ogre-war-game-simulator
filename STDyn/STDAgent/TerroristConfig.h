#ifndef _H_STD_TERRORIST_CONFIG_H
#define _H_STD_TERRORIST_CONFIG_H

#include <string>
#include <Ogre.h>

using namespace Ogre;

class TerroristConfig
{
public:
	TerroristConfig();
	virtual ~TerroristConfig() { }

public:
	void load(const std::string& filename);

public:
	Real scale;
	Real life;
	std::string meshName;
	Real elevation;
	std::string animation;
	Real depth;
	Real width;
	std::string explosionPSName;
	Real explosionDuration;
};

#endif