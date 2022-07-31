#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)

public:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	HRESULT InitImGui(ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut);
	HRESULT StartFrame(void);
	HRESULT Set_Contents(void);
	void InitWin32(HWND hWnd);
	LRESULT WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT Render(void);
	void Shutdown(void);

public:
	bool m_bImguiEnable = false;

public:
	void OnOffImgui(void) { m_bImguiEnable = !m_bImguiEnable; }
	bool isImguiEnable() { return m_bImguiEnable; }

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_DeviceContext = nullptr;

public:
	virtual void Free() override;
};

END