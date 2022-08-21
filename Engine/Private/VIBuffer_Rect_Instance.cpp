#include "..\Public\VIBuffer_Rect_Instance.h"
#include "Imgui_Manager.h"
#include "GameInstance.h"

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Rect_Instance::CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance & rhs)
	: CVIBuffer(rhs)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_VBInstDesc(rhs.m_VBInstDesc)
	, m_iInstanceStride(rhs.m_iInstanceStride)
	, m_Particledesc(rhs.m_Particledesc)
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Rect_Instance::NativeConstruct_Prototype(_uint flag)
{
	ZeroMemory(&m_Particledesc, sizeof(PARTICLEDESC));
#pragma region VERTEX_BUFFER
	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 4;
	m_iNumVBuffers = 2;	
	m_Particledesc.NumInstance = flag;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.StructureByteStride = m_iStride;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;

	VTXTEX*		pVertices = new VTXTEX[m_iNumVertices];
	
	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexUV = _float2(0.0f, 0.0f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexUV = _float2(1.0f, 0.0f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexUV = _float2(1.0f, 1.0f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexUV = _float2(0.0f, 1.0f);

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion
	

	if (FAILED(Reset_Buffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::NativeConstruct(void * pArg)
{

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Update(_double TimeDelta)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	if (CImgui_Manager::GetInstance()->m_Restart == true) {
		CImgui_Manager::GetInstance()->m_Restart = false;
		CImgui_Manager::GetInstance()->m_TotalTime = &m_AccTime;
		Reset_Buffer();
	}
	
	D3D11_MAPPED_SUBRESOURCE			SubResource;

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (int i = 0; i < m_Particledesc.NumInstance; ++i)
	{
		_vector vAxisRotation = XMVector3Normalize(XMLoadFloat3(&m_Particledesc.AxisRotation));
		_vector vDir = XMVector3Normalize(XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vDirection));
		_vector	vPosition = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vTranslation);
		
		if (XMVectorGetX(XMVector3Length(vAxisRotation)) > 0) {
			vDir = XMVector3Normalize(Rotate_Direction(TimeDelta, vDir, &((VTXMATRIX*)SubResource.pData)[i].vAngle, ((VTXMATRIX*)SubResource.pData)[i].fRotationSpeed));
		}

		vPosition += vDir * ((VTXMATRIX*)SubResource.pData)[i].fSpeed * 0.5f;
		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vTranslation, vPosition);

		((VTXMATRIX*)SubResource.pData)[i].Time += TimeDelta *( 1 + ((VTXMATRIX*)SubResource.pData)[i].fSpeed);

		if (((VTXMATRIX*)SubResource.pData)[i].Time > m_Particledesc.Duration && m_Particledesc.isRepeat == true) {
			((VTXMATRIX*)SubResource.pData)[i].Time = 0.0;
			((VTXMATRIX*)SubResource.pData)[i].vTranslation = ((VTXMATRIX*)SubResource.pData)[i].vOriginTrans;
			((VTXMATRIX*)SubResource.pData)[i].vAngle = ((VTXMATRIX*)SubResource.pData)[i].vOriginAngle;
		}
	}
	m_pDeviceContext->Unmap(m_pVBInstance, 0);
	m_AccTime += TimeDelta;
	if (m_AccTime >= m_Particledesc.Duration) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Render()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	ID3D11Buffer*	pVertexBuffers[] = {
		m_pVB, 
		m_pVBInstance
	};

	_uint			iStrides[] = {
		m_iStride, 
		m_iInstanceStride

	};

	_uint			iOffset[] = {
		0, 0
	};

	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVBuffers, pVertexBuffers, iStrides, iOffset);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);
	m_pDeviceContext->DrawIndexedInstanced(6, m_Particledesc.NumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Rect_Instance::Reset_Buffer()
{
	m_AccTime = 0.0;
	m_Particledesc = CImgui_Manager::GetInstance()->m_ParticleDesc;
	m_RandParicle = CImgui_Manager::GetInstance()->m_RandParicle;
#pragma region INSTANCE_BUFFER

	m_iInstanceStride = sizeof(VTXMATRIX);

	ZeroMemory(&m_VBInstDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBInstDesc.ByteWidth = m_iInstanceStride * m_Particledesc.NumInstance;
	m_VBInstDesc.StructureByteStride = m_iInstanceStride;
	m_VBInstDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstDesc.MiscFlags = 0;

	VTXMATRIX*		pInstanceVertices = new VTXMATRIX[m_Particledesc.NumInstance];

	for (int i = 0; i < m_Particledesc.NumInstance; ++i)
	{
		//Random°ª
		_float Scale = CGameInstance::GetInstance()->Get_Randomfloat(m_RandParicle.Scale.x, m_RandParicle.Scale.y);
		_float Speed = CGameInstance::GetInstance()->Get_Randomfloat(m_RandParicle.Speed.x, m_RandParicle.Speed.y);
		_float RotSpeed = CGameInstance::GetInstance()->Get_Randomfloat(m_RandParicle.RotateSpeed.x, m_RandParicle.RotateSpeed.y);
		_float3 Translation = _float3(CGameInstance::GetInstance()->Get_Randomfloat(-m_RandParicle.Translation.x, m_RandParicle.Translation.x), CGameInstance::GetInstance()->Get_Randomfloat(-m_RandParicle.Translation.y, m_RandParicle.Translation.y), CGameInstance::GetInstance()->Get_Randomfloat(-m_RandParicle.Translation.z, m_RandParicle.Translation.z));
		_float3 AxisRotation = _float3(CGameInstance::GetInstance()->Get_Randomfloat(-m_RandParicle.RandAxisRot.x, m_RandParicle.RandAxisRot.x), CGameInstance::GetInstance()->Get_Randomfloat(-m_RandParicle.RandAxisRot.y, m_RandParicle.RandAxisRot.y), CGameInstance::GetInstance()->Get_Randomfloat(-m_RandParicle.RandAxisRot.z, m_RandParicle.RandAxisRot.z));
		_float3 Direction = _float3();

		pInstanceVertices[i].vTranslation = _float4(m_Particledesc.Translation.x + Translation.x, m_Particledesc.Translation.y + Translation.y, m_Particledesc.Translation.z + Translation.z, 1.f);
		pInstanceVertices[i].vOriginTrans = pInstanceVertices[i].vTranslation;
		pInstanceVertices[i].fSpeed = m_Particledesc.Speed + Speed;
		pInstanceVertices[i].fRotationSpeed = m_Particledesc.RotateSpeed + RotSpeed;
		pInstanceVertices[i].vAngle = _float3(m_Particledesc.AxisRotation.x + AxisRotation.x, m_Particledesc.AxisRotation.y + AxisRotation.y, m_Particledesc.AxisRotation.z + AxisRotation.z);
		pInstanceVertices[i].vOriginAngle = pInstanceVertices[i].vAngle;
		pInstanceVertices[i].Time = 0.0;
		
		if (m_Particledesc.isSetDir == true) {
			Direction = _float3(m_Particledesc.Direction.x - pInstanceVertices[i].vTranslation.x, m_Particledesc.Direction.z - pInstanceVertices[i].vTranslation.y, m_Particledesc.Direction.z - pInstanceVertices[i].vTranslation.z);
			pInstanceVertices[i].vDirection = _float4(Direction.x, Direction.y, Direction.z, 0.f);
		}
		else {
			Direction = _float3(CGameInstance::GetInstance()->Get_Randomfloat(-m_RandParicle.Direction.x, m_RandParicle.Direction.x), CGameInstance::GetInstance()->Get_Randomfloat(-m_RandParicle.Direction.y, m_RandParicle.Direction.y), CGameInstance::GetInstance()->Get_Randomfloat(-m_RandParicle.Direction.z, m_RandParicle.Direction.z));
			pInstanceVertices[i].vDirection = _float4(m_Particledesc.Direction.x + Direction.x, m_Particledesc.Direction.y + Direction.y, m_Particledesc.Direction.z + Direction.z, 0.f);
		}
		if (m_Particledesc.isSetParticleDir == true) {
			_vector vRight = XMVector3Normalize(XMLoadFloat3(&(_float3&)pInstanceVertices[i].vDirection));
			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			_vector vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));
			vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

			XMStoreFloat4(&pInstanceVertices[i].vRight, vRight *(m_Particledesc.Scale.x + Scale));
			XMStoreFloat4(&pInstanceVertices[i].vUp, vUp *(m_Particledesc.Scale.y + Scale));
			XMStoreFloat4(&pInstanceVertices[i].vLook, vLook *(m_Particledesc.Scale.z + Scale));
		}
		else {
			pInstanceVertices[i].vRight = _float4(1.f * (m_Particledesc.Scale.x + Scale), 0.f, 0.f, 0.f);
			pInstanceVertices[i].vUp = _float4(0.f, 1.f * (m_Particledesc.Scale.y + Scale), 0.f, 0.f);
			pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f * (m_Particledesc.Scale.z + Scale), 0.f);
		}
	}

	ZeroMemory(&m_VBInstSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBInstSubResourceData.pSysMem = pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBInstDesc, &m_VBInstSubResourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVertices);
#pragma endregion

#pragma region INDEX_BUFFER

	m_iPrimitiveIndicesSize = sizeof(FACELISTINDICES16);
	m_iNumPrimitive = 2 * m_Particledesc.NumInstance;
	m_iNumIndicesPerPrimitive = 3;
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = m_iPrimitiveIndicesSize * m_iNumPrimitive;
	m_IBDesc.StructureByteStride = 0;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;

	FACELISTINDICES16*		pIndices = new FACELISTINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACELISTINDICES16) * m_iNumPrimitive);

	_uint		iNumFaces = 0;

	for (int i = 0; i < m_Particledesc.NumInstance; ++i)
	{
		pIndices[iNumFaces]._0 = 0;
		pIndices[iNumFaces]._1 = 1;
		pIndices[iNumFaces]._2 = 2;
		++iNumFaces;

		pIndices[iNumFaces]._0 = 0;
		pIndices[iNumFaces]._1 = 2;
		pIndices[iNumFaces]._2 = 3;
		++iNumFaces;
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;
#pragma endregion
	Safe_Delete_Array(pIndices);
	return S_OK;
}

_vector CVIBuffer_Rect_Instance::Rotate_Direction(_double Timedelta, _vector Dir, _float3* vRotate, _float Speed)
{
	_vector vLook = Dir;
	_vector AxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Cross(AxisY, vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	_matrix Mtx = XMMatrixIdentity();
	Mtx.r[0] = vRight;
	Mtx.r[1] = vUp;
	Mtx.r[2] = vLook;

	if (vRotate->x != 0) {
		Mtx *= XMMatrixRotationAxis(vRight ,vRotate->x);
		vRotate->x += (_float)Timedelta * Speed;
	}
	if (vRotate->y != 0) {
		Mtx *= XMMatrixRotationAxis(vUp, vRotate->y);
		vRotate->y += (_float)Timedelta * Speed;
	}
	if (vRotate->z != 0) {
		Mtx *= XMMatrixRotationAxis(vLook, vRotate->z);
		vRotate->z += (_float)Timedelta * Speed;
	}
	return Mtx.r[2];
}



CVIBuffer_Rect_Instance * CVIBuffer_Rect_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iNumInstance)
{
	CVIBuffer_Rect_Instance*	pInstance = new CVIBuffer_Rect_Instance(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(iNumInstance)))
	{
		MSG_BOX(TEXT("Failed to Created CVIBuffer_Rect_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Rect_Instance::Clone(void * pArg)
{
	CVIBuffer_Rect_Instance*	pInstance = new CVIBuffer_Rect_Instance(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CVIBuffer_Rect_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect_Instance::Free()
{
	__super::Free();
	
	Safe_Release(m_pVBInstance);

}
