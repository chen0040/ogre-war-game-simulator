#include "stdafx.h"
#include "STDNaviFrame.h"
#include "../STDEngine/STDEngine.h"
#include "../OSEnvironment/OSEnvironment.h"
#include "../GUI/GUIManager.h"
#include "../tinyxml/tinyxml.h"
#include "../STDAgent/STDAgentManager.h"
#include "../STDParticleSystem/STDParticleSystemManager.h"

STDNaviFrame::STDNaviFrame(std::string frameName)
: STDFrame(frameName)
{
	STDEngine* app=STDEngine::getSingletonPtr();

	mSTDOverlay= Ogre::OverlayManager::getSingleton().getByName("Xianshun/STDOverlay");
	if(mSTDOverlay == NULL)
	{
		throw Exception(42, "mSTDOverlay == NULL", "STDNaviFrame::STDNaviFrame()");
	}

	createWorld();

	mMousePicker.setCamera(app->getCamera());

	mTankConfig.load(OSEnvironment::getFullPath("..\\config\\Tank.xml"));
	mTerroristConfig.load(OSEnvironment::getFullPath("..\\config\\Terrorist.xml"));
	mURAVConfig.load(OSEnvironment::getFullPath("..\\config\\URAV.xml"));

	mURAV=mURAVConfig.createURAV("URAV", true);
	mURAV->createCannonLaser();

	mTank=mTankConfig.createTank("Tank", true);
	mTank->createCannonLaser();
	mTank->track(mURAV);
	mTank->switchMainView(true);

	mTerroristConfig.addEnemy(mURAV);
	mTerroristConfig.addEnemy(mTank);

	createSatelite();
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

void STDNaviFrame::createSatelite()
{
	TiXmlDocument doc;

	if(!doc.LoadFile(OSEnvironment::getFullPath("..\\config\\Satelite.xml").c_str()))
	{
		throw Exception(42, "Failed to load Satelite.xml", "STDNaviFrame::createSatelite()");
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

STDNaviFrame::~STDNaviFrame()
{
	if(mSateliteMap != NULL)
	{
		delete mSateliteMap;
		mSateliteMap=NULL;
	}

	std::map<std::string, STDAgent*>::iterator biter;
	STDAgent* agent=NULL;
	for(biter=mBuildings.begin(); biter != mBuildings.end(); ++biter)
	{
		agent=biter->second;
		STDAgentManager::getSingletonPtr()->removeAgent(agent->getName());
	}
	mBuildings.clear();
}

bool STDNaviFrame::frameEntered()
{
	if(!STDFrame::frameEntered())
	{
		return false;
	}

	mSTDOverlay->show();

	return true;
}

bool STDNaviFrame::frameEnded(const FrameEvent& evt)
{
	return mContinue;
}

bool STDNaviFrame::frameStarted(const FrameEvent& evt)
{
	STDEngine* app=STDEngine::getSingletonPtr();
	if(!app->getWindow()->isActive())
	{
		return mContinue;
	}

	if(!STDFrame::frameStarted(evt))
	{
		return false;
	}

	if(app->isJoystickEnabled())
	{
		if(mJoysticks.size() > 0)
		{
			const Real& sensitivity=app->getJoystickAxisSensitivity();	
			handleTankByJoystickAxe(0, sensitivity);

			if(mJoysticks.size() == 1)
			{
				handleCannonByJoystickPOVHat(0);
			}
			else 
			{
				handleCannonByJoystickAxe(1, sensitivity);
			}

			if(mJoysticks.size() > 2)
			{
				handleURAVByJoystickAxe(2, sensitivity);
			}
		}
		
	}

	mTankConfig.updateTanks(evt.timeSinceLastFrame);
	mURAVConfig.updateURAVs(evt.timeSinceLastFrame);
	mTerroristConfig.updateTerrorists(evt.timeSinceLastFrame);

	updateSateliteView();
	updateOverlay();

	STDParticleSystemManager::getSingletonPtr()->update(evt.timeSinceLastFrame);
	
	return mContinue;
}

void STDNaviFrame::handleTankByJoystickAxe(size_t joystickId, const Real& sensitivity)
{
	int axisMax=static_cast<int>(OIS::JoyStick::MAX_AXIS * sensitivity);
	int axisMin=static_cast<int>(OIS::JoyStick::MIN_AXIS * sensitivity);
	
	int y=mJoysticks[0]->getJoyStickState().mAxes[0].abs;
	int x=mJoysticks[0]->getJoyStickState().mAxes[1].abs;

	if(x > axisMax)
	{
		mTankConfig.turnLeft(true);
		mTankConfig.turnRight(false);
	}
	else if(x < axisMin)
	{
		mTankConfig.turnRight(true);
		mTankConfig.turnLeft(false);
	}
	else
	{
		mTankConfig.turnLeft(false);
		mTankConfig.turnRight(false);
	}

	if(y > axisMax)
	{
		mTankConfig.moveForward(true);
		mTankConfig.moveBackward(false);
	}
	else if(y < axisMin)
	{
		mTankConfig.moveBackward(true);
		mTankConfig.moveForward(false);
	}
	else
	{
		mTankConfig.moveForward(false);
		mTankConfig.moveBackward(false);
	}
}

void STDNaviFrame::handleCannonByJoystickPOVHat(size_t joystickId)
{
	int cannonDirection=mJoysticks[joystickId]->getJoyStickState().mPOV[0].direction;
	switch(cannonDirection)
	{
	case OIS::Pov::Centered:
		mTankConfig.resetCannonMotion();
		break;
	case OIS::Pov::North:
		mTankConfig.turnCannonUp(true);
		break;
	case OIS::Pov::South:
		mTankConfig.turnCannonDown(true);
		break;
	case OIS::Pov::East:
		mTankConfig.turnCannonLeft(true);
		break;
	case OIS::Pov::West:
		mTankConfig.turnCannonRight(true);
		break;
	case OIS::Pov::NorthEast:
		mTankConfig.turnCannonUp(true);
		mTankConfig.turnCannonLeft(true);
		break;
	case OIS::Pov::NorthWest:
		mTankConfig.turnCannonUp(true);
		mTankConfig.turnCannonRight(true);
		break;
	case OIS::Pov::SouthEast:
		mTankConfig.turnCannonDown(true);
		mTankConfig.turnCannonLeft(true);
		break;
	case OIS::Pov::SouthWest:
		mTankConfig.turnCannonDown(true);
		mTankConfig.turnCannonRight(true);
		break;
	default:
		break;
	}
}

void STDNaviFrame::handleURAVByJoystickAxe(size_t joystickId, const Real& sensitivity)
{
	int axisMax=static_cast<int>(OIS::JoyStick::MAX_AXIS * sensitivity);
	int axisMin=static_cast<int>(OIS::JoyStick::MIN_AXIS * sensitivity);
	
	int y=mJoysticks[joystickId]->getJoyStickState().mAxes[0].abs;
	int x=mJoysticks[joystickId]->getJoyStickState().mAxes[1].abs;

	if(x > axisMax)
	{
		mURAVConfig.turnLeft(true);
		mURAVConfig.turnRight(false);
	}
	else if(x < axisMin)
	{
		mURAVConfig.turnRight(true);
		mURAVConfig.turnLeft(false);
	}
	else
	{
		mURAVConfig.turnLeft(false);
		mURAVConfig.turnRight(false);
	}

	if(y > axisMax)
	{
		mURAVConfig.moveUp(true);
		mURAVConfig.moveDown(false);
	}
	else if(y < axisMin)
	{
		mURAVConfig.moveDown(true);
		mURAVConfig.moveUp(false);
	}
	else
	{
		mURAVConfig.moveUp(false);
		mURAVConfig.moveDown(false);
	}
}

void STDNaviFrame::handleCannonByJoystickAxe(size_t joystickId, const Real& sensitivity)
{
	int axisMax=static_cast<int>(OIS::JoyStick::MAX_AXIS * sensitivity);
	int axisMin=static_cast<int>(OIS::JoyStick::MIN_AXIS * sensitivity);
	
	int y=mJoysticks[joystickId]->getJoyStickState().mAxes[0].abs;
	int x=mJoysticks[joystickId]->getJoyStickState().mAxes[1].abs;

	if(x > axisMax)
	{
		mTankConfig.turnCannonLeft(true);
		mTankConfig.turnCannonRight(false);
	}
	else if(x < axisMin)
	{
		mTankConfig.turnCannonRight(true);
		mTankConfig.turnCannonLeft(false);
	}
	else
	{
		mTankConfig.turnCannonLeft(false);
		mTankConfig.turnCannonRight(false);
	}

	if(y > axisMax)
	{
		mTankConfig.turnCannonUp(true);
		mTankConfig.turnCannonDown(false);
	}
	else if(y < axisMin)
	{
		mTankConfig.turnCannonDown(true);
		mTankConfig.turnCannonUp(false);
	}
	else
	{
		mTankConfig.turnCannonUp(false);
		mTankConfig.turnCannonDown(false);
	}
}

void STDNaviFrame::updateSateliteView()
{
	Vector3 tankPos=mTank->getPosition();
	SceneNode* sateliteCamNode=mSateliteMap->getOgreCamera()->getParentSceneNode();
	sateliteCamNode->setPosition(tankPos.x, sateliteCamNode->getPosition().y, tankPos.z);
}

void STDNaviFrame::save()
{
	mTerroristConfig.save(OSEnvironment::getFullPath("..\\config\\Terrorist.xml"));
}

void STDNaviFrame::createNew()
{
	mTerroristConfig.createTerrorist();
}

void STDNaviFrame::open()
{
	mTerroristConfig.createTerrorists();
}

void STDNaviFrame::updateOverlay()
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
	const Vector3& camDir=mTank->getCannonDirection();
	oss << "Tank Cannon Orientation: (" << camDir.x << ", " << camDir.y << ", " << camDir.z << ")";
	panel->getChild("Xianshun/STD/TankCannonOrientation")->setCaption(oss.str());

	oss.str("");
	if(mTankConfig.isLaserConnected(mTank->getLaserTarget(), mURAV->getLaserTarget()))
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
	oss << "JoySticks: " << mJoysticks.size();
	panel->getChild("Xianshun/STD/JoySticks")->setCaption(oss.str());

	oss.str("");
	oss << "Tank HP: " << mTank->getLife();
	panel->getChild("Xianshun/STD/TankLife")->setCaption(oss.str());

	oss.str("");
	oss << "UAV HP: " << mURAV->getLife();
	panel->getChild("Xianshun/STD/URAVLife")->setCaption(oss.str());

	oss.str("");
	oss << "Terrorist [Count: " << mTerroristConfig.getTerroristCount();
	Terrorist* terrorist=mTerroristConfig.getNearbyTerrorist(mTank->getLaserTarget(), 10);
	if(terrorist != NULL)
	{
		oss << " Locked: " << terrorist->getName() << "]";
	}
	else
	{
		oss << "]";
	}

	panel->getChild("Xianshun/STD/TerroristCount")->setCaption(oss.str());

	panel=static_cast<Ogre::OverlayContainer*>(mSTDOverlay->getChild("Core/CaelumSys"));
	if(WorldInfoManager::getSingletonPtr()->isCaelumSystemAvailable())
	{
		std::ostringstream oss;

		int year, month, day, hour, minute, second;
		WorldInfoManager::getSingletonPtr()->getCaelumSystemDateTime(year, month, day, hour, minute, second);

		oss << "Current Time: " << year << "-" << month << "-" << day << " " << hour << ":" << minute << ":" << second;

		panel->getChild("CaelumSys/STD/DateTime")->setCaption(oss.str());
	}
	else
	{
		panel->hide();
	}	
}

bool STDNaviFrame::keyReleased(const OIS::KeyEvent& evt)
{
	if(!STDFrame::keyReleased(evt))
	{
		switch(evt.key)
		{
		case OIS::KC_UP:
			mTankConfig.moveForward(false);
			break;
		case OIS::KC_DOWN:
			mTankConfig.moveBackward(false);
			break;
		case OIS::KC_LEFT:
			mTankConfig.turnLeft(false);
			break;
		case OIS::KC_RIGHT:
			mTankConfig.turnRight(false);
			break;
		case OIS::KC_L:
			mURAVConfig.turnLeft(false);
			break;
		case OIS::KC_R:
			mURAVConfig.turnRight(false);
			break;
		case OIS::KC_U:
			mURAVConfig.moveUp(false);
			break;
		case OIS::KC_D:
			mURAVConfig.moveDown(false);
			break;
		case OIS::KC_F:
			mURAVConfig.moveForward(false);
			break;
		case OIS::KC_B:
			mURAVConfig.moveBackward(false);
			break;
		case OIS::KC_1:
			mTankConfig.turnCannonLeft(false);
			break;
		case OIS::KC_2:
			mTankConfig.turnCannonRight(false);
			break;
		case OIS::KC_3:
			mTankConfig.turnCannonUp(false);
			break;
		case OIS::KC_4:
			mTankConfig.turnCannonDown(false);
			break;
		case OIS::KC_SPACE:
			mTankConfig.shoot(false);
			break;
		default:
			break;
		}
	}

	return true;
}

bool STDNaviFrame::keyPressed(const OIS::KeyEvent& evt)
{
	if(!STDFrame::keyPressed(evt))
	{
		switch(evt.key)
		{
		case OIS::KC_UP:
			mTankConfig.moveForward(true);
			break;
		case OIS::KC_DOWN:
			mTankConfig.moveBackward(true);
			break;
		case OIS::KC_LEFT:
			mTankConfig.turnLeft(true);
			break;
		case OIS::KC_RIGHT:
			mTankConfig.turnRight(true);
			break;
		case OIS::KC_L:
			mURAVConfig.turnLeft(true);
			break;
		case OIS::KC_R:
			mURAVConfig.turnRight(true);
			break;
		case OIS::KC_U:
			mURAVConfig.moveUp(true);
			break;
		case OIS::KC_D:
			mURAVConfig.moveDown(true);
			break;
		case OIS::KC_F:
			mURAVConfig.moveForward(true);
			break;
		case OIS::KC_B:
			mURAVConfig.moveBackward(true);
			break;
		case OIS::KC_1:
			mTankConfig.turnCannonLeft(true);
			break;
		case OIS::KC_2:
			mTankConfig.turnCannonRight(true);
			break;
		case OIS::KC_3:
			mTankConfig.turnCannonUp(true);
			break;
		case OIS::KC_4:
			mTankConfig.turnCannonDown(true);
			break;
		case OIS::KC_5:
			mTank->switchMainView(false);
			mURAV->switchMainView(true);
			break;
		case OIS::KC_6:
			mURAV->switchMainView(false);
			mTank->switchMainView(true);
			break;
		case OIS::KC_7:
			if(mURAV->hasMainView())
			{
				if(mTerroristConfig.getNearbyTerrorist(mURAV->getLaserTarget()) == NULL)
				{
					Terrorist* agnt=mTerroristConfig.createTerrorist(mURAV->getPosition(), mURAV->getLaserTarget());
				}
			}
			break;
		case OIS::KC_SPACE:
			mTankConfig.shoot(true);
			break;
		default:
			break;
		}
	}

	return true;
}

bool STDNaviFrame::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	if(!STDFrame::mousePressed(evt, id))
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
	}

	return true;
}

bool STDNaviFrame::buttonPressed( const OIS::JoyStickEvent &arg, int button ) 
{ 
	if(STDEngine::getSingletonPtr()->isJoystickEnabled())
	{
		switch(button)
		{
		case OIS::MB_Left:
			mTankConfig.shoot(true);
			break;
		case OIS::MB_Right:
				mTank->switchMainView(false);
				mURAV->switchMainView(true);
				break;
		case OIS::MB_Middle:
				mURAV->switchMainView(false);
				mTank->switchMainView(true);
				break;
		case OIS::MB_Button3:
			if(mURAV->hasMainView())
			{
				if(mTerroristConfig.getNearbyTerrorist(mURAV->getLaserTarget()) == NULL)
				{
					Terrorist* agnt=mTerroristConfig.createTerrorist(mURAV->getPosition(), mURAV->getLaserTarget());
				}
			}
			break;
		case OIS::MB_Button4:
			mURAVConfig.turnLeft(true);
			break;
		case OIS::MB_Button5:
			mURAVConfig.turnRight(true);
			break;
		case OIS::MB_Button6:
			mURAVConfig.moveForward(true);
			break;
		case OIS::MB_Button7:
			mURAVConfig.moveBackward(true);
			break;
		default:
			break;
		}
	}

	return true; 
}

bool STDNaviFrame::buttonReleased( const OIS::JoyStickEvent &arg, int button ) 
{ 
	if(STDEngine::getSingletonPtr()->isJoystickEnabled())
	{
		switch(button)
		{
		case OIS::MB_Left:
			mTankConfig.shoot(false);
			break;
		case OIS::MB_Button4:
			mURAVConfig.turnLeft(false);
			break;
		case OIS::MB_Button5:
			mURAVConfig.turnRight(false);
			break;
		case OIS::MB_Button6:
			mURAVConfig.moveForward(false);
			break;
		case OIS::MB_Button7:
			mURAVConfig.moveBackward(false);
			break;
			default:
				break;
		}
	}
	return true; 
}

bool STDNaviFrame::axisMoved( const OIS::JoyStickEvent &arg, int axis ) 
{ 	
	return true; 
}

//Joystick Event, amd sliderID
bool STDNaviFrame::sliderMoved( const OIS::JoyStickEvent & arg, int sliderID) 
{
	return true;
}

//Joystick Event, amd povID
bool STDNaviFrame::povMoved( const OIS::JoyStickEvent & arg, int pov) 
{
	return true;
}

