#ifndef _H_STD_TANK_CONFIG_H
#define _H_STD_TANK_CONFIG_H

#include <string>
#include <vector>
#include <Ogre.h>
#include "Tank.h"

using namespace Ogre;

class TankConfig
{
public:
	TankConfig();
	virtual ~TankConfig();

public:
	void load(const std::string& filename);
	Tank* createTank(const std::string& name, bool createCannonView);
	void addTank(Tank* agent);
	void updateTanks(const Real& dt);
	void resetTankMotion();
	void resetCannonMotion();
	void turnLeft(bool turn) { mTurnLeft=turn; }
	void turnRight(bool turn) { mTurnRight=turn; }
	void moveForward(bool move) { mMoveForward=move; }
	void moveBackward(bool move) { mMoveBackward=move; }
	void turnCannonLeft(bool turn) { mTurnCannonLeft=turn; }
	void turnCannonRight(bool turn) { mTurnCannonRight=turn; }
	void turnCannonUp(bool turn) { mTurnCannonUp=turn; }
	void turnCannonDown(bool turn) { mTurnCannonDown=turn; }
	void shoot(bool enabled) { mShoot=enabled; }
	bool isLaserConnected(const Vector3& laser1, const Vector3& laser2) const;

private:
	bool mTurnLeft;
	bool mTurnRight;
	bool mMoveForward;
	bool mMoveBackward;
	bool mTurnCannonLeft;
	bool mTurnCannonRight;
	bool mTurnCannonUp;
	bool mTurnCannonDown;
	bool mShoot;

private:
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
	std::vector<Tank*> mTanks;
};

#endif

