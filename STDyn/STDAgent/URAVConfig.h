#ifndef _H_STD_URAV_CONFIG_H
#define _H_STD_URAV_CONFIG_H

#include <string>
#include <Ogre.h>

using namespace Ogre;

class URAVConfig
{
public:
	URAVConfig();
	virtual ~URAVConfig() { }

public:
	void load(const std::string& filename);

public:
	int mapResolution;
	std::string mapTitle;
	Real scale;
	float mapWidth;
	float mapAlpha;
	std::string meshName;
	Real frontalCamX;
	Real frontalCamY;
	Real frontalCamZ;
	Real backCamX;
	Real backCamY;
	Real backCamZ;
	Real posX;
	Real posY;
	Real posZ;
	Real speed;
	Real rotatingSpeed;
	Real camScanningSpeed;
	Real hoveringSpeed;
	Real life;
	Real collisionRadius;
	std::string explosionPSName;
	Real explosionDuration;
};
#endif