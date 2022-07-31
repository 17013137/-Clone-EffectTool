#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CBullet abstract : public CGameObject
{
public:
	typedef struct BulletDesc{
		_vector vLook;
		_vector vPos;
		_float Damage;
		_float CriticalPer;
}BULLETDESC;

protected:
	explicit CBullet(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CBullet(const CBullet& rhs);
	virtual ~CBullet() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg, CTransform::TRANSFORMDESC* pTransformDesc = nullptr) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float m_Damage;
	_float m_CriticalPer;

	_uint m_BoundCnt = 0;  //0 상태에서 충돌시 소멸
	_bool m_Penetration = false;
	

	list<CGameObject*> m_isCollisonlist;  //해당 리스트에 들어오면 이미 맞은애들
	

	/*
	*생성시 받아야할거
		날아가는 시작 방향
		시작 위치
		공격력
	*/

public:	
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END