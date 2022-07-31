#include "..\Public\Layer.h"
#include "GameObject.h"
#include "Collider.h"

CLayer::CLayer()
{
}

CComponent * CLayer::Get_Component(const _tchar * pComponentTag, _uint iIndex)
{
	if (iIndex >= m_ObjectList.size())
		return nullptr;

	auto	iter = m_ObjectList.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_Component(pComponentTag);
}

CTransform * CLayer::Get_Transform(_uint iIndex)
{
	auto iter = m_ObjectList.begin();

	for (_uint i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Get_Transform();
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_ObjectList.push_back(pGameObject);

	return S_OK;
}

void CLayer::Tick(_double TimeDelta)
{
	_int Evnet = 0;

	auto& iter = m_ObjectList.begin();

	for (; iter != m_ObjectList.end(); )
	{
		Evnet = (*iter)->Tick(TimeDelta);

		if (1 == Evnet)
		{
			Safe_Release(*iter);
			iter = m_ObjectList.erase(iter);
		}
		else
			++iter;
	}
}

void CLayer::LateTick(_double TimeDelta)
{
	for (auto& pGameObject : m_ObjectList)
		pGameObject->LateTick(TimeDelta);
}

CGameObject * CLayer::Get_PickingObject(_float *flag)
{
	_float MaxDist = (_float)INT_MAX;
	CGameObject* Result = nullptr;

	for (auto& iter : m_ObjectList) {
		CCollider* Collide = (CCollider*)iter->Get_Component(L"Com_Collide");
		if (Collide == nullptr)
			return nullptr;
		_float Dist = XMVectorGetX(XMVector3Length(Collide->Intersect_Ray()));
		if (Dist > 0) {
			if (MaxDist > Dist) {
				MaxDist = Dist;
				Result = iter;
			}
		}
	}

	return Result;
}

_int CLayer::Get_LayerIndex(CGameObject * Object)
{
	_int flag = 0;
	for (auto& iter : m_ObjectList) {
		if (iter == Object) {
			return flag;
		}
		flag++;
	}

	return -1;
}

CLayer * CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	for (auto& pGameObject : m_ObjectList)
		Safe_Release(pGameObject);

	m_ObjectList.clear();

}
