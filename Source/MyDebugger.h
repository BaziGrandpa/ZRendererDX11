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
	enum DebugType
	{
		Log,
		Warning,
		Error
	};

	struct DebugOutput
	{
		DebugType type;
		string content;

	};

	// Dont need to write out constructor, destructor, copy constructor, or assignment operator
public:
	static void ShowDebugUIPanel();
	static void ShowDebugOutputPanel();
	static void LogDebug(string content);
	static void LogWarning(string content);
	static void LogError(string content);

private:
	static int MAX_DEBUG_LINES;
	static vector<DebugOutput> m_allOutput;

	static void Output(const DebugOutput& output);
};


// all macro
#define DISPLAY_LOG //If we don't want to conatin any debug code, just comment this line

#ifdef DISPLAY_LOG
#define LOG_DEBUG(msg) MyDebugger::LogDebug(msg)
#else
#define LOG_DEBUG(msg)
#endif 


#endif
