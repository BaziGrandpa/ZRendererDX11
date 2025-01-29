#include "MyDebugger.h"

int MyDebugger::MAX_DEBUG_LINES = 50;
// Define and initialize the static queue
std::vector<MyDebugger::DebugOutput> MyDebugger::m_allOutput;

void MyDebugger::ShowDebugUIPanel() {
	//auto& settings = GlobalSettings::GetInstance();
	// Render my UI
	// Create a new ImGui window
	ImGui::Begin("Debug value");

	// Camera Z Position
	ImGui::SliderFloat("Camera Z Position", &GlobalSettings::s_cameraPositionZ, -100.0f, 10.0f);
	ImGui::Text("Slider Value: %.3f", GlobalSettings::s_cameraPositionZ);

	ImGui::SliderFloat("Camera Y Position", &GlobalSettings::s_cameraPositionY, -10.0f, 10.0f);
	ImGui::Text("Slider Value: %.3f", GlobalSettings::s_cameraPositionY);
	

	// Triangle Position
	ImGui::SliderFloat(" RotationX", &GlobalSettings::s_objectRotationX, -10.0f, 10.0f);
	ImGui::Text("Slider Value: %.3f", GlobalSettings::s_objectRotationX);

	ImGui::SliderFloat(" RotationY", &GlobalSettings::s_objectRotationY, -10.0f, 10.0f);
	ImGui::Text("Slider Value: %.3f", GlobalSettings::s_objectRotationY);

	// Triangle Position
	ImGui::SliderFloat(" RotationZ", &GlobalSettings::s_objectRotationZ, -10.0f, 10.0f);
	ImGui::Text("Slider Value: %.3f", GlobalSettings::s_objectRotationZ);

	// Test static
	ImGui::SliderFloat("object PositionY", &GlobalSettings::s_objectPositionY, -3.0f, 3.0f);
	ImGui::Text("Slider Value: %.3f", GlobalSettings::s_objectPositionY);

	// End the ImGui window
	ImGui::End();
}

void MyDebugger::ShowDebugOutputPanel() {

	if (m_allOutput.empty())
		return;
	ImGui::Begin("Output");

	for (const auto& message : m_allOutput) {

		ImVec4 textColor;
		switch (message.type) {
		case MyDebugger::DebugType::Log:
			textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White
			break;
		case MyDebugger::DebugType::Warning:
			textColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
			break;
		case MyDebugger::DebugType::Error:
			textColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
			break;
		}

		// Display the message with the specified color
		ImGui::TextColored(textColor, "%s", message.content.c_str());
	}

	ImGui::End();

}

void MyDebugger::LogDebug(string content) {

	MyDebugger::DebugOutput output{ MyDebugger::DebugType::Log,content };
	Output(output);
}

void MyDebugger::LogDebug(const DirectX::XMFLOAT4& vec) {
	string output = "[DEBUG]: XMFLOAT4(" + to_string(vec.x) + " " + to_string(vec.y) + " " + to_string(vec.z) + " " + to_string(vec.w);
	LogDebug(output);

}

void MyDebugger::LogWarning(string content) {

	MyDebugger::DebugOutput output{ MyDebugger::DebugType::Warning,content };
	Output(output);
}

void MyDebugger::LogError(string content) {

	MyDebugger::DebugOutput output{ MyDebugger::DebugType::Error,content };
	Output(output);
}

void MyDebugger::Output(const DebugOutput& output) {
	auto length = m_allOutput.size();

	if (length > MAX_DEBUG_LINES) {
		m_allOutput.erase(m_allOutput.begin());
	}

	m_allOutput.push_back(output);
}
