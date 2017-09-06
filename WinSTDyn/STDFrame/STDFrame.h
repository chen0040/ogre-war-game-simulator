#ifndef _H_STD_FRAME_H
#define _H_STD_FRAME_H

#include <string>
#include <Ogre.h>
#include <OIS/OIS.h>
#include <CEGUI/CEGUI.h>

using namespace Ogre;

class STDFrame : public FrameListener, public OIS::MouseListener, public OIS::KeyListener, public OIS::JoyStickListener
{
public:
	STDFrame(const std::string& frameName);
	virtual ~STDFrame();

protected:
	bool mLeftButtonDown;
	bool mRightButtonDown;
	bool mContinue;

protected:
	bool isKeyDown(OIS::KeyCode keyCode) const;

protected:
	OIS::Mouse* mMouse;
	OIS::Keyboard* mKeyboard;
	std::vector<OIS::JoyStick*> mJoysticks;
private:
	bool mWindowsMouse;

protected:
	std::string mFrameName;
public:
	const std::string& getName() const { return mFrameName; }

protected:
	std::string getUIId(const std::string& localId) const;
private:
	void resetMouse();

public:
	virtual void quit();
	virtual void open();
	virtual void save();
	virtual void createNew();

protected:
	void createInput();
	CEGUI::MouseButton convertButton(OIS::MouseButtonID id)
	{
		switch(id)
		{
		case OIS::MB_Left:
			return CEGUI::LeftButton;
		case OIS::MB_Right:
			return CEGUI::RightButton;
		case OIS::MB_Middle:
			return CEGUI::MiddleButton;
		default:
			return CEGUI::LeftButton;
		}
	}

protected:
	virtual void subscribeEvents() { }
	virtual void unsubscribeEvents() { }
	
protected:
	bool isMouseWithinWindow() const;

public:
	virtual bool frameStarted(const FrameEvent& evt);
	virtual bool frameEntered();
	virtual bool frameExited();

public:
	virtual void windowResized(RenderWindow* window);

private:
	void switchWindowMouse();

public:
	virtual bool keyPressed(const OIS::KeyEvent& evt);
	virtual bool keyReleased(const OIS::KeyEvent& evt);
	virtual bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
	virtual bool mouseMoved(const OIS::MouseEvent& evt);

public:
	virtual bool buttonPressed( const OIS::JoyStickEvent &arg, int button ) { return true; }
	virtual bool buttonReleased( const OIS::JoyStickEvent &arg, int button ) { return true; }

	virtual bool axisMoved( const OIS::JoyStickEvent &arg, int axis ) { return true; }

	//Joystick Event, amd sliderID
	virtual bool sliderMoved( const OIS::JoyStickEvent & arg, int sliderID) {return true;}
	//Joystick Event, amd povID
	virtual bool povMoved( const OIS::JoyStickEvent & arg, int pov) {return true;}
};
#endif