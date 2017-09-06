#ifndef _H_STD_AGENT_MANAGER_H
#define _H_STD_AGENT_MANAGER_H

#include <string>
#include <map>
#include <set>
#include "STDAgent.h"
#include "Tank.h"
#include "URAV.h"
#include "Terrorist.h"

class STDAgentManager
{
public:
	STDAgentManager();
	virtual ~STDAgentManager();

private:
	
	STDAgentManager(const STDAgentManager& rhs) { }
	STDAgentManager& operator=(const STDAgentManager& rhs) { return *this; }

public:
	STDAgent* createAgent(const std::string& name, const std::string& modelName);
	void removeAgent(const std::string& name);
	void removeAgent(STDAgent* agnt);
	void getAgentsWithinRadius(const Vector3& origin, const Real& radius, std::set<STDAgent*>& agents);

public:
	Tank* createTank(const std::string& name, const std::string& modelName);
	URAV* createURAV(const std::string& name, const std::string& modelName);
	Terrorist* createTerrorist(const std::string& name, const std::string& modelName);

public:
	static STDAgentManager* getSingletonPtr() { return mSingleton; }
	static STDAgentManager& getSingleton() { return *mSingleton; }

protected:
	std::map<std::string, STDAgent*> mAgents;
	static STDAgentManager* mSingleton;
};
#endif