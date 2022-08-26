#include "stdafx.h"
#include "..\Public\MeshEffect.h"
#include "GameInstance.h"
#include "Imgui_Manager.h"

CMeshEffect::CMeshEffect(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{
}

CMeshEffect::CMeshEffect(const CMeshEffect & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMeshEffect::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeshEffect::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);
	
	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (pArg != nullptr) {
		memcpy(&m_ModelIndex, pArg, sizeof(int));
	}
	
	for (auto& iter : m_Models) {
		m_NameTag.push_back(iter->Get_FileName());
	}

	if (CImgui_Manager::GetInstance()->m_MeshTag.size() == 0) {
		for (auto& iter : m_NameTag) {
			CImgui_Manager::Item item;
			strcpy_s(item.Name, iter);
			item.is_Selected = false;
			CImgui_Manager::GetInstance()->m_MeshTag.push_back(item);
		}
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}

	m_EffectDesc = &CImgui_Manager::GetInstance()->m_MeshEffDesc;

	return S_OK;
}

_int CMeshEffect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	if (m_Dead == true)
		return 1;

	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
	if (m_EffectDesc->isDuration == true) {
		if (m_EffectDesc->isStart == true) {
			if (m_EffectDesc->StartAlpha.isOn == true && m_EffectDesc->AccTime < m_EffectDesc->Duration) {
				if (m_EffectDesc->MaxAlpha < m_EffectDesc->Alpha) {
					m_EffectDesc->Alpha = m_EffectDesc->MaxAlpha;
				}
				m_EffectDesc->Alpha += TimeDelta * m_EffectDesc->StartAlpha.Speed;
			}

			if (m_EffectDesc->ScaleSpeed.x != 1.f)
				m_EffectDesc->AccScale.x = m_EffectDesc->AccScale.x + TimeDelta * m_EffectDesc->ScaleSpeed.x;
			if (m_EffectDesc->ScaleSpeed.y != 1.f)
				m_EffectDesc->AccScale.y = m_EffectDesc->AccScale.y + TimeDelta * m_EffectDesc->ScaleSpeed.y;
			if (m_EffectDesc->ScaleSpeed.z != 1.f)
				m_EffectDesc->AccScale.z = m_EffectDesc->AccScale.z + TimeDelta * m_EffectDesc->ScaleSpeed.z;

			m_pTransformCom->Scaled(m_EffectDesc->AccScale);

			m_EffectDesc->AccTime += TimeDelta;
			if (m_EffectDesc->AccTime > m_EffectDesc->Duration) {
				if (m_EffectDesc->EndAlpha.isOn == true) {
					if (m_EffectDesc->Alpha < 0.f) {
						m_EffectDesc->Alpha = 0.f;
						m_EffectDesc->isStart = false;
					}
					m_EffectDesc->Alpha -= TimeDelta * m_EffectDesc->EndAlpha.Speed;
				}
				else
					m_EffectDesc->isStart = false;
			}
		}
	}
	else {
		m_pTransformCom->Scaled(m_EffectDesc->Scale);
		if (m_EffectDesc->isStart == true) {
			if (m_EffectDesc->StartAlpha.isOn == true) {
				if (m_EffectDesc->MaxAlpha < m_EffectDesc->Alpha) {
					m_EffectDesc->Alpha = m_EffectDesc->MaxAlpha;
				}
				m_EffectDesc->Alpha += TimeDelta * m_EffectDesc->StartAlpha.Speed;
			}
		}
	}

	return 0;
}

void CMeshEffect::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	int temp = CImgui_Manager::GetInstance()->m_Tab;

	if (nullptr != m_pRendererCom  && CImgui_Manager::GetInstance()->m_Tab == 1)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);

	}
}

HRESULT CMeshEffect::Render()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_Models[m_ModelIndex])
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshContainers = m_Models[m_ModelIndex]->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		if (FAILED(m_Models[m_ModelIndex]->Bind_Material_OnShader(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			return E_FAIL;

		if (FAILED(m_Models[m_ModelIndex]->Render(m_pShaderCom, "g_BoneMatrices", i, m_EffectDesc->Shader)))
			return E_FAIL;
	}	

#ifdef _DEBUG
	//m_pSphereCom->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CMeshEffect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNonAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	/* For.Com_AABB */
	ColliderDesc.vPosition = _float3(0.f, 0.0f, 0.f);
	ColliderDesc.fRadius = 3.f;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;
	
	// LEVEL_GAMEPLAY -> Current LevelIndex��
	CModel* ComModel = nullptr;

	if (FAILED(__super::SetUp_Components(TEXT("Model0"), LEVEL_STATIC, TEXT("Prototype_Component_Model_BoxingEffect"), (CComponent**)&ComModel)))
		return E_FAIL;
	m_Models.push_back(ComModel);

	return S_OK;
}

HRESULT CMeshEffect::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;	
	if (FAILED(m_pShaderCom->Set_RawValue("g_RemoveAlpha", &m_EffectDesc->RemoveAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_EffectDesc->Alpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Color1", &m_EffectDesc->Color1, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Color2", &m_EffectDesc->Color2, sizeof(_float3))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CMeshEffect * CMeshEffect::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CMeshEffect*	pInstance = new CMeshEffect(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CMeshEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMeshEffect::Clone(void * pArg)
{
	CMeshEffect*	pInstance = new CMeshEffect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CMeshEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshEffect::Free()
{
	__super::Free();	
	
	for (auto* Models : m_Models) {
		Safe_Release(Models);
	}

	Safe_Release(m_pSphereCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}
