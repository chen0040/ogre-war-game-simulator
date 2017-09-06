#include "GUIManager.h"
#include "../tinyxml/tinyxml.h"
#include "../OSEnvironment/OSEnvironment.h"

GUIManager* GUIManager::mSingleton=NULL;

GUIManager::GUIManager(RenderWindow* rw, SceneManager* sm)
: mWindow(rw)
, mSceneMgr(sm)
{
	if(mSingleton==NULL)
	{
		mSingleton=this;
	}
	else
	{
		throw Ogre::Exception(42, "Multiple instances not allowed!", "GUIManager::GUIManager(RenderWindow* rw, SceneManager* sm)");
	}
	mGUIRenderer=new CEGUI::OgreCEGUIRenderer(mWindow, RENDER_QUEUE_OVERLAY, false, 3000, mSceneMgr);
	mGUISystem=new CEGUI::System(mGUIRenderer);

	CEGUI::SchemeManager::getSingletonPtr()->loadScheme((CEGUI::utf8*)"TaharezLookSkin.scheme");

	mGUISystem->setDefaultFont((CEGUI::utf8*)"BlueHighway-12");

	mGUISystem->setDefaultMouseCursor((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
	CEGUI::MouseCursor::getSingletonPtr()->setImage(mGUISystem->getDefaultMouseCursor());

	CEGUI::Window* sheet=CEGUI::WindowManager::getSingletonPtr()->createWindow((CEGUI::utf8*)"DefaultGUISheet", (CEGUI::utf8*)"Application/GUISheet");
	mGUISystem->setGUISheet(sheet);
}

GUIManager* GUIManager::getSingletonPtr()
{
	return mSingleton;
}

GUIManager& GUIManager::getSingleton()
{
	return *mSingleton;
}

GUIManager::~GUIManager()
{		
	if(mGUISystem != NULL)
	{
		delete mGUISystem;
		mGUISystem=NULL;
	}

	if(mGUIRenderer != NULL)
	{
		delete mGUIRenderer;
		mGUIRenderer=NULL;
	}
}




