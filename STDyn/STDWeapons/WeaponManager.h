#ifndef _H_ML_LAB_WEAPON_MANAGER_H
#define _H_ML_LAB_WEAPON_MANAGER_H

#include <stack>
#include "Projectile.h"

class WeaponManager
{
public:
	WeaponManager();
	virtual ~WeaponManager();

private:
	WeaponManager(const WeaponManager& rhs) { }
	WeaponManager operator=(const WeaponManager& rhs) { return *this; }

public:
	static WeaponManager* getSingletonPtr() { return mSingleton; }
	static WeaponManager& getSingleton() { return *mSingleton; }

public:
	Projectile* popProjectile();
	void pushProjectile(Projectile* p);

protected:
	std::stack<Projectile*> mProjectiles;
	int mProjectileCount;

protected:
	static WeaponManager* mSingleton;
};
#endif