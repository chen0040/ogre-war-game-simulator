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
	virtual bool keyPressed(const OIS::KeyEvent& evt);
	virtual bool keyReleased(const OIS::KeyEvent& evt);

public:
	virtual bool frameStarted(const FrameEvent& evt);
	virtual bool frameEnded(const FrameEvent& evt);
	virtual bool frameEntered();

private:
	void createWorld();
	void createSatelite();

private:
	void handleTankByJoystickAxe(size_t joystickId, const Real& sensitivity);
	void handleCannonByJoystickPOVHat(size_t joystickId);
	void handleCannonByJoystickAxe(size_t joystickId, const Real& sensitivity);
	void handleURAVByJoystickAxe(size_t joystickId, const Real& sensitivity);

private:
	STDAgent* createBuilding_Type1(const std::string& building_name, const Real& x, const Real& z, Real& building_width);
	STDAgent* createBuilding_Type2(const std::string& building_name, const Real& x, const Real& z, Real& building_width);

private:
	void createBuildings_Random();
	void createBuildings_Conformance(int type=1);

private:
	void updateSateliteView();
	void updateOverlay();

public:
	virtual void save();
	virtual void createNew();
	virtual void open();

protected:
	std::map<std::string, STDAgent*> mBuildings;
	MiniMap* mSateliteMap;
	MousePicker mMousePicker;
	Overlay* mSTDOverlay;

private:
	TankConfig mTankConfig;
	URAVConfig mURAVConfig;
	TerroristConfig mTerroristConfig;
private:
	Tank* mTank;
	URAV* mURAV;

public:
	virtual bool buttonPressed( const OIS::JoyStickEvent &arg, int button );
	virtual bool buttonReleased( const OIS::JoyStickEvent &arg, int button );

	virtual bool axisMoved( const OIS::JoyStickEvent &arg, int axis );

	//Joystick Event, amd sliderID
	virtual bool sliderMoved( const OIS::JoyStickEvent & arg, int sliderID);
	//Joystick Event, amd povID
	virtual bool povMoved( const OIS::JoyStickEvent & arg, int pov);
};
#endif