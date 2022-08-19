
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
	m_ParticleDesc.Angle = _float3(0.f, 0.f, 0.f);
	m_ParticleDesc.Scale = _float3(1.f, 1.f, 1.f);
	m_ParticleDesc.Translation = _float4(0.f, 0.f, 0.f, 1.f);
	m_ParticleDesc.Duration = 10.f;
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


		ImGui::Begin("EffectTool Ver.0.0");                        // Create a window called "Hello, world!" and append into it.
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		{
			ImGui::SetNextWindowSize(ImVec2(100, 50));
			if (ImGui::Button("Start", ImVec2(70, 30))) {
				m_Restart = true;
			}

			ImGui::Text("Shader State");
			ImGui::SliderInt("Image Index", &m_ParticleDesc.ImgIndex, 0, 20);
			ImGui::SliderInt("ShaderPass", &m_ParticleDesc.ShaderPass, 0, 10);

			ImGui::Text("Basic State");
			ImGui::SliderInt("Num Instance", &m_ParticleDesc.NumInstance, 1, 500);
			ImGui::SliderFloat("Speed", &m_ParticleDesc.Speed, 0.f, 10.f);
			ImGui::SliderFloat("Duration", &m_ParticleDesc.Duration, 0.f, 100.f);

			_float fScale[3] = { m_ParticleDesc.Scale.x,  m_ParticleDesc.Scale.y, m_ParticleDesc.Scale.z };
			ImGui::SliderFloat3("Scale", fScale, 0.1f, 10.f);
			m_ParticleDesc.Scale = _float3(fScale[0], fScale[1], fScale[2]);

			_float fAngle[3] = { m_ParticleDesc.Angle.x,  m_ParticleDesc.Angle.y, m_ParticleDesc.Angle.z };
			ImGui::SliderFloat3("Angle", fAngle, -359.9f, 359.9f);
			m_ParticleDesc.Angle = _float3(fAngle[0], fAngle[1], fAngle[2]);

			_float fTrans[3] = { m_ParticleDesc.Translation.x,  m_ParticleDesc.Translation.y, m_ParticleDesc.Translation.z };
			ImGui::SliderFloat3("Translation", fTrans, -20.f, 20.f);
			m_ParticleDesc.Translation = _float4(fTrans[0], fTrans[1], fTrans[2], 1.f);

			_float fDir[3] = { m_ParticleDesc.Direction.x,  m_ParticleDesc.Direction.y, m_ParticleDesc.Direction.z };
			ImGui::SliderFloat3("Direction", fDir, -1.f, 1.f);
			m_ParticleDesc.Direction = _float4(fDir[0], fDir[1], fDir[2], 1.f);

			ImGui::Text("Particle Range");
				_float fRandSpeed[2] = { m_RandParicle.Speed.x, m_RandParicle.Speed.y };
				ImGui::SliderFloat2("RandSpeed", fRandSpeed, 0.f, 10.f);
				m_RandParicle.Speed = _float2(fRandSpeed[0], fRandSpeed[1]);
			
				_float fRandScale[2] = { m_RandParicle.Scale.x, m_RandParicle.Scale.y };
				ImGui::SliderFloat2("RandScale", fRandScale, 0.f, 10.f);
				m_RandParicle.Scale = _float2(fRandScale[0], fRandScale[1]);
			
				_float fRandDir[3] = { m_RandParicle.Direction.x,  m_RandParicle.Direction.y, m_RandParicle.Direction.z };
				ImGui::SliderFloat3("RandDirection", fRandDir, 0.f, 1.f);
				m_RandParicle.Direction = _float3(fRandDir[0], fRandDir[1], fRandDir[2]);
			
				ImGui::PushStyleColor(ImGuiCol_Border, ImColor(100, 100, 100, 255).Value);
				ImGui::BeginChild("RandTranslation", ImVec2(300, 30), true);
				{
					_float StartTrans[3] = { m_RandParicle.Translation.x,  m_RandParicle.Translation.y, m_RandParicle.Translation.z};
					ImGui::SliderFloat3("Translation", StartTrans, 0.f, 20.f);
					m_RandParicle.Translation = _float3(StartTrans[0], StartTrans[1], StartTrans[2]);
				}
				ImGui::EndChild();

				ImGui::BeginChild("RandAngle", ImVec2(300, 60), true);
				{
					_float StartAngle[3] = { m_RandParicle.StartAngle.x,  m_RandParicle.StartAngle.y, m_RandParicle.StartAngle.z };
					ImGui::SliderFloat3("StartAngle", StartAngle, 0.f, 359.9f);
					m_RandParicle.StartAngle = _float3(StartAngle[0], StartAngle[1], StartAngle[2]);

					_float EndAngle[3] = { m_RandParicle.EndAngle.x,  m_RandParicle.EndAngle.y, m_RandParicle.EndAngle.z };
					ImGui::SliderFloat3("EndAngle", EndAngle, 0.f, 359.9f);
					m_RandParicle.EndAngle = _float3(EndAngle[0], EndAngle[1], EndAngle[2]);
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

