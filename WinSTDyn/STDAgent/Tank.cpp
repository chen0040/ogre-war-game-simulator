#include "stdafx.h"
#include "Tank.h"
#include "../STDEngine/STDEngine.h"
#include "../GUI/GUIManager.h"
#include "../OgreRayUtil/OgreRayUtil.h"
#include <sstream>

Tank::Tank(const std::string& name, const std::string& meshName)
: STDAgent(name, meshName)
, mCannonView(NULL)
, mLaserTarget(0, -1000, 0)
, mCannonLaser(NULL)
, mTrackingLaser(NULL)
{
	
}

Tank::~Tank()
{
	SceneManager* sm=STDEngine::getSingletonPtr()->getSceneMgr();

	if(mCannonLaser != NULL)
	{
		std::ostringstream oss;
		oss << this->getName() << "CannonLaser";

		SceneNode* laserNode=mCannonLaser->getParentSceneNode();
		laserNode->detachAllObjects();
		sm->getRootSceneNode()->removeAndDestroyChild(laserNode->getName());

		sm->destroyManualObject(oss.str());
		mCannonLaser=NULL;
	}

	if(mTrackingLaser != NULL)
	{
		std::ostringstream oss;
		oss << this->getName() << "TrackingLaser";

		SceneNode* laserNode=mTrackingLaser->getParentSceneNode();
		laserNode->detachAllObjects();
		sm->getRootSceneNode()->removeAndDestroyChild(laserNode->getName());

		sm->destroyManualObject(oss.str());
		mTrackingLaser=NULL;
	}

	if(mCannonView != NULL)
	{
		delete mCannonView;
		mCannonView=NULL;
	}
}

Vector3 Tank::getCannonPosition() const
{
	return this->getPosition() + Vector3(0, 30, 0);
}

MiniMap* Tank::createCannonView(const std::string& mapTitle, int mapResolution, const Real& mapWidth, const Real& mapHeight, const float& mapAlpha, const Vector3& offset)
{
	std::ostringstream oss;
	oss << this->getName() << "CannonView";

	RenderWindow* rw=STDEngine::getSingletonPtr()->getWindow();
	SceneManager* sm=STDEngine::getSingletonPtr()->getSceneMgr();

	mCannonView=new MiniMap(oss.str(), rw, sm, GUIManager::getSingletonPtr()->getGUIRenderer(), mapResolution);
	mCannonView->setSize(mapWidth, mapHeight);
	mCannonView->setPosition(mapWidth, 1.0f - mapHeight);
	mCannonView->setAlpha(mapAlpha);
	mCannonView->getMinimapWindow()->setText(mapTitle);

	this->addCamera("CannonCam", mCannonView->getOgreCamera(), offset);

	return mCannonView;
}

void Tank::createCannonLaser()
{
	std::ostringstream oss;
	oss << this->getName() << "CannonLaser";

	STDEngine* app=STDEngine::getSingletonPtr();

	app->createOrRetrieveColor("red", 1, 0, 0);

	SceneManager* sm=app->getSceneMgr();
	mCannonLaser=sm->createManualObject(oss.str());

	SceneNode* parentSceneNode=sm->getRootSceneNode();
	parentSceneNode->createChildSceneNode()->attachObject(mCannonLaser);
}

void Tank::track(STDAgent* trackedAgent)
{
	mTrackedAgent=trackedAgent;

	std::ostringstream oss;
	oss << this->getName() << "TrackingLaser";

	STDEngine* app=STDEngine::getSingletonPtr();

	app->createOrRetrieveColor("blue", 0, 0, 1);

	SceneManager* sm=app->getSceneMgr();
	mTrackingLaser=sm->createManualObject(oss.str());

	SceneNode* parentSceneNode=sm->getRootSceneNode();
	parentSceneNode->createChildSceneNode()->attachObject(mTrackingLaser);
}

bool Tank::getReward(STDAgent* agnt, float reward)
{
	STDAgent::getReward(agnt, reward);
	return true;
}

void Tank::run(const Ogre::Real &dt)
{
	STDAgent::run(dt);

	if(mCannonLaser != NULL)
	{
		std::set<MovableObject*> excludedObjs;
		excludedObjs.insert(this->getEntity());

		Vector3 cannonPosition=this->getCannonPosition();
		mLaserTarget=cannonPosition+this->getCamDirection("CannonCam") * 1000;
		OgreRayUtil::RaycastFromPoint(STDEngine::getSingletonPtr()->getRaySceneQuery(), cannonPosition, this->getCamDirection("CannonCam"), mLaserTarget, excludedObjs);

		mCannonLaser->clear();
		mCannonLaser->begin("red", RenderOperation::OT_LINE_LIST);
		mCannonLaser->position(this->getCannonPosition());
		mCannonLaser->position(mLaserTarget);
		mCannonLaser->end();
	}

	if(mTrackingLaser != NULL)
	{
		mTrackingLaser->clear();

		Vector3 tankCenter=this->getPosition();
		tankCenter.y+=30;
		Vector3 targetCenter=mTrackedAgent->getPosition();

		mTrackingLaser->begin("blue", RenderOperation::OT_LINE_LIST);
		mTrackingLaser->position(tankCenter);
		mTrackingLaser->position(targetCenter);
		mTrackingLaser->end();
	}
}

Vector3 Tank::getCannonDirection() const
{
	return this->getCamDirection("CannonCam");
}

void Tank::switchMainView(bool enabled)
{
	if(enabled)
	{
		if(!this->hasCamera("backCam"))
		{
			this->addCamera("backCam", STDEngine::getSingletonPtr()->getCamera(), mDriverPosition);
		}
	}
	else
	{
		if(this->hasCamera("backCam"))
		{
			this->removeCamera("backCam");
		}
	}
}