#include "Projectile.h"
#include "../STDEngine/STDEngine.h"
#include "../STDAgent/STDAgent.h"
#include "../SoundEngine/SoundManager.h"
#include <sstream>
#include "Gun.h"

size_t Projectile::mProjectileCount=0;

Projectile::Projectile()
: mVisible(true)
, mState(MOVING)
, mExplosionCounter(0)
, mExplosionDuration(1)
, mHitRange(10)
, mDamageRadius(10)
{
	mProjectileCount++;

	SceneManager* sm=STDEngine::getSingletonPtr()->getSceneMgr();
	std::ostringstream oss1;
	oss1 << "ProjPS" << mProjectileCount;
	mProj=sm->createParticleSystem(oss1.str(), "Xianshun/LaserPulse");

	std::ostringstream oss2;
	oss2 << "ProjectileNode" << mProjectileCount;
	mProjectileNode=sm->getRootSceneNode()->createChildSceneNode(oss2.str());
	mProjectileNode->attachObject(mProj);
	mProjectileNode->setScale(0.05f, 0.05f, 0.05f);

	std::ostringstream oss3;
	oss3 << "ExpPS" << mProjectileCount;
	mExpPS=sm->createParticleSystem(oss3.str(), "Xianshun/Smoke");

	std::ostringstream oss4;
	oss4 << "ExplosionNode" << mProjectileCount;
	mExplosionNode=sm->getRootSceneNode()->createChildSceneNode(oss4.str());
	mExplosionNode->attachObject(mExpPS);
}

Projectile::~Projectile()
{
	SceneManager* sm=STDEngine::getSingletonPtr()->getSceneMgr();
	mProjectileNode->detachAllObjects();
	mExplosionNode->detachAllObjects();

	sm->getRootSceneNode()->removeAndDestroyChild(mProjectileNode->getName());
	sm->getRootSceneNode()->removeAndDestroyChild(mExplosionNode->getName());

	sm->destroyParticleSystem(mProj);
	sm->destroyParticleSystem(mExpPS);
}

void Projectile::setPosition(const Vector3& pos)
{
	mProjectileNode->setPosition(pos);
}

void Projectile::update(Real dt)
{
	switch(mState)
	{
	case EXPLODED:
		doExplosion(dt);
		break;
	case MOVING:
		doMove(dt);
		break;
	default:
		break;
	}
}

void Projectile::doExplosion(Real dt)
{
	mExplosionCounter+=dt;
	if(mExplosionCounter > mExplosionDuration)
	{
		shutdown();
	}
}

void Projectile::doMove(Real dt)
{
	const Vector3& pos1=mProjectileNode->getPosition();
	const Vector3& pos2=mExplosionNode->getPosition();

	Vector3 direction=pos2-pos1;
	Real distance=direction.normalise();

	if(distance < mHitRange)
	{
		explode();
		doExplosion(dt);
	}
	else
	{
		//only required for projectile object that need to be oriented
		/*
		Vector3 src=mProjectileNode->getOrientation() * Vector3::UNIT_X;
		if(1.0 + src.dotProduct(direction) < 0.001f)
		{
			mProjectileNode->yaw(Degree(180));
		}
		else
		{
			mProjectileNode->rotate(src.getRotationTo(direction));
		}
		*/

		mProjectileNode->translate(direction * (dt * mSpeed));
	}
}

void Projectile::shutdown()
{
	mVisible=false;
	mProjectileNode->setVisible(false);
	mProjectileNode->setPosition(Vector3(0, -10, 0));
	mExplosionNode->setVisible(false);
	//mExplosionNode->setPosition(Vector3(0, -10, 0));
	mState=INVISIBLE;
}

bool Projectile::isVisible() const
{
	return mVisible;
}

void Projectile::explode()
{
	SoundManager::getSingletonPtr()->play("explosion");
	mExplosionCounter=0;
	mProjectileNode->setVisible(false);
	mExplosionNode->setVisible(true);

	std::set<STDAgent*> agents;
	STDAgentManager::getSingletonPtr()->getAgentsWithinRadius(mExplosionNode->getPosition(), mHitRange, agents);

	if(!agents.empty())
	{
		for(std::set<STDAgent*>::iterator iter=agents.begin(); iter != agents.end(); ++iter)
		{
			(*iter)->getHit(this->mImpact);
		}
	}
	mState=EXPLODED;
}

void Projectile::initialize(Gun *g, const Vector3& src, const Vector3& target)
{
	mImpact=g->getWeaponImpact();
	mPenetration=g->getWeaponPenetration();
	mSpeed=g->getWeaponSpeed();
	mHitRange=g->getHitRange();
	mDamageRadius=g->getDamageRadius();

	mProjectileNode->setPosition(src);
	mExplosionNode->setPosition(target);

	mVisible=true;
	mProjectileNode->setVisible(true);
	mExplosionNode->setVisible(false);

	SoundManager::getSingletonPtr()->play("laser_pulse");
	mState=MOVING;
}