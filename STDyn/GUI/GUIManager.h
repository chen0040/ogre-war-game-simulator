#ifndef _H_ML_LAB_GUI_MANAGER_H
#define _H_ML_LAB_GUI_MANAGER_H

#include <CEGUI/CEGUI.h>
#include <Ogre.h>
#include <OgreCEGUIRenderer.h>
#include <vector>
#include <stack>

using namespace Ogre;

class GUIManager
{
private:
	GUIManager(const GUIManager& rhs) { }
	GUIManager& operator=(const GUIManager& rhs) { return *this; }

public:
	GUIManager(RenderWindow* rw, SceneManager* sm);
	virtual ~GUIManager();
	static GUIManager* getSingletonPtr();
	static GUIManager& getSingleton();

protected:
	static GUIManager* mSingleton;

protected:
	CEGUI::OgreCEGUIRenderer* mGUIRenderer;
	CEGUI::System* mGUISystem;
	Ogre::RenderWindow* mWindow;
	Ogre::SceneManager* mSceneMgr;

public:
	CEGUI::OgreCEGUIRenderer* getGUIRenderer() { return mGUIRenderer; }
	CEGUI::System* getGUISystem() { return mGUISystem; }

public:
	CEGUI::Window* getGUISheet() { return mGUISystem->getGUISheet(); }
};
#endif