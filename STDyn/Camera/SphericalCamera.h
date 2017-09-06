#ifndef _H_ML_LAB_SPHERICAL_CAMERA_H
#define _H_ML_LAB_SPHERICAL_CAMERA_H

#include <Ogre.h>

using namespace Ogre;

class SphericalCamera
{
public:
	SphericalCamera();
	virtual ~SphericalCamera();

public:
	void setActualCamera(Camera* cam);
	void setRotateSpeed(Real speed) { mCameraRotateSpeed=speed; }
	void setZoomSpeed(Real speed) { mCameraZoomSpeed=speed; }
	virtual bool frameStarted(bool rotateLeft, bool rotateRight, bool rotateUp, bool rotateDown, bool zoomIn, bool zoomOut, Real dt);
	virtual std::string toString() const;
	virtual void reset();
	virtual void setFocus(const Vector3& pos);
	
protected:
	Camera* mCamera;

protected:
	Real mCameraRotateSpeed;
	Real mCameraZoomSpeed;
	Real mCamPitchValue;
	Real mCamYawValue;
	Real mCamRadius;
	Real mInitCamPitchValue;
	Real mInitCamYawValue;
	Real mInitCamRadius;
	Real mMinCamRadius;
	Real mMaxCamRadius;
	Real mMinCamPitchValue;
	Real mMaxCamPitchValue;
};
#endif