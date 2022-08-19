#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"

#include "Client_Object.h"

CLoader::CLoader(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: m_pDevice(pDeviceOut), m_pDeviceContext(pDeviceContextOut)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

unsigned int APIENTRY ThreadEntryFunc(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	switch (pLoader->Get_LevelIndex())
	{
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlayLevel();
		break;

	}


	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::NativeConstruct(LEVEL eLevel)
{
	m_eLevel = eLevel;
	
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntryFunc, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoading, TEXT("텍스쳐를 로드중입니다. "));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Particle/Particle(%d).dds"), 15))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Terrain/T(%d).png"), 5))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_TerrainMask */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TerrainMask"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Filter.bmp"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Brush */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	_matrix			PivotMatrix;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(0.01f));


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Bed"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/Bed/", "Bed.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Resources/Model/ForkLift/", "ForkLift.fbx", PivotMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("모델을 로드중입니다. "));

	/* For. Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;
	
	lstrcpy(m_szLoading, TEXT("게임오브젝트를 로드중입니다. "));
	/* For.Prototype_GameObject_ForkLift */
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rect_Effect"),
		CRect_Effect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Point_Effect */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Point_Effect"),
		CPoint_Effect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Default */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_First"),
		CCamera_First::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Default"),
		CCamera_Default::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bed"),
		CBed::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("로딩이 완료되었습니다.. "));
	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut, LEVEL eLevel)
{
	CLoader*	pInstance = new CLoader(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct(eLevel)))
	{
		MSG_BOX(TEXT("Failed to Created CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	
	DeleteCriticalSection(&m_CriticalSection);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
