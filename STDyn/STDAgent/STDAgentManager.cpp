#include "STDAgentManager.h"

STDAgentManager* STDAgentManager::mSingleton=NULL;

STDAgentManager::STDAgentManager()
{
	if(mSingleton==NULL)
	{
		mSingleton=this;
	}
	else
	{
		throw Exception(42, "multiple instances not allowed", "STDAgentManager::STDAgentManager()");
	}
}

STDAgentManager::~STDAgentManager()
{
	std::map<std::string, STDAgent*>::iterator iter;
	for(iter=mAgents.begin(); iter != mAgents.end(); ++iter)
	{
		delete iter->second;
	}
	mAgents.clear();
}

STDAgent* STDAgentManager::createAgent(const std::string &name, const std::string &modelName)
{
	if(mAgents.find(name) != mAgents.end())
	{
		throw Exception(42, "agent already exists!", "STDAgentManager::createAgent(const std::string&, const std::string&");
	}
	STDAgent* agnt=new STDAgent(name, modelName);
	mAgents[name]=agnt;
	return agnt;
}

void STDAgentManager::removeAgent(const std::string &name)
{
	std::map<std::string, STDAgent*>::iterator fnd=mAgents.find(name);
	if(fnd != mAgents.end())
	{
		delete fnd->second;
		mAgents.erase(fnd);
	}
}

void STDAgentManager::removeAgent(STDAgent *agnt)
{
	this->removeAgent(agnt->getName());
}

void STDAgentManager::getAgentsWithinRadius(const Vector3& origin, const Real& radius, std::set<STDAgent*>& agents)
{
	Real sqR=radius * radius;
	for(std::map<std::string, STDAgent*>::iterator iter=mAgents.begin(); iter != mAgents.end(); ++iter)
	{
		Vector3 pos=iter->second->getCenter();
		Vector3 disp=pos-origin;
		if(disp.squaredLength() <= sqR)
		{
			agents.insert(iter->second);
		}
	}
}