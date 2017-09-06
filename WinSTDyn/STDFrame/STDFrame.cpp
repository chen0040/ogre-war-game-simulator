#include "stdafx.h"
#include "STDFrame.h"
#include "../OSEnvironment/OSEnvironment.h"
#include "../STDEngine/STDEngine.h"

STDFrame::STDFrame(const std::string& frameName)
: mFrameName(frameName)
, mLeftButtonDown(false)
, mRightButtonDown(false)
, mContinue(true)
, mWindowsMouse(true)
{
	STDEngine* app=STDEngine::getSingletonPtr();
	mKeyboard=app->getKeyboard();
	mMouse=app->getMouse();
	std::vector<OIS::JoyStick*>& joysticks=app->getJoysticks();
	for(std::vector<OIS::JoyStick*>::iterator iter=joysticks.begin(); iter != joysticks.end(); ++iter)
	{
		mJoysticks.push_back(*iter);
	}
}

std::string STDFrame::getUIId(const std::string& localId) const
{
	return mFrameName+std::string("/")+localId;
}

bool STDFrame::frameEntered()
{
	mLeftButtonDown=false;
	mRightButtonDown=false;
	//clearKeyBuffer();

	mKeyboard->setEventCallback(this);
	mMouse->setEventCallback(this);

	std::vector<OIS::JoyStick*>::iterator iJoystick=mJoysticks.begin();
	std::vector<OIS::JoyStick*>::iterator iJoystickEnd=mJoysticks.end();
	for(; iJoystick != iJoystickEnd; ++iJoystick)
	{
		(*iJoystick)->setEventCallback(this);
	}

	//initialize CEGUI::MouseCursor to align with OIS::Mouse
	resetMouse();
	switchWindowMouse();

	subscribeEvents();

	return true;
}

void STDFrame::switchWindowMouse()
{
	bool inWindow=isMouseWithinWindow();
	if (inWindow && mWindowsMouse)
	{
	  mWindowsMouse = false;
	  // Show your custom cursor here
	  ::ShowCursor(FALSE);
	}
	else if (!inWindow && !mWindowsMouse)
	{
	  mWindowsMouse = true;
	  // Hide your custom cursor here
	  ::ShowCursor(TRUE);
	}
}
	
bool STDFrame::frameExited()
{
	mLeftButtonDown=false;
	mRightButtonDown=false;
	//clearKeyBuffer();

	unsubscribeEvents();

	return true;
}

void STDFrame::windowResized(RenderWindow* window)
{
	//CEGUI::System::getSingletonPtr()->injectMouseMove(nm->getMouseXPos()-pos.d_x, nm->getMouseXPos()-pos.d_y);

	resetMouse();
}

STDFrame::~STDFrame()
{
	
}

bool STDFrame::isKeyDown(OIS::KeyCode keyCode) const
{
	/*
	std::map<OIS::KeyCode, bool>::const_iterator fnd=mKeyDown.find(keyCode);
	if(fnd==mKeyDown.end())
	{
		return false;
	}
	return fnd->second;
	*/
	return mKeyboard->isKeyDown(keyCode);
}

/*
void STDFrame::clearKeyBuffer()
{
	std::map<OIS::KeyCode, bool>::iterator keyIter;
	for(keyIter=mKeyDown.begin(); keyIter != mKeyDown.end(); ++keyIter)
	{
		keyIter->second=false;
	}
}
*/

/*
void STDFrame::injectKeyDown(OIS::KeyCode keyCode, bool down)
{
	mKeyDown[keyCode]=down;
}
*/

bool STDFrame::frameStarted(const FrameEvent &evt)
{
	mKeyboard->capture();
	mMouse->capture();

	if( mJoysticks.size() > 0 ) 
	{
        std::vector<OIS::JoyStick*>::iterator itJoystick   = mJoysticks.begin();
        std::vector<OIS::JoyStick*>::iterator itJoystickEnd = mJoysticks.end();
        for(; itJoystick != itJoystickEnd; ++itJoystick ) 
		{
            (*itJoystick)->capture();
        }
    }

	return mContinue;
}

bool STDFrame::keyPressed(const OIS::KeyEvent& evt)
{
	switch(evt.key)
	{
	case OIS::KC_ESCAPE:
		mContinue=false;
		break;
	default:
		break;
	}

	//injectKeyDown(evt.key, true);

	CEGUI::System::getSingletonPtr()->injectChar(evt.text);
	return CEGUI::System::getSingletonPtr()->injectKeyDown(evt.key);
}

void STDFrame::resetMouse()
{
	const OIS::MouseState& ms=mMouse->getMouseState();

	CEGUI::Point& pos=CEGUI::MouseCursor::getSingletonPtr()->getPosition();

	float offsetX=ms.X.abs-pos.d_x;
	float offsetY=ms.Y.abs-pos.d_y;
	CEGUI::System::getSingletonPtr()->injectMouseMove(offsetX, offsetY);
}

void STDFrame::quit()
{
	mContinue=false;
}

void STDFrame::open()
{

}

void STDFrame::save()
{

}

void STDFrame::createNew()
{

}

bool STDFrame::keyReleased(const OIS::KeyEvent& evt)
{
	//injectKeyDown(evt.key, false);

	CEGUI::System::getSingletonPtr()->injectChar(evt.text);
	return CEGUI::System::getSingletonPtr()->injectKeyUp(evt.key);
}

bool STDFrame::mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	if(!isMouseWithinWindow())
	{
		return true;
	}
	
	if(CEGUI::System::getSingletonPtr()->injectMouseButtonDown(convertButton(id)))
	{		
		return true;
	}

	if(id==OIS::MB_Left)
	{
		mLeftButtonDown=true;
	}
	else if(id==OIS::MB_Right)
	{
		mRightButtonDown=true;
	}

	return false;
}

bool STDFrame::mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	if(!isMouseWithinWindow())
	{
		return true;
	}

	if(CEGUI::System::getSingletonPtr()->injectMouseButtonUp(convertButton(id)))
	{
		return true;
	}

	if(id==OIS::MB_Left)
	{
		mLeftButtonDown=false;
	}
	else if(id==OIS::MB_Right)
	{
		mRightButtonDown=false;
	}

	return false;
}
	
bool STDFrame::mouseMoved(const OIS::MouseEvent& evt)
{
	int xPos=evt.state.X.abs;
	int yPos=evt.state.Y.abs;

	if(evt.state.Z.rel != 0)
	{
		CEGUI::System::getSingletonPtr()->injectMouseWheelChange(evt.state.Z.rel);
	}

	switchWindowMouse();

	//Xianshun: this is where i finally solve the problem with mouse position for foreground and nonexclusive mouse :D
	return CEGUI::System::getSingletonPtr()->injectMousePosition(xPos, yPos); 

	//Xianshun: the following commented line causes the problem with the mouse position for foreground and nonexclusive mouse 
	//return CEGUI::System::getSingletonPtr()->injectMouseMove(evt.state.X.rel, evt.state.Y.rel); 
}

bool STDFrame::isMouseWithinWindow() const
{
	HWND ogreHwnd = NULL;
	RenderWindow* win=STDEngine::getSingletonPtr()->getWindow();
	win->getCustomAttribute("WINDOW", &ogreHwnd);
	if (ogreHwnd == NULL) return false;

	POINT p;
	::GetCursorPos(&p);
	::ScreenToClient(ogreHwnd,&p);

	RECT r;
	::GetClientRect(ogreHwnd,&r);

	int width = r.right - r.left;
	int height = r.bottom - r.top;

	bool inWindow = (p.x >= 0  && p.x < width && p.y >= 0 && p.y < height);
 
	return inWindow;
}
