////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"
#include "globalSettings.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#include "MyDebugger.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	//m_ColorShader = 0;
	m_TextureShader = 0;
	m_LightShader = 0;
	m_Light = 0;

	m_RenderTexture = 0;
	m_DisplayPlane = 0;
	m_animationController = 0;
	m_skinnedShader = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFilename[128];
	char textureFilename[128];
	bool result;


	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());

	// Create the camera object.
	m_Camera = new CameraClass;

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// Set the file name of the model.
	strcpy_s(modelFilename, "../ZRendererDX11/Resources/Models/SnowMan.fbx");

	// Set the name of the texture file that we will be loading.
	strcpy_s(textureFilename, "../ZRendererDX11/Resources/stone01.tga");


	//strcpy_s(textureFilename, "../ZRendererDX11/Resources/Dragon_color1.tga");
	// Create and initialize the model object.
	m_Model = new ModelClass;
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}


	// Create and initialize the light shader object.
	m_LightShader = new LightShaderClass;

	// compile vs,ps for light.
	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}
	m_skinnedShader = new SkinnedShaderClass;
	result = m_skinnedShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object SkinnedShaderClass.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the light object.
	m_Light = new LightClass;

	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	// Create and initialize the texture shader object.
	m_TextureShader = new TextureShaderClass;

	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the render to texture object.
	m_RenderTexture = new RenderTextureClass;

	int textureWidth = screenWidth / 5;
	int textureHeight = screenHeight / 5;
	result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), textureWidth, textureHeight, SCREEN_DEPTH, SCREEN_NEAR, 1);
	if (!result)
	{
		return false;
	}

	// Create and initialize the display plane object.
	m_DisplayPlane = new DisplayPlaneClass;

	result = m_DisplayPlane->Initialize(m_Direct3D->GetDevice(), 1.0f, 1.0f);
	if (!result)
	{
		return false;
	}


	// Initialize skinned mesh model, but here only get the 
	m_SkinnedMesh = new SkinnedMeshClass;
	m_SkinnedMesh->Initialize(modelFilename, "../ZRendererDX11/Resources/stone01.tga");

	// Initialize the line renderer object.
	m_LineRenderer = new LineRendererClass;
	//m_SkinnedMesh->ConstructBoneLines(m_LineRenderer);
	FbxScene* scene = m_SkinnedMesh->GetScene();


	m_Skeleton = new SkeletonClass;
	m_Skeleton->InitSkeletonRoot(scene->GetRootNode());
	m_Skeleton->InitAnimationData(scene);
	//m_Skeleton->ContructVisualBone(m_LineRenderer);

	m_animationController = new AnimationController(m_Skeleton, m_LineRenderer);
	m_animationController->Play();

	result = m_LineRenderer->Initialize(m_Direct3D->GetDevice(), hwnd);

	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the Direct3D object.
	// That is why it is important to set all the pointers to null in the class constructor. 
	// If it does find the pointer has been initialized then it will attempt to shut down the D3DClass 
	// and then clean up the pointer afterwards.

		// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	//// Release the color shader object.
	//if (m_ColorShader)
	//{
	//	m_ColorShader->Shutdown();
	//	delete m_ColorShader;
	//	m_ColorShader = 0;
	//}

		// Release the light object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	if (m_skinnedShader) {
		m_skinnedShader->Shutdown();
		delete m_skinnedShader;
		m_skinnedShader = 0;
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	if (m_LineRenderer)
	{
		m_LineRenderer->Shutdown();
		delete m_LineRenderer;
		m_LineRenderer = 0;
	}

	if (m_Skeleton)
	{
		delete m_Skeleton;
		m_Skeleton = 0;
	}
	if (m_animationController)
	{
		delete m_animationController;
		m_animationController = 0;
	}

	return;
}


bool ApplicationClass::Frame()
{

	// logic
	//auto& settings = GlobalSettings::GetInstance();
	m_Camera->SetPosition(0.0f, GlobalSettings::s_cameraPositionY, GlobalSettings::s_cameraPositionZ);
	m_animationController->Tick();


	bool result;
	// Render the scene to a render texture.
	//result = RenderSceneToTexture();
	//if (!result)
	//{
	//	return false;
	//}

	// Render the graphics scene.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}
bool ApplicationClass::RenderSceneToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;
	// Set the render target to be the render texture and clear it.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.5f, 1.0f, 1.0f);

	// Set the position of the camera for viewing the cube.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();

	result = RenderModels();

	if (!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.  And reset the viewport back to the original.
	m_Direct3D->SetBackBufferRenderTarget();
	m_Direct3D->ResetViewport();
	m_Camera->SetPosition(0.0f, 0.0f, GlobalSettings::s_cameraPositionZ);

	return true;
}

bool ApplicationClass::RenderModels() {

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	//m_Direct3D->GetOrthoMatrix(projectionMatrix);

	//int scale = 10;
	//// Rotate the world matrix by the rotation value so that the triangle will spin.
	//worldMatrix = XMMatrixScaling(scale, scale, scale);

	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationX(GlobalSettings::s_objectRotationX));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationY(GlobalSettings::s_objectRotationY));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationZ(GlobalSettings::s_objectRotationZ));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(0.0f, GlobalSettings::s_objectPositionY, 0.0f));


	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// In Render function usually do 2 things.
	// 1. update all the parameters and data that shader need for every frame.
	// 2. in d3d context, set current shader, and draw index.
	bool result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(),
		m_Light->GetDirection(), m_Light->GetDiffuseColor());


	return result;
}

bool ApplicationClass::RenderSkinnedModels() {
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	// Generate the view matrix based on the camera's position.
	m_Camera->Render();


	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	/*int scale = 100;
	worldMatrix = XMMatrixScaling(scale, scale, scale);*/

	// push vertex and index data to context.
	m_Model->RenderSkinnedMesh(m_Direct3D->GetDeviceContext());

	const int currentFrame = m_animationController->GetCurrentFrame();
	vector<XMMATRIX> animatedMatrixes = m_Skeleton->GetAnimatedMatrixesByFrame(currentFrame);

	// 1. set shader parameters 2.drawindex.
	bool result = m_skinnedShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, animatedMatrixes,
		m_Model->GetTexture(),
		m_Light->GetDirection(), m_Light->GetDiffuseColor());


	return result;
}

bool ApplicationClass::Render()
{
	//1. IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow(); // Show demo window! :)
	MyDebugger::ShowDebugUIPanel();
	MyDebugger::ShowDebugOutputPanel();


	// (Your code calls swapchain's Present() function)
	// Present the rendered scene to the screen.
	bool result;

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//2. Render models
	//result = RenderModels();
	result = RenderSkinnedModels();

	if (!result)
	{
		return false;
	}

	//3. Render lines.
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	// Setup matrices - Bottom left display plane.
	worldMatrix = XMMatrixTranslation(-1.5f, 2.5f, 0.0f);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	//m_Direct3D->GetOrthoMatrix(projectionMatrix);

	worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	//worldMatrix = XMMatrixRotationX(GlobalSettings::s_objectRotationX);
	//worldMatrix = XMMatrixRotationY(GlobalSettings::s_objectRotationY);
	//worldMatrix = XMMatrixRotationZ(GlobalSettings::s_objectRotationZ);
	int scale = 100;
	worldMatrix = XMMatrixScaling(scale, scale, scale);
	m_LineRenderer->Render(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDevice(), worldMatrix, viewMatrix, projectionMatrix);


	// Rendering imgui at the end,so it would be on top of everything
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	m_Direct3D->EndScene();

	return true;
}
