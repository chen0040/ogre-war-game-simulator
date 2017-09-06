#include "stdafx.h"
#include "STDParticleSystem.h"
#include "../STDEngine/STDEngine.h"

STDParticleSystem::STDParticleSystem(const std::string& id, const std::string& psName)
: mMaxDuration(10)
, mVisible(true)
{
	mParticleSystem=STDEngine::getSingletonPtr()->getSceneMgr()->createParticleSystem(id, psName);

	std::ostringstream oss;
	oss << id << "Node";
	mNode=STDEngine::getSingletonPtr()->getSceneMgr()->getRootSceneNode()->createChildSceneNode(oss.str());
	mNode->attachObject(mParticleSystem);
	mDuration=0;
}

STDParticleSystem::~STDParticleSystem()
{
	mNode->detachAllObjects();
	STDEngine::getSingletonPtr()->getSceneMgr()->getRootSceneNode()->removeAndDestroyChild(mNode->getName());
	STDEngine::getSingletonPtr()->getSceneMgr()->destroyParticleSystem(mParticleSystem->getName());
}

void STDParticleSystem::update(const Real& dt)
{
	mDuration += dt;
}

void STDParticleSystem::setPosition(const Vector3& pos)
{
	mNode->setPosition(pos);
}

void STDParticleSystem::setVisible(bool visible)
{
	mVisible=visible;
	mNode->setVisible(mVisible);
}