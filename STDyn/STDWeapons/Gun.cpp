#include "Gun.h"
#include "WeaponManager.h"
#include "../STDAgent/STDAgent.h"
#include "../STDAgent/STDAgent.h"

Gun::Gun(STDAgent *holder)
: mHolder(holder)
, mWeaponImpact(20)
, mWeaponPenetration(0.1)
, mWeaponSpeed(200)
, mTimeSinceLastWeaponFired(0)
, mMaxBulletCount(2000000)
, mWeaponChargingRate(0.2) //add a bullet per second
, mMaxShootingDistance(1000000)
, mHitRange(10)
, mFiringingInterval(1)
, mDamageRadius(10)
{
	mBulletCount=mMaxBulletCount;
}

Gun::~Gun()
{
	clearProjectiles();
}

bool Gun::isWithinShootingDistance(const Vector3& target) const
{
	Vector3 disp=mHolder->getPosition() - target;
	return disp.length() <= mMaxShootingDistance;
}

void Gun::shoot(const Vector3& src, const Vector3& target, Real dt)
{
	if(mBulletCount <= 0)
	{
		return;
	}

	mTimeSinceLastWeaponFired+=dt;
	if(mTimeSinceLastWeaponFired > mFiringingInterval)
	{
		fire(src, target);
		mTimeSinceLastWeaponFired=0;
	}
}

void Gun::fire(const Vector3& src, const Vector3& target)
{
	Projectile* p=WeaponManager::getSingletonPtr()->popProjectile();
	p->initialize(this, src, target);
	mProjectiles.insert(p);
	mBulletCount-=1;
}

void Gun::clearProjectiles()
{
	std::set<Projectile*>::iterator iter;
	Projectile* p=NULL;
	WeaponManager* wm=WeaponManager::getSingletonPtr();
	for(iter=mProjectiles.begin(); iter != mProjectiles.end(); ++iter)
	{
		p=*iter;
		wm->pushProjectile(*iter);
	}
	mProjectiles.clear();
}

void Gun::update(Real dt)
{
	WeaponManager* wm=WeaponManager::getSingletonPtr();
	std::set<Projectile*>::iterator iter=mProjectiles.begin(); 
	Projectile* p=NULL;
	while(iter != mProjectiles.end())
	{
		p=(*iter);
		p->update(dt);
		if(!p->isVisible())
		{
			wm->pushProjectile(p);
			iter=mProjectiles.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	//charging up the weapon
	mBulletCount+=(mWeaponChargingRate * dt);
	if(mBulletCount > mMaxBulletCount)
	{
		mBulletCount=mMaxBulletCount;
	}
}