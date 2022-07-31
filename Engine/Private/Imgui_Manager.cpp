
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "..\Public\Imgui_Manager.h"
#include "Component_Manager.h"
#include "Input_Device.h"
#include "Object_Manager.h"
#include "GameObject.h"
#include "Navigation.h"
#include "Cell.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

IMPLEMENT_SINGLETON(CImgui_Manager)
CImgui_Manager::CImgui_Manager()
{
}

HRESULT CImgui_Manager::InitImGui(ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut)
{
	ImGui_ImplDX11_Init(*ppDeviceOut, *ppDeviceContextOut);

	m_pDevice = *ppDeviceOut;
	m_DeviceContext = *ppDeviceContextOut;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_DeviceContext);

	return S_OK;
}

HRESULT CImgui_Manager::StartFrame(void)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	MapMenu_Contents();
	NaviMenu_Contents();

	return S_OK;
}

HRESULT CImgui_Manager::Set_Contents(void)
{

	static bool show_demo_window = false;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("MapTool Ver.0.0");                        // Create a window called "Hello, world!" and append into it.

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();
	}

	// 3. 맵툴
	return S_OK;
}

void CImgui_Manager::MapMenu_Contents()
{
	static bool show_another_window = true;
	//vector<const _tchar*> tcharPrototypelist = CComponent_Manager::GetInstance()->Get_ComponentTag(3);
	//vector<const char*> PrototypeList;
	static int selectCnt = -1;

	ImGuiStyle* style = &ImGui::GetStyle();
	static ImGuiTextFilter filter;

	if (show_another_window)
	{
		ImGui::Begin("Map Object", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		filter.Draw(" ", 200);

		if (ImGui::TreeNode("Object")) {
			//ImGui::PushItemWidth();
			
			if (ImGui::ListBox(".", &selectCnt, PrototypeList, IM_ARRAYSIZE(PrototypeList), 10)) {
				m_Navimode = false;
			}
			ImGui::TreePop();

			if (ImGui::Button("Save", ImVec2(50, 20)))
				SaveData();

			ImGui::SameLine(0.f, 10.f);

			if (ImGui::Button("Load", ImVec2(50, 20))) {
				LoadData();
				selectCnt = 0;
			}
		}


		if (m_Navimode == false) {
			Create_Object(selectCnt);
			Remote_PickObj();
		}

		ImGui::End();
	}
}

void CImgui_Manager::Create_Object(_int index)
{
	m_NextPick = false;
	if (index < 0)
		return;

	if (GetAsyncKeyState(VK_LBUTTON) & 0x0001) {
		m_NextPick = ObjectPicking();
		if (m_NextPick == false)
			TilePicking(index);
	}
}

_bool CImgui_Manager::ObjectPicking()
{
	_bool flag = false;

	for (auto& iter : m_ObjectList) {
		CCollider* CollideSphere = (CCollider*)iter->Object->Get_Component(L"Com_Sphere");
		if (CollideSphere == nullptr)
			continue;

		if (XMVectorGetX(XMVector3Length(CollideSphere->Intersect_Ray()))) {
			CollideSphere->Set_isCollison(true);
			CollideSphere->Set_Color(_float3(1.f, 0.f, 0.f));
			m_PickObj = iter->Object;

			flag = true;
		}
		else {
			CollideSphere->Set_isCollison(false);
			CollideSphere->Set_Color(_float3(0.f, 1.f, 0.f));
		}
	}

	return flag;
}

_bool CImgui_Manager::TilePicking(_int index)
{
	_bool flag = false;

	CCollider* Collide = (CCollider*)CObject_Manager::GetInstance()->Get_Component(3, L"Layer_BackGround", L"Com_AABB");
	if (Collide == nullptr)
		return false;

	_vector PickedPos = Collide->Intersect_Ray();

	if (XMVectorGetX(XMVector3Length(PickedPos)) > 0) {
		flag = true;
		//char strtemp[MAX_PATH] = "";
		//strcpy_s(strtemp, PrototypeList[0]);
		_tchar		szFullPath[MAX_PATH] = TEXT("");
		MultiByteToWideChar(CP_ACP, 0, PrototypeList[index], (int)strlen(PrototypeList[index]), szFullPath, MAX_PATH);
		CGameObject* Obj = nullptr;
		if ((Obj = CObject_Manager::GetInstance()->Add_GameObjToLayer(3, L"ObjectList", szFullPath, &PickedPos)) != nullptr) {
			CTransform* ObjTrans = Obj->Get_Transform();
			PickedPos = XMVectorSetW(PickedPos, 1.f);
			ObjTrans->Set_State(CTransform::STATE_POSITION, PickedPos);
			DATADESC* Datadesc = new DATADESC;
			Datadesc->Object = Obj;
			Datadesc->TagIndex = index;
			m_ObjectList.push_back(Datadesc);
		}
	}
	return flag;
}

void CImgui_Manager::Remote_PickObj()
{
	if (m_PickObj == nullptr)
		return;
	CTransform* PickTrans = m_PickObj->Get_Transform();

	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_W))
		PickTrans->Go_Screen_Up(0.01f);

	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_S))
		PickTrans->Go_Screen_Down(0.01f);

	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_A))
		PickTrans->Go_Screen_Left(0.01f);

	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_D))
		PickTrans->Go_Screen_Right(0.01f);

	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_Q))
		PickTrans->Turn(XMVectorSet(0.f, -1.f, 0.f, 0.f), 0.01f);

	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_E))
		PickTrans->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.01f);

	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_Z))
		PickTrans->Scailing(0.005f);

	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_X))
		PickTrans->Scailing(-0.005f);

	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_DELETE)) {
		CGameObject* Temp = m_PickObj;
		m_PickObj = nullptr;

		int i = 0;
		for (auto&iter : m_ObjectList) {
			if (iter->Object == Temp) {
				m_ObjectList.erase(m_ObjectList.begin() + i);
				break;
			}
			else {
				i++;
			}
		}

		Temp->Set_Dead();
	}
}

void CImgui_Manager::NaviMenu_Contents()
{
	static bool show_navi_window = true;

	static int selectCnt = -1;


	ImGuiStyle* style = &ImGui::GetStyle();
	static ImGuiTextFilter filter;

	if (show_navi_window)
	{
		ImGui::Begin("Navi", &show_navi_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Checkbox("isNaviMode", &m_Navimode);

		ImGui::Combo(" ", &m_CellType, Cell_TypeIndex, IM_ARRAYSIZE(Cell_TypeIndex));

		if (m_Navimode == true) {
			if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)
				Navi_Create();

			if (GetAsyncKeyState(VK_END) & 0x0001)
				Push_Cell();

			if (GetAsyncKeyState(VK_DELETE) & 0x0001)
				Undo_NaviBox();

			Remote_Navi();
		}

		if (ImGui::Button("Save", ImVec2(50, 20)))
			SaveNavi();

		ImGui::SameLine(0.f, 10.f);

		if (ImGui::Button("Load", ImVec2(50, 20))) {
			LoadNavi();
		}

		ImGui::Text("NumPad 1, 2 : Height");
		ImGui::Text("NumPad 4, 5, 6, 8 : Moves Created Last NaviMesh");
		ImGui::Text("END : Create Cell");
		ImGui::Text("DELETE : Delete Last Cell");

		ImGui::End();
	}
}


_bool CImgui_Manager::Remote_Navi()
{
	if (m_PickNavi == nullptr)
		return false;

	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_NUMPAD8))
		m_PickNavi->Get_Transform()->Go_Screen_Up(0.003f);

	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_NUMPAD5))
		m_PickNavi->Get_Transform()->Go_Screen_Down(0.003f);

	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_NUMPAD4))
		m_PickNavi->Get_Transform()->Go_Screen_Left(0.003f);
	
	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_NUMPAD6))
		m_PickNavi->Get_Transform()->Go_Screen_Right(0.003f);

	return true;
}

_bool  CImgui_Manager::Navi_Create()
{
	_bool flag = false;
	_vector PickedPos = _vector();
	
	list<CGameObject*>* NaviList = CObject_Manager::GetInstance()->Get_GameObjectList(3, L"NaviFlag");

	if (NaviList != nullptr) {
		for (auto& iter : *NaviList) {
			CCollider* ObjCol = (CCollider*)iter->Get_Component(L"Com_Sphere");
			_vector PickObj = ObjCol->Intersect_Ray();
			//ObjCol->Set_isCollison(false);
			if (XMVectorGetX(XMVector3Length(PickObj)) > 0) {
				ObjCol->Set_isCollison(true);
				PickedPos = iter->Get_Transform()->Get_State(CTransform::STATE_POSITION);
				flag = true;
				break;
			}
		}
	}

	CCollider* Collide = (CCollider*)CObject_Manager::GetInstance()->Get_Component(3, L"Layer_BackGround", L"Com_AABB");
	if (Collide == nullptr)
		return false;

	if(flag == false)
		PickedPos = Collide->Intersect_Ray();

	if (XMVectorGetX(XMVector3Length(PickedPos)) > 0 && m_NaviCount < 3) {
		CGameObject* Obj = nullptr;
		if ((Obj = CObject_Manager::GetInstance()->Add_GameObjToLayer(3, L"NaviFlag", L"Prototype_GameObject_NaviFlag", &PickedPos)) != nullptr) {
			CCollider* ObjCollide = (CCollider*)Obj->Get_Component(L"Com_Sphere");

			CTransform* ObjTrans = Obj->Get_Transform();
			PickedPos = XMVectorSetW(PickedPos, 1.f);
			ObjTrans->Set_State(CTransform::STATE_POSITION, PickedPos);
			m_PickNavi = Obj;
			m_CreateNavi[m_NaviCount] = Obj;
			ObjCollide->Set_Color(_float3(rand() % 11 / 10.f, rand() % 11 / 10.f, rand() % 11 / 10.f));
			m_NaviCount++;
		}
	}
	return true;
}

_bool CImgui_Manager::Push_Cell()
{
	if (m_NaviCount < 3)
		return false;

	CTransform* CPos;
	_float3 fPos;

	CPos = m_CreateNavi[0]->Get_Transform();
	XMStoreFloat3(&fPos, CPos->Get_State(CTransform::STATE_POSITION));
	m_Point.PointA = fPos;

	CPos = m_CreateNavi[1]->Get_Transform();
	XMStoreFloat3(&fPos, CPos->Get_State(CTransform::STATE_POSITION));
	m_Point.PointB = fPos;

	CPos = m_CreateNavi[2]->Get_Transform();
	XMStoreFloat3(&fPos, CPos->Get_State(CTransform::STATE_POSITION));
	m_Point.PointC = fPos;
	m_Point.Index = m_CellType;

	m_Cell.push_back(m_Point);
	CNavigation::GetInstance()->Push_Cell(m_pDevice, m_DeviceContext, &m_Point);

	m_NaviCount = 0;
	m_PickNavi = nullptr;

	return true;
}

void CImgui_Manager::Undo_NaviBox()
{
	if (m_NaviCount != 0)
		return;

	CNavigation::GetInstance()->Remove_LastCell();
	m_Cell.pop_back();
}

_bool CImgui_Manager::SaveNavi()
{
	_ulong		dwByte = 0;

	HANDLE		hFile = CreateFile(TEXT("../DataNavi/DataNavi.dat"), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);

	int i = 0;
	_tchar SzPath[MAX_PATH] = L"";
	while (hFile == INVALID_HANDLE_VALUE) {
		wsprintf(SzPath, TEXT("../DataNavi/DataNavi%d.dat"), i);
		hFile = CreateFile(SzPath, GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
		i++;
	}

	if (0 == hFile) {
		MSGBOX("FAILDE OPEN FILE !!!");
		return false;
	}

	for (auto iter : m_Cell) {
		WriteFile(hFile, &iter, sizeof(CELLDESC), &dwByte, nullptr);
	}

	CloseHandle(hFile);

	MSGBOX("SUCCESS !!!");
	return true;
}


_bool CImgui_Manager::LoadNavi()
{
	//_ulong			dwByte = 0;
	//
	//HANDLE			hFile = CreateFile(TEXT("../DataNavi/DataNavi.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//
	//if (0 == hFile)
	//	return E_FAIL;
	//
	//while (true)
	//{
	//	//네비게이션 이니셜라이즈로 로드먼저
	//
	//	CELLDESC vPoint;
	//
	//	ReadFile(hFile, &vPoint, sizeof(CELLDESC), &dwByte, nullptr);
	//	if (0 == dwByte)
	//		break;
	//
	//	_float3 fPos[3];
	//	memcpy(fPos, &vPoint, sizeof(_float3) * 3);
	//
	//	m_Cell.push_back(vPoint);
	//	for (int i = 0; i < 3; i++) {
	//		CGameObject* NaviMesh = CObject_Manager::GetInstance()->Add_GameObjToLayer(3, L"NaviFlag", L"Prototype_GameObject_NaviFlag");
	//		if (NaviMesh == nullptr) {
	//			MSGBOX("FAILED LOAD OBJECT !!!");
	//			return S_OK;
	//		}
	//
	//		NaviMesh->Get_Transform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPos[i].x, fPos[i].y, fPos[i].z, 1.f));
	//	}
	//}
	//
	//CloseHandle(hFile);
	CTransform* BaseTile = CObject_Manager::GetInstance()->Get_Transform(3, L"Layer_BackGround");
	if (BaseTile == nullptr) {
		MSGBOX("FAILED BaseTile Transform")
		return E_FAIL;
	}

	CNavigation::GetInstance()->Initialize(m_pDevice, m_DeviceContext, TEXT("../DataNavi/DataNavi.dat"), BaseTile);

	vector<CCell*> NaviCell = CNavigation::GetInstance()->Get_Cells();
	CELLDESC celldesc;

	for (auto& iter : NaviCell) {
		for (int j = 0; j < 3; j++) {
			CGameObject* NaviMesh = CObject_Manager::GetInstance()->Add_GameObjToLayer(3, L"NaviFlag", L"Prototype_GameObject_NaviFlag");
			if (NaviMesh == nullptr) {
				MSGBOX("LOAD FAILED !!!!");
				return S_OK;
			}
			NaviMesh->Get_Transform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(iter->Get_fPoint(j).x, iter->Get_fPoint(j).y, iter->Get_fPoint(j).z, 1.f));
		}

		celldesc.PointA = iter->Get_fPoint(0);
		celldesc.PointB = iter->Get_fPoint(1);
		celldesc.PointC = iter->Get_fPoint(2);
		celldesc.Index = iter->Get_TypeIndex();
		m_Cell.push_back(celldesc);
	}

	MSGBOX("GOOD !");
	return S_OK;
}

_bool CImgui_Manager::SaveData()
{
	_ulong		dwByte = 0;
	//CREATE_ALWAYS
	HANDLE		hFile = CreateFile(TEXT("../Data/MapData.dat"), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);

	int i = 0;
	_tchar SzPath[MAX_PATH] = L"";
	while(hFile == INVALID_HANDLE_VALUE) {
		wsprintf(SzPath, TEXT("../Data/MapData%d.dat"), i);
		hFile = CreateFile(SzPath, GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
		i++;
	}

	if (0 == hFile) {
		MSGBOX("FAILDE OPEN FILE !!!");
		return false;
	}
	


	//클래스 태그, 월드매트릭스

	for (auto iter : m_ObjectList) {
		SAVEDESC SaveDesc;

		SaveDesc.TagIndex = iter->TagIndex;
		SaveDesc.WorldMtx = iter->Object->Get_Transform()->Get_WorldFloat4x4();

		WriteFile(hFile, &(SaveDesc), sizeof(SAVEDESC), &dwByte, nullptr);
	}

	CloseHandle(hFile);

	MSGBOX("SUCCESS !!");

	return true;
}

_bool CImgui_Manager::LoadData()
{
	_ulong			dwByte = 0;

	HANDLE			hFile = CreateFile(TEXT("../Data/MapData.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile) {
		MSGBOX("FAILED !!!");
		return false;
	}

	while (true)
	{
		SAVEDESC SaveData;

		ReadFile(hFile, &SaveData, sizeof(SAVEDESC), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		_tchar		szFullPath[MAX_PATH] = TEXT("");
		MultiByteToWideChar(CP_ACP, 0, PrototypeList[SaveData.TagIndex], (int)strlen(PrototypeList[SaveData.TagIndex]), szFullPath, MAX_PATH);

		CGameObject* Obj = CObject_Manager::GetInstance()->Add_GameObjToLayer(3, L"Object", szFullPath);

		DATADESC* Data = new DATADESC;
		Data->TagIndex = SaveData.TagIndex;
		Data->Object = Obj;

		Obj->Get_Transform()->Set_WorldMTX(SaveData.WorldMtx);

		m_ObjectList.push_back(Data);
	}

	CloseHandle(hFile);

	MSGBOX("SUCCESS !!");

	return true;
}

HRESULT CImgui_Manager::Render()
{
	// Imgui render옵션이 켜져 있다면
	if (m_bImguiEnable) {
		StartFrame();
		Set_Contents();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	return S_OK;
}

void CImgui_Manager::InitWin32(HWND hWnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hWnd);
}

LRESULT CImgui_Manager::WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
}

void CImgui_Manager::Shutdown(void)
{
}

void CImgui_Manager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

