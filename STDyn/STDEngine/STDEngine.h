#ifndef _H_STD_ENGINE_H
#define _H_STD_ENGINE_H

#include <Ogre.h>
#include <OIS/OIS.h>
#include <CEGUI/CEGUI.h>
#include <OgreCEGUIRenderer.h>
#include "../GUI/GUIManager.h"
#include "../STDFrame/STDFrame.h"
#include "../World/WorldInfoManager.h"
#include "../STDWeapons/WeaponManager.h"
#include "../STDAgent/STDAgentManager.h"
#include "../STDParticleSystem/STDParticleSystemManager.h"

using namespace Ogre;

class STDEngine : public WindowEventListener
{
public:
	STDEngine();
	virtual ~STDEngine();

public:
	void go();
	void createOrRetrieveColor(const Ogre::String& matName, Real Red, Real Green, Real Blue);
protected:
	void createRoot();
	void defineResources();
	void setupRenderSystem();
	void createRenderWindow();
	void initializeResources();
	void setupScene();
	void createSceneMgr();
	void setupInput();
	void setupGUI();
	void setupFrameListener();
	void startRenderLoop();
	bool render();
	void cleanup();
	

private:
	Root* mRoot;
	RenderWindow* mWindow;
	SceneManager* mSceneMgr;
	RenderSystem* mSystem;
	Camera* mCamera;

public:
	Camera* getCamera() { return mCamera; }
	RenderSystem* getSystem() { return mSystem; }
	RenderWindow* getWindow() { return mWindow; }
	SceneManager* getSceneMgr() { return mSceneMgr; }

protected:
	OIS::Keyboard* mKeyboard;
	OIS::Mouse* mMouse;
	std::vector<OIS::JoyStick*> mJoySticks;
	OIS::InputManager* mInputManager;

protected:
	GUIManager* mGUIManager;
	WeaponManager* mWeaponMgr;
	STDAgentManager* mAgentMgr;
	STDParticleSystemManager* mParticleSystemMgr;

public:
	OIS::Keyboard* getKeyboard() { return mKeyboard; }
	OIS::Mouse* getMouse() { return mMouse; }
	std::vector<OIS::JoyStick*> getJoySticks() { return mJoySticks; }

protected:
	static STDEngine* mSingleton;
public:
	static STDEngine* getSingletonPtr() { return mSingleton; }
	static STDEngine& getSingleton() { return *mSingleton; }

public:
	STDFrame* mCurrentFrame;
	WorldInfoManager* mWorld;

public:
	void logMessage(const std::string& message);

public:
	virtual void windowMoved(RenderWindow* rw);
	virtual void windowResized(RenderWindow* rw);
	virtual bool windowClosing(RenderWindow* rw);
	virtual void windowClosed(RenderWindow* rw) { }
	virtual void windowFocusChange(RenderWindow* rw);
};
#endif
