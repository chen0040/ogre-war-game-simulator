#include "WeaponManager.h"
#include "../STDEngine/STDEngine.h"
#include <Ogre.h>

WeaponManager* WeaponManager::mSingleton=NULL;

WeaponManager::WeaponManager()
: mProjectileCount(10)
{
	if(mSingleton==NULL)
	{
		mSingleton=this;
	}
	else
	{
		throw new Ogre::Exception(42, "multiple instances not allowed!", "WeaponManager::WeaponManager()");
	}

	for(int i=0; i<mProjectileCount; ++i)
	{
		Projectile* p=new Projectile();
		pushProjectile(p);
	}
}

WeaponManager::~WeaponManager()
{
	Projectile* p=NULL;
	while(!mProjectiles.empty())
	{
		p=mProjectiles.top();
		delete p;
		mProjectiles.pop();
	}
}

Projectile* WeaponManager::popProjectile()
{
	Projectile* p=NULL;
	if(mProjectiles.empty())
	{
		mProjectileCount++;
		p=new Projectile();
	}
	else
	{
		p=mProjectiles.top();
		mProjectiles.pop();
	}
	return p;
}

void WeaponManager::pushProjectile(Projectile *p)
{
	p->shutdown();
	mProjectiles.push(p);
}

