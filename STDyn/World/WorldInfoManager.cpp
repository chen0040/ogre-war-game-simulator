#include "WorldInfoManager.h"

WorldInfoManager* WorldInfoManager::mSingleton=NULL;

Real getWorldHeight(Real x, Real z, void* data)
{
	return WorldInfoManager::getSingletonPtr()->getHeight(x, z);
}