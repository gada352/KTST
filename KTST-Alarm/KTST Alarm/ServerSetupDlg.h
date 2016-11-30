#pragma once


// CServerSetupDlg 对话框

class CServerSetupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CServerSetupDlg)

public:
	CServerSetupDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CServerSetupDlg();

public:
	void SetServerInfo (CString strServerName, CString strServerPort, CString strDBName);
	void GetServerInfo (CString &strServerName, CString &strServerPort, CString &strDBName);


private:
	CString m_strServerName;
	CString m_strServerPort;
	CString m_strDBName;

// 对话框数据
	enum { IDD = IDD_SERVER_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
