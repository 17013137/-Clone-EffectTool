
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "..\Public\Imgui_Manager.h"
#include "Component_Manager.h"
#include "Input_Device.h"
#include "Object_Manager.h"
#include "GameObject.h"
#include "Navigation.h"
#include "Cell.h"
#include "Object_Manager.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

IMPLEMENT_SINGLETON(CImgui_Manager)
CImgui_Manager::CImgui_Manager()
{
}

HRESULT CImgui_Manager::InitImGui(ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut)
{

	ImGui_ImplDX11_Init(*ppDeviceOut, *ppDeviceContextOut);

	m_pDevice = *ppDeviceOut;
	m_DeviceContext = *ppDeviceContextOut;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_DeviceContext);

	ZeroMemory(&m_ParticleDesc, sizeof(PARTICLEDESC));
	m_ParticleDesc.NumInstance = 1;
	m_ParticleDesc.AxisRotation = _float3(0.f, 0.f, 0.f);
	m_ParticleDesc.Scale = _float3(1.f, 1.f, 1.f);
	m_ParticleDesc.Translation = _float4(0.f, 0.f, 0.f, 1.f);
	m_ParticleDesc.Duration = 10.f;
	m_ParticleDesc.AlphaSpeed = 1.f;
	m_ParticleDesc.Alpha = 1.f;
	m_ParticleDesc.RemoveAlpha = 0.f;
	ZeroMemory(&m_RandParicle, sizeof(PARTICLERAND));
	return S_OK;
}

HRESULT CImgui_Manager::StartFrame(void)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	return S_OK;
}

HRESULT CImgui_Manager::Set_Contents(void)
{

	static bool show_demo_window = false;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;


		ImGui::Begin("EffectTool Ver.0.0", nullptr);                        // Create a window called "Hello, world!" and append into it.
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		{
			ImGui::SetNextWindowSize(ImVec2(100, 50));
			if (ImGui::Button("Start", ImVec2(50, 20)))
				m_Restart = true;

			ImGui::SameLine(0, 5.f);

			if (ImGui::Button("ReSet", ImVec2(50, 20))) {
				m_Color1 = _float4(1.f, 1.f, 1.f, 1.f);
				m_Color2 = _float4(0.f, 0.f, 0.f, 1.f);
				ZeroMemory(&m_ParticleDesc, sizeof(PARTICLEDESC));
				m_ParticleDesc.NumInstance = 1;
				m_ParticleDesc.AxisRotation = _float3(0.f, 0.f, 0.f);
				m_ParticleDesc.Scale = _float3(1.f, 1.f, 1.f);
				m_ParticleDesc.Translation = _float4(0.f, 0.f, 0.f, 1.f);
				m_ParticleDesc.Duration = 10.f;
				m_ParticleDesc.AlphaSpeed = 1.f;
				m_ParticleDesc.Alpha = 1.f;
				m_ParticleDesc.RemoveAlpha = 0.f;
				ZeroMemory(&m_RandParicle, sizeof(PARTICLERAND));
				m_Restart = true;
			}

			if (m_TotalTime != nullptr) {
				ImGui::Text("Time : %.1f", *m_TotalTime);
			}
			if (m_Texture != nullptr) {
				ImGui::ImageButton(m_Texture->m_Textures[m_ParticleDesc.ImgIndex], ImVec2(50, 50));
				_float Color[4] = { m_Color1.x, m_Color1.y, m_Color1.z, m_Color1.w};
				_float Color2[4] = { m_Color2.x, m_Color2.y, m_Color2.z, m_Color2.w };

				ImGui::ColorEdit4("ColorEdit", Color, 0);

				ImGui::ColorEdit4("Color2Edit", Color2, 0);

				ImGui::SliderFloat("Alpha", &m_ParticleDesc.Alpha, 0, 1.f);

				ImGui::SliderFloat("RemoveAlpha", &m_ParticleDesc.RemoveAlpha, 0, 1.f);

				ImGui::SliderFloat("Start/End AlphaSpeed", &m_ParticleDesc.AlphaSpeed, 0.01f, 10.f);

				m_Color1 = _float4(Color[0], Color[1], Color[2], Color[3]);
				m_Color2 = _float4(Color2[0], Color2[1], Color2[2], Color2[3]);
				ImGui::SliderInt("Image Index", &m_ParticleDesc.ImgIndex, 0, m_Texture->m_Textures.size()-1);
				ImGui::SliderInt("ShaderPass", &m_ParticleDesc.ShaderPass, 0, 10);

				if(ImGui::TreeNode("ParticleData")) {
					ImGui::Checkbox("Set_ParticleDir", &m_ParticleDesc.isSetParticleDir);
					ImGui::TreePop();
				}
			}
			ImGui::PushStyleColor(ImGuiCol_Border, ImColor(100, 100, 100, 255).Value);
	
			ImGui::BeginChild("State", ImVec2(300, 60), true);
			{
				ImGui::SliderFloat("Duration", &m_ParticleDesc.Duration, 0.f, 10.f);
				ImGui::Checkbox("Repeat", &m_ParticleDesc.isRepeat);
			}
			ImGui::EndChild();

			ImGui::BeginChild("Basic State", ImVec2(300, 210), true);
			ImGui::Text("Basic State");
			ImGui::SliderInt("Num Instance", &m_ParticleDesc.NumInstance, 1, 500);
			ImGui::SliderFloat("Speed", &m_ParticleDesc.Speed, 0.f, 1.f);

			_float fScale[3] = { m_ParticleDesc.Scale.x,  m_ParticleDesc.Scale.y, m_ParticleDesc.Scale.z };
			ImGui::SliderFloat3("Scale", fScale, 0.1f, 10.f);
			m_ParticleDesc.Scale = _float3(fScale[0], fScale[1], fScale[2]);
			_float fTrans[3] = { m_ParticleDesc.Translation.x,  m_ParticleDesc.Translation.y, m_ParticleDesc.Translation.z };
			ImGui::SliderFloat3("Translation", fTrans, -20.f, 20.f);
			m_ParticleDesc.Translation = _float4(fTrans[0], fTrans[1], fTrans[2], 1.f);

			ImGui::Checkbox("Set_Dir", &m_ParticleDesc.isSetDir);
			if (m_ParticleDesc.isSetDir == true) {
				_float fSetDir[3] = { m_ParticleDesc.Direction.x,  m_ParticleDesc.Direction.y, m_ParticleDesc.Direction.z };
				ImGui::SliderFloat3("Set_Dir", fSetDir, -50.f, 50.f);
				m_ParticleDesc.Direction = _float4(fSetDir[0], fSetDir[1], fSetDir[2], 0.f);
			}
			else {
				_float fDir[3] = { m_ParticleDesc.Direction.x,  m_ParticleDesc.Direction.y, m_ParticleDesc.Direction.z };
				ImGui::SliderFloat3("Direction", fDir, -1.f, 1.f);
				m_ParticleDesc.Direction = _float4((_float)fDir[0], (_float)fDir[1], (_float)fDir[2], 0.f);
			}
			_float fAngle[3] = { m_ParticleDesc.AxisRotation.x,  m_ParticleDesc.AxisRotation.y, m_ParticleDesc.AxisRotation.z };
			ImGui::SliderFloat3("RotateAxis", fAngle, -1.f, 1.f);
			m_ParticleDesc.AxisRotation = _float3((_float)fAngle[0], (_float)fAngle[1], (_float)fAngle[2]);

			ImGui::SliderFloat("RotateSpeed", &m_ParticleDesc.RotateSpeed, 0.f, 20.f);

			ImGui::EndChild();

			//랜덤값 주는곳
			ImGui::BeginChild("Random_Range", ImVec2(300, 180), true);
			ImGui::Text("Particle Range"); 
			_float fRandSpeed[2] = { m_RandParicle.Speed.x, m_RandParicle.Speed.y };
			{
				ImGui::SliderFloat2("RandSpeed", fRandSpeed, 0.f, 1.f);
				m_RandParicle.Speed = _float2(fRandSpeed[0], fRandSpeed[1]);

				_float fRandScale[2] = { m_RandParicle.Scale.x, m_RandParicle.Scale.y };
				ImGui::SliderFloat2("RandScale", fRandScale, 0.f, 10.f);
				m_RandParicle.Scale = _float2(fRandScale[0], fRandScale[1]);

				_float fRandDir[3] = { m_RandParicle.Direction.x,  m_RandParicle.Direction.y, m_RandParicle.Direction.z };
				ImGui::SliderFloat3("RandDirection", fRandDir, 0.f, 1.f);
				m_RandParicle.Direction = _float3(fRandDir[0], fRandDir[1], fRandDir[2]);
				_float StartTrans[3] = { m_RandParicle.Translation.x,  m_RandParicle.Translation.y, m_RandParicle.Translation.z};
				ImGui::SliderFloat3("Translation", StartTrans, 0.f, 20.f);
				m_RandParicle.Translation = _float3(StartTrans[0], StartTrans[1], StartTrans[2]);
				_float AxisRot[3] = { m_RandParicle.RandAxisRot.x,  m_RandParicle.RandAxisRot.y, m_RandParicle.RandAxisRot.z };
				ImGui::SliderFloat3("Rand_Rot", AxisRot, 0.f, 1.f);
				m_RandParicle.RandAxisRot = _float3(AxisRot[0], AxisRot[1], AxisRot[2]);

				_float RotateSpeed[2] = { m_RandParicle.RotateSpeed.x, m_RandParicle.RotateSpeed.y };
				ImGui::SliderFloat2("Rand_RotSpeed", RotateSpeed, 0.f, 5.f);
				m_RandParicle.RotateSpeed = _float2(RotateSpeed[0], RotateSpeed[1]);
			}
			ImGui::EndChild();
			ImGui::PopStyleColor();

		}
		ImGui::End();
	}


	return S_OK;
}



HRESULT CImgui_Manager::Render()
{
	// Imgui render옵션이 켜져 있다면
	if (m_bImguiEnable) {
		StartFrame();
		Set_Contents();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	return S_OK;
}

void CImgui_Manager::InitWin32(HWND hWnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hWnd);
}

LRESULT CImgui_Manager::WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
}

void CImgui_Manager::Shutdown(void)
{
}

void CImgui_Manager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

