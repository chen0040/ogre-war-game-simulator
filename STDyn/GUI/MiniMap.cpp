#include "MiniMap.h"

MiniMap::MiniMap(const std::string& id, RenderWindow* rw, SceneManager* sm, CEGUI::OgreCEGUIRenderer* renderer, uint resolution)
: mSceneMgr(sm)
, mWindow(rw)
, mGUIRenderer(renderer)
{
	mId=id;
	mRootId=std::string("Minimap/")+mId;
	mParentWindow=CEGUI::System::getSingletonPtr()->getGUISheet();

	CEGUI::WindowManager* wm=CEGUI::WindowManager::getSingletonPtr();
	if(wm->isWindowPresent(mRootId))
	{
		throw Exception(42, "Attempt to duplicate CEGUI window with same id", "MiniMap::MiniMap(const std::string& id, SceneManager* sm)");
	}

	TexturePtr pText=TextureManager::getSingletonPtr()->createManual(getUIId("R2Tex"), ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, resolution, resolution, 0, PF_R8G8B8, TU_RENDERTARGET);
	RenderTarget* rt=pText->getBuffer()->getRenderTarget();

	mCam=mSceneMgr->createCamera(getUIId("Camera"));
	
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(750, 0, 750))->attachObject(mCam);
	mCam->setNearClipDistance(5.0f);

	Viewport* vp=rt->addViewport(mCam);
	vp->setBackgroundColour(ColourValue::Black);
	vp->setOverlaysEnabled(false);
	vp->setClearEveryFrame(true);

	CEGUI::Texture* tex=mGUIRenderer->createTexture(pText);
	CEGUI::Imageset* is=CEGUI::ImagesetManager::getSingletonPtr()->createImageset(getUIId("R2tImageSet"), tex);
	is->defineImage(getUIId("R2tImage"), CEGUI::Point(0, 0), CEGUI::Size(pText->getWidth(), pText->getHeight()), CEGUI::Point(0, 0));

	mRootWindow=static_cast<CEGUI::FrameWindow*>(wm->createWindow((CEGUI::utf8*)"TaharezLook/FrameWindow", mRootId));
	mRootWindow->setSize((CEGUI::UVector2(cegui_reldim(0.25f), cegui_reldim(mWindow->getWidth() * 0.25f / mWindow->getHeight()))));
	mRootWindow->setPosition(CEGUI::UVector2(cegui_reldim(0.75f), cegui_reldim(0.25f)));
	mRootWindow->setText((CEGUI::utf8*)"MiniMap");
	mRootWindow->setCloseButtonEnabled(false);

	mMap=static_cast<CEGUI::DefaultWindow*>(wm->createWindow((CEGUI::utf8*)"TaharezLook/StaticImage", getUIId("StaticIMage")));
	mMap->setSize(CEGUI::UVector2(cegui_reldim(0.9f), cegui_reldim(0.8f)));
	mMap->setPosition(CEGUI::UVector2(cegui_reldim(0.05f), cegui_reldim(0.1f)));
	mMap->setProperty((CEGUI::utf8*)"Image", CEGUI::PropertyHelper::imageToString(&(is->getImage(getUIId("R2tImage")))));
	mRootWindow->addChildWindow(mMap);

	mAlphaScroller=static_cast<CEGUI::Scrollbar*>(wm->createWindow((CEGUI::utf8*)"TaharezLook/HorizontalScrollbar", getUIId("AlphaScroller")));
	mAlphaScroller->setSize(CEGUI::UVector2(cegui_reldim(0.9f), cegui_reldim(0.07f)));
	mAlphaScroller->setPosition(CEGUI::UVector2(cegui_reldim(0.05f), cegui_reldim(0.90f)));
	mAlphaScroller->setMaxSize(CEGUI::UVector2(cegui_reldim(0.9f), cegui_absdim(20)));
	mAlphaScroller->setAlpha(0.5f);
	mRootWindow->addChildWindow(mAlphaScroller);

	mEventAlphaScroll=mAlphaScroller->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiniMap::onScroll_Alpha, this));

	mParentWindow->addChildWindow(mRootWindow);
}

MiniMap::~MiniMap()
{
	mEventAlphaScroll->disconnect();

	CEGUI::WindowManager* wm=CEGUI::WindowManager::getSingletonPtr();

	mRootWindow->removeChildWindow(mAlphaScroller);
	mRootWindow->removeChildWindow(mMap);
	mParentWindow->removeChildWindow(mRootWindow);

	wm->destroyWindow(mAlphaScroller);
	wm->destroyWindow(mMap);
	wm->destroyWindow(mRootWindow);
}

void MiniMap::setAlpha(float alpha)
{
	if(alpha > 0.1f)
	{
		mRootWindow->setAlpha(alpha);
		mAlphaScroller->setScrollPosition(alpha);
	}
}

void MiniMap::setPosition(float x, float y)
{
	mRootWindow->setPosition(CEGUI::UVector2(cegui_reldim(x), cegui_reldim(y)));
}
	
void MiniMap::setSize(float w, float h)
{
	mRootWindow->setSize(CEGUI::UVector2(cegui_reldim(w), cegui_reldim(h)));
}

void MiniMap::hide()
{
	enable(false);
}

bool MiniMap::onScroll_Alpha(const CEGUI::EventArgs& evt)
{
	float alpha=mAlphaScroller->getScrollPosition();
	float ms=0.3f;
	if(alpha > ms)
	{
		mRootWindow->setAlpha(alpha);
	}
	else
	{
		mAlphaScroller->setScrollPosition(ms);
	}

	return true;
}

void MiniMap::show()
{
	enable(true);
}

void MiniMap::enable(bool enabled)
{
	mEnabled=enabled;
	if(mEnabled)
	{
		mRootWindow->show();
	}
	else
	{
		mRootWindow->hide();
	}
}

std::string MiniMap::getUIId(const std::string localId)
{
	return mRootId+std::string("/")+localId;
}