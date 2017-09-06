#ifndef _H_STD_TANK_CONFIG_H
#define _H_STD_TANK_CONFIG_H

#include <string>
#include <Ogre.h>

using namespace Ogre;

class TankConfig
{
public:
	TankConfig();
	virtual ~TankConfig() { }

public:
	void load(const std::string& filename);

public:
	int mapResolution;
	std::string mapTitle;
	Real scale;
	Real elevation;
	float mapWidth;
	float mapAlpha;
	std::string meshName;
	Real backCamX;
	Real backCamY;
	Real backCamZ;
	Real cannonCamX;
	Real cannonCamY;
	Real cannonCamZ;
	Real tankPosX;
	Real tankPosZ;
	Real missileSpeed;
	Real firingInterval;
	Real tankSpeed;
	Real tankRotatingSpeed;
	Real tankCamScanningSpeed;
	Real hitRange;
	Real damageRadius;
	Real life;
	Real missileImpact;
	Real collisionRadius;
	Real laserConnectLimit;
	std::string explosionPSName;
	Real explosionDuration;
};

#endif

