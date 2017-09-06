#ifndef _H_ML_LAB_World_INFO_MANAGER_H
#define _H_ML_LAB_World_INFO_MANAGER_H

#include <Ogre.h>

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

public:
	WorldInfoManager(SceneManager* sm)
		: mSceneMgr(sm)
		, mRaySceneQuery(NULL)
		, mWorldType(WORLDTYPE_NONE)
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

	void createWorld_Terrain(const String& terrainCfg)
	{
		mSceneMgr->setWorldGeometry(terrainCfg);
		
		mRaySceneQuery=mSceneMgr->createRayQuery(Ray());

		mWorldType=WORLDTYPE_TERRAIN;
	}

	void createWorld_FlatGround(const String& materialName)
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

	void createSkyDome(const String& materialName)
	{
		mSceneMgr->setSkyDome(true, materialName, 5, 8);
	}

	void createSkyPlane(const String& materialName)
	{
		Plane plane;
        plane.d = 1000;
        plane.normal = Vector3::NEGATIVE_UNIT_Y;
		 mSceneMgr->setSkyPlane(true, plane, materialName, 1500, 50, true, 1.5f, 150, 150);
	}

	virtual ~WorldInfoManager()
	{
		mSceneMgr->destroyQuery(mRaySceneQuery);
	}

	static WorldInfoManager* getSingletonPtr()
	{
		return mSingleton;
	}

	static WorldInfoManager& getSingleton()
	{
		return *mSingleton;
	}

public:
	Real getHeight(Real x, Real z)
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

	Real getMinX() const
	{
		switch(mWorldType)
		{
		case WORLDTYPE_FLAT_GROUND:
			return -750;
		default:
			return 0;
		}
	}

	Real getMaxX() const
	{
		switch(mWorldType)
		{
		case WORLDTYPE_FLAT_GROUND:
			return 750;
		default:
			return 1500;
		}
	}

	Real getMinZ() const
	{
		switch(mWorldType)
		{
		case WORLDTYPE_FLAT_GROUND:
			return -2250;
		default:
			return 0;
		}
	}

	Real getMaxZ() const
	{
		switch(mWorldType)
		{
		case WORLDTYPE_FLAT_GROUND:
			return 2250;
		default:
			return 1500;
		}
	}

};

Real getWorldHeight(Real x, Real z, void* data=NULL);

#endif