#include "stdafx.h"
#include "URAV.h"
#include "../STDEngine/STDEngine.h"
#include "../GUI/GUIManager.h"
#include "../OgreRayUtil/OgreRayUtil.h"
#include <sstream>

URAV::URAV(const std::string& name, const std::string& modelName)
: STDAgent(name, modelName)
, mFrontView(NULL)
, mLaserTarget(0, 1000, 0)
{

}

URAV::~URAV()
{
	if(mFrontView != NULL)
	{
		delete mFrontView;
		mFrontView=NULL;
	}

	if(mCannonLaser != NULL)
	{
		std::ostringstream oss;
		oss << this->getName() << "CannonLaser";

		SceneManager* sm=STDEngine::getSingletonPtr()->getSceneMgr();

		SceneNode* laserNode=mCannonLaser->getParentSceneNode();
		laserNode->detachAllObjects();
		sm->getRootSceneNode()->removeAndDestroyChild(laserNode->getName());

		sm->destroyManualObject(oss.str());
		mCannonLaser=NULL;
	}
}

MiniMap* URAV::createFrontView(const std::string& mapTitle, int mapResolution, const Real& mapWidth, const Real& mapHeight, const float& mapAlpha, const Vector3& offset)
{
	std::ostringstream oss;
	oss << this->getName() << "FrontView";

	RenderWindow* rw=STDEngine::getSingletonPtr()->getWindow();
	SceneManager* sm=STDEngine::getSingletonPtr()->getSceneMgr();

	mFrontView=new MiniMap(oss.str(), rw, sm, GUIManager::getSingletonPtr()->getGUIRenderer(), mapResolution);
	mFrontView->setSize(mapWidth, mapHeight);
	mFrontView->setPosition(0.0f, 1.0f - mapHeight);
	mFrontView->setAlpha(mapAlpha);
	mFrontView->getOgreCamera()->getParentSceneNode()->yaw(Degree(180));
	mFrontView->getMinimapWindow()->setText(mapTitle);
	
	this->addCamera("frontalCam", mFrontView->getOgreCamera(), offset);
	return mFrontView;
}

void URAV::createCannonLaser()
{
	std::ostringstream oss;
	oss << this->getName() << "CannonLaser";

	STDEngine* app=STDEngine::getSingletonPtr();

	app->createOrRetrieveColor("green", 0, 1, 0);

	SceneManager* sm=app->getSceneMgr();
	mCannonLaser=sm->createManualObject(oss.str());

	SceneNode* parentSceneNode=sm->getRootSceneNode();
	parentSceneNode->createChildSceneNode()->attachObject(mCannonLaser);
}

void URAV::run(const Real& dt)
{
	STDAgent::run(dt);

	Vector3 center=this->getPosition();
	mLaserTarget=center+this->getCamDirection("frontalCam") * 1000;
	OgreRayUtil::RaycastFromPoint(STDEngine::getSingletonPtr()->getRaySceneQuery(), center, this->getCamDirection("frontalCam"), mLaserTarget);

	if(mCannonLaser != NULL)
	{
		mCannonLaser->clear();
		mCannonLaser->begin("green", RenderOperation::OT_LINE_LIST);
		mCannonLaser->position(center);
		mCannonLaser->position(mLaserTarget);
		mCannonLaser->end();
	}
}

Vector3 URAV::getCannonPosition() const
{
	return this->getPosition();
}

void URAV::switchMainView(bool enabled)
{
	if(enabled)
	{
		if(!this->hasCamera("backCam"))
		{
			this->addCamera("backCam", STDEngine::getSingletonPtr()->getCamera(), mDriverPosition);
			this->getCamera("backCam")->getParentSceneNode()->yaw(Degree(180));
		}
	}
	else
	{
		if(this->hasCamera("backCam"))
		{
			this->getCamera("backCam")->getParentSceneNode()->yaw(Degree(-180));
			this->removeCamera("backCam");
		}
	}
}

bool URAV::hasMainView() const
{
	return this->hasCamera("backCam");
}