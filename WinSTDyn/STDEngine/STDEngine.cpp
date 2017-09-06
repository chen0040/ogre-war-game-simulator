#include "stdafx.h"
#include "STDEngine.h"
#include "../tinyxml/tinyxml.h"
#include "../OSEnvironment/OSEnvironment.h"
#include "../STDFrame/STDNaviFrame.h"

STDEngine* STDEngine::mSingleton=NULL;

STDEngine::STDEngine()
: mRoot(NULL)
, mWorld(NULL)
, mCurrentFrame(NULL)
, mWeaponMgr(NULL)
, mAgentMgr(NULL)
, mParticleSystemMgr(NULL)
, mWindow(NULL)
, mSceneMgr(NULL)
, mStage(ACS_SETUP_NULL)
, mFPS(30)
, mCaelumSystemEnabled(false)
, mCaelumSystemCloudSpeed(0.000025f, -0.000045f)
, mJoystickAxisSensitivity(0.5f)
, mJoystickEnabled(true)
{
	if(mSingleton==NULL)
	{
		mSingleton=this;
	}
	else
	{
		throw Exception(42, "multiple instances not allowed", "STDEngine::STDEngine()");
	}

	TiXmlDocument doc;
	if(!doc.LoadFile(OSEnvironment::getFullPath("..\\config\\Joysticks.xml").c_str()))
	{
		throw Exception(42, "failed to load Joysticks.xml", "STDEngine::STDEngine()");
	}

	TiXmlElement* xmlRoot=doc.RootElement();
	if(strcmp(xmlRoot->Attribute("enabled"), "true")==0)
	{
		mJoystickEnabled=true;
	}
	else
	{
		mJoystickEnabled=false;
	}

	xmlRoot->QueryFloatAttribute("axes_sensitivy", &mJoystickAxisSensitivity);
}

STDEngine::~STDEngine()
{

}

bool STDEngine::setup(unsigned long hwnd, unsigned long hwndInput, int width, int height, bool incremental,  std::string& action)
{
	float percentage=0;
	if(incremental)
	{
		return incrementalSetup(hwnd, hwndInput, width, height, percentage, action);
	}
	else
	{
		bool setup=false;
		while(setup==false)
		{
			setup=incrementalSetup(hwnd, hwndInput, width, height, percentage, action);
		}
		return true;
	}
}

bool STDEngine::incrementalSetup(unsigned long hwnd, unsigned long hwndInput, int width, int height, float& percentage, std::string& action)
{
	bool setup=false;
	static float interval=1/13.0f;
	
	switch(mStage)
	{
	case ACS_SETUP_NULL:
		mStage=ACS_SET_PATH;
		action="Set Path...";
		percentage=interval * 1;
		break;
	case ACS_SET_PATH:
		OSEnvironment::setCurrentDir2ExecutableDir();
		mStage=ACS_CREATE_ROOT;
		action="Create Root...";
		percentage=interval * 2;
		break;
	case ACS_CREATE_ROOT:
		createRoot();
		mStage=ACS_DEFINE_RESOURCES;
		action="Define Resources...";
		percentage=interval * 3;
		break;
	case ACS_DEFINE_RESOURCES:
		defineResources();
		mStage=ACS_SETUP_RENDER_SYSTEM;
		action="Setup Render System...";
		percentage=interval * 4;
		break;
	case ACS_SETUP_RENDER_SYSTEM:
		setupRenderSystem();
		mStage=ACS_CREATE_RENDER_WINDOW;
		action="Create Render Window...";
		percentage=interval * 5;
		break;
	case ACS_CREATE_RENDER_WINDOW:
		createRenderWindow(hwnd, width, height);
		mStage=ACS_INITIALIZE_RESOURCES;
		action="Initialize Resources...";
		percentage=interval * 6;
		break;
	case ACS_INITIALIZE_RESOURCES:
		initializeResources();
		mStage=ACS_SETUP_SCENEMGR;
		action="Create SceneManager...";
		percentage=interval * 7;
		break;
	case ACS_SETUP_SCENEMGR:
		createSceneMgr();
		mStage=ACS_SETUP_INPUT;
		action="Setup Input...";
		percentage=interval * 8;
		break;
	case ACS_SETUP_INPUT:
		setupInput(hwndInput);	
		mStage=ACS_SETUP_SCENE;
		action="Setup Scene...";
		percentage=interval * 9;
		break;
	case ACS_SETUP_SCENE:
		setupScene();	
		mStage=ACS_SETUP_GUI;
		action="Setup GUI...";
		percentage=interval * 10;
		break;
	case ACS_SETUP_GUI:
		setupGUI();
		mStage=ACS_SETUP_FRAME_LISTENER;
		action="Setup Frame Listener...";
		percentage=interval * 11;
		break;
	case ACS_SETUP_FRAME_LISTENER:
		setupFrameListener();
		mStage=ACS_SETUP_COMPLETED;
		action="Setup Completed...";
		percentage=interval * 12;
		setup=true;
		break;
	case ACS_SETUP_COMPLETED:
		setup=true;
		action="Setup Completed...";
		percentage=interval * 13;
		break;
	}

	return setup;
}

void STDEngine::createSceneMgr()
{
	mSceneMgr=mRoot->createSceneManager(ST_GENERIC); 
}

void STDEngine::createRoot()
{
	mRoot=new Root("plugins.cfg", "ogre.cfg", "Ogre.log");
	LogManager::getSingletonPtr()->createLog("user.log");
}

void STDEngine::defineResources()
{
	String typeName, archName, sectName;
	ConfigFile config;
	config.load("resources.cfg");
	ConfigFile::SectionIterator si=config.getSectionIterator();
	while(si.hasMoreElements())
	{
		sectName=si.peekNextKey();
		ConfigFile::SettingsMultiMap* smm=si.getNext();
		ConfigFile::SettingsMultiMap::iterator smmi;
		for(smmi=smm->begin(); smmi != smm->end(); ++smmi)
		{
			typeName=smmi->first;
			archName=smmi->second;
			ResourceGroupManager::getSingletonPtr()->addResourceLocation(archName, typeName, sectName);
		}
	}

	TiXmlDocument doc;
	if(!doc.LoadFile(OSEnvironment::getFullPath("..\\config\\Resources.xml").c_str()))
	{
		throw Exception(42, "Failed to load Resources.xml", "STDEngine::defineResources()");
	}

	TiXmlElement* xmlRoot=doc.RootElement();
	for(TiXmlElement* xmlLevel1=xmlRoot->FirstChildElement(); xmlLevel1 != NULL; xmlLevel1 = xmlLevel1->NextSiblingElement())
	{
		ResourceGroupManager::getSingleton().addResourceLocation(xmlLevel1->Attribute("src"), "FileSystem");
	}
}

void STDEngine::setupRenderSystem()
{
	TiXmlDocument doc;
	if(!doc.LoadFile(OSEnvironment::getFullPath("..\\config\\ogre.xml").c_str()))
	{
		throw Exception(52, "Failed to load ogre.xml", "Application::setupRenderSystem()");
	}

	TiXmlElement* xmlRoot=doc.RootElement();
	xmlRoot->QueryIntAttribute("fps", &mFPS);

	if(strcmp(xmlRoot->Attribute("ogre_config"), "true")==0)
	{
		//!mRoot->restoreConfig() && 
		if(!mRoot->showConfigDialog())
		{
			throw Exception(52, "Failed to load config", "Application::setupRenderSystem()");
		}
		mSystem=mRoot->getRenderSystem();
	}
	else
	{
		
		for(TiXmlElement* xmlLevel1=xmlRoot->FirstChildElement(); xmlLevel1 != NULL; xmlLevel1=xmlLevel1->NextSiblingElement())
		{
			if(strcmp(xmlLevel1->Attribute("id"), xmlRoot->Attribute("RenderSystem"))==0)
			{
				mSystem=mRoot->getRenderSystemByName(xmlLevel1->Attribute("name"));
				for(TiXmlElement* xmlLevel2=xmlLevel1->FirstChildElement(); xmlLevel2 != NULL; xmlLevel2=xmlLevel2->NextSiblingElement())
				{
					if(strcmp(xmlLevel2->Value(), "Config")==0)
					{
						mSystem->setConfigOption(xmlLevel2->Attribute("name"), xmlLevel2->Attribute("value"));
					}
				}
				mRoot->setRenderSystem(mSystem);
				break;
			}
		}
	}

	if(mSystem==NULL)
	{
		throw Exception(42, "mSystem==NULL", "Application::setupRenderSystem()");
	}
}

void STDEngine::createRenderWindow(unsigned long hwnd, int width, int height)
{
	/*
	bool fullscreen=true;

	ConfigOptionMap& com=mSystem->getConfigOptions();

	for(ConfigOptionMap::const_iterator comi=com.begin(); comi != com.end(); ++comi)
	{
		const ConfigOption& co=comi->second;
		if(co.name.compare("Full Screen")==0 && co.currentValue.compare("No")==0)
		{
			fullscreen=false;
		}
	}

	if(fullscreen==false)
	{
		mRoot->initialise(false, "Urban Warfare Simulator");
		NameValuePairList miscParams;
		miscParams["border"]="fixed";
		mWindow=mRoot->createRenderWindow("Urban Warfare Simulator", 800, 600, false, &miscParams);
	}
	else
	{
		mRoot->initialise(true, "Urban Warfare Simulator");
		mWindow=mRoot->getAutoCreatedWindow();
	}
	*/

	mRoot->initialise(false);
	NameValuePairList miscParams;
	miscParams.insert(Ogre::NameValuePairList::value_type("externalWindowHandle",	Ogre::StringConverter::toString(hwnd)));
	mWindow=mRoot->createRenderWindow("Urban Warfare Simulator", width, height, false, &miscParams);

	//WindowEventUtilities::addWindowEventListener(mWindow, this);
}

void STDEngine::initializeResources()
{
	TextureManager::getSingletonPtr()->setDefaultNumMipmaps(5);
	ResourceGroupManager::getSingletonPtr()->initialiseAllResourceGroups();
}

void STDEngine::setupScene()
{
	 /*
	//for creating shadow
	 mSceneMgr->setAmbientLight(ColourValue(0, 0, 0));
     mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	 //for creating directional light
	Light* light = mSceneMgr->createLight("Light3");
	light->setType(Light::LT_DIRECTIONAL);
	light->setDiffuseColour(ColourValue(.25, .25, 0));
	light->setSpecularColour(ColourValue(.25, .25, 0));
	light->setDirection(Vector3( 0, -1, 0)); 

	//for creating point light
	light = mSceneMgr->createLight("SunRay");
	light->setType(Light::LT_SPOTLIGHT);
	light->setDiffuseColour(1, 1, 1);
	light->setSpecularColour(1, 1, 1.0);
	light->setDirection(0, -1, 0);
	light->setPosition(Vector3(0, 500, 0));
	light->setSpotlightRange(Degree(90), Degree(150));

	light = mSceneMgr->createLight("SunSpot");
	light->setType(Light::LT_POINT);
	light->setPosition(Vector3(0, 50, 0));
	light->setDiffuseColour(1.0, 1.0, 1.0);
	light->setSpecularColour(1.0, 1.0, 1.0);
	*/

	 //for creating the street
	mWorld=new WorldInfoManager(mSceneMgr);

	//for creating the main camera
	mCamera=mSceneMgr->createCamera("DefaultCamera");
	mCamera->setNearClipDistance(5);
	
    Ogre::SceneNode * camNode= mSceneMgr->getRootSceneNode()->createChildSceneNode("CamNode");
    camNode->setPosition(0, 0.0, 0);  
    camNode->attachObject(mCamera);

	Viewport* vp=mWindow->addViewport(mCamera);
	mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));

	mWorld->createWorld_FlatGround("Examples/Rockwall");

	ColourValue fadeColour(0.9f, 0.9f, 0.9f);
	vp->setBackgroundColour(fadeColour);
	if(mCaelumSystemEnabled)
	{
		//for creating Caelum system
		mWorld->createCaelumSystem(this->mCaelumSystemTimeScale, this->mCaelumSystemCloudSpeed);
		if(!mCaelumSystemDateTime.empty())
		{
			/*
			std::ostringstream oss;
			for(size_t i=0; i<mCaelumSystemDateTime.size(); ++i)
			{
				oss << mCaelumSystemDateTime[i] << " ";
			}
			OSEnvironment::showWinMsgBox(oss.str().c_str());
			*/

			mWorld->setCaelumSystemDateTime(mCaelumSystemDateTime[0], 
				mCaelumSystemDateTime[1],
				mCaelumSystemDateTime[2],
				mCaelumSystemDateTime[3],
				mCaelumSystemDateTime[4],
				mCaelumSystemDateTime[5]);
			
			Caelum::CaelumSystem* cs=mWorld->getCaelumSystem();

			/*
			std::ostringstream oss;
			oss << "Latitude: " << WorldInfoManager::getSingletonPtr()->getCaelumSystem()->getObserverLatitude();
			oss << " Longitude: " << mWorld->getCaelumSystem()->getObserverLongitude();
			OSEnvironment::showWinMsgBox(oss.str().c_str());
			*/
		}
	}
	else
	{
		//for creating the fog effect
		mSceneMgr->setFog(FOG_LINEAR, fadeColour, 0.0f, 500, 5000);

		//for creating the sky plane
		mWorld->createSkyPlane("Examples/CloudySky");
	}

	//create ray scene query
	mRaySceneQuery=mSceneMgr->createRayQuery(Ray());	 
	mRaySceneQuery->setSortByDistance(true);

	mParticleSystemMgr=new STDParticleSystemManager();
	mWeaponMgr=new WeaponManager();
	mAgentMgr=new STDAgentManager();
}

void STDEngine::createOrRetrieveColor(const Ogre::String& matName, Real Red, Real Green, Real Blue)
{
	MaterialManager* mm= MaterialManager::getSingletonPtr();
	Ogre::ResourceManager::ResourceCreateOrRetrieveResult result = mm->createOrRetrieve(matName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::MaterialPtr mp = result.first;
	if ((bool)result.second) {
		//if first time created, do initialization
		mp->setReceiveShadows(false); 
		mp->getTechnique(0)->setLightingEnabled(true); 	
    }
	
	mp->getTechnique(0)->getPass(0)->setDiffuse(Red,Green, Blue,0); 
	mp->getTechnique(0)->getPass(0)->setAmbient(Red, Green, Blue); 
	mp->getTechnique(0)->getPass(0)->setSelfIllumination(Red, Green, Blue); 
}

void STDEngine::setCaelumSystemDateTime(int year, int month, int day, int hour, int minute, int second)
{
	mCaelumSystemDateTime.clear();
	mCaelumSystemDateTime.push_back(year);
	mCaelumSystemDateTime.push_back(month);
	mCaelumSystemDateTime.push_back(day);
	mCaelumSystemDateTime.push_back(hour);
	mCaelumSystemDateTime.push_back(minute);
	mCaelumSystemDateTime.push_back(second);
}

void STDEngine::setupInput(unsigned long hwnd)
{
	OIS::ParamList pl;
	//size_t hwnd;
	//mWindow->getCustomAttribute("WINDOW", &hwnd);
	pl.insert(std::make_pair<std::string, std::string>(std::string("WINDOW"), Ogre::StringConverter::toString(hwnd)));
	
	if(!mWindow->isFullScreen())
	{
		#if defined OIS_WIN32_PLATFORM
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
		#elif defined OIS_LINUX_PLATFORM
		pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
		pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
		pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
		pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
		#endif
	}
	
	mInputManager=OIS::InputManager::createInputSystem(pl);

	try{
		mMouse=static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));
		mKeyboard=static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));

		unsigned int width, height, depth;
		int left, top;
		mWindow->getMetrics(width, height, depth, left, top);

		const OIS::MouseState& ms=mMouse->getMouseState();
		ms.width=width;
		ms.height=height;

		size_t joystickNum=mInputManager->numJoySticks();
		 if (joystickNum > 0) 
		 {
            mJoysticks.resize(joystickNum);

            std::vector<OIS::JoyStick*>::iterator itJoystick    = mJoysticks.begin();
            std::vector<OIS::JoyStick*>::iterator itJoystickEnd = mJoysticks.end();
            for(; itJoystick != itJoystickEnd; ++itJoystick ) 
			{
                (*itJoystick) = static_cast<OIS::JoyStick*>( mInputManager->createInputObject( OIS::OISJoyStick, true ) );
            }
        }

	}catch(OIS::Exception& e)
	{
		throw Exception(42, e.eText, "STDEngine::setupInput()");
	}
}

void STDEngine::setupGUI()
{
	mGUIManager=new GUIManager(mWindow, mSceneMgr);

	CEGUI::Window* sheet=mGUIManager->getGUISheet();
}

void STDEngine::setupFrameListener()
{
	mCurrentFrame=new STDNaviFrame("Default Frame");

	mCurrentFrame->frameEntered();

	mRoot->addFrameListener(mCurrentFrame);
}

void STDEngine::startRenderLoop()
{
	bool bContinue=true;
	while(bContinue)
	{
		bContinue=render();
	}
}	

bool STDEngine::render()
{
	return mRoot->renderOneFrame();
}

void STDEngine::save()
{
	this->mCurrentFrame->save();
}

void STDEngine::open()
{
	this->mCurrentFrame->open();
}

void STDEngine::createNew()
{
	this->mCurrentFrame->createNew();
}

void STDEngine::cleanup()
{
	if(mCurrentFrame != NULL)
	{
		delete mCurrentFrame;
		mCurrentFrame=NULL;
	}

	if(mGUIManager != NULL)
	{
		delete mGUIManager;
		mGUIManager = NULL;
	}
	
	if(mWorld != NULL)
	{
		delete mWorld;
		mWorld=NULL;
	}

	if(mWeaponMgr != NULL)
	{
		delete mWeaponMgr;
		mWeaponMgr=NULL;
	}

	if(mAgentMgr != NULL)
	{
		delete mAgentMgr;
		mAgentMgr=NULL;
	}

	if(mParticleSystemMgr != NULL)
	{
		delete mParticleSystemMgr;
		mParticleSystemMgr=NULL;
	}

	if(mRaySceneQuery != NULL)
	{
		mSceneMgr->destroyQuery(mRaySceneQuery);
		mRaySceneQuery=NULL;
	}

	if(mRoot != NULL)
	{
		if(mKeyboard)
		{
			mInputManager->destroyInputObject(mKeyboard);
			mKeyboard=NULL;
		}
		if(mMouse)
		{
			mInputManager->destroyInputObject(mMouse);
			mMouse=NULL;
		}
		 if( mJoysticks.size() > 0 )
        {
			std::vector<OIS::JoyStick*>::iterator curr = mJoysticks.begin();
			std::vector<OIS::JoyStick*>::iterator joystickEnd=mJoysticks.end();
			for(; curr != joystickEnd; ++curr)
            {
                mInputManager->destroyInputObject( *curr );
            }

            mJoysticks.clear();
        }
		if(mInputManager)
		{
			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager=NULL;
		}

		LogManager::getSingletonPtr()->destroyLog("user.log");

		//WindowEventUtilities::removeWindowEventListener(mWindow, this);
		mRoot->shutdown();
		//delete mRoot;
		mRoot=NULL;
		mWindow=NULL;
		mSystem=NULL;
		mCamera=NULL;
		mSceneMgr=NULL;
		mCurrentFrame=NULL;
	}
}

/*
void STDEngine::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width= mWindow->getWidth();
	unsigned int height=mWindow->getHeight();

	const OIS::MouseState & ms = this->mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
	mCurrentFrame->windowResized(rw);

	GUIManager::getSingletonPtr()->getGUIRenderer()->setDisplaySize(CEGUI::Size(width, height));
}

void STDEngine::windowFocusChange(Ogre::RenderWindow *rw)
{
	//only call when another window is active, for example, when the user press "Ctrl+Alt+Del"
}

bool STDEngine::windowClosing(RenderWindow* rw)
{
	mCurrentFrame->quit();
	return true;
}

void STDEngine::windowMoved(RenderWindow* rw)
{
	
}
*/

void STDEngine::injectWindowResized()
{
	mWindow->windowMovedOrResized();
	const OIS::MouseState & ms = this->mMouse->getMouseState();
	ms.width = mWindow->getWidth();
	ms.height = mWindow->getHeight();
	mCurrentFrame->windowResized(mWindow);

	GUIManager::getSingletonPtr()->getGUIRenderer()->setDisplaySize(CEGUI::Size(mWindow->getWidth(), mWindow->getHeight()));

	/*
	mItemFile->getPopupMenu()->sizeToContent();
	mItemHelp->getPopupMenu()->sizeToContent();
	mItemPages->getPopupMenu()->sizeToContent();
	mItemWindow->getPopupMenu()->sizeToContent();
	*/
}

void STDEngine::logMessage(const std::string& message)
{
	LogManager::getSingletonPtr()->getLog("user.log")->logMessage(message);
}

