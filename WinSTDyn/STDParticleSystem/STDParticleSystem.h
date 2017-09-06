#ifndef _H_STD_PARTICLE_SYSTEM_H
#define _H_STD_PARTICLE_SYSTEM_H

#include <string>
#include <Ogre.h>

using namespace Ogre;

class STDParticleSystem
{
public:
	STDParticleSystem(const std::string& id, const std::string& psName);
	virtual ~STDParticleSystem();

public:
	Real getDuration();
	void setDuration(const Real& d) { mDuration=d; }
	void setMaxDuration(const Real& d) { mMaxDuration=d; }

public:
	void update(const Real& dt);
	bool terminated() const { return mDuration >= mMaxDuration; }
	ParticleSystem* getOgreParticleSystem() { return mParticleSystem; }
	void setPosition(const Vector3& pos);
	void setVisible(bool visible);

private:
	ParticleSystem* mParticleSystem;
	SceneNode* mNode;
	Real mDuration;
	Real mMaxDuration;
	bool mVisible;
};
#endif