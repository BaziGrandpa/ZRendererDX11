#pragma once
// Filename: mydebugger.h
#ifndef _MYDEBUGGER_H_
#define _MYDEBUGGER_H_

#include <vector>
#include <string>

#include "globalSettings.h"
#include "imgui/imgui.h"

using namespace std;

class MyDebugger
{

	// Dont need to write out constructor, destructor, copy constructor, or assignment operator
public:
	static void ShowDebugUIPanel();
	static void ShowDebugOutputPanel();


private:
	static int MAX_DEBUG_LINES;
	static vector<string> m_allOutput;
	

};




#endif
