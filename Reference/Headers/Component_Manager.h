#pragma once

#include "Model.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"
#include "Collider.h"
#include "Transform.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Line.h"

/* ������Ʈ���� ������ ��Ƶд�. */

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Reserve_Manager(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, CComponent* pPrototype);
	CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);
	vector<const _tchar*> Get_ComponentTag(_uint iLevelIndex);

private:
	_uint											m_iNumLevels = 0;
	map<const _tchar*, CComponent*>*				m_pPrototypes = nullptr;
	typedef map<const _tchar*, CComponent*>			PROTOTYPES;

	_tchar* ModelTag = L"Prototype_Component_Model";

private:
	CComponent* Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END