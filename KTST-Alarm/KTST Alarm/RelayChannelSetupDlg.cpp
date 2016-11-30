// RelayChannelSetupDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "KTST Alarm.h"
#include "RelayChannelSetupDlg.h"
#include "afxdialogex.h"
#include "SelectChannelDlg.h"


// CRelayChannelSetupDlg �Ի���

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


// CRelayChannelSetupDlg ��Ϣ�������
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

	//ͨ������
	m_strChannelName = m_RelayChannel.m_strChannelName;

	//�����ʱ
	m_nOutputDelay = m_RelayChannel.m_nOutputDelay;

	//���
	CButton *pButton = (CButton *)GetDlgItem(IDC_ENERGIZED);
	pButton->SetCheck(m_RelayChannel.m_bEnergized);

	//��·
	pButton = (CButton *)GetDlgItem(IDC_BYPASS);
	pButton->SetCheck(m_RelayChannel.m_bBypass);


	//���ʽ
	RefreshExpressionDisplayInfo (TRUE);


	//�޸ı��ʽ�༭������
// 	CFont *pFont = m_ctrlExpressionEditor.GetFont();
// 	LOGFONT lg;
// 	pFont->GetLogFont(&lg);
// 	lg.lfWeight = FW_BOLD;	//�Ӵ�
// 	if (m_fontEditor.m_hObject != NULL)
// 		m_fontEditor.DeleteObject();
// 	m_fontEditor.CreateFontIndirect(&lg);
// 	m_ctrlExpressionEditor.SetFont(&m_fontEditor, TRUE);

	UpdateData (FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

//ˢ�±��ʽ�ڱ༭������ʾ��Ϣ
void CRelayChannelSetupDlg::RefreshExpressionDisplayInfo (BOOL bRefreshString)
{
	//�ڴ���ˢ�±��ʽ����ʾ��Ϣ
	CString strText;
	int nTextLen;
	int nIndex = 0;	//��¼���ʽ��������(��1��ʼ)

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
		pExpression->nIndexBegin = nIndex + 1;			//��ʼ������
		pExpression->nIndexEnd = nIndex + nTextLen;		//����������

		nIndex += nTextLen;
	}

	//��������ʾ���ʽ
	ShowExpression ();
}

//�õ����ʽ����ʾ�ַ���
CString CRelayChannelSetupDlg::GetExpressionDisplayInfo (const RelayExpression &exp)
{
	CString strDisplay;

	switch (exp.eExpressionType)
	{
	case etOperator:
		{
			if (exp.strOperator == _T("("))
				strDisplay = _T("( ");		//��ʾʱ�ӿո�
			else if (exp.strOperator == _T(")"))
				strDisplay = _T(") ");		//��ʾʱ�ӿո�
			else if (exp.strOperator == _T("&"))
				strDisplay = _T("  &  ");		//��ʾʱ�ӿո�
			else if (exp.strOperator == _T("|"))
				strDisplay = _T("  |  ");		//��ʾʱ�ӿո�
			else
				strDisplay = exp.strOperator;	//ԭʼ��ʽ
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

//�����豸�ŵõ��ɼ��豸����
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

//�����豸��ͨ���ŵõ��ɼ�ͨ������
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

//�����豸��ͨ���ŵõ��ɼ�ͨ������
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

//������ʾ���ʽ
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
	m_ctrlExpressionEditor.SetSel (m_nCurCursorPos, m_nCurCursorPos);		//���ù��λ��

	ShowExpressionStatusInfo ();		//��ʾ���ʽ״̬��Ϣ
}

//��ʾ���ʽ״̬��Ϣ
void CRelayChannelSetupDlg::ShowExpressionStatusInfo ()
{
	CString strLoad, strText, strStatus;

	int nExpressionLen = m_RelayChannel.m_listExpression.GetCount ();

	//�жϱ��ʽ�Ƿ���Ч
 	m_bExpressionValid = IsExpressionValid ();
	if (m_bExpressionValid)
		strStatus.LoadString(IDS_VALID);
	else
		strStatus.LoadString(IDS_INVALID);

	strLoad.LoadString(IDS_EXP_STATUS_TEMPLATE);
	strText.Format(strLoad, nExpressionLen, _MAX_RELAY_EXPRESSION_LEN, strStatus);

	SetDlgItemText (IDC_EXPRESSION_STATUS_INFO, strText);
}

//�жϱ��ʽ�Ƿ���Ч
BOOL CRelayChannelSetupDlg::IsExpressionValid ()
{
	if (m_RelayChannel.m_listExpression.GetCount () <= 0)
		return TRUE;

	//�жϱ��ʽ���Ƿ�����Чͨ��
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

//����һ�����ʽԪ��
void CRelayChannelSetupDlg::InsertExpression (const RelayExpression &expNew)
{
	CString strDisplay = GetExpressionDisplayInfo(expNew);
	m_nCurCursorPos += strDisplay.GetLength ();		//��������������λ��

	RelayExpression *pExpression = new RelayExpression();

	//�������ʽԪ����Ϣ
	pExpression->eExpressionType = expNew.eExpressionType;
	pExpression->strOperator = expNew.strOperator;
	pExpression->nDeviceNo = expNew.nDeviceNo;
	pExpression->nChannelNo = expNew.nChannelNo;

	pExpression->strDisplay = strDisplay;

	//�����������е�ָ��λ��
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

	//ˢ�±��ʽ����������ƴ�ַ���
	RefreshExpressionDisplayInfo (FALSE);
}

//���ݱ��ʽ�ı���ĵ�ǰ���λ���ҵ���Ӧͨ�����ʽ�����е�λ��
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

//���ͨ�����ʽEdit������㵽��һ�����ʽ���м䣬������Ƶ���ǰ���ʽ�ĺ���
//						���ѡ���˶�����ʽ���򽫹���Ƶ����һ����ѡ���ʽ�ĺ���
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

//�жϼ̵������ʽ�༭�Ƿ�ﵽ���ֵ
BOOL CRelayChannelSetupDlg::IsExpressionEditorFull ()
{
	if (m_RelayChannel.m_listExpression.GetCount() >= _MAX_RELAY_EXPRESSION_LEN)
		return TRUE;
	
	return FALSE;
}

BOOL CRelayChannelSetupDlg::PreTranslateMessage(MSG* pMsg)
{
	//���ʽ�༭��
	if (pMsg->hwnd == m_ctrlExpressionEditor.m_hWnd)
	{
		//���ε��Ҽ�����
		if (pMsg->message == WM_RBUTTONUP)
			return TRUE;

		//���ε�������˫��
		if (pMsg->message == WM_LBUTTONDBLCLK)
			return TRUE;

		//������̧��
		if (pMsg->message == WM_LBUTTONUP)
			this->MoveCursorToExpElementEnd ();		//������Ƶ�ѡ����Ԫ�صĺ���

		//����̧��	(�����ϡ��¡�����)
		if (pMsg->message == WM_KEYUP && (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_UP || pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_DOWN))
			this->MoveCursorToExpElementEnd ();		//������Ƶ�ѡ��Ԫ�صĺ���
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


HBRUSH CRelayChannelSetupDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (IDC_EXPRESSION == pWnd->GetDlgCtrlID())
	{
		if (m_bExpressionValid)
			pDC->SetTextColor(RGB(0, 0, 255));	//��Ч������ɫ
		else
			pDC->SetTextColor(RGB(200, 0, 0));	//��Ч������ɫ
	}

	return hbr;
}

//ѡ�񱨾�ͨ��
void CRelayChannelSetupDlg::OnBnClickedSelectChannel()
{
	if (IsExpressionEditorFull())
		return;

	CSelectChannelDlg dlg;

	dlg.SetDeviceListPtr(m_pCollectionDeviceList);
	dlg.SetChannelListPtr(m_pCollectionChannelList);

	if (dlg.DoModal() == IDOK)
	{
		//��ѡ�еĲɼ�ͨ��ѭ�����뵽��ע�б���
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

//������
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

//������
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

//��
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

//��
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

//��
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

//ɾ��
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

		RefreshExpressionDisplayInfo (FALSE);	//ˢ�����б��ʽԪ�ص�����
	}

	m_ctrlExpressionEditor.SetFocus();
}


void CRelayChannelSetupDlg::OnBnClickedOk()
{
	if (!UpdateData(TRUE))
		return;

	//**	�Ƚ��кϷ��Լ��	***************************

	//ͨ������
	m_strChannelName.Trim();
	if (m_strChannelName.IsEmpty())
	{
		AfxMessageBox(IDS_CH_NAME_NULL);
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_RELAY_CHANNEL_NAME);
		pEdit->SetWindowText(_T(""));
		pEdit->SetFocus ();
		return;
	}

	//���ʽ��ֵ
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


	//**	����ͨ������	*******************************

	//ͨ������
	m_RelayChannel.m_strChannelName = m_strChannelName;

	//�����ʱ
	m_RelayChannel.m_nOutputDelay = m_nOutputDelay;

	//���
	CButton *pButton = (CButton *)GetDlgItem(IDC_ENERGIZED);
	m_RelayChannel.m_bEnergized = pButton->GetCheck();

	//��·
	pButton = (CButton *)GetDlgItem(IDC_BYPASS);
	m_RelayChannel.m_bBypass = pButton->GetCheck();

	//���ʽ
	//���ʽ�ڱ༭ʱֱ���޸ĵ�m_RelayChannel�ı��ʽ����m_listExpression�����Բ����ٱ���

	m_RelayChannel.SaveRelayChannelBuffer();

	CDialogEx::OnOK();
}
