#ifndef _H_STD_AGENT_H
#define _H_STD_AGENT_H

#include <Ogre.h>
#include <string>
#include <vector>
#include <map>
#include "../STDWeapons/Gun.h"

using namespace Ogre;

class STDAgent
{
public:
	STDAgent(const std::string& name, const std::string& modelName);
	virtual ~STDAgent();

protected:
	Entity* mEntity;
	SceneNode* mNode;
	std::string mEntityName;
	std::string mNodeName;
	std::string mAgentName;
	AnimationState* mAnimationState;
	std::map<std::string, Camera*> mCameras;
	bool mSelected;
	Real mSpeed;
	Real mTurnSpeed;
	Real mHoverSpeed;
	bool mAnimable;
	Real mElevation;
	Vector3 mAlignment;
	std::vector<std::string> mAnimationStateNames;
	Gun* mGun;
	Real mCamScanningSpeed;
	Real mCollisionRadius;
	Real mLife;
	bool mDestructable;
	std::string mExplosionPSName;
	Real mExplosionDuration;
	float mScore;

public:
	void yaw(const Real& degree);
	void pitch(const Real& degree);
	void roll(const Real& degree);
	void setMaterial(const String& materialName);

public:
	virtual void run(const Real& dt);

protected:
	STDAgent* getEnemy() const;
public:
	void addEnemy(STDAgent* agnt) { mEnemies.insert(agnt); }
private:
	std::set<STDAgent*> mEnemies;
	float mSensorRange;

public:
	bool getReward(STDAgent* agnt, float reward);
	float getScore() const { return mScore; }

public:
	void setPosition(const Vector3& pos);
	const Vector3& getPosition() const { return mNode->getPosition(); }
	void setPosition(const Real& x, const Real& y, const Real& z);
	void setScale(Real x, Real y, Real z);
	void setAnimationState(const std::string& animation);
	void setSelected(bool selected);
	bool isSelected() const;
	bool hasEntity(MovableObject* mo) const;
	void move(Real dt, bool landcraft=true);
	void hover(Real dt);
	void turn(Real dt);
	const std::vector<std::string>& getAnimationStateNames() const { return mAnimationStateNames; }
	bool isAnimable() const;
	void setElevation(const Real& value);
	void setAlignment(const Vector3& align);
	Vector3 getCenter() const;
	Gun* getGun() { return mGun; }
	Gun* createGun();
	Entity* getEntity() { return mEntity; }
	Real getCamScanningSpeed() const { return mCamScanningSpeed; }
	void setCamScanningSpeed(const Real& val) { mCamScanningSpeed=val; }
	void setSpeed(const Real& val) { mSpeed=val; }
	void setRotatingSpeed(const Real& val) { mTurnSpeed=val; }
	void setHoverSpeed(const Real& val) { mHoverSpeed=val; }
	Vector3 getDirection() const;
	Real getLife() const { return mLife; }
	void setLife(const Real& life) { mLife=life; }
	const std::string& getName() const { return mAgentName; }
	void getHit(const Real& damage);
	Real getCollisionRadius() const { return mCollisionRadius; }
	void setCollisionRadius(const Real& radius) { mCollisionRadius=radius; }
	void setDestructable(bool destructable) { mDestructable=destructable; }
	void setExplosionParticleSystem(const std::string& psName) { mExplosionPSName=psName; }
	void setExplosionDuration(const Real& d) { mExplosionDuration=d; }

public:
	void addCamera(const std::string& camName, Camera* cam, const Vector3& offset);
	void removeCamera(const std::string& camName);
	bool hasCamera(const std::string& camName) const;
	Ogre::Camera* getCamera(const std::string& camName);
	Vector3 getCamDirection(const std::string& camName) const;
	Vector3 getCamOffsetPosition(const std::string& camName) const;
	void pitchCamera(const std::string& camName, const Real& dt, const Real& speed);
	void yawCamera(const std::string& camName, const Real& dt, const Real& speed);
};
#endif