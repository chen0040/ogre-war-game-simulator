#ifndef _H_AGENT_TANK_H
#define _H_AGENT_TANK_H

#include "STDAgent.h"
#include "../GUI/MiniMap.h"

using namespace Ogre;

class Tank : public STDAgent
{
public:
	Tank(const std::string& name, const std::string& modelName);
	virtual ~Tank();

public:
	Vector3 getCannonPosition() const;
	const Vector3& getLaserTarget() const { return mLaserTarget; }
	MiniMap* createCannonView(const std::string& mapTitle, int mapResolution, const Real& mapWidth, const Real& mapHeight, const float& mapAlpha, const Vector3& offset);
	MiniMap* getCannonView() { return mCannonView; }
	void createCannonLaser();
	void track(STDAgent* trackedAgent);
	Vector3 getCannonDirection() const;
	void switchMainView(bool enabled);
	void setDriverPosition(const Vector3& pt) { mDriverPosition=pt; }

public:
	virtual void run(const Real& dt);
	virtual bool getReward(STDAgent* agnt, float reward);

private:
	MiniMap* mCannonView;
	Vector3 mLaserTarget;
	ManualObject* mCannonLaser;
	ManualObject* mTrackingLaser;
	STDAgent* mTrackedAgent;
	Vector3 mDriverPosition;
};

#endif