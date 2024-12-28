#include "MyDebugger.h"

int MyDebugger::MAX_DEBUG_LINES = 50;
// Define and initialize the static queue
std::vector<std::string> MyDebugger::m_allOutput =
{
	"Initial debug message 1", "Initial debug message 2", "Initial debug message 3"
}
;

void MyDebugger::ShowDebugUIPanel() {
	auto& settings = GlobalSettings::GetInstance();
	// Render my UI
	// Create a new ImGui window
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

void MyDebugger::ShowDebugOutputPanel() {


	ImGui::Begin("Output");
	// End the ImGui window
	for (const auto& message : m_allOutput) {
		ImGui::Text("%s", message.c_str());
	}
	ImGui::End();

}
