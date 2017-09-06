#ifndef _H_ML_LAB_GUN_H
#define _H_ML_LAB_GUN_H

#include <set>
#include "Projectile.h"

class STDAgent;

class Gun
{
public:
	Gun(STDAgent* holder);
	virtual ~Gun();

public:
	void shoot(const Vector3& src, const Vector3& target, Real dt);
	void setWeaponImpact(const Real& impact) { mWeaponImpact=impact; }
	void setWeaponPenetration(const Real& p) { mWeaponPenetration=p; }
	void setWeaponSpeed(const Real& s) { mWeaponSpeed=s; }
	void setWeaponChargingRate(const Real& rate) { mWeaponChargingRate=rate; }
	void setFiringInterval(const Real& value) { mFiringingInterval=value; }
	void setHitRange(const Real& val) { mHitRange=val; }
	Real getWeaponImpact() const { return mWeaponImpact; }
	Real getWeaponPenetration() const { return mWeaponPenetration; }
	Real getWeaponSpeed() const { return mWeaponSpeed; }
	Real getWeaponChargingRate() const { return mWeaponChargingRate; }
	Real getFiringInterval() const { return mFiringingInterval; }
	bool hasBullets() const { return getBulletCount() > 0; }
	size_t getBulletCount() const { return static_cast<size_t>(mBulletCount); }
	size_t getMaxBulletCount() const { return mMaxBulletCount; }
	void setMaxBulletCount(size_t count) { mMaxBulletCount=count; }
	bool isWithinShootingDistance(const Vector3& target) const;
	Real getHitRange() const { return mHitRange; }
	Real getDamageRadius() const { return mDamageRadius; }
	void setDamageRadius(const Real& r) { mDamageRadius=r; }

	void update(Real dt);
	void clearProjectiles();

private:
	void fire(const Vector3& src, const Vector3& target);

protected:
	STDAgent* mHolder;
	Real mTimeSinceLastWeaponFired;
	std::set<Projectile*> mProjectiles;
	Real mWeaponImpact;
	Real mWeaponPenetration;
	Real mWeaponSpeed;
	Real mWeaponChargingRate;
	Real mBulletCount;
	size_t mMaxBulletCount;
	Real mMaxShootingDistance;
	Real mHitRange;
	Real mFiringingInterval;
	Real mDamageRadius;
};
#endif