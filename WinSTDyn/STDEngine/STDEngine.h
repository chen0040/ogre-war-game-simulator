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

class STDEngine //: public WindowEventListener
{
public:
	enum SetupStatus
	{
		ACS_SETUP_NULL,
		ACS_SET_PATH,
		ACS_CREATE_ROOT,
		ACS_DEFINE_RESOURCES,
		ACS_SETUP_RENDER_SYSTEM,
		ACS_CREATE_RENDER_WINDOW,
		ACS_INITIALIZE_RESOURCES,
		ACS_SETUP_SCENEMGR,
		ACS_SETUP_SCENE,
		ACS_SETUP_INPUT,
		ACS_SETUP_GUI,
		ACS_SETUP_FRAME_LISTENER,
		ACS_SETUP_COMPLETED
	};

public:
	STDEngine();
	virtual ~STDEngine();

public:
	bool setup(unsigned long hwnd, unsigned long hwndInput, int width, int height, bool incremental=false, std::string& action=std::string("setup"));
	bool incrementalSetup(unsigned long hwnd, unsigned long hwndInput, int width, int heigt, float& percentage, std::string& action);

public:
	void createOrRetrieveColor(const Ogre::String& matName, Real Red, Real Green, Real Blue);

protected:
	void createRoot();
	void defineResources();
	void setupRenderSystem();
	void createRenderWindow(unsigned long hwnd, int width, int height);
	void initializeResources();
	void setupScene();
	void createSceneMgr();
	void setupInput(unsigned long hwnd);
	void setupGUI();
	void setupFrameListener();
	void startRenderLoop();
	void cleanup();

public:
	bool render();

public:
	void save();
	void open();
	void createNew();

private:
	SetupStatus mStage;
	int mFPS;

public:
	bool setupCompleted() const { return mStage==ACS_SETUP_COMPLETED; }
	int getFPS() const { return mFPS; }
	

private:
	Root* mRoot;
	RenderWindow* mWindow;
	SceneManager* mSceneMgr;
	RenderSystem* mSystem;
	Camera* mCamera;
	RaySceneQuery* mRaySceneQuery;

public:
	Camera* getCamera() { return mCamera; }
	RenderSystem* getSystem() { return mSystem; }
	RenderWindow* getWindow() { return mWindow; }
	SceneManager* getSceneMgr() { return mSceneMgr; }
	RaySceneQuery* getRaySceneQuery() { return mRaySceneQuery; }

protected:
	OIS::Keyboard* mKeyboard;
	OIS::Mouse* mMouse;
	std::vector<OIS::JoyStick*> mJoysticks;
	OIS::InputManager* mInputManager;

protected:
	GUIManager* mGUIManager;
	WeaponManager* mWeaponMgr;
	STDAgentManager* mAgentMgr;
	STDParticleSystemManager* mParticleSystemMgr;

public:
	OIS::Keyboard* getKeyboard() { return mKeyboard; }
	OIS::Mouse* getMouse() { return mMouse; }
	std::vector<OIS::JoyStick*>& getJoysticks() { return mJoysticks; }

protected:
	static STDEngine* mSingleton;
public:
	static STDEngine* getSingletonPtr() { return mSingleton; }
	static STDEngine& getSingleton() { return *mSingleton; }

private:
	Real mJoystickAxisSensitivity;
public:
	Real getJoystickAxisSensitivity() const { return mJoystickAxisSensitivity; }
	void setJoystickAxisSensitivity(const Real& val) { mJoystickAxisSensitivity=val; }

private:
	bool mJoystickEnabled;
public:
	bool isJoystickEnabled() const { return mJoystickEnabled; }
	void enableJoystick(bool enabled) { mJoystickEnabled=enabled; }

private:
	bool mCaelumSystemEnabled;
	Real mCaelumSystemTimeScale;
	Vector2 mCaelumSystemCloudSpeed;
	std::vector<int> mCaelumSystemDateTime;
public:
	void enableCaelumSystem(bool enabled) { mCaelumSystemEnabled=enabled; }
	void setCaelumSystemCloudSpeed(const Vector2& speed) { mCaelumSystemCloudSpeed=speed; }
	void setCaelumSystemTimeScale(Real timeScale) { mCaelumSystemTimeScale=timeScale; }
	void setCaelumSystemDateTime(int year, int month, int day, int hour, int minute, int second);

public:
	STDFrame* mCurrentFrame;
	WorldInfoManager* mWorld;

public:
	void logMessage(const std::string& message);

	/*
public:
	virtual void windowMoved(RenderWindow* rw);
	virtual void windowResized(RenderWindow* rw);
	virtual bool windowClosing(RenderWindow* rw);
	virtual void windowClosed(RenderWindow* rw) { }
	virtual void windowFocusChange(RenderWindow* rw);
	*/
public:
	void injectWindowResized();
};
#endif
