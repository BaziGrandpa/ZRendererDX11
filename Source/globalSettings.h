#pragma once
#ifndef _GLOBALSETTING_H_
#define _GLOBALSETTING_H_

#include "imgui/imgui.h"

class GlobalSettings {
public:
	// Access the singleton instance
	static GlobalSettings& GetInstance() {
		static GlobalSettings instance; // Static instance
		return instance;
	}

	// Variables accessible through the instance
	float sliderValue = 0.0f;
	int toggleFlag = 0;
	float cameraZPositin = -5.0f;
	static float s_objectRotation;
	static float s_objectPositionY;
	static float s_global_var;

private:
	// Prevent direct instantiation or copying
	GlobalSettings() = default;
	GlobalSettings(const GlobalSettings&) = delete;
	GlobalSettings& operator=(const GlobalSettings&) = delete;
};


// static member initialization
float GlobalSettings::s_global_var = 0.0f;	
float GlobalSettings::s_objectRotation = 0.0f;
float GlobalSettings::s_objectPositionY = 0.0f;


void RenderDebugUI() {
	auto& settings = GlobalSettings::GetInstance();
	// Render my UI
	// Create a new ImGui window
	float sliderValue = 0.0f;
	ImGui::Begin("Debug value");

	// Camera Z Position
	ImGui::SliderFloat("Camera Z Position", &settings.cameraZPositin, -10.0f, 10.0f);
	ImGui::Text("Slider Value: %.3f", settings.cameraZPositin);

	// Triangle Position
	ImGui::SliderFloat(" Rotation", &GlobalSettings::s_objectRotation, -3.0f, 3.0f);
	ImGui::Text("Slider Value: %.3f", GlobalSettings::s_objectRotation);

	// Test static
	ImGui::SliderFloat("object PositionY", &GlobalSettings::s_objectPositionY, -3.0f, 3.0f);
	ImGui::Text("Slider Value: %.3f", GlobalSettings::s_objectPositionY);

	// End the ImGui window
	ImGui::End();
}


#endif
