#include "stdafx.h"
#include "..\Public\MainApp.h"
#include "GameInstance.h"
#include "KeyMgr.h"

#include "BackGround.h"
#include "LoadingLogo.h"
#include "LoadingBar.h"

#include "Level_Logo.h"
#include "Level_Loading.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);


	// D3D11_DEPTH_STENCIL_DESC
	// D3D11_BLEND_DESC



	/*D3D11_RASTERIZER_DESC
	D3D11_DEPTH_STENCIL_DESC
	D3D11_BLEND_DESC*/

	//m_pDeviceContext->OMSetBlendState(0xffffffff);


	/*
	m_pDeviceContext->OMSetDepthStencilState();
	m_pDeviceContext->RasterizerSTate();*/
}

HRESULT CMainApp::NativeConstruct()
{
	/*D3D11_SAMPLER_DESC*/

	CGraphic_Device::GRAPHICDESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(CGraphic_Device::GRAPHICDESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iWinCX = g_iWinCX;
	GraphicDesc.iWinCY = g_iWinCY;
	GraphicDesc.eWinMode = CGraphic_Device::MODE_WIN;

	/* ��ġ�ʱ�ȭ ��. ������ ����� Ȱ���ϱ����� ���� �ʱ�ȭ �۾��� ��ģ��. */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pDeviceContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(_double TimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Tick_Engine(TimeDelta);
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Render_Begin(_float4(0.3f, 0.3f, 0.3f, 1.f));

	if (FAILED(m_pRenderer->Render_GameObjects()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Engine()))
		return E_FAIL;

	m_pGameInstance->RenderImgui();
	m_pGameInstance->Render_End();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelIndex)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	CLevel*			pLevel = nullptr;

	switch (eLevelIndex)
	{
	case LEVEL_LOGO:
		pLevel = CLevel_Logo::Create(m_pDevice, m_pDeviceContext);
		break;

	default:
		pLevel = CLevel_Loading::Create(m_pDevice, m_pDeviceContext, (LEVEL)eLevelIndex);
		break;
	}

	if (FAILED(m_pGameInstance->Open_Level(eLevelIndex, pLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	/* For.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LoadingLogo"),
		CLoadingLogo::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LoadingBar"),
		CLoadingBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	///* For.Prototype_GameObject_Monster */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"), CMonster::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), m_pRenderer = CRenderer::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer);

	/* For.Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxAnim*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnim.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNonAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxNonAnim.hlsl"), VTXNONANIMMODEL_DECLARATION::Elements, VTXNONANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE_DECLARATION::Elements, VTXCUBE_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance.hlsl"), VTXINSTANCE_DECLARATION::Elements, VTXINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_RectInstance"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* For. Prototype_Component_VIBuffer_PointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_PointInstance"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;



	/* For.Prototype_Component_Texture_Default */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MainMenu"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Menu/Menu.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LoadingLogo"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Resources/Textures/Loading/Logo(%d).dds"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_OBB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_SPHERE*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	return S_OK;
}



CMainApp * Client::CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX(TEXT("Failed to Created CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::CMainApp::Free()
{
	CKeyMgr::Get_Instance()->Destroy_Instance();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}

