#include "..\Public\GameObject.h"

#include "GameInstance.h"
#include "Level_Manager.h"

const _tchar*	CGameObject::m_pTransformTag = TEXT("Com_Transform");

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice), m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice), m_pDeviceContext(rhs.m_pDeviceContext)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

HRESULT CGameObject::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::NativeConstruct(void * pArg, CTransform::TRANSFORMDESC* pTransformDesc)
{
	if (nullptr != Find_Component(m_pTransformTag))
		return E_FAIL;

	m_pTransformCom = CTransform::Create(m_pDevice, m_pDeviceContext, pTransformDesc);

	m_Components.insert(COMPONENTS::value_type(m_pTransformTag, m_pTransformCom));

	m_iLevelIndex = CLevel_Manager::GetInstance()->Get_CurrentLevelIndex();

	//if (pArg != nullptr) {
	//	_float4x4 WorldMtx = _float4x4();
	//	memcpy(&WorldMtx, pArg, sizeof(_float4x4));
	//	m_pTransformCom->Set_WorldMTX(WorldMtx);
	//}

	Safe_AddRef(m_pTransformCom);

	return S_OK;
}

_int CGameObject::Tick(_double TimeDelta)
{
	if (true == m_Dead)
		return 1;
	if (m_Culling == true)
		return 0;

	return 0;
}

void CGameObject::LateTick(_double TimeDelta)
{
	if (m_Culling == true)
		return;
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::SetUp_Components(const _tchar* pComponentTag, _uint iLevelIndex, const _tchar* pPrototypeTag, CComponent** ppOut, void* pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(pGameInstance);

	CComponent*	pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.insert(COMPONENTS::value_type(pComponentTag, pComponent));

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	Safe_Release(pGameInstance);

	return S_OK;
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTagFinder(pComponentTag));

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	Safe_Release(m_pTransformCom);

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
