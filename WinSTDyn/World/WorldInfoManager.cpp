#include "stdafx.h"
#include "WorldInfoManager.h"
#include "../STDEngine/STDEngine.h"

WorldInfoManager* WorldInfoManager::mSingleton=NULL;

WorldInfoManager::WorldInfoManager(SceneManager* sm)
: mSceneMgr(sm)
, mRaySceneQuery(NULL)
, mWorldType(WORLDTYPE_NONE)
, mCaelumSystem(NULL)
, mCaelumSystemEnabled(false)
, mSkyPlaneEnabled(false)
, mSkyPlaneMaterial("")
{
	if(mSingleton==NULL)
	{
		mSingleton=this;
	}
	else
	{
		throw Exception(42, "Multiple instances of WorldInfoManager not allow", "WorldInfoManager::WorldInfoManager(SceneManager* mgr)");
	}
}

void WorldInfoManager::createWorld_Terrain(const String& terrainCfg)
{
	mSceneMgr->setWorldGeometry(terrainCfg);
	
	mRaySceneQuery=mSceneMgr->createRayQuery(Ray());

	mWorldType=WORLDTYPE_TERRAIN;
}

Real WorldInfoManager::getHeight(Real x, Real z)
{
	Real y=0;
	if(mRaySceneQuery != NULL)
	{
		Ray ray(Vector3(x, 1000, z), Vector3::NEGATIVE_UNIT_Y);
		mRaySceneQuery->setRay(ray);
		mRaySceneQuery->setSortByDistance(true);

		RaySceneQueryResult& result=mRaySceneQuery->execute();
		RaySceneQueryResult::iterator ri;
		for(ri=result.begin(); ri !=result.end(); ++ri)
		{
			if(ri->worldFragment)
			{
				y=ri->worldFragment->singleIntersection.y;
				break;
			}
		}
	}

	return y;
}

void WorldInfoManager::createSkyPlane(const String& materialName)
{
	mSkyPlaneMaterial=materialName;
	Plane plane;
    plane.d = 1000;
    plane.normal = Vector3::NEGATIVE_UNIT_Y;
	 mSceneMgr->setSkyPlane(true, plane, materialName, 1500, 50, true, 1.5f, 150, 150);

	 mSkyPlaneEnabled=true;
}

void WorldInfoManager::createWorld_FlatGround(const String& materialName)
{
	Plane plane(Vector3::UNIT_Y, 0);

    MeshManager::getSingleton().createPlane("ground",
        ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
        1500,4500,20,20,true,1,5,5,Vector3::UNIT_Z);

    Entity* ent = mSceneMgr->createEntity("GroundEntity", "ground");
	ent->setCastShadows(false);
    mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(0, 0, 0))->attachObject(ent);

    ent->setMaterialName(materialName);
    ent->setCastShadows(false);

	mWorldType=WORLDTYPE_FLAT_GROUND;
}

Real getWorldHeight(Real x, Real z, void* data)
{
	return WorldInfoManager::getSingletonPtr()->getHeight(x, z);
}

Real WorldInfoManager::getMaxZ() const
{
	switch(mWorldType)
	{
	case WORLDTYPE_FLAT_GROUND:
		return 2250;
	default:
		return 1500;
	}
}

WorldInfoManager* WorldInfoManager::getSingletonPtr()
{
	return mSingleton;
}

WorldInfoManager& WorldInfoManager::getSingleton()
{
	return *mSingleton;
}

void WorldInfoManager::createSkyDome(const String& materialName)
{
	mSceneMgr->setSkyDome(true, materialName, 5, 8);
}


Real WorldInfoManager::getMinX() const
{
	switch(mWorldType)
	{
	case WORLDTYPE_FLAT_GROUND:
		return -750;
	default:
		return 0;
	}
}

Real WorldInfoManager::getMinZ() const
{
	switch(mWorldType)
	{
	case WORLDTYPE_FLAT_GROUND:
		return -2250;
	default:
		return 0;
	}
}

Real WorldInfoManager::getMaxX() const
{
	switch(mWorldType)
	{
	case WORLDTYPE_FLAT_GROUND:
		return 750;
	default:
		return 1500;
	}
}

void WorldInfoManager::createCaelumSystem()
{
	createCaelumSystem(512, Vector2(0.000025, -0.000045));
}

void WorldInfoManager::createCaelumSystem(Real timeScale, const Vector2& cloudSpeed)
{
		STDEngine* engine=STDEngine::getSingletonPtr();

		// Initialise CaelumSystem
		Caelum::CaelumSystem::CaelumComponent componentMask=Caelum::CaelumSystem::CAELUM_COMPONENTS_DEFAULT;
		mCaelumSystem = new Caelum::CaelumSystem(Root::getSingletonPtr(), engine->getSceneMgr(), componentMask);

		// Set time acceleration.
        mCaelumSystem->getUniversalClock ()->setTimeScale (timeScale);

		//remove the fog effect
		mCaelumSystem->setSceneFogDensityMultiplier(0.0004f); // or some other small value.
		mCaelumSystem->setManageSceneFog(true);

		// Set Cloud Speed;
		Caelum::CloudSystem::LayerVector& lv=mCaelumSystem->getCloudSystem()->getLayerVector();
		Caelum::CloudSystem::LayerVector::iterator lviter;
		Caelum::FlatCloudLayer* lvp;
		for(lviter=lv.begin(); lviter != lv.end(); ++lviter)
		{
			lvp=*lviter;
			lvp->setCloudSpeed(cloudSpeed);
		}

		engine->getWindow()->addListener(mCaelumSystem);
		Root::getSingletonPtr()->addFrameListener(mCaelumSystem);

		mCaelumSystemEnabled=true; 
}

void WorldInfoManager::setCaelumSystemDateTime(int year, int month, int day, int hour, int minute, int second)
{
	mCaelumSystem->getUniversalClock()->setGregorianDateTime(year, month, day, hour, minute, second);	
}

void WorldInfoManager::getCaelumSystemDateTime(int& year, int& month, int& day, int& hour, int& minute, int& second)
{
	Caelum::LongReal calcJd=mCaelumSystem->getUniversalClock()->getJulianDay();
	Caelum::LongReal sec=0;

    Caelum::Astronomy::getGregorianDateTimeFromJulianDay (calcJd, year, month, day, hour, minute, sec);
	second=sec;
}

void WorldInfoManager::setCaelumSystemCloudSpeed(const Vector2& cloudSpeed)
{
	// Set Cloud Speed;
	Caelum::CloudSystem::LayerVector& lv=mCaelumSystem->getCloudSystem()->getLayerVector();
	Caelum::CloudSystem::LayerVector::iterator lviter;
	Caelum::FlatCloudLayer* lvp;
	for(lviter=lv.begin(); lviter != lv.end(); ++lviter)
	{
		lvp=*lviter;
		lvp->setCloudSpeed(cloudSpeed);
	}
}

void WorldInfoManager::setCaelumSystemTimeScale(const Real& timeScale)
{
	mCaelumSystem->getUniversalClock ()->setTimeScale (timeScale);
}

std::string WorldInfoManager::getCaelumSystemDateTimeText()
{
	std::ostringstream oss;

	int year, month, day, hour, minute, second;
	getCaelumSystemDateTime(year, month, day, hour, minute, second);

	oss << year << "-" << month << "-" << day << " " << hour << ":" << minute << ":" << second;
	return oss.str();
}

void WorldInfoManager::enableSkyPlane(bool enabled)
{
	mSkyPlaneEnabled=enabled;
	if(mSkyPlaneEnabled && mSkyPlaneMaterial.compare("") != 0)
	{
		this->createSkyPlane(mSkyPlaneMaterial);
	}
	else
	{
		Plane plane;
		plane.d = 1000;
		plane.normal = Vector3::NEGATIVE_UNIT_Y;
		mSceneMgr->setSkyPlane(enabled, plane, mSkyPlaneMaterial);
		mSkyPlaneEnabled=false;
	}
}

void WorldInfoManager::enableCaelumSystem(bool enabled)
{
	if(enabled)
	{
		if(mCaelumSystem != NULL)
		{
			/*
			mCaelumSystem->setEnsureSingleLightSource(true); 
			mCaelumSystem->setEnsureSingleShadowSource(true);
			mCaelumSystem->getSun()->setForceDisable(false);
			mCaelumSystem->getMoon()->setForceDisable(false);
			*/
			if(mCaelumSystemEnabled==false)
			{
				STDEngine::getSingletonPtr()->getWindow()->addListener(mCaelumSystem);
				Root::getSingletonPtr()->addFrameListener(mCaelumSystem);
			}
			mCaelumSystemEnabled=true;
		}
		else
		{
			mCaelumSystemEnabled=false;
		}
	}
	else
	{
		if(mCaelumSystem != NULL)
		{
			/*
			mCaelumSystem->setEnsureSingleLightSource(false); 
			mCaelumSystem->setEnsureSingleShadowSource(false);
			mCaelumSystem->getSun()->setForceDisable(true);
			mCaelumSystem->getMoon()->setForceDisable(true);
			*/
			if(mCaelumSystemEnabled==true)
			{
				STDEngine::getSingletonPtr()->getWindow()->removeListener(mCaelumSystem);
				Root::getSingletonPtr()->removeFrameListener(mCaelumSystem);
			}
		}
		mCaelumSystemEnabled=false;
	}
}

WorldInfoManager::~WorldInfoManager()
{
	mSceneMgr->destroyQuery(mRaySceneQuery);
	  if (mCaelumSystem != NULL) 
	  {
        mCaelumSystem->shutdown (false);
        mCaelumSystem = 0;
    }
}
