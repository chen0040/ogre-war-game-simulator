#ifndef _H_ML_LAB_PROJECTILE_H
#define _H_ML_LAB_PROJECTILE_H

#include <Ogre.h>

using namespace Ogre;

class Gun;

enum ProjectileState
{
	EXPLODED,
	MOVING,
	INVISIBLE
};

class Projectile
{
public:
	Projectile();
	virtual ~Projectile();

public:
	virtual void update(Real dt);
	virtual void setPosition(const Vector3& pos);
	virtual void initialize(Gun* g, const Vector3& src, const Vector3& target); 
	Real getImpact() const { return mImpact; }
	Real getPenetration() const { return mPenetration; }
	virtual bool isVisible() const;
	void shutdown();

protected:
	void doExplosion(Real dt);
	void doMove(Real dt);
	virtual void explode();

protected:
	ParticleSystem* mProj;
	ParticleSystem* mExpPS;
	SceneNode* mProjectileNode;
	SceneNode* mExplosionNode;

protected:
	static size_t mProjectileCount;
	Real mImpact;
	Real mPenetration;
	Real mSpeed;
	bool mVisible;
	ProjectileState mState;
	Real mHitRange;
	Real mDamageRadius;

protected:
	Real mExplosionDuration;
	Real mExplosionCounter;
};
#endif