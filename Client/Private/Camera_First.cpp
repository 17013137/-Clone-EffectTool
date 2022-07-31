#include "stdafx.h"
#include "..\Public\Camera_First.h"
#include "GameInstance.h"
#include "KeyMgr.h"

CCamera_First::CCamera_First(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CCamera(pDeviceOut, pDeviceContextOut)
{
}

CCamera_First::CCamera_First(const CCamera_First & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_First::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_First::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 10.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (m_CameraDesc.pPlayerTrans == nullptr)
		return E_FAIL;


	return S_OK;
	
}

_int CCamera_First::Tick(_double TimeDelta)
{
	
	if (CKeyMgr::Get_Instance()->Key_Down(VK_F1))
		flag = flag == TRUE ? FALSE : TRUE;

	
	m_pTransformCom->Set_WorldMTX(m_CameraDesc.pPlayerTrans->Get_WorldFloat4x4());
	//
	if (flag == true) {
		_vector temp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 3.f;
		temp = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - temp;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, temp);
	}
	else {

		_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 0.22f;
		_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * 0.15f;
		//더하고 노말라이즈
		vLook = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vLook + vUp;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLook);
	}

	__super::Tick(TimeDelta);

	return 0;
}

void CCamera_First::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CCamera_First::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CCamera_First * CCamera_First::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CCamera_First*	pInstance = new CCamera_First(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CCamera_First"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_First::Clone(void * pArg)
{
	CCamera_First*	pInstance = new CCamera_First(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CCamera_First"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_First::Free()
{
	__super::Free();

}
