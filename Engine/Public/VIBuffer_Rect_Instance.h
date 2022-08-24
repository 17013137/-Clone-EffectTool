#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instance final : public CVIBuffer
{
protected:
	CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& rhs);
	virtual ~CVIBuffer_Rect_Instance() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(_uint flag);
	virtual HRESULT NativeConstruct(void* pArg);
	HRESULT Update(_double TimeDelta);
	virtual HRESULT Render() override;

public:
	HRESULT Reset_Buffer();
	_vector Rotate_Direction(_double Timedelta, _vector Dir, _float3* vRotate, _float Speed);

private:
	PARTICLEDESC			m_Particledesc;
	PARTICLERAND			m_RandParicle;
	_double					m_AccTime = 0.0;
public:
	ID3D11Buffer*			m_pVBInstance = nullptr;
	D3D11_BUFFER_DESC		m_VBInstDesc;
	D3D11_SUBRESOURCE_DATA	m_VBInstSubResourceData;
	_uint					m_iInstanceStride = 0;
	vector<VTXMATRIX>		m_SaveData;

public:
	static CVIBuffer_Rect_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

