// DeviceManageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "KTST Alarm.h"
#include "DeviceManageDlg.h"
#include "afxdialogex.h"

#include "DeviceInfoDlg.h"

// CDeviceManageDlg 对话框

IMPLEMENT_DYNAMIC(CDeviceManageDlg, CDialogEx)

CDeviceManageDlg::CDeviceManageDlg(CDeviceIO *pDeviceIO, CWnd* pParent /*=NULL*/)
	: CDialogEx(CDeviceManageDlg::IDD, pParent)
	, m_pDeviceIO (pDeviceIO)
{
	if (m_pDeviceIO)
	{
		TCHAR szGUID[BUF_LEN_M];
		memset (szGUID, 0x0, sizeof(szGUID));
		m_pDeviceIO->GetLcdeGid(szGUID);
		m_strLocalGUID = szGUID;
	}

	m_bShowAllDevice = FALSE;
}

CDeviceManageDlg::~CDeviceManageDlg()
{
	this->ClearMemory ();
}

void CDeviceManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEVICE_LIST, m_ctrlDeviceList);
}


BEGIN_MESSAGE_MAP(CDeviceManageDlg, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_DEVICE_LIST, &CDeviceManageDlg::OnItemchangedDeviceList)
	ON_BN_CLICKED(IDC_ADD_DEVICE, &CDeviceManageDlg::OnBnClickedAddDevice)
	ON_BN_CLICKED(IDC_MODIFY_DEVICE, &CDeviceManageDlg::OnBnClickedModifyDevice)
	ON_BN_CLICKED(IDC_DELETE_DEVICE, &CDeviceManageDlg::OnBnClickedDeleteDevice)
	ON_BN_CLICKED(IDC_EXTENDED_DEF, &CDeviceManageDlg::OnBnClickedExtendedDef)
	ON_BN_CLICKED(IDOK, &CDeviceManageDlg::OnBnClickedOk)
	ON_NOTIFY(NM_RCLICK, IDC_DEVICE_LIST, &CDeviceManageDlg::OnRclickDeviceList)
	ON_COMMAND(ID_SHOW_ALL_DEVICE, OnShowAllDevice)
	ON_COMMAND(ID_SHOW_LOCAL_DEVICE, OnShowLocalDevice)
	ON_BN_CLICKED(IDCANCEL, &CDeviceManageDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDeviceManageDlg 消息处理程序

void CDeviceManageDlg::SetDevice (const DeviceInfo &device)
{
	m_listDevice.AddTail(new DeviceInfo(device));
}

CDeviceInfoList* CDeviceManageDlg::GetDeviceListPtr ()
{
	return &m_listDevice;
}

void CDeviceManageDlg::ClearMemory ()
{
	FreeObjPtrList(&m_listDevice);
}


BOOL CDeviceManageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ctrlDeviceList.SetExtendedStyle (m_ctrlDeviceList.GetExtendedStyle () | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//设置设备列表列头
	CString strLoad;
	int nIndex = 0;
	strLoad.LoadString (IDS_NAME);
	m_ctrlDeviceList.InsertColumn (nIndex, strLoad, LVCFMT_LEFT, 110);
	nIndex ++;
	strLoad.LoadString (IDS_NO);
	m_ctrlDeviceList.InsertColumn (nIndex, strLoad, LVCFMT_CENTER, 40);
	nIndex ++;
	strLoad.LoadString(IDS_DEVICE_TYPE);
	m_ctrlDeviceList.InsertColumn (nIndex, strLoad, LVCFMT_CENTER, 120);
	nIndex ++;
	strLoad.LoadString (IDS_DRIVER_TYPE);
	m_ctrlDeviceList.InsertColumn (nIndex, strLoad, LVCFMT_CENTER, 120);
	nIndex ++;
	strLoad.LoadString (IDS_CHANNEL_COUNT);
	m_ctrlDeviceList.InsertColumn (nIndex, strLoad, LVCFMT_CENTER, 50);
	nIndex ++;
// 	strLoad.LoadString (IDS_SAMPLE_INTERVAL);
// 	m_ctrlDeviceList.InsertColumn (nIndex, strLoad, LVCFMT_CENTER, 110);
// 	nIndex ++;


	DeviceInfo *pDevice = NULL;
	POSITION pos = m_listDevice.GetHeadPosition ();
	while (pos != NULL)
	{
		pDevice = (DeviceInfo *)m_listDevice.GetNext(pos);
		if (pDevice && m_strLocalGUID.Compare(pDevice->strDeviceGUID)==0)
		{
			//将本地设备的一些信息重新写回注册表
			this->ModifyDeviceInfoInRegister (pDevice);
		}
	}

	//删除注册表中的无效设备
	this->DeleteInvalidDeviceInRegister ();

	m_pDeviceIO->Initialize(SOFTWARE_NAME);	//DeviceIO重新初始化，让各种类型的动态库也重新初始化，因为注册表中修改了设备信息，删除了一些无效设备

	//初始化设备列表
	this->RefreshDeviceList ();

	GetDlgItem(IDC_MODIFY_DEVICE)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELETE_DEVICE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EXTENDED_DEF)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDeviceManageDlg::RefreshDeviceList ()
{
	m_ctrlDeviceList.DeleteAllItems();

	//初始化设备列表
	int nIndex = 0;
	DeviceInfo *pDevice = NULL;
	POSITION pos = m_listDevice.GetHeadPosition ();
	while (pos != NULL)
	{
		pDevice = (DeviceInfo *)m_listDevice.GetNext(pos);

		if (pDevice->nOptFlag == OPT_DELETE)
			continue;

		if (pDevice->nDeviceType != devTypeDriver && pDevice->nDeviceType != devTypeRelay)
			continue;

		if (m_bShowAllDevice		//显示全部设备
			|| m_strLocalGUID.Compare(pDevice->strDeviceGUID)==0)	//只显示本地设备
		{
			this->InsertItemToList(nIndex, pDevice);	//向设备列表中插入一条记录
			nIndex ++;
		}
	}
}

//修改注册表中设备信息
BOOL CDeviceManageDlg::ModifyDeviceInfoInRegister (DeviceInfo *pDevice)
{
	if (pDevice == NULL || m_pDeviceIO == NULL)
		return FALSE;

	int nRet = m_pDeviceIO->MoadDeRg (pDevice->nDriverType, pDevice->nDeviceNo, pDevice->nChannelCount, pDevice->strDeviceName);
	if (nRet != DEVICEIO_ERROR_SUCCESS)
		return FALSE;

	return TRUE;
}

//将设备信息添加到指定的列表索引中
void CDeviceManageDlg::InsertItemToList (int nItem, DeviceInfo *pDevice, BOOL bReplace/*=FALSE*/)
{
	if (pDevice == NULL)
		return;

	CString strText;
	TCHAR szDeviceTypeName[BUF_LEN_S];
	int nSubItem = 0;

	//设备名称
	if (bReplace)
		m_ctrlDeviceList.SetItemText (nItem, nSubItem, pDevice->strDeviceName);
	else
		m_ctrlDeviceList.InsertItem (nItem, pDevice->strDeviceName);
	nSubItem ++;

	//设备编号
	strText.Format (_T("%d"), pDevice->nDeviceNo);
	m_ctrlDeviceList.SetItemText (nItem, nSubItem, strText);
	nSubItem ++;

	//设备类型
	switch (pDevice->nDeviceType)
	{
	case devTypeProgrammableController:		//可编程控制器设备
		strText.LoadString (IDS_DEV_TYPE_CONTROLLER);
		break;
	case devTypeDriver:						//驱动设备
		strText.LoadString(IDS_DEV_TYPE_DRIVER);
		break;
	case devTypeProcessSample:				//过程量采集设备
		strText.LoadString(IDS_DEV_TYPE_PROCESS);
		break;
	case devTypeRelay:						//继电器设备
		strText.LoadString(IDS_DEV_TYPE_RELAY);
		break;
	default:
		strText = _T("--");
		break;
	}
	m_ctrlDeviceList.SetItemText (nItem, nSubItem, strText);
	nSubItem ++;

	//驱动类型
	memset(szDeviceTypeName, 0x0, sizeof(szDeviceTypeName));
	m_pDeviceIO->GetTypeDefaultInfo(pDevice->nDriverType, szDeviceTypeName);
	strText = szDeviceTypeName;
	if (strText.IsEmpty())
		strText.Format(_T("No Item"));
	m_ctrlDeviceList.SetItemText (nItem, nSubItem, strText);
	nSubItem ++;

	//通道数
	strText.Format (_T("%d"), pDevice->nChannelCount);
	m_ctrlDeviceList.SetItemText (nItem, nSubItem, strText);
	nSubItem ++;

// 	//采集时间间隔
// 	strText.Format (_T("%d"), pDevice->nSampleInterval);
// 	m_ctrlDeviceList.SetItemText (nItem, nSubItem, strText);
// 	nSubItem ++;

	m_ctrlDeviceList.SetItemData (nItem, (DWORD )pDevice);	//在列表中保存设备对应的内存链表指针
}

//删除注册表中的无效设备
BOOL CDeviceManageDlg::DeleteInvalidDeviceInRegister ()
{
	int nDeviceTypeArr[_MAX_DEVICE], nDeviceNoArr[_MAX_DEVICE], nDriverTypeCnt, nDeviceCnt, nInvalidDeviceCnt, i, j, nDriverType, nDeviceNo;

	nDriverTypeCnt = 0;
	nDeviceCnt = 0;
	nInvalidDeviceCnt = 0;
	m_pDeviceIO->GetCardTypeNum(nDriverTypeCnt);
	for (i=0; i<nDriverTypeCnt; i++)
	{
		nDeviceCnt = 0;
		m_pDeviceIO->GetCardTypeID(i, nDriverType);
		m_pDeviceIO->GetCardNum(nDriverType, nDeviceCnt);

		for (j=0; j<nDeviceCnt; j++)
		{
			m_pDeviceIO->GetCardID(nDriverType, j, nDeviceNo);

			if (!this->ExistInDeviceList (nDriverType, nDeviceNo, m_strLocalGUID))	//判断设备链表中是否有此设备
			{
				nDeviceTypeArr[nInvalidDeviceCnt] = nDriverType;	//临时保存设备链表中不存在的设备
				nDeviceNoArr[nInvalidDeviceCnt] = nDeviceNo;
				nInvalidDeviceCnt ++;
			}
		}
	}

	//删除设备链表中不存在的设备
	for (i=0; i<nInvalidDeviceCnt; i++)
	{
		if (m_pDeviceIO->DltDeRg (nDeviceTypeArr[i], nDeviceNoArr[i]) != DEVICEIO_ERROR_SUCCESS)
			return FALSE;
	}

	return TRUE;
}

//判断指定设备在内存链表中是否存在
BOOL CDeviceManageDlg::ExistInDeviceList (int nDriverType, int nDeviceNo, CString strGUID)
{
	DeviceInfo *pDevice = NULL;
	POSITION pos = m_listDevice.GetHeadPosition();
	while (pos != NULL)
	{
		pDevice = (DeviceInfo *)m_listDevice.GetAt(pos);

		if (nDriverType==pDevice->nDriverType && nDeviceNo==pDevice->nDeviceNo)
		{
			if (strGUID.Compare (pDevice->strDeviceGUID) == 0)
				return TRUE;
		}

		m_listDevice.GetNext(pos);
	}

	return FALSE;
}

void CDeviceManageDlg::OnItemchangedDeviceList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//判断项目改变为状态改变
	if ((pNMLV->uChanged & LVIF_STATE) == LVIF_STATE)
	{
		if (pNMLV->iItem < 0)
			return;

		DeviceInfo *pDevice = (DeviceInfo *)pNMLV->lParam;	//项目中保存着设备对应内存指针
		if (pDevice == NULL)
			return;

		if ((pNMLV->uOldState & LVIS_SELECTED) == LVIS_SELECTED)		//失去选中状态
		{
			GetDlgItem(IDC_MODIFY_DEVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_DELETE_DEVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_EXTENDED_DEF)->EnableWindow(FALSE);
		}
		else if ((pNMLV->uNewState & LVIS_SELECTED) == LVIS_SELECTED)	//得到选中状态
		{
			GetDlgItem(IDC_MODIFY_DEVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_DELETE_DEVICE)->EnableWindow(TRUE);

			if (m_strLocalGUID.Compare (pDevice->strDeviceGUID) == 0)		//本地设备
			{
				BOOL bNeed = FALSE;
				int nHandle;
				int nRet = m_pDeviceIO->GetCardHandle(pDevice->nDriverType, pDevice->nDeviceNo, nHandle);
				if (nRet != DEVICEIO_ERROR_SUCCESS)
					return;

				//判断此设备能否通道扩展定义
				nRet = m_pDeviceIO->IsChannelDefineNeeded(nHandle, bNeed);
				if (nRet != DEVICEIO_ERROR_SUCCESS)
					return;

				if (bNeed)
					GetDlgItem(IDC_EXTENDED_DEF)->EnableWindow(TRUE);
				else
					GetDlgItem(IDC_EXTENDED_DEF)->EnableWindow(FALSE);
			}
			else	//非本地设备
			{
				GetDlgItem(IDC_EXTENDED_DEF)->EnableWindow(FALSE);
			}
		}
	}

	*pResult = 0;
}

//添加设备
void CDeviceManageDlg::OnBnClickedAddDevice()
{
	int nDeviceCnt = 0;
	DeviceInfo *pDevice = NULL;
	POSITION pos = m_listDevice.GetHeadPosition ();
	while (pos != NULL)
	{
		pDevice = m_listDevice.GetNext (pos);
		if (pDevice->nOptFlag != OPT_DELETE)
			nDeviceCnt ++;
	}

	if (nDeviceCnt >= _MAX_DEVICE_IN_LIST)
	{
		CString strLoad, strText;
		strLoad.LoadString (IDS_DEV_LIST_FULL);
		strText.Format(strLoad, _MAX_DEVICE_IN_LIST);
		AfxMessageBox(strText);
		return;
	}

	CDeviceInfoDlg dlg(1);

	//向对话框传入已占用的设备编号
	pos = m_listDevice.GetHeadPosition ();
	while (pos != NULL)
	{
		pDevice = m_listDevice.GetNext (pos);
		if (pDevice->nOptFlag != OPT_DELETE)
			dlg.SetDeviceNoBeingUsed (pDevice->nDeviceNo);
	}

	int nDriverType, nRet;
	CString strDriverTypeName;
	DWORD dwDeviceTypeBit;
	TCHAR szDeviceTypeName[BUF_LEN_S];

	int nDeviceTypeCnt = 0;
	m_pDeviceIO->GetCardTypeNum(nDeviceTypeCnt);
	for (int i=0; i<nDeviceTypeCnt; i++)
	{
		nDriverType = 0;
		nRet = m_pDeviceIO->GetCardTypeID(i, nDriverType);	//设备类型
		if (nRet != DEVICEIO_ERROR_SUCCESS)
			continue;

		memset (szDeviceTypeName, 0, sizeof(szDeviceTypeName));
		strDriverTypeName.Empty();
		nRet = m_pDeviceIO->GetTypeDefaultInfo(nDriverType, szDeviceTypeName);	//设备类型名称
		if (nRet != DEVICEIO_ERROR_SUCCESS)
			continue;
		strDriverTypeName = szDeviceTypeName;
		if (strDriverTypeName.IsEmpty())
			strDriverTypeName.Format(_T("%d"), nDriverType);

		dwDeviceTypeBit = 0;
		nRet = m_pDeviceIO->GetDevicePropInfo(nDriverType, dwDeviceTypeBit);	//设备属性位
		if (nRet != DEVICEIO_ERROR_SUCCESS)
			continue;

		if (dwDeviceTypeBit != 0)
			dlg.SetDriverType (nDriverType, strDriverTypeName, dwDeviceTypeBit);
	}

	if (dlg.DoModal () == IDOK)
	{
		pDevice = new DeviceInfo();

		dlg.GetDeviceInfo (pDevice->strDeviceName, pDevice->nDeviceNo, pDevice->nDeviceType, pDevice->nDriverType, pDevice->nChannelCount);

		pDevice->strDeviceGUID = m_strLocalGUID;

		pDevice->nOptFlag = OPT_ADD;					//表示此设备是新加设备
		m_listDevice.AddTail(pDevice);					//将此设备信息加到链表中

		int nIndex = m_ctrlDeviceList.GetItemCount();	//用记录总数做为新加记录的索引
		this->InsertItemToList (nIndex, pDevice);		//向列表中加记录

		this->ModifyDeviceInfoInRegister (pDevice);		//将本地设备的一些信息重新写回注册表

		m_pDeviceIO->Initialize (SOFTWARE_NAME);		//调各个动态库重新初始化

		//选中新添加的设备，必须放在DeviceIO的Initialize后，否则在设备列表的OnItemChange函数里不能判断出新加的设备
		m_ctrlDeviceList.SetItemState(nIndex, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_ctrlDeviceList.EnsureVisible(nIndex, FALSE);
	}
}

//修改设备
void CDeviceManageDlg::OnBnClickedModifyDevice()
{
	int nIndex = m_ctrlDeviceList.GetNextItem (-1, LVNI_SELECTED);
	if (nIndex < 0)
		return;

	CDeviceInfoDlg dlg(2);

	DeviceInfo *pDevice = (DeviceInfo *)m_ctrlDeviceList.GetItemData(nIndex);
	if (pDevice == NULL)
		return;
	dlg.SetDeviceInfo (pDevice->strDeviceName, pDevice->nDeviceNo, pDevice->nDeviceType, pDevice->nDriverType, pDevice->nChannelCount);

	//向对话框传入已占用的设备编号
	POSITION pos = m_listDevice.GetHeadPosition ();
	while (pos != NULL)
	{
		pDevice = m_listDevice.GetNext (pos);
		if (pDevice->nOptFlag != OPT_DELETE)
			dlg.SetDeviceNoBeingUsed (pDevice->nDeviceNo);
	}

	int nDriverType, nRet;
	CString strDriverTypeName;
	DWORD dwDeviceTypeBit;
	TCHAR szDeviceTypeName[BUF_LEN_S];

	int nDeviceTypeCnt = 0;
	m_pDeviceIO->GetCardTypeNum(nDeviceTypeCnt);
	for (int i=0; i<nDeviceTypeCnt; i++)
	{
		nDriverType = 0;
		nRet = m_pDeviceIO->GetCardTypeID(i, nDriverType);	//设备类型
		if (nRet != DEVICEIO_ERROR_SUCCESS)
			continue;

		memset (szDeviceTypeName, 0, sizeof(szDeviceTypeName));
		strDriverTypeName.Empty();
		nRet = m_pDeviceIO->GetTypeDefaultInfo(nDriverType, szDeviceTypeName);	//设备类型名称
		if (nRet != DEVICEIO_ERROR_SUCCESS)
			continue;
		strDriverTypeName = szDeviceTypeName;
		if (strDriverTypeName.IsEmpty())
			strDriverTypeName.Format(_T("%d"), nDriverType);

		dwDeviceTypeBit = 0;
		nRet = m_pDeviceIO->GetDevicePropInfo(nDriverType, dwDeviceTypeBit);	//设备属性位
		if (nRet != DEVICEIO_ERROR_SUCCESS)
			continue;

		if (dwDeviceTypeBit != 0)
			dlg.SetDriverType (nDriverType, strDriverTypeName, dwDeviceTypeBit);
	}

	if (dlg.DoModal () == IDOK)
	{
		pDevice = (DeviceInfo *)m_ctrlDeviceList.GetItemData(nIndex);

		dlg.GetDeviceInfo (pDevice->strDeviceName, pDevice->nDeviceNo, pDevice->nDeviceType, pDevice->nDriverType, pDevice->nChannelCount);

		if (pDevice->nOptFlag != OPT_ADD)			//此设备为原来数据库中已有设备(非新加设备)
			pDevice->nOptFlag = OPT_MODIFY;			//表示设备信息被修改

		this->InsertItemToList (nIndex, pDevice, TRUE);		//修改设备列表中记录

		if (m_strLocalGUID.Compare(pDevice->strDeviceGUID) == 0)			//修改的是本地添加的设备
		{
			this->ModifyDeviceInfoInRegister (pDevice);		//将本地设备的一些信息重新写回注册表
			m_pDeviceIO->Initialize (SOFTWARE_NAME);		//调各个动态库重新初始化
		}
	}
}

//删除设备
void CDeviceManageDlg::OnBnClickedDeleteDevice()
{
	int nIndex = m_ctrlDeviceList.GetNextItem (-1, LVNI_SELECTED);
	if (nIndex < 0)
		return;

	DeviceInfo *pDevice = (DeviceInfo *)m_ctrlDeviceList.GetItemData(nIndex);
	if (pDevice == NULL)
		return;

	//提示是否删除
	if (AfxMessageBox(IDS_CONFIRM_DEL_DEV, MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2) != IDYES)
		return;

	if (m_strLocalGUID.Compare (pDevice->strDeviceGUID) == 0)	//如果是本地的设备，则将注册表中的信息也删除
	{
		m_pDeviceIO->DltDeRg (pDevice->nDriverType, pDevice->nDeviceNo);
		m_pDeviceIO->Initialize(SOFTWARE_NAME);		//调各个动态库重新初始化
	}

	//删除时对原有设备只是将此设备信息中的nOptFlag置成OPT_DELETE就行，对新加设备将此设备信息从链表中删除
	if (pDevice->nOptFlag == OPT_ADD)
	{
		//删除设备链表中的设备
		POSITION pos = m_listDevice.Find (pDevice);
		m_listDevice.RemoveAt (pos);
		FreeAndNullPtr(pDevice);
	}
	else
		pDevice->nOptFlag = OPT_DELETE;

	m_ctrlDeviceList.DeleteItem (nIndex);
}

//扩展定义
void CDeviceManageDlg::OnBnClickedExtendedDef()
{
	int nIndex = m_ctrlDeviceList.GetNextItem (-1, LVNI_SELECTED);
	if (nIndex < 0)
		return;

	DeviceInfo *pDevice = (DeviceInfo *)m_ctrlDeviceList.GetItemData(nIndex);
	if (pDevice == NULL)
		return;

	int nHandle;
	int nRet = m_pDeviceIO->GetCardHandle(pDevice->nDriverType, pDevice->nDeviceNo, nHandle);
	if (nRet != DEVICEIO_ERROR_SUCCESS)
		return;

	nRet = m_pDeviceIO->ChannelDefine(nHandle, this);
	if (nRet != DEVICEIO_ERROR_SUCCESS)
		return;

	m_pDeviceIO->Initialize(SOFTWARE_NAME);		//调各个动态库重新初始化
}

//退出
void CDeviceManageDlg::OnBnClickedOk()
{

	CDialogEx::OnOK();
}

void CDeviceManageDlg::OnBnClickedCancel()
{

	CDialogEx::OnCancel();
}

//右键点击设备列表
void CDeviceManageDlg::OnRclickDeviceList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CPoint ptMenu = pNMItemActivate->ptAction;
	m_ctrlDeviceList.ClientToScreen(&ptMenu);

	CMenu menu, *pSubMenu;
	menu.LoadMenu(IDR_RIGHT_DEV_MANAGE);
	pSubMenu = menu.GetSubMenu(0);

	if (m_bShowAllDevice)	//显示全部设备
		pSubMenu->CheckMenuRadioItem(ID_SHOW_ALL_DEVICE, ID_SHOW_ALL_DEVICE, ID_SHOW_ALL_DEVICE, MF_BYCOMMAND);
	else	//只显示本地设备
		pSubMenu->CheckMenuRadioItem(ID_SHOW_LOCAL_DEVICE, ID_SHOW_LOCAL_DEVICE, ID_SHOW_LOCAL_DEVICE, MF_BYCOMMAND);

	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, ptMenu.x, ptMenu.y, this);

	*pResult = 0;
}

void CDeviceManageDlg::OnShowAllDevice() 
{
	if (m_bShowAllDevice)
		return;
	m_bShowAllDevice = TRUE;

	this->RefreshDeviceList();

	GetDlgItem(IDC_MODIFY_DEVICE)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELETE_DEVICE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EXTENDED_DEF)->EnableWindow(FALSE);
}

void CDeviceManageDlg::OnShowLocalDevice() 
{
	if (!m_bShowAllDevice)
		return;
	m_bShowAllDevice = FALSE;

	this->RefreshDeviceList();

	GetDlgItem(IDC_MODIFY_DEVICE)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELETE_DEVICE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EXTENDED_DEF)->EnableWindow(FALSE);
}
