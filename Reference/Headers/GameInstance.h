#pragma once

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Timer_Manager.h"
#include "PIpeLine.h"
#include "Light_Manager.h"
#include "Imgui_Manager.h"
#include "Picking.h"
#include "Frustum.h"
#include "Navigation.h"

/* 클라이언트에 보여줘야할 인터페이스를 보관하고. 보여준다. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance : public CBase {

	DECLARE_SINGLETON(CGameInstance)

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.GameInstance */
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, CGraphic_Device::GRAPHICDESC GraphicDesc, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut);
	void Tick_Engine(_double TimeDelta);
	HRESULT Render_Engine();
	HRESULT Render_Begin(_float4 vClearColor);
	HRESULT Render_End();
	HRESULT Clear_LevelResource(_uint iLevelIndex);

public: /* For.Input_Device */
	_char Get_DIKeyState(_uchar byKeyID);
	_char Get_DIMButtonState(CInput_Device::MOUSEBUTTONSTATE eDIMBState);
	_long Get_DIMMoveState(CInput_Device::MOUSEMOVESTATE eDIMMState);

public: /* For.Timer_Manager */
	HRESULT Add_Timers(const _tchar* pTimerTag);
	_double Compute_TimeDelta(const _tchar* pTimerTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pLevel);
	void Set_LevelIndex(_uint iLevelIndex);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObjectToLayer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	CGameObject* Add_GameObjToLayer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	CComponent* Get_Component(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex = 0);
	CTransform* Get_Transform(_uint iLevelIndex, const _tchar * pLayerTag, _uint iIndex = 0);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, CComponent* pPrototype);
	CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);

public: /* For.PipeLine */
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState);
	_float4x4 Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE eState);
	_vector Get_CamPosition();
	_float4 Get_CamPositionFloat4();

public: /*For.Light_Manager */
	const LIGHTDESC* Get_LightDesc(_uint iIndex);

	HRESULT Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);

public: /*For.Frustum */
	_bool isInFrustum_WorldSpace(_vector vWorldPos, _float fRange = 0.f);

public: /*For.Picking   */
	const _float3& Get_WorldRay();
	const _float3& Get_WorldRayPos();

public: /* For.Navigation */
	HRESULT Initialize_Navigation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pNavigationData, CTransform* pTransform);
	HRESULT Render_Navigation();
	HRESULT Render_NavigationCell(_uint iIndex);

public:
	void OnOffgui();
	HRESULT RenderImgui();
	LRESULT WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM IParam);


private:
	CGraphic_Device*			m_pGraphic_Device = nullptr;
	CInput_Device*				m_pInput_Device = nullptr;
	CLevel_Manager*				m_pLevel_Manager = nullptr;
	CObject_Manager*			m_pObject_Manager = nullptr;
	CComponent_Manager*			m_pComponent_Manager = nullptr;
	CTimer_Manager*				m_pTimer_Manager = nullptr;
	CPipeLine*					m_pPipeLine = nullptr;
	CLight_Manager*				m_pLight_Manager = nullptr;
	CImgui_Manager*				m_pImgui_Manager = nullptr;
	CFrustum*					m_pFrustum = nullptr;
	CPicking*					m_pPicking = nullptr;
	CNavigation*				m_pNavigation = nullptr;

public:
	static void Release_Engine();
	virtual void Free() override;
};

END