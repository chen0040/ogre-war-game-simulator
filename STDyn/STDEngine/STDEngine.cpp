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
{
	if(mSingleton==NULL)
	{
		mSingleton=this;
	}
	else
	{
		throw Exception(42, "multiple instances not allowed", "STDEngine::STDEngine()");
	}
}

STDEngine::~STDEngine()
{

}

void STDEngine::go()
{
	OSEnvironment::setCurrentDir2ExecutableDir();
	
	createRoot();
	defineResources();
	setupRenderSystem();
	createRenderWindow();
	initializeResources();
	createSceneMgr();
	setupInput();
	setupScene();
	setupGUI();
	setupFrameListener();
	startRenderLoop();
	cleanup();
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

	ResourceGroupManager::getSingleton().addResourceLocation("../media2/models", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("../media2/materials/scripts", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("../media2/materials/textures", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("../media2/particles", "FileSystem");
	ResourceGroupManager::getSingleton().addResourceLocation("../media2/overlays", "FileSystem");
}

void STDEngine::setupRenderSystem()
{
	TiXmlDocument doc;
	if(!doc.LoadFile(OSEnvironment::getFullPath("..\\config\\ogre.xml").c_str()))
	{
		throw Exception(52, "Failed to load mllab_ogre.xml", "STDEngine::setupRenderSystem()");
	}

	TiXmlElement* xmlRoot=doc.RootElement();
	if(strcmp(xmlRoot->Attribute("ogre_config"), "true")==0)
	{
		//!mRoot->restoreConfig() && 
		if(!mRoot->showConfigDialog())
		{
			throw Exception(52, "Failed to load config", "STDEngine::setupRenderSystem()");
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
		throw Exception(42, "mSystem==NULL", "STDEngine::setupRenderSystem()");
	}
}

void STDEngine::createRenderWindow()
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

	mRoot->initialise(true, "Urban Warfare Simulator");
	mWindow=mRoot->getAutoCreatedWindow();

	WindowEventUtilities::addWindowEventListener(mWindow, this);
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
	mWorld->createWorld_FlatGround("Examples/Rockwall");

	//for creating the main camera
	mCamera=mSceneMgr->createCamera("DefaultCamera");
	mCamera->setNearClipDistance(5);
	
    Ogre::SceneNode * camNode= mSceneMgr->getRootSceneNode()->createChildSceneNode("CamNode");
    camNode->setPosition(0, 0.0, 0);  
    camNode->attachObject(mCamera);

	Viewport* vp=mWindow->addViewport(mCamera);
	mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));

	ColourValue fadeColour(0.9f, 0.9f, 0.9f);
	vp->setBackgroundColour(fadeColour);

	//for creating the fog effect
	mSceneMgr->setFog(FOG_LINEAR, fadeColour, 0.0f, 500, 5000);

	//for creating the sky plane
	mWorld->createSkyPlane("Examples/CloudySky");

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

void STDEngine::setupInput()
{
	OIS::ParamList pl;
	size_t hwnd;
	mWindow->getCustomAttribute("WINDOW", &hwnd);
	std::ostringstream oss;
	oss << hwnd;
	pl.insert(std::make_pair<std::string, std::string>(std::string("WINDOW"), oss.str()));
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

		if(mInputManager->numJoySticks() > 0)
		{
			mJoySticks.resize(mInputManager->numJoySticks());

			for(std::vector<OIS::JoyStick*>::iterator curr = mJoySticks.begin(); curr != mJoySticks.end(); ++curr)
            {
                (*curr) = static_cast<OIS::JoyStick*>(mInputManager->createInputObject(OIS::OISJoyStick, true));
            }
		}

		unsigned int width, height, depth;
		int left, top;
		mWindow->getMetrics(width, height, depth, left, top);

		const OIS::MouseState& ms=mMouse->getMouseState();
		ms.width=width;
		ms.height=height;

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
	bool bContinue=mRoot->renderOneFrame();
	if(bContinue)
	{
		WindowEventUtilities::messagePump();
	}
	return bContinue;
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
		 if( mJoySticks.size() > 0 )
        {
			for(std::vector<OIS::JoyStick*>::iterator curr = mJoySticks.begin(); curr != mJoySticks.end(); ++curr)
            {
                mInputManager->destroyInputObject( *curr );
            }

            mJoySticks.clear();
        }
		if(mInputManager)
		{
			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager=NULL;
		}

		LogManager::getSingletonPtr()->destroyLog("user.log");

		WindowEventUtilities::removeWindowEventListener(mWindow, this);
		mRoot->shutdown();
		delete mRoot;
		mRoot=NULL;
		mWindow=NULL;
		mSystem=NULL;
		mCamera=NULL;
		mSceneMgr=NULL;
		mCurrentFrame=NULL;
	}
}

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

void STDEngine::logMessage(const std::string& message)
{
	LogManager::getSingletonPtr()->getLog("user.log")->logMessage(message);
}

