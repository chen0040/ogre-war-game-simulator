#include "CIWidget.h"
#include "GUIManager.h"
#include <Ogre.h>
#include <sstream>

CIWidget::CIWidget(CIWidget* parentWidget, CEGUI::Window* parentWindow)
{
	mParentWidget=parentWidget;
	mParentWindow=parentWindow;

	if(mParentWindow == NULL)
	{
		mParentWindow=CEGUI::System::getSingletonPtr()->getGUISheet();		
	}
}

void CIWidget::initializeComponents(const std::string& rootId)
{
	mRootId=rootId;
	CEGUI::WindowManager* wm=CEGUI::WindowManager::getSingletonPtr();
	if(wm->isWindowPresent(mRootId))
	{
		std::ostringstream oss;
		oss << "wm->isWindowPresent(" << mRootId << ")==true";
		throw Ogre::Exception(42, oss.str().c_str(), "CIWidget::initializeComponents(const std::string& rootId)");
	}
	else
	{
		this->create();
	}
	this->subscribeEvents();
}

CIWidget::~CIWidget()
{
	this->unsubscribeEvents();
}

void CIWidget::showWindow()
{
	mRootWindow->show();
}

void CIWidget::hideWindow()
{
	mRootWindow->hide();
}

std::string CIWidget::getUIId(const std::string& localUIId) const
{
	return mRootId+std::string("/")+localUIId;
}

void CIWidget::setPosition(float x, float y)
{
	mRootWindow->setPosition(CEGUI::UVector2(cegui_reldim(x), cegui_reldim(y)));
}