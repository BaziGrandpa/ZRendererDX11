#pragma once
#ifndef _GLOBALSETTING_H_
#define _GLOBALSETTING_H_



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

	// Can directly use static rather then Singleton.
	static float s_objectRotation;
	static float s_objectPositionY;
	static float s_global_var;

private:
	// Prevent direct instantiation or copying
	GlobalSettings() = default;
	GlobalSettings(const GlobalSettings&) = delete;
	GlobalSettings& operator=(const GlobalSettings&) = delete;
};






#endif
