#include "STDEngine/STDEngine.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
#include <iostream>
int main(int argc, char** argc)
#endif
{
	try{
		STDEngine eng;
		eng.go();
	}catch(Ogre::Exception& e)
	{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == PLATFORM_WIN32
		MessageBoxA(NULL, e.getFullDescription().c_str(), "An Ogre exception has occurred", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		std::cout << e.getFullDescription().c_str() << std::endl;
#endif
	}
}