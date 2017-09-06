#ifndef _H_AGENT_URAV_H
#define _H_AGENT_URAV_H

#include "STDAgent.h"
#include "../GUI/MiniMap.h"

using namespace Ogre;

class URAV : public STDAgent
{
public:
	URAV(const std::string& name, const std::string& modelName);
	virtual ~URAV();

public:
	const Vector3& getLaserTarget() const { return mLaserTarget; }
	MiniMap* createFrontView(const std::string& mapTitle, int mapResolution, const Real& mapWidth, const Real& mapHeight, const float& mapAlpha, const Vector3& offset);
	MiniMap* getFrontView() { return mFrontView; }
	void createCannonLaser();
	Vector3 getCannonPosition() const;
	void switchMainView(bool enabled);
	bool hasMainView() const;
	void setDriverPosition(const Vector3& pos) { mDriverPosition=pos; }

public:
	virtual void run(const Real& dt);

private:
	Vector3 mLaserTarget;
	Vector3 mDriverPosition;
	MiniMap* mFrontView;
	ManualObject* mCannonLaser;
};
#endif
