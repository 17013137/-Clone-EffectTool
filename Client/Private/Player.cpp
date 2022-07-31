#include "stdafx.h"
#include "GameInstance.h"
#include "KeyMgr.h"

CPlayer::CPlayer(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	m_Manager = CPlayer_Manager::Get_Instance();
	if (m_Manager == nullptr)
		return E_FAIL;

	m_pModelCom->Set_AnimationIndex(m_Anim_State, 1.0, 1.0);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 2.5f, 0.f, 1.f));
	return S_OK;
}

_int CPlayer::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	Mouse_Move(TimeDelta);
	Key_Input(TimeDelta);
	State_Tick(TimeDelta);
	Animation_Tick(TimeDelta);
	m_pModelCom->Update(TimeDelta);
	m_pAABBCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	return 0;
}



void CPlayer::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	m_PrevSpeed = m_Speed;
	m_PrevPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);

	
}

HRESULT CPlayer::Render()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshContainers = m_pModelCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material_OnShader(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, "g_BoneMatrices", i, 0)))
			return E_FAIL;
	}

#ifdef _DEBUG
	//m_pAABBCom->Render();
	//m_pSphereCom->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CPlayer::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CStatus::STATUSDESC temp;
	temp.fMaxHp = 100.f;
	temp.fMaxShield = 50.f;
	temp.fSpeed = 2.f;
	m_Speed = temp.fSpeed;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Status"), LEVEL_STATIC, TEXT("Prototype_Component_Status"), (CComponent**)&m_pStatus, &temp)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	/* For.Com_AABB */
	//ColliderDesc.vPosition = _float3(0.f, 0.7f, 0.f);
	//ColliderDesc.vSize = _float3(0.7f, 1.4f, 0.7f);
	if (FAILED(__super::SetUp_Components(TEXT("Com_AABB"), m_iLevelIndex, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.vPosition = _float3(0.f, 0.0f, 0.f);
	ColliderDesc.fRadius = 0.2f;
	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), m_iLevelIndex, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayer::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;	
	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CPlayer::State_Tick(_double TimeDelta)
{
	switch (m_State)
	{
	case Client::CPlayer::STATE_JUMP:
		if (FAILED(m_pTransformCom->Go_Jumping(m_PrevPos, m_PrevSpeed, TimeDelta))) {
			m_State = STATE_IDLE;
			m_isJump = false;
		}
		break;
	case Client::CPlayer::STATE_SLIDE:
		break;
	case Client::CPlayer::STATE_ATTACK:
		break;
	case Client::CPlayer::STATE_REROAD:
		break;
	case Client::CPlayer::STATE_DAMAGED:
		break;
	case Client::CPlayer::STATE_CC:
		break;
	case Client::CPlayer::STATE_DIE:
		m_State = STATE_DIE;
		break;
	default:
		break;
	}

	m_pStatus->Status_Tick(TimeDelta);
}

void CPlayer::Animation_Tick(_double TimeDelta)
{
	Update_WeaponState();

	if (m_isJump== true) {
		m_Anim_State = ANIM_RUN_FALL;
	}

	if (m_isSit == true) {
		m_Anim_State = ANIM_SPRINT_FALL;
	}

	if (m_isSecondAttack == true)
		m_Anim_State = ANIM_SECONDARYSHOOT;
	
	if(m_isDie == true)
		m_Anim_State = ANIM_DEATH;

	m_iAnimIndex = (_uint)m_Anim_Type * (_uint)ANIMATION_STATE::ANIM_END + (_uint)m_Anim_State;

	if (m_isAttack == true) {			// 공격상태일때 애니메이션 셋
		if(m_WeaponType == TYPE_BOW)
			m_iAnimIndex = TYPEWEAPON_END * (_uint)ANIM_END + (_uint)ANIM_SKILL_END + (_uint)m_Weapon;
		else
			m_iAnimIndex = TYPEWEAPON_END * (_uint)ANIM_END + (_uint)ANIM_SKILL_END + (_uint)m_Weapon + (_uint)m_Rifle_State;

		if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex) == true) {
			if(m_WeaponType == TYPE_RIFLE)
				m_pModelCom->Set_FinishFalse(m_iAnimIndex);   // 활은 처음 공격 시작할 때 리셋시켜버리는중

			m_Manager->Set_PlayerState(CPlayer_Manager::PLAYER_IDLE);
			m_isAttack = false;
			CPlayer_Manager::Get_Instance()->isAttack = false;
		}
		else {
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, m_Manager->Get_WeaponDesc().fAttackSpeed * 2.0 , 1.0, false);
			m_Manager->Set_PlayerState(CPlayer_Manager::PLAYER_ATTACK);
		}
		m_pTransformCom->Aim_ReBounding(m_Manager->Get_WeaponDesc().fRebound * 0.1, &m_isAttack, TimeDelta / m_Manager->Get_WeaponDesc().fReboundTime);
	}
	else if (m_isReroad == true) {				// 장전
		m_iAnimIndex = (_uint)TYPEWEAPON_END * (_uint)ANIM_END + (_uint)ANIM_SKILL_END + (_uint)m_Weapon + (_uint)m_Rifle_State;

		if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex) == true) {
			m_pModelCom->Set_FinishFalse(m_iAnimIndex);
			m_Manager->Set_PlayerState(CPlayer_Manager::PLAYER_IDLE);
			m_isReroad = false;
			CPlayer_Manager::Get_Instance()->isReroad = false; //
		}
		else {
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, m_Manager->Get_WeaponDesc().fReroadSpeed * 1.5, 1.0, false);
			m_Manager->Set_PlayerState(CPlayer_Manager::PLAYER_REROAD);
		}
	}
	else if (m_isSkill == true) {
		m_iAnimIndex = (_uint)TYPEWEAPON_END * (_uint)ANIM_END + (_uint)ANIM_PUCNH;		//현재는 펀치고정;
		if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex) == true) {
			m_pModelCom->Set_FinishFalse(m_iAnimIndex);
			m_isSkill = false;
		}
		else
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.8, 1.0);

		m_pTransformCom->Skill_Anim(TimeDelta, &m_isSkill);
	}
	else if (m_isSwap == true) {					// 무기 스왑
		m_iAnimIndex = (_uint)ANIM_SWITCH_IN;
		if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex) == true)
			m_isSwap = false;
		else {

			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 0.5, 1.0, false);
			//일정 프레임일때 m_Manager->Set_WeaponType(m_WeaponType);
			if (m_pModelCom->Get_isLinear() == false) {
				m_Manager->Set_WeaponType(m_WeaponType);
			}
			
		}
	}
	else {  // 공격상태가 아닐떄 애니메이션 셋
		switch (m_Anim_State)
		{
		case Client::CPlayer::ANIM_DEATH:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex) == true) {
				m_pModelCom->Set_FinishFalse(m_iAnimIndex);
				m_isDie = false;
			}
			else
				m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0, false);
		case Client::CPlayer::ANIM_SECONDARYSHOOT:
			if (m_pModelCom->Get_isAnimEnd(m_iAnimIndex) == true) {
				m_pModelCom->Set_FinishFalse(m_iAnimIndex);
				m_isSecondAttack = false;
			}
			else
				m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0, false);
			break;
		case Client::CPlayer::ANIM_STAB:
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
			break;
		case Client::CPlayer::ANIM_IDLE:
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 0.3);
			break;
		case Client::CPlayer::ANIM_RUN_FALL:
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
			break;
		case Client::CPlayer::ANIM_RUN_LOOP:
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
			break;
		case Client::CPlayer::ANIM_SPRINT_FALL:   //앉기
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0, false);
			break;
		case Client::CPlayer::ANIM_SPRINT_LOOP:
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
			break;
		case Client::CPlayer::ANIM_RUN:
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
			break;
		case Client::CPlayer::ANIM_SPRINT:
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
			break;
		case Client::CPlayer::ANIM_SWITCH_IN:
			m_pModelCom->Set_AnimationIndex(m_iAnimIndex, 1.0, 1.0);
			break;
		default:
			break;
		}
	}

}

void CPlayer::Update_WeaponState()
{
	

	m_Anim_Type = m_Manager->Get_PoseType();
	m_Weapon = m_Manager->Get_NowWeapon();
}

void CPlayer::Setting_Bullet()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CBullet::BulletDesc BulletDesc;
	BulletDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	BulletDesc.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	BulletDesc.Damage = m_Manager->Get_WeaponDesc().fWeaponDamage;
	BulletDesc.CriticalPer = m_Manager->Get_WeaponDesc().fCritical_Rate;

	switch (m_WeaponType)
	{
	case Client::TYPE_RIFLE:
		pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Bullet", TEXT("Prototype_GameObject_RifleBullet"), &BulletDesc);
		break;
	case Client::TYPE_SHOTGUN:
		for (_uint i = 0; i < m_Manager->Get_BulletCnt() ; i++)
			pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Bullet", TEXT("Prototype_GameObject_ShotgunBullet"), &BulletDesc);
		break;
	case Client::TYPE_BOW:
		if (m_Weapon == BOW2) {
			pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"RocketBullet", TEXT("Prototype_GameObject_RocketBullet"), &BulletDesc);
		}
		else
			pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Bullet", TEXT("Prototype_GameObject_BowBullet"), &BulletDesc);
		break;
	case Client::TYPEWEAPON_END:
		break;
	default:
		break;
	}

	
	Safe_Release(pGameInstance);
}

void CPlayer::Key_Input(double TimeDelta)
{
	_bool flag = false;

	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_F2)) { // 다이 모션 테스트
		flag = true;
		m_isDie = true;
		m_State = STATE_DIE;
	}

	if (m_isDie == true) {
		return;
	}

	_float tSpeed = m_Speed;
	CGameInstance* gameins = CGameInstance::GetInstance();
	Safe_AddRef(gameins);
	 
	if (CKeyMgr::Get_Instance()->Key_Down(VK_SPACE)) {			//점프
		tSpeed += 0.1f;
		m_isJump = true;
		m_State = STATE_JUMP;
		m_OriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		flag = true;
		return;
	}
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_CONTROL)&&  m_isJump == false) {		//앉기
		tSpeed /= 2.f;
		m_pTransformCom->Go_SitDown(TimeDelta);
		m_isSit = true;
	}
	else if (m_isSit == true) {										//앉기->일어나기
		if (FAILED(m_pTransformCom->Go_SitUp(TimeDelta))) {
			m_isSit = false;
		}
	}

	if (CKeyMgr::Get_Instance()->Key_Pressing('W') || CKeyMgr::Get_Instance()->Key_Pressing('A') || CKeyMgr::Get_Instance()->Key_Pressing('S') || CKeyMgr::Get_Instance()->Key_Pressing('D') && flag == false) {
		flag = true;
		m_Anim_State = ANIM_RUN;
		if (CKeyMgr::Get_Instance()->Key_Pressing('W')) {
			if (CKeyMgr::Get_Instance()->Key_Pressing(VK_SHIFT)) {
				m_Anim_State = ANIM_SPRINT;
				tSpeed *= 2.0f;
			}
			m_pTransformCom->Go_MyStraight(TimeDelta * tSpeed);
		}
		if (CKeyMgr::Get_Instance()->Key_Pressing('S')) {
			m_pTransformCom->Go_MyBackWard(TimeDelta* tSpeed); 
		}
		if (CKeyMgr::Get_Instance()->Key_Pressing('A')) {
			m_pTransformCom->Go_MyLeft(TimeDelta* tSpeed);
		}
		if (CKeyMgr::Get_Instance()->Key_Pressing('D')) {
			m_pTransformCom->Go_MyRight(TimeDelta* tSpeed);
		}
	}

	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LBUTTON) && m_isAttack == false  && m_isSecondAttack == false && m_isSkill == false && m_isReroad == false && m_isSwap == false) {
		flag = true;
		m_isAttack = true;
		m_Anim_State = ANIM_IDLE;
		m_Rifle_State = RIF_SHOOT;
		if (m_WeaponType == TYPE_BOW)
			m_pModelCom->Set_FinishFalse(TYPEWEAPON_END * (_uint)ANIM_END + (_uint)ANIM_SKILL_END + (_uint)m_Weapon);
		if (m_WeaponType == TYPE_SHOTGUN)
			m_pModelCom->Set_FinishFalse(TYPEWEAPON_END * (_uint)ANIM_END + (_uint)ANIM_SKILL_END + (_uint)m_Weapon + 1);
		
		Setting_Bullet();
		CPlayer_Manager::Get_Instance()->isAttack = true;
	}

	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_RBUTTON) && m_isAttack == false && m_isSecondAttack == false && m_isSkill == false && m_isReroad == false && m_isSwap == false) {
		flag = true;
		m_isSecondAttack = true;
		m_Anim_State = ANIM_SECONDARYSHOOT;
	}
	

	if (CKeyMgr::Get_Instance()->Key_Down('R')  && m_isReroad == false && m_isSecondAttack == false && m_isAttack == false && m_isSkill == false && m_isSwap == false) { //장전
		flag = true;
		m_isReroad = true;
		m_Rifle_State = RIF_REROAD;
		CPlayer_Manager::Get_Instance()->isReroad = true;
		
	}

	if (CKeyMgr::Get_Instance()->Key_Pressing('V') && m_isSkill == false && m_isSwap == false) { //근접스킬
		flag = true;
		m_isAttack = false;
		m_isSkill = true;
	}


	if (CKeyMgr::Get_Instance()->Key_Pressing('1')  && m_isSecondAttack == false && m_isAttack == false && m_isSkill == false) { //근접스킬
		flag = true;
		m_isSwap = true;
		m_WeaponType = TYPE_RIFLE;
		//m_Manager->Set_WeaponType(m_WeaponType);
		m_pModelCom->Set_FinishFalse(ANIM_SWITCH_IN);
	}
	else if (CKeyMgr::Get_Instance()->Key_Pressing('2') && m_isSecondAttack == false && m_isAttack == false && m_isSkill == false) {
		flag = true;
		m_isSwap = true;
		m_WeaponType = TYPE_SHOTGUN;
		//m_Manager->Set_WeaponType(m_WeaponType);
		m_pModelCom->Set_FinishFalse(ANIM_SWITCH_IN);
	}
	else if (CKeyMgr::Get_Instance()->Key_Pressing('3') && m_isSecondAttack == false && m_isAttack == false && m_isSkill == false) {
		flag = true;
		m_isSwap = true;
		m_WeaponType = TYPE_BOW;
		//m_Manager->Set_WeaponType(m_WeaponType);
		m_pModelCom->Set_FinishFalse(ANIM_SWITCH_IN);
	}

	//else if (CKeyMgr::Get_Instance()->Key_Pressing('4') && m_isSecondAttack == false && m_isAttack == false && m_isSkill == false) {
	//	flag = true;
	//	m_isSwap = true;
	//	m_Manager->Set_Weapon(ROCKET_LUNCHER);
	//}

	if (flag == false && m_isJump == false) {
		m_State = STATE_IDLE;
		m_Anim_State = ANIM_IDLE;
	}

	Safe_Release(gameins);
}

void CPlayer::Mouse_Move(_double TimeDelta)
{
	CGameInstance* gameins = CGameInstance::GetInstance();
	Safe_AddRef(gameins);

	_long		MouseMove;

	if (MouseMove = gameins->Get_DIMMoveState(CInput_Device::DIMM_X))
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * 0.1f);

	if (MouseMove = gameins->Get_DIMMoveState(CInput_Device::DIMM_Y))
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove* 0.1f);

	Safe_Release(gameins);
}


CPlayer * CPlayer::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CPlayer*	pInstance = new CPlayer(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();	
	
	m_Manager->Destroy_Instance();
	Safe_Release(m_pAABBCom);
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pStatus);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);

}
