////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"

//#if _DEBUG
//#pragma comment (lib,"E:\\Work\\AutoDesk\\FBX\\SDK\\2020.3.7\\lib\\x64\\debug\\libfbxsdk-md.lib")
//#pragma comment (lib,"E:\\Work\\AutoDesk\\FBX\\SDK\\2020.3.7\\lib\\x64\\debug\\libxml2-md.lib")
//#pragma comment (lib,"E:\\Work\\AutoDesk\\FBX\\SDK\\2020.3.7\\lib\\x64\\debug\\zlib-md.lib")
//#else
//#endif


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;


	// Create the system object.
	System = new SystemClass;

	// Initialize and run the system object.
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// Shutdown and release the system object.
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}

