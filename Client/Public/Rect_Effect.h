#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Imgui_Manager.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer_Rect_Instance;
END

BEGIN(Client)

class CRect_Effect final : public CGameObject
{
private:
	explicit CRect_Effect(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CRect_Effect(const CRect_Effect& rhs);
	virtual ~CRect_Effect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	

private:	
	CRenderer*						m_pRendererCom = nullptr;
	CShader*						m_pShaderCom = nullptr;	
	CTexture*						m_pTextureCom = nullptr;
	CVIBuffer_Rect_Instance*		m_pVIBufferCom = nullptr;
	CImgui_Manager*					m_imguiMgr = nullptr;

public:
	PARTICLEDESC m_Paricledesc;
	_bool m_isRemove = false;
	_int *m_ShaderIndex = nullptr;
	_int *m_ImgIndex = nullptr;
	_float4 *m_Color1 = nullptr;
	_float4 *m_Color2 = nullptr;
	_float *m_RemoveAlpha = nullptr;
	_float m_Alpha = 1.f;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();	

public:
	static CRect_Effect* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END