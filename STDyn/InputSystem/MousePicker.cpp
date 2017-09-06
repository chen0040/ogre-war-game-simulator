#include "MousePicker.h"
#include "../STDEngine/STDEngine.h"
#include "../OSEnvironment/OSEnvironment.h"
#include "../OgreRayUtil/OgreRayUtil.h"

MousePicker::MousePicker()
: mCamera(NULL)
{
	SceneManager* sm=STDEngine::getSingletonPtr()->getSceneMgr();
	mRaySceneQuery=sm->createRayQuery(Ray());	
	mRaySceneQuery->setSortByDistance(true);
}

MousePicker::~MousePicker()
{
	SceneManager* sm=STDEngine::getSingletonPtr()->getSceneMgr();
	sm->destroyQuery(mRaySceneQuery);
}

Vector3 MousePicker::getWorldIntersection(const OIS::MouseEvent& evt)
{
	CEGUI::Point& mousePoint=CEGUI::MouseCursor::getSingletonPtr()->getPosition(); 

	Ray mouseRay=mCamera->getCameraToViewportRay(mousePoint.d_x / static_cast<float>(evt.state.width), mousePoint.d_y / static_cast<float>(evt.state.height));
	mRaySceneQuery->setRay(mouseRay);

	RaySceneQueryResult& result=mRaySceneQuery->execute();
	RaySceneQueryResult::iterator ri;

	Vector3 pos;
	for(ri=result.begin(); ri != result.end(); ++ri)
	{
		if(!(ri->movable && ri->movable->getName().substr(0, 5)!="tile[") && ri->worldFragment)
		{
			pos = ri->worldFragment->singleIntersection;
			break;
		}
	}

	return pos;
}

MovableObject* MousePicker::pick(const OIS::MouseEvent& evt)
{
	return pick(evt, "");
}

MovableObject* MousePicker::pick(const OIS::MouseEvent& evt, const std::string& prefix)
{
	CEGUI::Point& mousePoint=CEGUI::MouseCursor::getSingletonPtr()->getPosition();
	Ray mouseRay=mCamera->getCameraToViewportRay(mousePoint.d_x / static_cast<float>(evt.state.width), mousePoint.d_y / static_cast<float>(evt.state.height));

	/*
	mRaySceneQuery->setRay(mouseRay);
	

	RaySceneQueryResult& result=mRaySceneQuery->execute();
	RaySceneQueryResult::iterator ri;
	for(ri=result.begin(); ri != result.end(); ++ri)
	{
		if(ri->movable)
		{
			const String& entityName = ri->movable->getName();
			if(entityName.substr(0, 5)!="tile[")
			{
				if(prefix.compare("")==0)
				{
					return ri->movable;
					break;
				}
				else
				{
					if(entityName.substr(0, prefix.length()) == prefix)
					{
						return ri->movable;
						break;
					}
				}
			}
		}
	}
	*/

	Vector3 direction=mouseRay.getDirection();
	direction.normalise();

	return OgreRayUtil::RaycastFromPoint(mRaySceneQuery, mouseRay.getOrigin(), direction);
}

