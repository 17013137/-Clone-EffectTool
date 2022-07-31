#pragma once

#include "Component.h"

/* 1. 객체들을 그리는순서에 따라 그룹지어 모아놓는다. */
/* 2. 모아놓은 객체들의 렌더함수를 호출해준다.  */
/* 3. 그룹에 맞는 렌더상태를 설정한다.  */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { GROUP_PRIORITY, GROUP_NONBLEND, GROUP_ALPHABLEND, GROUP_UI, GROUP_END };
private:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eGroup, class CGameObject* pGameObject);
	HRESULT Render_GameObjects();


private:
	list<class CGameObject*>			m_RenderList[GROUP_END];
	typedef list<class CGameObject*>	RENDERLIST;

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_UI();


public:
	static CRenderer* Create(ID3D11Device* ppDeviceOut, ID3D11DeviceContext* ppDeviceContextOut);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

