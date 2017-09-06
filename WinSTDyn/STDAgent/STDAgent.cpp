#include "stdafx.h"
#include "STDAgent.h"
#include "STDAgentManager.h"
#include "../STDEngine/STDEngine.h"
#include "../OSEnvironment/OSEnvironment.h"
#include "../World/WorldInfoManager.h"
#include "../STDParticleSystem/STDParticleSystemManager.h"

STDAgent::STDAgent(const std::string& name, const std::string& modelName)
: mAnimationState(NULL)
, mSelected(false)
, mSpeed(30)
, mElevation(0)
, mTurnSpeed(10)
, mHoverSpeed(10)
, mAlignment(Vector3::UNIT_X)
, mGun(NULL)
, mLife(1000)
, mCollisionRadius(2)
, mDestructable(true)
, mExplosionPSName("XS/Explosion")
, mExplosionDuration(1)
, mScore(0)
, mSensorRange(200)
{
	mAgentName=name;
	mEntityName=name;
	mNodeName=OSEnvironment::append(name, "Node");
	SceneManager* sceneMgr=STDEngine::getSingletonPtr()->getSceneMgr();
	mEntity=sceneMgr->createEntity(name, modelName);
	mNode=sceneMgr->getRootSceneNode()->createChildSceneNode(mNodeName);
	mNode->attachObject(mEntity);

	const AnimationStateSet* ass=mEntity->getAllAnimationStates();
	if(ass != NULL)
	{
		ConstAnimationStateIterator asi=ass->getAnimationStateIterator();
		while(asi.hasMoreElements())
		{
			mAnimationStateNames.push_back(asi.peekNextKey());
			asi.moveNext();
		}
		mAnimable=true;
	}
	else
	{
		mAnimable=false;
	}
}

Gun* STDAgent::createGun()
{
	mGun=new Gun(this);
	return mGun;
}

STDAgent::~STDAgent()
{
	if(mGun != NULL)
	{
		delete mGun;
		mGun=NULL;
	}

	SceneManager* sceneMgr=STDEngine::getSingletonPtr()->getSceneMgr();

	mNode->detachAllObjects();
	sceneMgr->getRootSceneNode()->removeAndDestroyChild(mNodeName);

	sceneMgr->destroyEntity(mEntityName);
}

void STDAgent::setPosition(const Vector3& pos)
{
	Vector3 pos1(pos.x, pos.y + mElevation, pos.z);
	mNode->setPosition(pos1);
}

void STDAgent::setPosition(const Real& x, const Real& y, const Real& z)
{
	this->setPosition(Vector3(x, y, z));
}

void STDAgent::setScale(Real x, Real y, Real z)
{
	mNode->scale(x, y, z);
}

bool STDAgent::getReward(STDAgent* agnt, float reward)
{
	std::set<STDAgent*>::iterator fnd=mEnemies.find(agnt);
	if(fnd != mEnemies.end())
	{
		this->mScore+=reward;
		return true;
	}
	return false;
}

STDAgent* STDAgent::getEnemy() const
{
	STDAgentManager* sam=STDAgentManager::getSingletonPtr();
	std::set<STDAgent*> agnts;
	sam->getAgentsWithinRadius(this->getPosition(), this->mSensorRange, agnts);
	std::set<STDAgent*>::const_iterator aiter=agnts.begin();
	std::set<STDAgent*>::const_iterator aiter_end=agnts.end();
	for(; aiter != aiter_end; ++aiter)
	{
		std::set<STDAgent*>::const_iterator fnd=mEnemies.find(*aiter);
		if(fnd != mEnemies.end())
		{
			return *fnd;
		}
	}
	return NULL;
}

void STDAgent::run(const Real& dt)
{
	if(mAnimationState != NULL)
	{
		mAnimationState->addTime(dt);
	}

	if(mGun != NULL)
	{
		mGun->update(dt);
	}
}

void STDAgent::yaw(const Real& degree)
{
	mNode->yaw(Degree(degree));
}
	
void STDAgent::pitch(const Real& degree)
{
	mNode->pitch(Degree(degree));
}
	
void STDAgent::roll(const Real& degree)
{
	mNode->roll(Degree(degree));
}

void STDAgent::setAnimationState(const std::string& animation)
{
	AnimationState* as=mEntity->getAnimationState(animation);
	if(mAnimationState != as)
	{
		if(mAnimationState != NULL)
		{
			mAnimationState->setEnabled(false);
			mAnimationState->setLoop(false);
		}
		mAnimationState=as;
		mAnimationState->setEnabled(true);
		mAnimationState->setLoop(true);
	}
}

void STDAgent::addCamera(const std::string& camName, Ogre::Camera *cam, const Vector3& offset)
{
	mCameras[camName]=cam;
	SceneNode* camNode=cam->getParentSceneNode();
	camNode->getParentSceneNode()->removeChild(camNode);
	mNode->addChild(camNode);
	camNode->setPosition(offset);
}

void STDAgent::removeCamera(const std::string& camName)
{
	std::map<std::string, Ogre::Camera*>::iterator fnd=mCameras.find(camName);
	if(fnd != mCameras.end())
	{
		mCameras.erase(fnd);
	}
}

bool STDAgent::hasCamera(const std::string& camName) const
{
	std::map<std::string, Ogre::Camera*>::const_iterator fnd=mCameras.find(camName);
	return fnd != mCameras.end();
}

Ogre::Camera* STDAgent::getCamera(const std::string& camName)
{
	std::map<std::string, Ogre::Camera*>::iterator fnd=mCameras.find(camName);
	if(fnd != mCameras.end())
	{
		return fnd->second;
	}
	else
	{
		return NULL;
	}
}

Vector3 STDAgent::getCamOffsetPosition(const std::string &camName) const
{
	std::map<std::string, Ogre::Camera*>::const_iterator fnd=mCameras.find(camName);
	if(fnd != mCameras.end())
	{
		return fnd->second->getParentSceneNode()->getPosition();
	}
	else
	{
		throw Exception(42, "fnd == mCameras.end()", "STDAgent::getCamPosition(const std::string&) const");
	}

	return Vector3(0, 0, 0);
}

Vector3 STDAgent::getCamDirection(const std::string& camName) const
{
//	Vector3 direction = mCameras[camName]->getParentSceneNode()->getOrientation() * (mNode->getOrientation() * mAlignment);
	

	/*
	worldPos = parentWorldPos + parentWorldOrientation * position
	worldPos - parentWorldPos = parentWorldOrientation * position
	inv(parentWorldOrientation) * (worldPos-parentWorldPos) = inv(parentWorldOrientation) * parentWorldOrientation * position
	position = inv(parentWorldOrientation) * (worldPos-parentWorldPos)
	*/

	std::map<std::string, Ogre::Camera*>::const_iterator fnd=mCameras.find(camName);
	if(fnd != mCameras.end())
	{
		Vector3 direction = fnd->second->getParentSceneNode()->_getDerivedOrientation() * mAlignment;
		direction.normalise();
		return direction;
	}
	else
	{
		throw Exception(42, "fnd == mCameras.end()", "STDAgent::getCamPosition(const std::string&) const");
		
	}

	return Vector3(0, 0, 0);
}

void STDAgent::setSelected(bool selected)
{
	mSelected=selected;
	mNode->showBoundingBox(selected);
} 

bool STDAgent::isSelected() const
{
	return mSelected;
}

bool STDAgent::hasEntity(MovableObject* mo) const
{
	return mEntity==mo;
}

void STDAgent::move(Real dt, bool landcraft)
{
	Vector3 direction=mNode->getOrientation() * mAlignment;
	direction.y=0;
	direction.normalise();
	
	Vector3 pos=mNode->getPosition() + direction * (dt * mSpeed);

	if(landcraft)
	{
		Vector3 pos1(pos.x, getWorldHeight(pos.x, pos.z), pos.z);
		this->setPosition(pos1);
	}
	else
	{
		this->setPosition(pos);
	}	
}

void STDAgent::turn(Real dt)
{
	this->yaw(mTurnSpeed * dt);
}

void STDAgent::hover(Real dt)
{
	this->setPosition(mNode->getPosition()+Vector3(0, mHoverSpeed * dt, 0));
}

bool STDAgent::isAnimable() const
{
	return mAnimable;
}

void STDAgent::setMaterial(const String& materialName)
{
	mEntity->setMaterialName(materialName);
}

void STDAgent::setElevation(const Real& value)
{
	mElevation=value;
}

void STDAgent::setAlignment(const Ogre::Vector3 &align)
{
	mAlignment=align;
}

Vector3 STDAgent::getCenter() const
{
	Vector3 pos=mNode->getPosition();
	pos.y+=(mEntity->getBoundingRadius() * mNode->getScale().y); 
	return pos;
}

void STDAgent::pitchCamera(const std::string& camName, const Real& dt, const Real& speed)
{
	mCameras[camName]->getParentSceneNode()->pitch(Degree(dt * speed));
}
	
void STDAgent::yawCamera(const std::string& camName, const Real& dt, const Real& speed)
{
	mCameras[camName]->getParentSceneNode()->yaw(Degree(dt * speed));
}

Vector3 STDAgent::getDirection() const
{
	Vector3 direction = mNode->getOrientation() * mAlignment;
	direction.normalise();

	return direction;
}

void STDAgent::getHit(const Ogre::Real &damage)
{
	if(mDestructable==false)
	{
		return;
	}

	Real pLife=mLife;
	 mLife-=damage;
	 if(mLife * pLife <= 0)
	 {
		 STDParticleSystem* ps=STDParticleSystemManager::getSingletonPtr()->createParticleSystem(mExplosionPSName, mExplosionDuration);
		ps->setPosition(this->getPosition());
	 }
}