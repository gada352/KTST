// SelectChannelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "KTST Alarm.h"
#include "SelectChannelDlg.h"
#include "afxdialogex.h"


// CSelectChannelDlg 对话框

IMPLEMENT_DYNAMIC(CSelectChannelDlg, CDialogEx)

CSelectChannelDlg::CSelectChannelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSelectChannelDlg::IDD, pParent)
{
	m_pListDeviceInfo = NULL;
	m_pListChannelInfo = NULL;
	m_nCurSelDevice = -1;
}

CSelectChannelDlg::~CSelectChannelDlg()
{
	m_listSelChannelInfo.RemoveAll();
}

void CSelectChannelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEVICE_LIST, m_ctrlDeviceList);
	DDX_Control(pDX, IDC_CHANNEL_LIST, m_ctrlChannelList);
}


BEGIN_MESSAGE_MAP(CSelectChannelDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_DEVICE_LIST, &CSelectChannelDlg::OnSelchangeDeviceList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CHANNEL_LIST, &CSelectChannelDlg::OnItemchangedChannelList)
	ON_BN_CLICKED(IDOK, &CSelectChannelDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSelectChannelDlg 消息处理程序
void CSelectChannelDlg::SetDeviceListPtr (CDeviceInfoList *pListDeviceInfo)
{
	m_pListDeviceInfo = pListDeviceInfo;
}

void CSelectChannelDlg::SetChannelListPtr (CChannelInfoList *pListChannelInfo)
{
	m_pListChannelInfo = pListChannelInfo;
}

CChannelInfoList* CSelectChannelDlg::GetSelChannelListPrt ()
{
	return &m_listSelChannelInfo;
}


BOOL CSelectChannelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ctrlChannelList.SetExtendedStyle (m_ctrlChannelList.GetExtendedStyle () 
									| LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//设置列表列头
	CString strLoad;
	int nIndex = 0;
	strLoad.LoadString (IDS_NO);
	m_ctrlChannelList.InsertColumn (nIndex, strLoad, LVCFMT_CENTER, 60);
	nIndex ++;
	strLoad.LoadString (IDS_NAME);
	m_ctrlChannelList.InsertColumn (nIndex, strLoad, LVCFMT_LEFT, 180);
	nIndex ++;

	this->RefreshDeviceList ();		//刷新设备列表

	if (m_ctrlDeviceList.GetCount() > 0)
	{
		m_ctrlDeviceList.SetCurSel(0);

		m_nCurSelDevice = -1;
		this->OnSelchangeDeviceList();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSelectChannelDlg::RefreshDeviceList ()
{
	m_ctrlDeviceList.ResetContent();
	m_nCurSelDevice = -1;

	if (m_pListDeviceInfo == NULL)
		return;

	int nIndex;
	DeviceInfo *pDevice = NULL;
	POSITION pos = m_pListDeviceInfo->GetHeadPosition ();
	while (pos != NULL)
	{
		pDevice = m_pListDeviceInfo->GetNext (pos);

		nIndex = m_ctrlDeviceList.AddString(pDevice->strDeviceName);
		if (nIndex >= 0)
			m_ctrlDeviceList.SetItemData(nIndex, (DWORD )pDevice);
	}
}

//刷新通道列表
void CSelectChannelDlg::RefreshChannelList ()
{
	m_ctrlChannelList.DeleteAllItems();

	if (m_nCurSelDevice < 0)
		return;

	DeviceInfo *pDevice = (DeviceInfo *)m_ctrlDeviceList.GetItemData(m_nCurSelDevice);
	if (pDevice == NULL)
		return;

	if (m_pListChannelInfo == NULL)
		return;

	int nIndex = 0;
	int nSubItem;
	CString strText;
	ChannelInfo *pChannel = NULL;
	POSITION pos = m_pListChannelInfo->GetHeadPosition ();
	while (pos != NULL)
	{
		pChannel = m_pListChannelInfo->GetNext (pos);

		if (pChannel->nDeviceNo == pDevice->nDeviceNo)
		{
			nSubItem = 0;
			strText.Format(_T("%d"), pChannel->nChannelNo);
			m_ctrlChannelList.InsertItem(nIndex, strText);	//通道工作允许复选框列
			nSubItem ++;
			m_ctrlChannelList.SetItemText(nIndex, nSubItem, pChannel->strChannelName);
			nSubItem ++;

			m_ctrlChannelList.SetItemData(nIndex, (DWORD )pChannel);

			nIndex ++;
		}
	}
}


void CSelectChannelDlg::OnSelchangeDeviceList()
{
	int nIndex = m_ctrlDeviceList.GetCurSel();
	if (nIndex<0 || nIndex==m_nCurSelDevice)
		return;

	m_nCurSelDevice = nIndex;	//保存新选中设备索引

	//显示新选中设备的通道
	this->RefreshChannelList ();
}


void CSelectChannelDlg::OnItemchangedChannelList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);


	*pResult = 0;
}


void CSelectChannelDlg::OnBnClickedOk()
{
	m_listSelChannelInfo.RemoveAll();

	ChannelInfo *pChannel = NULL;
	int nItem = -1;
	int nCount = m_ctrlChannelList.GetSelectedCount();

	for (int i=0; i<nCount; i++)
	{
		nItem = m_ctrlChannelList.GetNextItem(nItem, LVNI_SELECTED);
		pChannel = (ChannelInfo *)m_ctrlChannelList.GetItemData(nItem);
		if (pChannel)
			m_listSelChannelInfo.AddTail(pChannel);
	}

	CDialogEx::OnOK();
}
