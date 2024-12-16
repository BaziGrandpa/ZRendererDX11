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
	float triangleRotation = 0.0f;

private:
    // Prevent direct instantiation or copying
    GlobalSettings() = default;
    GlobalSettings(const GlobalSettings&) = delete;
    GlobalSettings& operator=(const GlobalSettings&) = delete;
};


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
	ImGui::SliderFloat("Triangle Position", &settings.triangleRotation, -3.0f, 3.0f);
	ImGui::Text("Slider Value: %.3f", settings.triangleRotation);

    // End the ImGui window
    ImGui::End();
}


#endif
