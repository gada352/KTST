// RelayManageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "KTST Alarm.h"
#include "RelayManageDlg.h"
#include "afxdialogex.h"

#include "RelayChannelSetupDlg.h"

// CRelayManageDlg 对话框

IMPLEMENT_DYNAMIC(CRelayManageDlg, CDialogEx)

CRelayManageDlg::CRelayManageDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRelayManageDlg::IDD, pParent)
{
	m_nCurSelDevice = -1;
}

CRelayManageDlg::~CRelayManageDlg()
{
	this->ClearMemory ();
}

void CRelayManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RELAY_DEVICE_LIST, m_ctrlDeviceList);
	DDX_Control(pDX, IDC_RELAY_CHANNEL_LIST, m_ctrlChannelList);
}


BEGIN_MESSAGE_MAP(CRelayManageDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_RELAY_DEVICE_LIST, &CRelayManageDlg::OnSelchangeRelayDeviceList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_RELAY_CHANNEL_LIST, &CRelayManageDlg::OnItemchangedRelayChannelList)
	ON_BN_CLICKED(IDC_RELAY_CHANNEL_SETUP, &CRelayManageDlg::OnBnClickedRelayChannelSetup)
	ON_BN_CLICKED(IDOK, &CRelayManageDlg::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_RELAY_CHANNEL_LIST, &CRelayManageDlg::OnDblclkRelayChannelList)
END_MESSAGE_MAP()


// CRelayManageDlg 消息处理程序


BOOL CRelayManageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//检查继电器通道的表达式是否有效
	CheckAllRelayChannelExpression ();

	m_ctrlChannelList.SetExtendedStyle (m_ctrlChannelList.GetExtendedStyle () 
										| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//设置列表列头
	CString strLoad;
	int nIndex = 0;
	strLoad.LoadString(IDS_NO);
	m_ctrlChannelList.InsertColumn(nIndex, strLoad, LVCFMT_LEFT, 40);
	nIndex ++;
	strLoad.LoadString(IDS_NAME);
	m_ctrlChannelList.InsertColumn(nIndex, strLoad, LVCFMT_LEFT, 140);
	nIndex ++;
	strLoad.LoadString(IDS_OUTPUT_DELAY);
	m_ctrlChannelList.InsertColumn(nIndex, strLoad, LVCFMT_RIGHT, 100);
	nIndex ++;
	strLoad.LoadString(IDS_ENERGIZED);
	m_ctrlChannelList.InsertColumn(nIndex, strLoad, LVCFMT_CENTER, 40);
	nIndex ++;
	strLoad.LoadString(IDS_BYPASS);
	m_ctrlChannelList.InsertColumn(nIndex, strLoad, LVCFMT_CENTER, 40);
	nIndex ++;
	strLoad.LoadString(IDS_EXPRESSION);
	m_ctrlChannelList.InsertColumn(nIndex, strLoad, LVCFMT_CENTER, 100);
	nIndex ++;

	GetDlgItem(IDC_RELAY_CHANNEL_SETUP)->EnableWindow(FALSE);

	this->RefreshRelayDeviceList ();		//刷新设备列表

	if (m_ctrlDeviceList.GetCount() > 0)
	{
		m_nCurSelDevice = 0;
		m_ctrlDeviceList.SetCurSel(m_nCurSelDevice);

		RefreshRelayChannelList();
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//传入继电器模块设备
void CRelayManageDlg::SetRelayDevice (const DeviceInfo &device)
{
	RelayDevice *pDevice = new RelayDevice();
	pDevice->SetDeviceInfo(device);
	m_listRelayDevice.AddTail(pDevice);
}

//传入继电器模块设备
void CRelayManageDlg::SetRelayChannel (const ChannelInfo &channel)
{
	RelayChannel *pChannel = new RelayChannel();
	pChannel->SetChannelInfo(channel);
	pChannel->AnalyzeRelayChannelBuffer();	//解析继电器通道缓冲区
	m_listRelayChannel.AddTail(pChannel);
}

//传入采集模块设备
void CRelayManageDlg::SetCollectionDevice (const DeviceInfo &device)
{
	m_listCollectionDevice.AddTail(new DeviceInfo(device));
}

//传入采集模块设备通道
void CRelayManageDlg::SetCollectionChannel (const ChannelInfo &channel)
{
	m_listCollectionChannel.AddTail(new ChannelInfo(channel));
}

//获得继电器通道链表
CRelayChannelList* CRelayManageDlg::GetRelayChannelListPtr ()
{
	return &m_listRelayChannel;
}

void CRelayManageDlg::ClearMemory ()
{
	FreeObjPtrList(&m_listRelayDevice);
	FreeObjPtrList(&m_listRelayChannel);

	FreeObjPtrList(&m_listCollectionDevice);
	FreeObjPtrList(&m_listCollectionChannel);
}

void CRelayManageDlg::RefreshRelayDeviceList ()
{
	m_ctrlDeviceList.ResetContent();
	m_nCurSelDevice = -1;

	int nIndex;
	RelayDevice *pDevice = NULL;
	POSITION pos = m_listRelayDevice.GetHeadPosition ();
	while (pos != NULL)
	{
		pDevice = m_listRelayDevice.GetNext (pos);

		nIndex = m_ctrlDeviceList.AddString(pDevice->m_strDeviceName);
		if (nIndex >= 0)
			m_ctrlDeviceList.SetItemData(nIndex, (DWORD )pDevice);
	}
}

//刷新通道列表
void CRelayManageDlg::RefreshRelayChannelList ()
{
	m_ctrlChannelList.DeleteAllItems();

	if (m_nCurSelDevice < 0)
		return;

	RelayDevice *pDevice = (RelayDevice *)m_ctrlDeviceList.GetItemData(m_nCurSelDevice);
	if (pDevice == NULL)
		return;

	int nItem = 0;
	int nSubItem;
	CString strText;

	RelayChannel *pChannel = NULL;
	POSITION pos = m_listRelayChannel.GetHeadPosition ();
	while (pos != NULL)
	{
		pChannel = m_listRelayChannel.GetNext (pos);
		if (pChannel->m_nDeviceNo == pDevice->m_nDeviceNo)
		{
			nSubItem = 0;
			strText.Format(_T("%d"), pChannel->m_nChannelNo);
			m_ctrlChannelList.InsertItem(nItem, strText);
			nSubItem ++;

			UpdateRelayChannelInfo(nItem, pChannel);

			m_ctrlChannelList.SetItemData(nItem, (DWORD )pChannel);

			nItem ++;
		}
	}
}

//更新通道列表中指定通道的信息
void CRelayManageDlg::UpdateRelayChannelInfo (int nItem, RelayChannel *pChannel)
{
	int nCount = m_ctrlChannelList.GetItemCount();
	if (nItem < 0 || nItem >= nCount || pChannel == NULL)
		return;

	CString strText, strLoad;
	int nSubItem = 1;

	//通道名称
	m_ctrlChannelList.SetItemText(nItem, nSubItem, pChannel->m_strChannelName);
	nSubItem ++;

	//输出延时
	strText.Format(_T("%d"), pChannel->m_nOutputDelay);
	m_ctrlChannelList.SetItemText(nItem, nSubItem, strText);
	nSubItem ++;

	//充电
	strText.Empty();
	if (pChannel->m_bEnergized)
		strText = _T("√");
	m_ctrlChannelList.SetItemText(nItem, nSubItem, strText);
	nSubItem ++;

	//旁路
	strText.Empty();
	if (pChannel->m_bBypass)
		strText = _T("√");
	m_ctrlChannelList.SetItemText(nItem, nSubItem, strText);
	nSubItem ++;

	//表达式
	strText.Empty();
	if (pChannel->m_listExpression.GetCount() > 0)
	{
		strText.LoadString(IDS_HAVE_SET);
		if (!pChannel->m_bExpressionValid)
		{
			strLoad.LoadString(IDS_INVALID);
			strText += _T(" (");
			strText += strLoad;
			strText += _T(")");
		}
	}
	m_ctrlChannelList.SetItemText(nItem, nSubItem, strText);
	nSubItem ++;
}

//检查所有继电器通道的表达式是否有效
void CRelayManageDlg::CheckAllRelayChannelExpression ()
{
	RelayExpression *pExpression = NULL;
	POSITION posExpression;

	RelayChannel *pChannel = NULL;
	POSITION posChannel = m_listRelayChannel.GetHeadPosition();
	while (posChannel)
	{
		pChannel = m_listRelayChannel.GetNext(posChannel);
		if (pChannel == NULL)
			continue;

		posExpression = pChannel->m_listExpression.GetHeadPosition();
		while (posExpression)
		{
			pExpression = pChannel->m_listExpression.GetNext(posExpression);
			if (pExpression && pExpression->eExpressionType == etChannel)
			{
				if (!IsCollectionChannel(pExpression->nDeviceNo, pExpression->nChannelNo))
				{
					pChannel->m_bExpressionValid = FALSE;
					break;
				}
			}
		}
	}
}

//判断指定通道是否有采集通道
BOOL CRelayManageDlg::IsCollectionChannel (int nDeviceNo, int nChannelNo)
{
	ChannelInfo *pChannel = NULL;
	POSITION pos = m_listCollectionChannel.GetHeadPosition();
	while (pos)
	{
		pChannel = m_listCollectionChannel.GetNext(pos);
		if (pChannel && pChannel->nDeviceNo == nDeviceNo && pChannel->nChannelNo == nChannelNo)
			return TRUE;
	}

	return FALSE;
}

void CRelayManageDlg::OnSelchangeRelayDeviceList()
{
	int nIndex = m_ctrlDeviceList.GetCurSel();
	if (nIndex<0 || nIndex==m_nCurSelDevice)
		return;

	m_nCurSelDevice = nIndex;	//保存新选中设备索引

	if (m_nCurSelDevice >= 0)
		RefreshRelayChannelList ();

	GetDlgItem(IDC_RELAY_CHANNEL_SETUP)->EnableWindow(FALSE);
}


void CRelayManageDlg::OnItemchangedRelayChannelList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	//判断项目改变为状态改变
	if ((pNMLV->uChanged & LVIF_STATE) == LVIF_STATE)
	{
		if ((pNMLV->uOldState & LVIS_SELECTED) == LVIS_SELECTED)			//失去选中状态
		{
			GetDlgItem(IDC_RELAY_CHANNEL_SETUP)->EnableWindow(FALSE);		//控制参数设置按钮
		}
		else if ((pNMLV->uNewState & LVIS_SELECTED) == LVIS_SELECTED)		//得到选中状态
		{
			GetDlgItem(IDC_RELAY_CHANNEL_SETUP)->EnableWindow(TRUE);		//控制参数设置按钮
		}
	}

	*pResult = 0;
}


void CRelayManageDlg::OnDblclkRelayChannelList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	if (m_ctrlChannelList.GetNextItem(-1, LVNI_SELECTED) >= 0)
		OnBnClickedRelayChannelSetup();

	*pResult = 0;
}

//参数设置
void CRelayManageDlg::OnBnClickedRelayChannelSetup()
{
	int nIndex = m_ctrlChannelList.GetNextItem (-1, LVNI_SELECTED);
	if (nIndex < 0)
		return;

	RelayChannel *pChannel = (RelayChannel *)m_ctrlChannelList.GetItemData (nIndex);
	if (pChannel == NULL)
		return;


	CRelayChannelSetupDlg dlg;

	ChannelInfo ci;
	pChannel->GetChannelInfo(ci);
	dlg.SetRelayChannel(ci);

	dlg.SetCollectionDeviceListPtr (&m_listCollectionDevice);
	dlg.SetCollectionChannelListPtr (&m_listCollectionChannel);

	if (dlg.DoModal() == IDOK)
	{
		dlg.GetRelayChannel(ci);

		pChannel->SetChannelInfo(ci);
		pChannel->AnalyzeRelayChannelBuffer();
		pChannel->m_bExpressionValid = TRUE;	//认为通道设置OK后的表达式有效
		pChannel->m_nOptFlag = OPT_MODIFY;

		UpdateRelayChannelInfo (nIndex, pChannel);
	}
}


void CRelayManageDlg::OnBnClickedOk()
{
	//保存每个继电器通道缓冲区信息
	RelayChannel *pChannel = NULL;
	POSITION pos = m_listRelayChannel.GetHeadPosition ();
	while (pos != NULL)
	{
		pChannel = m_listRelayChannel.GetNext (pos);
		if (pChannel)
			pChannel->SaveRelayChannelBuffer();	//保存继电器通道信息：输出延时、表达式等
	}

	CDialogEx::OnOK();
}
