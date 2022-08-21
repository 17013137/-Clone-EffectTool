#include "stdafx.h"
#include "..\Public\Rect_Effect.h"
#include "GameInstance.h"

CRect_Effect::CRect_Effect(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CRect_Effect::CRect_Effect(const CRect_Effect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRect_Effect::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRect_Effect::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	/* For.Com_VIBuffer */

	if (FAILED(__super::SetUp_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_RectInstance"), (CComponent**)&m_pVIBufferCom, pArg)))
		return E_FAIL;

	m_imguiMgr = CImgui_Manager::GetInstance();
	m_ImgIndex = &m_imguiMgr->m_ParticleDesc.ImgIndex;
	m_ShaderIndex = &m_imguiMgr->m_ParticleDesc.ShaderPass;
	m_Color1 = &m_imguiMgr->m_Color1;
	m_Color2 = &m_imguiMgr->m_Color2;
	m_RemoveAlpha = &m_imguiMgr->m_ParticleDesc.RemoveAlpha;
	m_AlphaSpeed = &m_imguiMgr->m_ParticleDesc.AlphaSpeed;
	m_Alpha = &m_imguiMgr->m_ParticleDesc.AlphaSpeed;

	CImgui_Manager::GetInstance()->m_Texture = m_pTextureCom;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	return S_OK;
}

_int CRect_Effect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pVIBufferCom->Update(TimeDelta)))
		m_isRemove = true;

	if (m_isRemove == true)
		*m_Alpha -= TimeDelta * (*m_AlphaSpeed);

	return 0;
}

void CRect_Effect::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
}

HRESULT CRect_Effect::Render()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin(*m_ShaderIndex);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CRect_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	/* For.Com_Texture */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CRect_Effect::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	_matrix InverseView = XMMatrixTranspose(XMMatrixInverse(nullptr, pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW)));
	_float4x4 flag;
	XMStoreFloat4x4(&flag, InverseView);
	if (FAILED(m_pShaderCom->Set_RawValue("g_CamInverseMatrix", &flag, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Color1", m_Color1, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Color2", m_Color2, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_RemoveAlpha", m_RemoveAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", m_Alpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", *m_ImgIndex)))
		return E_FAIL;
	
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CRect_Effect * CRect_Effect::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CRect_Effect*	pInstance = new CRect_Effect(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CRect_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRect_Effect::Clone(void * pArg)
{
	CRect_Effect*	pInstance = new CRect_Effect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CRect_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRect_Effect::Free()
{
	__super::Free();	
	
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
