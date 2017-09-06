#ifndef _H_ML_LAB_CI_WIDGET_H
#define _H_ML_LAB_CI_WIDGET_H

#include <string>
#include <CEGUI/CEGUI.h>

class CIWidget
{
public:
	CIWidget(CIWidget* ParentWidget, CEGUI::Window* parentWindow);
	virtual ~CIWidget();

protected:
	virtual void initializeComponents(const std::string& rootId);
	virtual void create()=0;
	virtual void subscribeEvents() { }
	virtual void unsubscribeEvents() { }

protected:
	std::string getUIId(const std::string& localUIId) const;

public:
	const std::string& getRootId() const { return mRootId; }	
	CEGUI::Window* getRootWindow() { return mRootWindow; }

public:
	void showWindow();
	void hideWindow();

public:
	void setPosition(float x, float y);

protected:
	CEGUI::Window* mRootWindow;
	CEGUI::Window* mParentWindow;
	CIWidget* mParentWidget; 

private:
	std::string mRootId;
};
#endif