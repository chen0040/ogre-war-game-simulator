#include "STDNaviFrame.h"
#include "../STDEngine/STDEngine.h"
#include "../OSEnvironment/OSEnvironment.h"
#include "../GUI/GUIManager.h"
#include "../OgreRayUtil/OgreRayUtil.h"
#include "../tinyxml/tinyxml.h"
#include "../STDAgent/STDAgentManager.h"
#include "../STDParticleSystem/STDParticleSystemManager.h"

STDNaviFrame::STDNaviFrame(std::string frameName)
: STDFrame(frameName)
, mRedLaser(NULL)
, mGreenLaser(NULL)
, mBlueLaser(NULL)
, mTankLaserTarget(0, -1000, 0)
, mURAVLaserTarget(0, 1000, 0)
, mTerroristCreationCounter(0)
{
	STDEngine* app=STDEngine::getSingletonPtr();
	//mCamera.setActualCamera(app->getCamera());

	mSTDOverlay= Ogre::OverlayManager::getSingleton().getByName("Xianshun/STDOverlay");
	if(mSTDOverlay == NULL)
	{
		throw Exception(42, "mSTDOverlay == NULL", "STDNaviFrame::STDNaviFrame()");
	}

	createWorld();
	createURAV();
	createTank();
	createSatelite();

	createLasers();

	mMousePicker.setCamera(app->getCamera());

	mRaySceneQuery=app->getSceneMgr()->createRayQuery(Ray());	 
	mRaySceneQuery->setSortByDistance(true);

	mTerroristConfig.load(OSEnvironment::getFullPath("..\\config\\Terrorist.xml"));
}

void STDNaviFrame::createLasers()
{
	STDEngine* app=STDEngine::getSingletonPtr();
	app->createOrRetrieveColor("red", 1, 0, 0);
	app->createOrRetrieveColor("blue", 0, 0, 1);
	app->createOrRetrieveColor("green", 0, 1, 0);

	SceneManager* sm=app->getSceneMgr();
	mRedLaser=sm->createManualObject("RedLaser");
	mBlueLaser=sm->createManualObject("BlueLaser");
	mGreenLaser=sm->createManualObject("GreenLaser");

	SceneNode* parentSceneNode=sm->getRootSceneNode();
	parentSceneNode->createChildSceneNode()->attachObject(mRedLaser);
	parentSceneNode->createChildSceneNode()->attachObject(mGreenLaser);
	parentSceneNode->createChildSceneNode()->attachObject(mBlueLaser);
}

void STDNaviFrame::createWorld()
{
	TiXmlDocument doc;
	if(!doc.LoadFile(OSEnvironment::getFullPath("..\\config\\BuildingLayout.xml").c_str()))
	{
		createBuildings_Conformance();
		return;
	}

	TiXmlElement* xmlRoot=doc.RootElement();

	std::string architecture_selected=xmlRoot->Attribute("architecture");
	for(TiXmlElement* xmlLevel1 = xmlRoot->FirstChildElement(); xmlLevel1 != NULL; xmlLevel1=xmlLevel1->NextSiblingElement())
	{
		if(strcmp(xmlLevel1->Value(), "architecture")==0)
		{
			if(architecture_selected.compare(xmlLevel1->Attribute("name"))==0)
			{
				if(architecture_selected.compare("random")==0)
				{
					createBuildings_Random();
					break;
				}
				else if(architecture_selected.compare("conformance")==0)
				{
					int type=1;
					xmlLevel1->QueryIntAttribute("type", &type);
					createBuildings_Conformance(type);
					break;
				}
			}
		}
	}
	
}

void STDNaviFrame::createBuildings_Conformance(int type)
{
	const Real BUILDING_SCALE=0.15f;
	const Real BUIDLING_ELEVATION=10.0f;
	const Real LEFT_POSITION=WorldInfoManager::getSingletonPtr()->getMinX()+200;
	const Real RIGHT_POSITION=WorldInfoManager::getSingletonPtr()->getMaxX()-200;
	const Real BUILDING_WIDTH=700.0f;
	std::string building_name="";

	Real minZ=WorldInfoManager::getSingletonPtr()->getMinZ();
	Real maxZ=WorldInfoManager::getSingletonPtr()->getMaxZ();

	Real building_width=0;
	Real z=minZ;
	size_t building_index=1;
	while(z < maxZ)
	{
		building_name=OSEnvironment::append("Building", OSEnvironment::toString(building_index++));
		if(type==1)
		{
			mBuildings[building_name]=createBuilding_Type1(building_name, LEFT_POSITION, z, building_width);
		}
		else
		{
			mBuildings[building_name]=createBuilding_Type2(building_name, LEFT_POSITION, z, building_width);
		}
		z+=building_width;
	}

	z=minZ;
	while(z < maxZ)
	{
		building_name=OSEnvironment::append("Buillding", OSEnvironment::toString(building_index++));
		if(type==1)
		{
			mBuildings[building_name]=createBuilding_Type1(building_name, RIGHT_POSITION,  z, building_width);
		}
		else
		{
			mBuildings[building_name]=createBuilding_Type2(building_name, RIGHT_POSITION,  z, building_width);
		}
		z+=building_width;
	}
}

void STDNaviFrame::createBuildings_Random()
{
	const Real BUILDING_SCALE=0.15f;
	const Real BUIDLING_ELEVATION=10.0f;
	Real LEFT_POSITION=WorldInfoManager::getSingletonPtr()->getMinX()+200;
	Real RIGHT_POSITION=WorldInfoManager::getSingletonPtr()->getMaxX()-200;
	std::string building_name="";

	Real minZ=WorldInfoManager::getSingletonPtr()->getMinZ();
	Real maxZ=WorldInfoManager::getSingletonPtr()->getMaxZ();

	Real building_width=0;
	Real z=minZ;
	size_t building_index=1;
	int index=0;
	STDAgent* building=NULL;
	while(z < maxZ)
	{
		building_name=OSEnvironment::append("Building", OSEnvironment::toString(building_index++));
		index=rand() % 2 + 1;

		switch(index)
		{
		case 1:
			building=createBuilding_Type1(building_name, LEFT_POSITION, z, building_width);
			break;
		default:
			building=createBuilding_Type2(building_name, LEFT_POSITION, z, building_width);
			break;
		}

		mBuildings[building_name]=building;
		z+=building_width;
	}

	z=minZ;
	while(z < maxZ)
	{
		building_name=OSEnvironment::append("Buillding", OSEnvironment::toString(building_index++));

		index=rand() % 2 + 1;

		switch(index)
		{
		case 1:
			building=createBuilding_Type1(building_name, RIGHT_POSITION, z, building_width);
			break;
		default:
			building=createBuilding_Type2(building_name, RIGHT_POSITION, z, building_width);
			break;
		}

		z+=building_width;
	}
}

STDAgent* STDNaviFrame::createBuilding_Type1(const std::string& building_name, const Real& x, const Real& z, Real& building_width)
{
	const Real BUILDING_SCALE=0.15f;
	const Real BUIDLING_ELEVATION=10.0f;

	STDAgent* building=STDAgentManager::getSingletonPtr()->createAgent(building_name, "building.mesh");
	building->setScale(BUILDING_SCALE, BUILDING_SCALE, BUILDING_SCALE);
	building->setElevation(BUIDLING_ELEVATION);
	building->pitch(90);
	building->setPosition(x, getWorldHeight(x, z), z);
	building->setDestructable(false);
	//building->getEntity()->setRenderQueueGroup(RENDER_QUEUE_9);

	building_width=700.0f;
	return building;
}

STDAgent* STDNaviFrame::createBuilding_Type2(const std::string& building_name, const Real& x, const Real& z, Real& building_width)
{
	const Real BUILDING_SCALE=0.15f;
	const Real BUIDLING_ELEVATION=10.0f;

	STDAgent* building=STDAgentManager::getSingletonPtr()->createAgent(building_name, "bbuilding.mesh");
	building->setScale(BUILDING_SCALE, BUILDING_SCALE, BUILDING_SCALE);
	building->setElevation(BUIDLING_ELEVATION);
	building->pitch(90);
	building->setPosition(x, getWorldHeight(x, z), z);
	building->setDestructable(false);
	//building->getEntity()->setRenderQueueGroup(RENDER_QUEUE_9);

	building_width=700.0f;
	return building;
}

void STDNaviFrame::createTank()
{
	mTankConfig.load(OSEnvironment::getFullPath("..\\config\\Tank.xml"));

	mTank=STDAgentManager::getSingletonPtr()->createAgent("Tank", mTankConfig.meshName);
	mTank->setScale(mTankConfig.scale, mTankConfig.scale, mTankConfig.scale);
	mTank->setElevation(mTankConfig.elevation);
	mTank->setSpeed(mTankConfig.tankSpeed);
	mTank->setRotatingSpeed(mTankConfig.tankRotatingSpeed);
	mTank->setCamScanningSpeed(mTankConfig.tankCamScanningSpeed);
	mTank->setLife(mTankConfig.life);
	mTank->setCollisionRadius(mTankConfig.collisionRadius);

	RenderWindow* rw=STDEngine::getSingletonPtr()->getWindow();
	SceneManager* sm=STDEngine::getSingletonPtr()->getSceneMgr();
	float mapHeight=rw->getWidth() * mTankConfig.mapWidth / rw->getHeight();

	mTankMap=new MiniMap("TankMap", rw, sm, GUIManager::getSingletonPtr()->getGUIRenderer(), mTankConfig.mapResolution);
	mTankMap->setSize(mTankConfig.mapWidth, mapHeight);
	mTankMap->setPosition(mTankConfig.mapWidth, 1.0f - mapHeight);
	mTankMap->setAlpha(mTankConfig.mapAlpha);
	mTankMap->getMinimapWindow()->setText(mTankConfig.mapTitle);

	mTank->addCamera("backCam", STDEngine::getSingletonPtr()->getCamera(), Vector3(mTankConfig.backCamX, mTankConfig.backCamY, mTankConfig.backCamZ));
	mTank->addCamera("cannonCam", mTankMap->getOgreCamera(), Vector3(mTankConfig.cannonCamX, mTankConfig.cannonCamY, mTankConfig.cannonCamZ));

	mTank->setAlignment(-Vector3::UNIT_Z);
	mTank->setPosition(mTankConfig.tankPosX, getWorldHeight(mTankConfig.tankPosX, mTankConfig.tankPosZ), mTankConfig.tankPosZ);
	mTank->setExplosionDuration(mTankConfig.explosionDuration);
	mTank->setExplosionParticleSystem(mTankConfig.explosionPSName);

	Gun* tankCannon=mTank->createGun();
	tankCannon->setWeaponSpeed(mTankConfig.missileSpeed);
	tankCannon->setFiringInterval(mTankConfig.firingInterval);
	tankCannon->setHitRange(mTankConfig.hitRange);
	tankCannon->setDamageRadius(mTankConfig.damageRadius);
	tankCannon->setWeaponImpact(mTankConfig.missileImpact);
}

void STDNaviFrame::createSatelite()
{
	TiXmlDocument doc;

	if(!doc.LoadFile(OSEnvironment::getFullPath("..\\config\\Satelite.xml").c_str()))
	{
		throw Exception(42, "Failed to load Satelite.xml", "STDNaviFrame::createURAV()");
	}

	int mapResolution=256;
	std::string mapTitle="Bird's View";
	float mapWidth=0.20f;
	float mapAlpha=1.0f;
	Real posX=0;
	Real posY=1000;
	Real posZ=0;

	TiXmlElement* xmlRoot=doc.RootElement();
	for(TiXmlElement* xmlLevel1 = xmlRoot->FirstChildElement(); xmlLevel1 != NULL; xmlLevel1 = xmlLevel1->NextSiblingElement())
	{
		if(strcmp(xmlLevel1->Value(), "map")==0)
		{
			xmlLevel1->QueryFloatAttribute("width", &mapWidth);
			xmlLevel1->QueryFloatAttribute("alpha", &mapAlpha);
			xmlLevel1->QueryIntAttribute("resolution", &mapResolution);
			mapTitle=xmlLevel1->Attribute("title");
		}
		else if(strcmp(xmlLevel1->Value(), "position")==0)
		{
			xmlLevel1->QueryFloatAttribute("x", &posX);
			xmlLevel1->QueryFloatAttribute("z", &posZ);
			xmlLevel1->QueryFloatAttribute("y", &posY);
		}
	}

	RenderWindow* rw=STDEngine::getSingletonPtr()->getWindow();
	SceneManager* sm=STDEngine::getSingletonPtr()->getSceneMgr();
	float mapHeight=rw->getWidth() * mapWidth / rw->getHeight();

	mSateliteMap=new MiniMap("SateliteMap", rw, sm, GUIManager::getSingletonPtr()->getGUIRenderer(), mapResolution);
	mSateliteMap->setSize(mapWidth, mapHeight);
	mSateliteMap->setPosition(1-mapWidth, 1.0f - mapHeight);
	mSateliteMap->setAlpha(1.0f);
	mSateliteMap->getMinimapWindow()->setText(mapTitle);

	SceneNode* camNode=mSateliteMap->getOgreCamera()->getParentSceneNode();
	camNode->setPosition(posX, posY, posZ);
	camNode->pitch(Degree(-90));
}

void STDNaviFrame::createURAV()
{
	mURAVConfig.load(OSEnvironment::getFullPath("..\\config\\URAV.xml"));

	mURAV=STDAgentManager::getSingletonPtr()->createAgent("URAV", mURAVConfig.meshName);
	mURAV->setScale(mURAVConfig.scale, mURAVConfig.scale, mURAVConfig.scale);
	mURAV->setSpeed(mURAVConfig.speed);
	mURAV->setRotatingSpeed(mURAVConfig.rotatingSpeed);
	mURAV->setHoverSpeed(mURAVConfig.hoveringSpeed);
	mURAV->setCamScanningSpeed(mURAVConfig.camScanningSpeed);
	mURAV->setLife(mURAVConfig.life);
	mURAV->setAlignment(-Vector3::UNIT_Z);
	mURAV->setCollisionRadius(mURAVConfig.collisionRadius);

	RenderWindow* rw=STDEngine::getSingletonPtr()->getWindow();
	SceneManager* sm=STDEngine::getSingletonPtr()->getSceneMgr();
	float mapHeight=rw->getWidth() * mURAVConfig.mapWidth / rw->getHeight();

	mURAVMap=new MiniMap("URAVMap", rw, sm, GUIManager::getSingletonPtr()->getGUIRenderer(), mURAVConfig.mapResolution);
	mURAVMap->setSize(mURAVConfig.mapWidth, mapHeight);
	mURAVMap->setPosition(0.0f, 1.0f - mapHeight);
	mURAVMap->setAlpha(mURAVConfig.mapAlpha);
	mURAVMap->getOgreCamera()->getParentSceneNode()->yaw(Degree(180));
	mURAVMap->getMinimapWindow()->setText(mURAVConfig.mapTitle);
	
	mURAV->addCamera("frontalCam", mURAVMap->getOgreCamera(), Vector3(mURAVConfig.frontalCamX, mURAVConfig.frontalCamY, mURAVConfig.frontalCamZ));

	mURAV->yaw(180);
	mURAV->setPosition(mURAVConfig.posX, mURAVConfig.posY, mURAVConfig.posZ);
	mURAV->setExplosionDuration(mURAVConfig.explosionDuration);
	mURAV->setExplosionParticleSystem(mURAVConfig.explosionPSName);
}

void STDNaviFrame::open()
{
}
	
void STDNaviFrame::save()
{
}

STDNaviFrame::~STDNaviFrame()
{
	if(mTank != NULL)
	{
		STDAgentManager::getSingletonPtr()->removeAgent(mTank->getName());
		mTank=NULL;
	}
	if(mURAV != NULL)
	{
		STDAgentManager::getSingletonPtr()->removeAgent(mURAV->getName());
		mURAV=NULL;
	}

	if(mTankMap != NULL)
	{
		delete mTankMap;
		mTankMap=NULL;
	}

	if(mURAVMap != NULL)
	{
		delete mURAVMap;
		mURAVMap=NULL;
	}

	if(mSateliteMap != NULL)
	{
		delete mSateliteMap;
		mSateliteMap=NULL;
	}

	SceneManager* sm=STDEngine::getSingletonPtr()->getSceneMgr();

	if(mRedLaser != NULL)
	{
		SceneNode* laserNode=mRedLaser->getParentSceneNode();
		laserNode->detachAllObjects();
		sm->getRootSceneNode()->removeAndDestroyChild(laserNode->getName());

		sm->destroyManualObject("RedLaser");
		mRedLaser=NULL;
	}

	if(mGreenLaser != NULL)
	{
		SceneNode* laserNode=mGreenLaser->getParentSceneNode();
		laserNode->detachAllObjects();
		sm->getRootSceneNode()->removeAndDestroyChild(laserNode->getName());

		sm->destroyManualObject("GreenLaser");
		mGreenLaser=NULL;
	}

	if(mBlueLaser != NULL)
	{
		SceneNode* laserNode=mBlueLaser->getParentSceneNode();
		laserNode->detachAllObjects();
		sm->getRootSceneNode()->removeAndDestroyChild(laserNode->getName());

		sm->destroyManualObject("BlueLaser");
		mBlueLaser=NULL;
	}

	std::map<std::string, STDAgent*>::iterator biter;
	STDAgent* agent=NULL;
	for(biter=mBuildings.begin(); biter != mBuildings.end(); ++biter)
	{
		agent=biter->second;
		STDAgentManager::getSingletonPtr()->removeAgent(agent->getName());
	}
	mBuildings.clear();

	for(biter=mTerrorists.begin(); biter != mTerrorists.end(); ++biter)
	{
		agent=biter->second;
		STDAgentManager::getSingletonPtr()->removeAgent(agent->getName());
	}
	mTerrorists.clear();

	if(mRaySceneQuery != NULL)
	{
		sm->destroyQuery(mRaySceneQuery);
		mRaySceneQuery=NULL;
	}
}

bool STDNaviFrame::frameExited()
{
	return STDFrame::frameExited();
}

void STDNaviFrame::subscribeEvents()
{
}

void STDNaviFrame::unsubscribeEvents()
{
	
}

bool STDNaviFrame::frameEntered()
{
	if(!STDFrame::frameEntered())
	{
		return false;
	}

	//mCamera.reset();

	mSTDOverlay->show();

	return true;
}

bool STDNaviFrame::frameStarted(const FrameEvent& evt)
{
	if(!STDEngine::getSingletonPtr()->getWindow()->isActive())
	{
		return mContinue;
	}

	if(!STDFrame::frameStarted(evt))
	{
		return false;
	}

	/*
	if(mTank->isSelected() || mURAV->isSelected())
	{
	}
	else
	{
		mCamera.frameStarted(
		isKeyDown(OIS::KC_LEFT), 
		isKeyDown(OIS::KC_RIGHT), 
		isKeyDown(OIS::KC_UP), 
		isKeyDown(OIS::KC_DOWN), 
		isKeyDown(OIS::KC_A), 
		isKeyDown(OIS::KC_S), 
		evt.timeSinceLastFrame);
	}
	*/

	Vector3 tankCenter=mTank->getPosition() + Vector3(0, 30, 0);
	Vector3 tankDisp=mTankLaserTarget - tankCenter;
	if(isKeyDown(OIS::KC_UP))
	{	
		if(mTank->getDirection().dotProduct(tankDisp) > mTank->getCollisionRadius())
		{
			mTank->move(evt.timeSinceLastFrame);
		}
	}
	else if(isKeyDown(OIS::KC_DOWN))
	{
		mTank->move(-evt.timeSinceLastFrame);
	}
	if(isKeyDown(OIS::KC_LEFT))
	{
		mTank->turn(evt.timeSinceLastFrame);
	}
	else if(isKeyDown(OIS::KC_RIGHT))
	{
		mTank->turn(-evt.timeSinceLastFrame);
	}

	Vector3 URAVDisp=mURAVLaserTarget - mURAV->getPosition();
	if(isKeyDown(OIS::KC_U))
	{
		mURAV->hover(evt.timeSinceLastFrame);
	}
	else if(isKeyDown(OIS::KC_D))
	{
		mURAV->hover(-evt.timeSinceLastFrame);
	}
	if(isKeyDown(OIS::KC_F))
	{
		if(URAVDisp.length() > mURAV->getCollisionRadius())
		{
			mURAV->move(-evt.timeSinceLastFrame, false);
		}
	}
	else if(isKeyDown(OIS::KC_B))
	{
		mURAV->move(evt.timeSinceLastFrame, false);
	}
	if(isKeyDown(OIS::KC_L))
	{
		mURAV->turn(evt.timeSinceLastFrame);
	}
	else if(isKeyDown(OIS::KC_R))
	{
		mURAV->turn(-evt.timeSinceLastFrame);
	}

	Real camScanningSpeed=mTank->getCamScanningSpeed();
	if(isKeyDown(OIS::KC_1))
	{
		mTank->yawCamera("cannonCam", evt.timeSinceLastFrame, camScanningSpeed);
	}
	else if(isKeyDown(OIS::KC_2))
	{
		mTank->yawCamera("cannonCam", evt.timeSinceLastFrame, -camScanningSpeed);
	}

	if(isKeyDown(OIS::KC_3))
	{
		mTank->pitchCamera("cannonCam", evt.timeSinceLastFrame, camScanningSpeed);
	}
	else if(isKeyDown(OIS::KC_4))
	{
		mTank->pitchCamera("cannonCam", evt.timeSinceLastFrame, -camScanningSpeed);
	}

	mTank->run(evt.timeSinceLastFrame);
	mURAV->run(evt.timeSinceLastFrame);
	std::map<std::string, STDAgent*>::iterator titer=mTerrorists.begin();
	while(titer != mTerrorists.end())
	{
		titer->second->run(evt.timeSinceLastFrame);
		if(titer->second->getLife() <= 0)
		{
			STDAgentManager::getSingletonPtr()->removeAgent(titer->second->getName());
			titer=mTerrorists.erase(titer);
		}
		else
		{
			++titer;
		}
	}

	updateLasers(evt.timeSinceLastFrame);
	updateSateliteView(evt.timeSinceLastFrame);
	updateOverlay(evt.timeSinceLastFrame);
	updateBackCameraHolder(evt.timeSinceLastFrame);
	updateCommands(evt.timeSinceLastFrame);

	STDParticleSystemManager::getSingletonPtr()->update(evt.timeSinceLastFrame);
	
	return mContinue;
}

void STDNaviFrame::updateCommands(const Real& dt)
{
	if(isKeyDown(OIS::KC_SPACE))
	{
		Vector3 tankCenter=mTank->getPosition();
		tankCenter.y+=30;
		mTank->getGun()->shoot(tankCenter, mTankLaserTarget, dt);
	}

	if(mKeyboard->isKeyDown(OIS::KC_7))
	{
		if(mURAV->hasCamera("backCam"))
		{
			bool terroristAlreadyExists=false;
			Real radius=mTerroristConfig.width;
			Real sqR=radius * radius;
			for(std::map<std::string, STDAgent*>::iterator iter=mTerrorists.begin(); iter != mTerrorists.end(); ++iter)
			{
				Vector3 pos=iter->second->getCenter();
				Vector3 disp=pos - mURAVLaserTarget;
				if(disp.squaredLength() <= sqR)
				{
					terroristAlreadyExists=true;
					break;
				}
			}

			if(terroristAlreadyExists==false)
			{
				createTerrorist();
			}
		}
	}
}

void STDNaviFrame::createTerrorist()
{
	size_t terrorId=(++mTerroristCreationCounter);
	std::ostringstream oss;
	oss << "Terrorist" << terrorId;
	STDAgent* terrorist=STDAgentManager::getSingletonPtr()->createAgent(oss.str(), mTerroristConfig.meshName);
	terrorist->setScale(mTerroristConfig.scale, mTerroristConfig.scale, mTerroristConfig.scale);
	terrorist->getEntity()->setRenderQueueGroup(RENDER_QUEUE_1);
	
	terrorist->setLife(mTerroristConfig.life);
	Vector3 disp=mURAVLaserTarget - mURAV->getPosition();
	Real length=disp.normalise();
	Vector3 pos=mURAV->getPosition() + disp * (length+ mTerroristConfig.depth);
	pos.y-=mTerroristConfig.elevation;
	terrorist->setPosition(pos);

	if(mTerroristConfig.animation.compare("") != 0)
	{
		terrorist->setAnimationState(mTerroristConfig.animation);
	}
	terrorist->setExplosionDuration(mTerroristConfig.explosionDuration);
	terrorist->setExplosionParticleSystem(mTerroristConfig.explosionPSName);
	mTerrorists[oss.str()]=terrorist;
}

void STDNaviFrame::updateBackCameraHolder(const Real& dt)
{
	if(isKeyDown(OIS::KC_5))
	{
		if(mTank->hasCamera("backCam"))
		{
			mTank->removeCamera("backCam");
			mURAV->addCamera("backCam", STDEngine::getSingletonPtr()->getCamera(), Vector3(mURAVConfig.backCamX, mURAVConfig.backCamY, mURAVConfig.backCamZ));
			mURAV->getCamera("backCam")->getParentSceneNode()->yaw(Degree(180));
		}
	}
	else if(isKeyDown(OIS::KC_6))
	{
		if(mURAV->hasCamera("backCam"))
		{
			mURAV->removeCamera("backCam");
			mTank->addCamera("backCam", STDEngine::getSingletonPtr()->getCamera(), Vector3(mTankConfig.backCamX, mTankConfig.backCamY, mTankConfig.backCamZ));
			mTank->getCamera("backCam")->getParentSceneNode()->yaw(Degree(180));
		}
	}
}

void STDNaviFrame::updateSateliteView(const Real& dt)
{
	Vector3 tankPos=mTank->getPosition();
	SceneNode* sateliteCamNode=mSateliteMap->getOgreCamera()->getParentSceneNode();
	sateliteCamNode->setPosition(tankPos.x, sateliteCamNode->getPosition().y, tankPos.z);
}

void STDNaviFrame::updateOverlay(const Real& dt)
{
	Ogre::OverlayContainer* panel=static_cast<Ogre::OverlayContainer*>(mSTDOverlay->getChild("Core/STDPanel"));

	std::ostringstream oss;
	const Vector3& tankPos=mTank->getPosition();
	oss << "Tank Position: (" << tankPos.x << ", " << tankPos.y << ", " << tankPos.z << ")";
	panel->getChild("Xianshun/STD/TankPosition")->setCaption(oss.str()); 

	oss.str("");
	const Vector3& tankDir=mTank->getDirection();
	oss << "Tank Orientation: (" << tankDir.x << ", " << tankDir.y << ", " << tankDir.z << ")";
	panel->getChild("Xianshun/STD/TankOrientation")->setCaption(oss.str());

	oss.str("");
	const Vector3& uravPos=mURAV->getPosition();
	oss << "UAV Position: (" << uravPos.x << ", " << uravPos.y << ", " << uravPos.z << ")";
	panel->getChild("Xianshun/STD/UAVPosition")->setCaption(oss.str()); 

	oss.str("");
	const Vector3& uravDir=mURAV->getDirection();
	oss << "UAV Orientation: (" << uravDir.x << ", " << uravDir.y << ", " << uravDir.z << ")";
	panel->getChild("Xianshun/STD/UAVOrientation")->setCaption(oss.str());

	oss.str("");
	const Vector3& camDir=mTank->getCamDirection("cannonCam");
	oss << "Tank Cannon Orientation: (" << camDir.x << ", " << camDir.y << ", " << camDir.z << ")";
	panel->getChild("Xianshun/STD/TankCannonOrientation")->setCaption(oss.str());

	oss.str("");
	Vector3 disp=mTankLaserTarget - mURAVLaserTarget;
	if(disp.length() <= mTankConfig.laserConnectLimit)
	{
		oss << "Laser Target Locked: true";
	}
	else
	{
		oss << "Laser Target Locked: false";
	}
	panel->getChild("Xianshun/STD/LaserConnect")->setCaption(oss.str());

	oss.str("");
	oss << "Tank Missiles: " << mTank->getGun()->getBulletCount();
	panel->getChild("Xianshun/STD/TankMissileCount")->setCaption(oss.str());

	oss.str("");
	oss << "JoySticks: " << mJoySticks.size();
	panel->getChild("Xianshun/STD/JoySticks")->setCaption(oss.str());

	oss.str("");
	oss << "Tank HP: " << mTank->getLife();
	panel->getChild("Xianshun/STD/TankLife")->setCaption(oss.str());

	oss.str("");
	oss << "UAV HP: " << mURAV->getLife();
	panel->getChild("Xianshun/STD/URAVLife")->setCaption(oss.str());

	oss.str("");
	oss << "Terrorist [Count: " << mTerrorists.size();

	STDAgent* targetedTerrorist=NULL;
	Real radius=10;
	Real sqR=radius * radius;
	for(std::map<std::string, STDAgent*>::iterator iter=mTerrorists.begin(); iter != mTerrorists.end(); ++iter)
	{
		Vector3 pos=iter->second->getCenter();
		Vector3 disp=pos - mURAVLaserTarget;
		if(disp.squaredLength() <= sqR)
		{
			targetedTerrorist=iter->second;
			break;
		}
	}

	if(targetedTerrorist != NULL)
	{
		oss << " Locked: " << targetedTerrorist->getName() << "]";
	}
	else
	{
		oss << "]";
	}

	panel->getChild("Xianshun/STD/TerroristCount")->setCaption(oss.str());

	
}

void STDNaviFrame::updateLasers(const Real& dt)
{
	mBlueLaser->clear();

	Vector3 tankCenter=mTank->getPosition();
	tankCenter.y+=30;
	Vector3 URAVCenter=mURAV->getPosition();

	mBlueLaser->begin("blue", RenderOperation::OT_LINE_LIST);
	mBlueLaser->position(tankCenter);
	mBlueLaser->position(URAVCenter);
	mBlueLaser->end();

	std::set<MovableObject*> excludedObjs;
	excludedObjs.insert(mTank->getEntity());

	mTankLaserTarget=tankCenter+mTank->getCamDirection("cannonCam") * 1000;
	OgreRayUtil::RaycastFromPoint(mRaySceneQuery, tankCenter, mTank->getCamDirection("cannonCam"), mTankLaserTarget, excludedObjs);

	mRedLaser->clear();
	mRedLaser->begin("red", RenderOperation::OT_LINE_LIST);
	mRedLaser->position(tankCenter);
	mRedLaser->position(mTankLaserTarget);
	mRedLaser->end();

	mURAVLaserTarget=URAVCenter+mURAV->getCamDirection("frontalCam") * 1000;
	OgreRayUtil::RaycastFromPoint(mRaySceneQuery, URAVCenter, mURAV->getCamDirection("frontalCam"), mURAVLaserTarget);

	mGreenLaser->clear();
	mGreenLaser->begin("green", RenderOperation::OT_LINE_LIST);
	mGreenLaser->position(URAVCenter);
	mGreenLaser->position(mURAVLaserTarget);
	mGreenLaser->end();
}

bool STDNaviFrame::keyReleased(const OIS::KeyEvent& evt)
{
	if(!STDFrame::keyReleased(evt))
	{
		return keyReleased3DWorld(evt);
	}

	return true;
}

bool STDNaviFrame::keyPressed(const OIS::KeyEvent& evt)
{
	if(!STDFrame::keyPressed(evt))
	{
		return keyPressed3DWorld(evt);
	}

	return true;
}

bool STDNaviFrame::keyPressed3DWorld(const OIS::KeyEvent& evt)
{
	switch(evt.key)
	{
	case OIS::KC_DELETE:
		break;
	default:
		break;
	}

	return true;
}

bool STDNaviFrame::keyReleased3DWorld(const OIS::KeyEvent& evt)
{
	return true;
}

bool STDNaviFrame::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	if(!STDFrame::mouseReleased(evt, id))
	{
		return mouseReleased3DWorld(evt, id);
	}
	return true;
}

bool STDNaviFrame::mouseReleased3DWorld(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	if(mLeftButtonDown == false)
	{
		
	}
	return true;
}

bool STDNaviFrame::mouseMoved3DWorld(const OIS::MouseEvent& evt)
{
	if(mLeftButtonDown)
	{
		
	}

	return true;
}

bool STDNaviFrame::mouseMoved(const OIS::MouseEvent& evt)
{
	if(!STDFrame::mouseMoved(evt))
	{
		return mouseMoved3DWorld(evt);
	}

	return true;
}

bool STDNaviFrame::mousePressed3DWorld(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	if(mLeftButtonDown)
	{
		mTank->setSelected(false);
		mURAV->setSelected(false);

		MovableObject* mo=mMousePicker.pick(evt);

		if(mTank->hasEntity(mo))
		{
			mTank->setSelected(true);
		}
		else if(mURAV->hasEntity(mo))
		{
			mURAV->setSelected(true);
		}
	}

	return true;
}

bool STDNaviFrame::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	if(!STDFrame::mousePressed(evt, id))
	{
		return mousePressed3DWorld(evt, id);
	}

	return true;
}

