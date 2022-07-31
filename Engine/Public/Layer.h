#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
public:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_Component(const _tchar* pComponentTag, _uint iIndex = 0);
	class CTransform* Get_Transform(_uint iIndex = 0);

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Tick(_double TimeDelta);
	void LateTick(_double TimeDelta);

public:
	list<class CGameObject*>* Get_ObjectList() { return &m_ObjectList; }
	CGameObject* Get_PickingObject(_float *flag = nullptr);
	_int Get_LayerIndex(CGameObject* Object);
	_int Get_LayerSize() { return (_int)m_ObjectList.size(); }

private:
	list<class CGameObject*>			m_ObjectList;
	typedef list<class CGameObject*>	OBJECTLIST;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END