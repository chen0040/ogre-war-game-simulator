#include "STDParticleSystemManager.h"
#include <sstream>

STDParticleSystemManager* STDParticleSystemManager::mSingleton=NULL;

STDParticleSystemManager::STDParticleSystemManager()
{
	if(mSingleton == NULL)
	{
		mSingleton=this;
	}
	else
	{
		throw Exception(42, "multiple instances not allowed", "STDParticleSystemManager::STDParticleSystemManager()");
	}
}

STDParticleSystemManager::~STDParticleSystemManager()
{
	std::map<std::string, std::stack<STDParticleSystem*> >::iterator iter1;
	for(iter1=mParticleSystems.begin(); iter1 != mParticleSystems.end(); ++iter1)
	{
		std::stack<STDParticleSystem*>& psStack=iter1->second;
		while(!psStack.empty())
		{
			STDParticleSystem* ps=psStack.top();
			delete ps;
			psStack.pop();
		}
	}
	mParticleSystems.clear();

	std::map<std::string, std::set<STDParticleSystem*> >::iterator iter2;
	for(iter2=mOnlineParticleSystems.begin(); iter2 != mOnlineParticleSystems.end(); ++iter2)
	{
		std::set<STDParticleSystem*>& psSet=iter2->second;
		for(std::set<STDParticleSystem*>::iterator iter22=psSet.begin(); iter22 != psSet.end(); ++iter22)
		{
			delete (*iter22);
		}
	}
	mOnlineParticleSystems.clear();
}

STDParticleSystem* STDParticleSystemManager::createParticleSystem(const std::string& psType, const Real& maxDuration)
{
	std::map<std::string, std::stack<STDParticleSystem*> >::iterator fnd=mParticleSystems.find(psType);
	STDParticleSystem* ps=NULL;
	if(fnd == mParticleSystems.end())
	{
		if(mPSCounters.find(psType) == mPSCounters.end())
		{
			mPSCounters[psType]=0;
		}
		else
		{
			mPSCounters[psType]+=1;
		}
		std::ostringstream oss;
		oss << psType << mPSCounters[psType];
		ps=new STDParticleSystem(oss.str(), psType);
	}
	else
	{
		if(fnd->second.empty())
		{
			if(mPSCounters.find(psType) == mPSCounters.end())
			{
				mPSCounters[psType]=0;
			}
			else
			{
				mPSCounters[psType]+=1;
			}
			std::ostringstream oss;
			oss << psType << mPSCounters[psType];

			ps=new STDParticleSystem(oss.str(), psType);
		}
		else
		{
			ps=fnd->second.top();
			mParticleSystems[psType].pop();
		}
	}

	ps->setDuration(0);
	ps->setMaxDuration(maxDuration);
	ps->setVisible(true);

	std::map<std::string, std::set<STDParticleSystem*> >::iterator fnd2=mOnlineParticleSystems.find(psType);
	if(fnd2 == mOnlineParticleSystems.end())
	{
		std::set<STDParticleSystem*> psStack;
		psStack.insert(ps);
		mOnlineParticleSystems[psType]=psStack;
	}
	else
	{
		mOnlineParticleSystems[psType].insert(ps);
	}

	return ps;
}

void STDParticleSystemManager::update(const Real& dt)
{
	std::map<std::string, std::set<STDParticleSystem*> >::iterator fnd=mOnlineParticleSystems.begin();
	while(fnd != mOnlineParticleSystems.end())
	{
		std::set<STDParticleSystem*>& psSet=(fnd->second);
		std::set<STDParticleSystem*>::iterator iter=psSet.begin();
		while(iter != psSet.end())
		{
			if((*iter)->terminated())
			{
				std::map<std::string, std::stack<STDParticleSystem*> >::iterator fnd2=mParticleSystems.find(fnd->first);
				if(fnd2 == mParticleSystems.end())
				{
					std::stack<STDParticleSystem*> psStack;
					psStack.push(*iter);
					mParticleSystems[fnd->first]=psStack;
				}
				else
				{
					mParticleSystems[fnd->first].push(*iter);
				}

				(*iter)->setVisible(false);

				iter=psSet.erase(iter);
			}
			else
			{
				(*iter)->update(dt);
				++iter;
			}
		}
		++fnd;
	}
}