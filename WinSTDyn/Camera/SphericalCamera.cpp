#include "stdafx.h"
#include "SphericalCamera.h"
#include <sstream>

SphericalCamera::SphericalCamera()
: mCameraRotateSpeed(15.0f)
, mInitCamPitchValue(-14.83f)
, mInitCamYawValue(-57.0698f)
, mMinCamPitchValue(-90.0f)
, mMaxCamPitchValue(-10.0f)
, mMinCamRadius(100)
, mMaxCamRadius(1200)
, mInitCamRadius(400)
, mCameraZoomSpeed(30)
{
	mCamPitchValue=mInitCamPitchValue;
	mCamYawValue=mInitCamYawValue;
	mCamRadius=mInitCamRadius;
}

SphericalCamera::~SphericalCamera()
{

}

bool SphericalCamera::frameStarted(bool rotateLeft, bool rotateRight, bool rotateUp, bool rotateDown, bool zoomIn, bool zoomOut, Real dt)
{
	if(rotateLeft)
	{
		mCamYawValue-=dt*mCameraRotateSpeed;
	}
	else if(rotateRight)
	{
		mCamYawValue+=dt*mCameraRotateSpeed;
	}

	if(rotateUp)
	{
		if(mCamPitchValue > mMinCamPitchValue)
		{
			mCamPitchValue-=dt*mCameraRotateSpeed;
		}
		else
		{
			mCamPitchValue=mMinCamPitchValue;
		}
	}
	else if(rotateDown)
	{
		if(mCamPitchValue < mMaxCamPitchValue)
		{
			mCamPitchValue+=dt*mCameraRotateSpeed;
		}
		else
		{
			mCamPitchValue=mMaxCamPitchValue;
		}
	}

	if(zoomIn)
	{
		if(mCamRadius > mMinCamRadius)
		{
			mCamRadius-=dt*mCameraZoomSpeed;
		}
		else
		{
			mCamRadius=mMinCamRadius;
		}
	}
	else if(zoomOut)
	{
		if(mCamRadius < mMaxCamRadius)
		{
			mCamRadius+=dt*mCameraZoomSpeed;
		}
		else
		{
			mCamRadius=mMaxCamRadius;
		}
	}

	mCamera->setPosition(0.0f,0.0f,0.0f);
    mCamera->setOrientation(Ogre::Quaternion::IDENTITY);
    mCamera->yaw(Ogre::Degree(mCamYawValue));
    mCamera->pitch(Ogre::Degree(mCamPitchValue));
    mCamera->moveRelative(Ogre::Vector3(0.0f,0.0f,mCamRadius));

	return true;
}

void SphericalCamera::setActualCamera(Camera* cam)
{
	mCamera=cam;
}

void SphericalCamera::reset()
{
	mCamera->getParentSceneNode()->setPosition(0, 0, 0);

	mCamPitchValue=mInitCamPitchValue;
	mCamYawValue=mInitCamYawValue;
	mCamRadius=mInitCamRadius;

	mCamera->setPosition(0.0,0.0,0.0);
    mCamera->setOrientation(Ogre::Quaternion::IDENTITY);
    mCamera->yaw(Ogre::Degree(mCamYawValue));
    mCamera->pitch(Ogre::Degree(mCamPitchValue));
    mCamera->moveRelative(Ogre::Vector3(0.0,0.0,mCamRadius));
}

void SphericalCamera::setFocus(const Vector3& pos)
{
	mCamera->getParentSceneNode()->setPosition(pos);
}

std::string SphericalCamera::toString() const
{
	std::ostringstream oss;
	oss << "Yaw: " << mCamYawValue << "\nPitch: " << mCamPitchValue << "\nRadius: " << mCamRadius;
	return oss.str();
}