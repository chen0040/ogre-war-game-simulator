#ifndef _H_STD_PARTICLE_SYSTEM_MANAGER_H
#define _H_STD_PARTICLE_SYSTEM_MANAGER_H

#include <string>
#include <map>
#include <stack>
#include <set>
#include "STDParticleSystem.h"

class STDParticleSystemManager
{
public:
	STDParticleSystemManager();
	~STDParticleSystemManager();

public:
	STDParticleSystem* createParticleSystem(const std::string& psType, const Real& maxDuration);

public:
	void update(const Real& dt);

private:
	std::map<std::string, std::stack<STDParticleSystem*> > mParticleSystems;
	std::map<std::string, std::set<STDParticleSystem*> > mOnlineParticleSystems;
	std::map<std::string, size_t> mPSCounters;

public:
	static STDParticleSystemManager* getSingletonPtr() { return mSingleton; }
	static STDParticleSystemManager& getSingleton() { return *mSingleton; }

protected:
	static STDParticleSystemManager* mSingleton;
};
#endif