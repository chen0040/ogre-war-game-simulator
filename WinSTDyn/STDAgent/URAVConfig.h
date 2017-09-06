#ifndef _H_STD_URAV_CONFIG_H
#define _H_STD_URAV_CONFIG_H

#include <string>
#include <vector>
#include <Ogre.h>
#include "URAV.h"

using namespace Ogre;

class URAVConfig
{
public:
	URAVConfig();
	virtual ~URAVConfig();

public:
	void load(const std::string& filename);
	void updateURAVs(const Real& dt);
	void addURAV(URAV* agent);
	void resetURAVMotion();
	URAV* createURAV(const std::string& name, bool createFrontalView);
	Vector3 getMainViewCameraOffset() const;

private:
	bool mMoveForward;
	bool mMoveBackward;
	bool mTurnLeft;
	bool mTurnRight;
	bool mMoveUp;
	bool mMoveDown;

public:
	void turnLeft(bool turn) { mTurnLeft=turn; }
	void turnRight(bool turn) { mTurnRight=turn; }
	void moveForward(bool move) { mMoveForward=move; }
	void moveBackward(bool move) { mMoveBackward=move; }
	void moveUp(bool up) { mMoveUp=up; }
	void moveDown(bool down) { mMoveDown=down; }

private:
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
	std::vector<URAV*> mURAVs;
};
#endif