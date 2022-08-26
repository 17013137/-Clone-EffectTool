#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)
typedef struct Item {
	char Name[MAX_PATH];
	bool is_Selected = false;
}Item;

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
	void DynamicListBox(char* NameTag, vector<Item>* ItemBox, int* SelCnt);
	HRESULT Save();
	HRESULT Load();

public: //메쉬이펙트 함수
	void Start_MeshEffect();
	void Reset_MeshEffect();
public:
	bool m_bImguiEnable = false;

public:  //파티클
	PARTICLEDESC m_ParticleDesc;
	PARTICLERAND m_RandParicle;
	_bool m_Restart = false;
	CTexture* m_Texture = nullptr;
	_double* m_TotalTime = nullptr;
	char m_DataTag[MAX_PATH] = ""; //파티클 Save/Load
	vector<VTXMATRIX>* m_SaveData = nullptr;
	_bool m_isStart = false;
	_bool m_isEnd = false;
	_double m_AccTime = 0.0;

public: //메쉬 이펙트
	vector<Item> m_MeshTag;
	_int m_MeshCnt = 0;
	MESHEFFECTDESC m_MeshEffDesc;


public: //탭
	_int m_Tab = 0;

public: //더미
	_bool m_isDummy = false;
	int m_DummyCnt = 0;
	_uint m_DummyMaxCnt = 0;
	_float3 m_TotalScale = _float3(1.f, 1.f, 1.f);

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