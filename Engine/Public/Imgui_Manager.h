#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameObject;

static const char* PrototypeList[]{ "Prototype_GameObject_Bed", "Prototype_GameObject_House_00", "Prototype_GameObject_House_01", "Prototype_GameObject_House_02" , "Prototype_GameObject_House_04",
"Prototype_GameObject_ParkingRoad", "Prototype_GameObject_CampgroundRoad", "Prototype_GameObject_LandScape" };



class ENGINE_DLL CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)
public:
	char* Cell_TypeIndex[5]{ "Type1", "Type2", "Type3", "Type4", "Type5" };

public:
	typedef struct DATADESC {
		_uint TagIndex;
		CGameObject* Object;
	}DATADESC;

	typedef struct SAVEDESC {
		_uint TagIndex;
		_float4x4 WorldMtx;
	}SAVEDESC;

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
	//맵툴함수
	void MapMenu_Contents();
	void Create_Object(_int index);
	_bool ObjectPicking();
	_bool TilePicking(_int index);
	void Remote_PickObj();
	_bool SaveData();
	_bool LoadData();

	//네비메쉬함수
	void NaviMenu_Contents();
	_bool Remote_Navi();
	_bool Push_Cell();
	void Undo_NaviBox();
	_bool SaveNavi();
	_bool LoadNavi();

private:
	_bool Navi_Create();

public:
	bool m_bImguiEnable = false;
	_bool m_NextPick = false;
	CGameObject* m_PickObj = nullptr;
	vector<DATADESC*> m_ObjectList;

	//네비
	_bool m_Navimode = false;
	_uint m_NaviCount = 0;
	CGameObject* m_PickNavi = nullptr;
	vector<CELLDESC> m_Cell;
	CGameObject* m_CreateNavi[3] = { nullptr };
	CELLDESC m_Point = { _float3() };
	_int m_CellType = 0;

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