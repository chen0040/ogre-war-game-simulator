#ifndef _H_ML_LAB_World_INFO_MANAGER_H
#define _H_ML_LAB_World_INFO_MANAGER_H

#include <Ogre.h>
#include "../Caelum/Caelum.h"

using namespace Ogre;

enum WorldType
{
	WORLDTYPE_TERRAIN,
	WORLDTYPE_FLAT_GROUND,
	WORLDTYPE_NONE
};

class WorldInfoManager 
{
protected:
	static WorldInfoManager* mSingleton;
	SceneManager* mSceneMgr;
	RaySceneQuery* mRaySceneQuery;
	WorldType mWorldType;
	Caelum::CaelumSystem *mCaelumSystem;

public:
	WorldInfoManager(SceneManager* sm);
	virtual ~WorldInfoManager();

	void createWorld_Terrain(const String& terrainCfg);
	void createWorld_FlatGround(const String& materialName);
	void createSkyDome(const String& materialName);
	void createCaelumSystem();
	void createCaelumSystem(Real timeScale, const Vector2& cloudSpeed);
	void createSkyPlane(const String& materialName);

public:
	void enableCaelumSystem(bool enabled);
	void enableSkyPlane(bool enabled);
	bool isCaelumSystemEnabled() const { return mCaelumSystemEnabled; }
	bool isSkyPlaneEnabled() const { return mSkyPlaneEnabled; }
	bool isCaelumSystemAvailable() const { return mCaelumSystem != NULL; }
	bool isSkyPlaneAvailable() const { return mSkyPlaneMaterial.compare("") != 0; }
	void setCaelumSystemDateTime(int year, int month, int day, int hour, int minute, int second);
	void getCaelumSystemDateTime(int& year, int& month, int& day, int& hour, int& minute, int& second);
	std::string getCaelumSystemDateTimeText();
	void setCaelumSystemCloudSpeed(const Vector2& speed);
	void setCaelumSystemTimeScale(const Real& timeScale);
	Caelum::CaelumSystem*getCaelumSystem() { return mCaelumSystem; }

public:
	static WorldInfoManager* getSingletonPtr();
	static WorldInfoManager& getSingleton();

private:
	bool mCaelumSystemEnabled;
	bool mSkyPlaneEnabled;
	String mSkyPlaneMaterial;

public:
	Real getHeight(Real x, Real z);
	Real getMinX() const;
	Real getMaxX() const;
	Real getMinZ() const;
	Real getMaxZ() const;

};

Real getWorldHeight(Real x, Real z, void* data=NULL);

#endif