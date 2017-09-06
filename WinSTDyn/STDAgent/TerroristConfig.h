#ifndef _H_STD_TERRORIST_CONFIG_H
#define _H_STD_TERRORIST_CONFIG_H

#include <vector>
#include <string>
#include <map>
#include <Ogre.h>
#include "Terrorist.h"

using namespace Ogre;

class TerroristConfig
{
public:
	TerroristConfig();
	virtual ~TerroristConfig();

public:
	void load(const std::string& filename);
	void updateTerrorists(const Real& dt);
	Terrorist* createTerrorist(const Vector3& rayBegin, const Vector3& rayEnd);
	Terrorist* createTerrorist();
	Terrorist* createTerrorist(const Vector3& pos);
	void createTerrorists();
	Terrorist* getNearbyTerrorist(const Vector3& pt, const Real& radius);
	Terrorist* getNearbyTerrorist(const Vector3& pt);
	size_t getTerroristCount() const { return mTerrorists.size(); }
	void save(const std::string& filename);

private:
	Terrorist* getBestTerrorist();
	std::pair<Terrorist*, Terrorist*> getBestTerroristPair();

public:
	void addEnemy(STDAgent* agnt) { mEnemies.push_back(agnt); }

public:
	Real scale;
	Real life;
	std::string meshName;
	Real elevation;
	std::string animation;
	Real depth;
	Real width;
	std::string explosionPSName;
	Real explosionDuration;
	std::map<std::string, Terrorist*> mTerrorists;
	size_t mTerroristCreationCounter;
	std::vector<Vector3> mLocations;
	std::vector<STDAgent*> mEnemies;
};

#endif