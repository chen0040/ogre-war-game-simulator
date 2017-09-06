#ifndef _H_STD_NAVI_FRAME_H
#define _H_STD_NAVI_FRAME_H

#include <set>
#include <map>
#include "STDFrame.h"
#include "../Camera/SphericalCamera.h"
#include "../STDAgent/STDAgent.h"
#include "../GUI/MiniMap.h"
#include "../InputSystem/MousePicker.h"
#include "../STDAgent/TankConfig.h"
#include "../STDAgent/URAVConfig.h"
#include "../STDAgent/TerroristConfig.h"

class STDNaviFrame : public STDFrame
{

public:
	STDNaviFrame(std::string frameName);
	virtual ~STDNaviFrame();

public:
	virtual bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
	virtual bool mouseMoved(const OIS::MouseEvent& evt);
	virtual bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
	virtual bool keyPressed(const OIS::KeyEvent& evt);
	virtual bool keyReleased(const OIS::KeyEvent& evt);

protected:
	virtual bool keyPressed3DWorld(const OIS::KeyEvent& evt);
	virtual bool keyReleased3DWorld(const OIS::KeyEvent& evt);
	virtual bool mousePressed3DWorld(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
	virtual bool mouseMoved3DWorld(const OIS::MouseEvent& evt);
	virtual bool mouseReleased3DWorld(const OIS::MouseEvent& evt, OIS::MouseButtonID id);

public:
	virtual bool frameStarted(const FrameEvent& evt);
	virtual bool frameEntered();
	virtual bool frameExited();

protected:
	virtual void subscribeEvents();
	virtual void unsubscribeEvents();

public:
	virtual void open();
	virtual void save();

private:
	void createWorld();
	void createTank();
	void createURAV();
	void createSatelite();
	void createLasers();
	void createTerrorist();

private:
	STDAgent* createBuilding_Type1(const std::string& building_name, const Real& x, const Real& z, Real& building_width);
	STDAgent* createBuilding_Type2(const std::string& building_name, const Real& x, const Real& z, Real& building_width);

private:
	void createBuildings_Random();
	void createBuildings_Conformance(int type=1);

private:
	void updateLasers(const Real& dt);
	void updateSateliteView(const Real& dt);
	void updateOverlay(const Real& dt);
	void updateBackCameraHolder(const Real& dt);
	void updateCommands(const Real& dt);

protected:
	//SphericalCamera mCamera;
	STDAgent* mURAV;
	STDAgent* mTank;
	std::map<std::string, STDAgent*> mBuildings;
	std::map<std::string, STDAgent*> mTerrorists;
	MiniMap* mURAVMap;
	MiniMap* mTankMap;
	MiniMap* mSateliteMap;
	MousePicker mMousePicker;
	ManualObject* mRedLaser;
	ManualObject* mBlueLaser;
	ManualObject* mGreenLaser;
	RaySceneQuery* mRaySceneQuery;
	Overlay* mSTDOverlay;
	Vector3 mTankLaserTarget;
	Vector3 mURAVLaserTarget;
	
private:
	size_t mTerroristCreationCounter;

private:
	TankConfig mTankConfig;
	URAVConfig mURAVConfig;
	TerroristConfig mTerroristConfig;
};
#endif