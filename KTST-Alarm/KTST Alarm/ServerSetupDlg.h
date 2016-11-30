#pragma once


// CServerSetupDlg �Ի���

class CServerSetupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CServerSetupDlg)

public:
	CServerSetupDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CServerSetupDlg();

public:
	void SetServerInfo (CString strServerName, CString strServerPort, CString strDBName);
	void GetServerInfo (CString &strServerName, CString &strServerPort, CString &strDBName);


private:
	CString m_strServerName;
	CString m_strServerPort;
	CString m_strDBName;

// �Ի�������
	enum { IDD = IDD_SERVER_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
