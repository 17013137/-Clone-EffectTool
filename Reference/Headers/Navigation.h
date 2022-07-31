#pragma once

#include "Base.h"

BEGIN(Engine)

class CNavigation final : public CBase
{
	DECLARE_SINGLETON(CNavigation)
private:
	CNavigation();
	virtual ~CNavigation() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pNavigationData, class CTransform* pTranform);
	_bool Move_OnNavigation(_float3* vPosition, _uint* pCurrentIndex, _fvector PlayerDir, _bool isJump = false);
	_bool Sky_Move_OnNavi(_fvector vPosition, _uint* pCurrentIndex);
	_int Search_MyIndex(_fvector vPosition);

#ifdef _DEBUG
public:
	HRESULT Render();
	HRESULT Render_Cell(_int iIndex);

#endif // _DEBUG

public:
	HRESULT Push_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CELLDESC* Cell);
	HRESULT Remove_LastCell();
	vector<class CCell*> Get_Cells() { return m_Cells; }

private:
	vector<class CCell*>				m_Cells;
	typedef vector<class CCell*>		CELLS;
	class CTransform*			m_pTransform = nullptr;

#ifdef _DEBUG
private:
	class CShader*				m_pDebugShader = nullptr;
#endif // _DEBUG

private:
	HRESULT SetUp_Neighbor();
	_int m_NowIndex = 0;
	_int m_PrevIndex = 1;

public:
	virtual void Free() override;
};

END