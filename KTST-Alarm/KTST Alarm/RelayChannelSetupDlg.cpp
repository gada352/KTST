// RelayChannelSetupDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "KTST Alarm.h"
#include "RelayChannelSetupDlg.h"
#include "afxdialogex.h"
#include "SelectChannelDlg.h"


// CRelayChannelSetupDlg 对话框

IMPLEMENT_DYNAMIC(CRelayChannelSetupDlg, CDialogEx)

CRelayChannelSetupDlg::CRelayChannelSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRelayChannelSetupDlg::IDD, pParent)
{
	m_strChannelName = _T("");
	m_nOutputDelay = 0;

	m_pCollectionDeviceList = NULL;
	m_pCollectionChannelList = NULL;

	m_nCurCursorPos = 0;
	m_bExpressionValid = TRUE;
}

CRelayChannelSetupDlg::~CRelayChannelSetupDlg()
{
}

void CRelayChannelSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RELAY_CHANNEL_NAME, m_strChannelName);
	DDV_MaxChars(pDX, m_strChannelName, BUF_LEN_S-1);
	DDX_Control(pDX, IDC_EXPRESSION, m_ctrlExpressionEditor);
	DDX_Text(pDX, IDC_RELAY_OUTPUT_DELAY, m_nOutputDelay);
	DDV_MinMaxInt(pDX, m_nOutputDelay, 0, 300);
}


BEGIN_MESSAGE_MAP(CRelayChannelSetupDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_SELECT_CHANNEL, &CRelayChannelSetupDlg::OnBnClickedSelectChannel)
	ON_BN_CLICKED(IDC_LEFT_PARENTHESES, &CRelayChannelSetupDlg::OnBnClickedLeftParentheses)
	ON_BN_CLICKED(IDC_RIGHT_PARENTHESES, &CRelayChannelSetupDlg::OnBnClickedRightParentheses)
	ON_BN_CLICKED(IDC_AND, &CRelayChannelSetupDlg::OnBnClickedAnd)
	ON_BN_CLICKED(IDC_OR, &CRelayChannelSetupDlg::OnBnClickedOr)
	ON_BN_CLICKED(IDC_NOT, &CRelayChannelSetupDlg::OnBnClickedNot)
	ON_BN_CLICKED(IDC_DELETE, &CRelayChannelSetupDlg::OnBnClickedDelete)
	ON_BN_CLICKED(IDOK, &CRelayChannelSetupDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CRelayChannelSetupDlg 消息处理程序
void CRelayChannelSetupDlg::SetRelayChannel (const ChannelInfo &channel)
{
	m_RelayChannel.SetChannelInfo(channel);
}

void CRelayChannelSetupDlg::GetRelayChannel (ChannelInfo &channel)
{
	m_RelayChannel.GetChannelInfo(channel);
}

void CRelayChannelSetupDlg::SetCollectionDeviceListPtr (CDeviceInfoList *pCollectionDeviceList)
{
	m_pCollectionDeviceList = pCollectionDeviceList;
}

void CRelayChannelSetupDlg::SetCollectionChannelListPtr (CChannelInfoList *pCollectionChannelList)
{
	m_pCollectionChannelList = pCollectionChannelList;
}

BOOL CRelayChannelSetupDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_RelayChannel.AnalyzeRelayChannelBuffer();

	//通道名称
	m_strChannelName = m_RelayChannel.m_strChannelName;

	//输出延时
	m_nOutputDelay = m_RelayChannel.m_nOutputDelay;

	//充电
	CButton *pButton = (CButton *)GetDlgItem(IDC_ENERGIZED);
	pButton->SetCheck(m_RelayChannel.m_bEnergized);

	//旁路
	pButton = (CButton *)GetDlgItem(IDC_BYPASS);
	pButton->SetCheck(m_RelayChannel.m_bBypass);


	//表达式
	RefreshExpressionDisplayInfo (TRUE);


	//修改表达式编辑框字体
// 	CFont *pFont = m_ctrlExpressionEditor.GetFont();
// 	LOGFONT lg;
// 	pFont->GetLogFont(&lg);
// 	lg.lfWeight = FW_BOLD;	//加粗
// 	if (m_fontEditor.m_hObject != NULL)
// 		m_fontEditor.DeleteObject();
// 	m_fontEditor.CreateFontIndirect(&lg);
// 	m_ctrlExpressionEditor.SetFont(&m_fontEditor, TRUE);

	UpdateData (FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//刷新表达式在编辑器的显示信息
void CRelayChannelSetupDlg::RefreshExpressionDisplayInfo (BOOL bRefreshString)
{
	//内存中刷新表达式的显示信息
	CString strText;
	int nTextLen;
	int nIndex = 0;	//记录表达式的索引号(从1开始)

	RelayExpression *pExpression = NULL;
	POSITION pos = m_RelayChannel.m_listExpression.GetHeadPosition();
	while (pos)
	{
		pExpression = m_RelayChannel.m_listExpression.GetNext(pos);
		if (pExpression == NULL)
			continue;

		if (bRefreshString)
			pExpression->strDisplay = GetExpressionDisplayInfo (*pExpression);

		nTextLen = pExpression->strDisplay.GetLength ();
		pExpression->nIndexBegin = nIndex + 1;			//开始索引号
		pExpression->nIndexEnd = nIndex + nTextLen;		//结束索引号

		nIndex += nTextLen;
	}

	//界面上显示表达式
	ShowExpression ();
}

//得到表达式的显示字符串
CString CRelayChannelSetupDlg::GetExpressionDisplayInfo (const RelayExpression &exp)
{
	CString strDisplay;

	switch (exp.eExpressionType)
	{
	case etOperator:
		{
			if (exp.strOperator == _T("("))
				strDisplay = _T("( ");		//显示时加空格
			else if (exp.strOperator == _T(")"))
				strDisplay = _T(") ");		//显示时加空格
			else if (exp.strOperator == _T("&"))
				strDisplay = _T("  &  ");		//显示时加空格
			else if (exp.strOperator == _T("|"))
				strDisplay = _T("  |  ");		//显示时加空格
			else
				strDisplay = exp.strOperator;	//原始形式
		}
		break;

	case etChannel:
		{
			CString strLoad, strDeviceName, strChannelName;

			strDeviceName = GetCollectionDeviceName(exp.nDeviceNo);
			strChannelName = GetCollectionChannelName(exp.nDeviceNo, exp.nChannelNo);
			if (strDeviceName.IsEmpty() || strChannelName.IsEmpty())
			{
				strDisplay.LoadString (IDS_EXP_CH_TEMPLATE_INVALID);
			}
			else
			{
				strLoad.LoadString (IDS_EXP_CH_TEMPLATE);
				strDisplay.Format(strLoad, strDeviceName, strChannelName);
			}
		}
		break;
	}

	return strDisplay;
}

//根据设备号得到采集设备名称
CString CRelayChannelSetupDlg::GetCollectionDeviceName (int nDeviceNo)
{
	CString strDeviceName;

	if (m_pCollectionDeviceList)
	{
		DeviceInfo *pDevice = NULL;
		POSITION pos = m_pCollectionDeviceList->GetHeadPosition();
		while (pos)
		{
			pDevice = m_pCollectionDeviceList->GetNext(pos);
			if (pDevice && pDevice->nDeviceNo == nDeviceNo)
			{
				strDeviceName = pDevice->strDeviceName;
				break;
			}
		}
	}

	return strDeviceName;
}

//根据设备号通道号得到采集通道名称
CString CRelayChannelSetupDlg::GetCollectionChannelName (int nDeviceNo, int nChannelNo)
{
	CString strChannelName;

	if (m_pCollectionChannelList)
	{
		ChannelInfo *pChannel = NULL;
		POSITION pos = m_pCollectionChannelList->GetHeadPosition();
		while (pos)
		{
			pChannel = m_pCollectionChannelList->GetNext(pos);
			if (pChannel && pChannel->nDeviceNo == nDeviceNo && pChannel->nChannelNo == nChannelNo)
			{
				strChannelName = pChannel->strChannelName;
				break;
			}
		}
	}

	return strChannelName;
}

//根据设备号通道号得到采集通道名称
BOOL CRelayChannelSetupDlg::IsCollectionChannel (int nDeviceNo, int nChannelNo)
{
	if (m_pCollectionChannelList)
	{
		ChannelInfo *pChannel = NULL;
		POSITION pos = m_pCollectionChannelList->GetHeadPosition();
		while (pos)
		{
			pChannel = m_pCollectionChannelList->GetNext(pos);
			if (pChannel && pChannel->nDeviceNo == nDeviceNo && pChannel->nChannelNo == nChannelNo)
				return TRUE;
		}
	}

	return FALSE;
}

//界面显示表达式
void CRelayChannelSetupDlg::ShowExpression ()
{
	CString strExpression;

	RelayExpression *pExpression = NULL;
	POSITION pos = m_RelayChannel.m_listExpression.GetHeadPosition();
	while (pos)
	{
		pExpression = m_RelayChannel.m_listExpression.GetNext(pos);
		if (pExpression)
			strExpression += pExpression->strDisplay;
	}

	m_ctrlExpressionEditor.SetWindowText (strExpression);
	m_ctrlExpressionEditor.SetSel (m_nCurCursorPos, m_nCurCursorPos);		//设置光标位置

	ShowExpressionStatusInfo ();		//显示表达式状态信息
}

//显示表达式状态信息
void CRelayChannelSetupDlg::ShowExpressionStatusInfo ()
{
	CString strLoad, strText, strStatus;

	int nExpressionLen = m_RelayChannel.m_listExpression.GetCount ();

	//判断表达式是否有效
 	m_bExpressionValid = IsExpressionValid ();
	if (m_bExpressionValid)
		strStatus.LoadString(IDS_VALID);
	else
		strStatus.LoadString(IDS_INVALID);

	strLoad.LoadString(IDS_EXP_STATUS_TEMPLATE);
	strText.Format(strLoad, nExpressionLen, _MAX_RELAY_EXPRESSION_LEN, strStatus);

	SetDlgItemText (IDC_EXPRESSION_STATUS_INFO, strText);
}

//判断表达式是否有效
BOOL CRelayChannelSetupDlg::IsExpressionValid ()
{
	if (m_RelayChannel.m_listExpression.GetCount () <= 0)
		return TRUE;

	//判断表达式中是否有无效通道
	RelayExpression *pExpression = NULL;
	POSITION pos = m_RelayChannel.m_listExpression.GetHeadPosition();
	while (pos)
	{
		pExpression = m_RelayChannel.m_listExpression.GetNext(pos);
		if (pExpression && pExpression->eExpressionType == etChannel)
		{
			if (!IsCollectionChannel(pExpression->nDeviceNo, pExpression->nChannelNo))
				return FALSE;
		}
	}

	CString strExpression = m_RelayChannel.GetExpressionString();
	int nReturnValue;

	return m_RelayChannel.Cal (strExpression, nReturnValue);
}

//插入一个表达式元素
void CRelayChannelSetupDlg::InsertExpression (const RelayExpression &expNew)
{
	CString strDisplay = GetExpressionDisplayInfo(expNew);
	m_nCurCursorPos += strDisplay.GetLength ();		//计算插入后光标所在位置

	RelayExpression *pExpression = new RelayExpression();

	//拷贝表达式元素信息
	pExpression->eExpressionType = expNew.eExpressionType;
	pExpression->strOperator = expNew.strOperator;
	pExpression->nDeviceNo = expNew.nDeviceNo;
	pExpression->nChannelNo = expNew.nChannelNo;

	pExpression->strDisplay = strDisplay;

	//插入在链表中的指定位置
	int nBegin, nEnd;
	m_ctrlExpressionEditor.GetSel (nBegin, nEnd);

	if (nEnd == 0)
		m_RelayChannel.m_listExpression.AddHead (pExpression);
	else
	{
		POSITION pos = GetExpressionPosFromIndex (nEnd);
		if (pos)
			m_RelayChannel.m_listExpression.InsertAfter (pos, pExpression);
	}

	//刷新表达式，不用重新拼字符串
	RefreshExpressionDisplayInfo (FALSE);
}

//根据表达式文本框的当前光标位置找到对应通道表达式链表中的位置
POSITION CRelayChannelSetupDlg::GetExpressionPosFromIndex (int nCursorIndex)
{
	POSITION posFind = NULL;
	RelayExpression *pExpression = NULL;
	POSITION pos = m_RelayChannel.m_listExpression.GetHeadPosition ();
	while (pos)
	{
		posFind = pos;
		pExpression = m_RelayChannel.m_listExpression.GetNext (pos);
		if (pExpression && nCursorIndex >= pExpression->nIndexBegin && nCursorIndex <= pExpression->nIndexEnd)
			break;
	}

	return posFind;
}

//点击通道表达式Edit框，如果点到了一个表达式的中间，将光标移到当前表达式的后面
//						如果选择了多个表达式，则将光标移到最后一个被选表达式的后面
void CRelayChannelSetupDlg::MoveCursorToExpElementEnd ()
{
	int nBegin, nEnd;
	m_ctrlExpressionEditor.GetSel (nBegin, nEnd);

	if (nEnd == 0)
		m_nCurCursorPos = 0;
	else
	{
		RelayExpression *pExpression = NULL;
		POSITION pos = GetExpressionPosFromIndex (nEnd);
		if (pos != NULL)
		{
			pExpression = m_RelayChannel.m_listExpression.GetAt (pos);
			if (pExpression)
				m_nCurCursorPos = pExpression->nIndexEnd;
		}
	}

	m_ctrlExpressionEditor.SetFocus ();
	m_ctrlExpressionEditor.SetSel (m_nCurCursorPos, m_nCurCursorPos);
}

//判断继电器表达式编辑是否达到最大值
BOOL CRelayChannelSetupDlg::IsExpressionEditorFull ()
{
	if (m_RelayChannel.m_listExpression.GetCount() >= _MAX_RELAY_EXPRESSION_LEN)
		return TRUE;
	
	return FALSE;
}

BOOL CRelayChannelSetupDlg::PreTranslateMessage(MSG* pMsg)
{
	//表达式编辑框
	if (pMsg->hwnd == m_ctrlExpressionEditor.m_hWnd)
	{
		//屏蔽掉右键输入
		if (pMsg->message == WM_RBUTTONUP)
			return TRUE;

		//屏蔽掉鼠标左键双击
		if (pMsg->message == WM_LBUTTONDBLCLK)
			return TRUE;

		//鼠标左键抬起
		if (pMsg->message == WM_LBUTTONUP)
			this->MoveCursorToExpElementEnd ();		//将光标移到选中字元素的后面

		//键盘抬起	(输入上、下、左、右)
		if (pMsg->message == WM_KEYUP && (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_UP || pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_DOWN))
			this->MoveCursorToExpElementEnd ();		//将光标移到选中元素的后面
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


HBRUSH CRelayChannelSetupDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (IDC_EXPRESSION == pWnd->GetDlgCtrlID())
	{
		if (m_bExpressionValid)
			pDC->SetTextColor(RGB(0, 0, 255));	//有效字体颜色
		else
			pDC->SetTextColor(RGB(200, 0, 0));	//无效字体颜色
	}

	return hbr;
}

//选择报警通道
void CRelayChannelSetupDlg::OnBnClickedSelectChannel()
{
	if (IsExpressionEditorFull())
		return;

	CSelectChannelDlg dlg;

	dlg.SetDeviceListPtr(m_pCollectionDeviceList);
	dlg.SetChannelListPtr(m_pCollectionChannelList);

	if (dlg.DoModal() == IDOK)
	{
		//将选中的采集通道循环插入到关注列表中
		CChannelInfoList *pListSelChannel = dlg.GetSelChannelListPrt();
		if (pListSelChannel == NULL || pListSelChannel->GetCount() <= 0)
			return;

		RelayExpression expNew;
		ChannelInfo *pSelChannel = NULL;
		POSITION pos = pListSelChannel->GetHeadPosition();
		while (pos)
		{
			pSelChannel = pListSelChannel->GetNext(pos);
			if (pSelChannel)
			{
				expNew.eExpressionType = etChannel;
				expNew.nDeviceNo = pSelChannel->nDeviceNo;
				expNew.nChannelNo = pSelChannel->nChannelNo;

				InsertExpression (expNew);
			}
		}
	}

	m_ctrlExpressionEditor.SetFocus();
}

//左括号
void CRelayChannelSetupDlg::OnBnClickedLeftParentheses()
{
	if (IsExpressionEditorFull())
		return;
	
	RelayExpression expNew;
	expNew.eExpressionType = etOperator;
	expNew.strOperator = _T("(");
	InsertExpression (expNew);

	m_ctrlExpressionEditor.SetFocus();
}

//右括号
void CRelayChannelSetupDlg::OnBnClickedRightParentheses()
{
	if (IsExpressionEditorFull())
		return;

	RelayExpression expNew;
	expNew.eExpressionType = etOperator;
	expNew.strOperator = _T(")");
	InsertExpression (expNew);

	m_ctrlExpressionEditor.SetFocus();
}

//与
void CRelayChannelSetupDlg::OnBnClickedAnd()
{
	if (IsExpressionEditorFull())
		return;

	RelayExpression expNew;
	expNew.eExpressionType = etOperator;
	expNew.strOperator = _T("&");
	InsertExpression (expNew);

	m_ctrlExpressionEditor.SetFocus();
}

//或
void CRelayChannelSetupDlg::OnBnClickedOr()
{
	if (IsExpressionEditorFull())
		return;

	RelayExpression expNew;
	expNew.eExpressionType = etOperator;
	expNew.strOperator = _T("|");
	InsertExpression (expNew);

	m_ctrlExpressionEditor.SetFocus();
}

//非
void CRelayChannelSetupDlg::OnBnClickedNot()
{
	if (IsExpressionEditorFull())
		return;

	RelayExpression expNew;
	expNew.eExpressionType = etOperator;
	expNew.strOperator = _T("~");
	InsertExpression (expNew);

	m_ctrlExpressionEditor.SetFocus();
}

//删除
void CRelayChannelSetupDlg::OnBnClickedDelete()
{
	if (m_nCurCursorPos <= 0)
		return;

	POSITION pos = GetExpressionPosFromIndex (m_nCurCursorPos);
	if (pos)
	{
		RelayExpression *pExpression = m_RelayChannel.m_listExpression.GetAt (pos);
		m_nCurCursorPos = pExpression->nIndexBegin - 1;

		m_RelayChannel.m_listExpression.RemoveAt (pos);
		delete pExpression;
		pExpression = NULL;

		RefreshExpressionDisplayInfo (FALSE);	//刷新所有表达式元素的索引
	}

	m_ctrlExpressionEditor.SetFocus();
}


void CRelayChannelSetupDlg::OnBnClickedOk()
{
	if (!UpdateData(TRUE))
		return;

	//**	先进行合法性检查	***************************

	//通道名称
	m_strChannelName.Trim();
	if (m_strChannelName.IsEmpty())
	{
		AfxMessageBox(IDS_CH_NAME_NULL);
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_RELAY_CHANNEL_NAME);
		pEdit->SetWindowText(_T(""));
		pEdit->SetFocus ();
		return;
	}

	//表达式的值
	BOOL bExpressionValid = IsExpressionValid ();
	if (!bExpressionValid)
	{
		AfxMessageBox(IDS_EXPRESSION_INVALID);
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EXPRESSION);
		pEdit->SetFocus();
		pEdit->SetSel(0, -1);
		return;
	}
	//*****************************************************


	//**	保存通道参数	*******************************

	//通道名称
	m_RelayChannel.m_strChannelName = m_strChannelName;

	//输出延时
	m_RelayChannel.m_nOutputDelay = m_nOutputDelay;

	//充电
	CButton *pButton = (CButton *)GetDlgItem(IDC_ENERGIZED);
	m_RelayChannel.m_bEnergized = pButton->GetCheck();

	//旁路
	pButton = (CButton *)GetDlgItem(IDC_BYPASS);
	m_RelayChannel.m_bBypass = pButton->GetCheck();

	//表达式
	//表达式在编辑时直接修改的m_RelayChannel的表达式链表m_listExpression，所以不用再保存

	m_RelayChannel.SaveRelayChannelBuffer();

	CDialogEx::OnOK();
}
