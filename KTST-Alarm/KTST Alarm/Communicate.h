#if !defined(AFX_COMMUNICATE_H__FBA19E89_4366_4456_96A5_6E4FA481834D__INCLUDED_)
#define AFX_COMMUNICATE_H__FBA19E89_4366_4456_96A5_6E4FA481834D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Communicate.h : header file
#include <afxsock.h>
#include <afxtempl.h>

#define COMMUNICATE_SUCCESS							0
#define COMMUNICATE_ERROR_FAILURE					3001
#define COMMUNICATE_ERROR_PACK_LEN					3002	//��������
#define COMMUNICATE_ERROR_SYS_FLAG					3003	//ͨѶ����ϵͳ��ʶ����

#define COMMUNICATE_MAX_DGRAM_LEN					17000   //��������
#define COMMUNICATE_MAX_HEAD_LEN					10		//��ͷ���� 

#define COMMUNICATE_SOCKET_PORT						8111
#define MAX_RECV_BUFFER_SIZE						0x7fffffff //����󣬷���ʵʱͼ�ζ��ˣ�ͨ���᳤ʱ�䶪��;      
#define MAX_SEND_BUFFER_SIZE						0x7fffffff 

#define COMMUNICATE_REPEAT_TIMES					1

#define COMMUNICATE_SEND_WAIT_TIME					500	//ԭ����200�����ǵ���ҳ�ͻ��˱Ƚ�����
#define COMMUNICATE_RECEIVE_WAIT_TIME				1000
#define COMMUNICATE_TIMER_ELAPSE					50

#define MESUREVALUE_NODE_LEN						7 //[ChNo(1byte)��ͨ��״̬��1byte��0 ������1��������2����  ��������ֵ��4byte��float������������1byte��]


//����ͨѶ�ṹ��������ͣ�
#define COMMUNICATE_STRUCT_FLAG_DEFAULT				0
#define COMMUNICATE_STRUCT_FLAG_SEND				1
#define COMMUNICATE_STRUCT_FLAG_RECEIVE				2

#define COMMUNICATE_PACK_PROPERTY_DEFAULT			0
#define COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER	1 //����Ҫ�Է���Ӧ��
#define COMMUNICATE_PACK_PROPERTY_REQUIRE_ANSWER	2 //��Ҫ�Է���Ӧ��
#define COMMUNICATE_PACK_PROPERTY_ANSWER			3 //�Է����Ļ�Ӧ��


typedef struct tagUdpComm	
{
	BYTE	byRecBuff[17000];
	CString	 strRequstIP;
	UINT	 uRequstPort;	
}tagUdpComm;

/////////////////////////////////////////////////////////////////////////////
// CCommunicate command target

class CCommunicate : public CAsyncSocket
{
// Attributes
public:

private:
	typedef struct _communicateStruct
	{
		CString strTargetAddr;
		UINT uTargetPort;
		DWORD dwContext;
		int nRepeatTimes;
		DWORD dwStartingTime;
		int nFlag;

		int nBuffLen;
		BYTE buff[COMMUNICATE_MAX_DGRAM_LEN];
	
		_communicateStruct()
		{
			strTargetAddr = _T("");
			uTargetPort = COMMUNICATE_SOCKET_PORT;
			nRepeatTimes = COMMUNICATE_REPEAT_TIMES;
			dwStartingTime = 0;
			nFlag = COMMUNICATE_STRUCT_FLAG_DEFAULT;
			nBuffLen = 0;
			memset(buff, 0, COMMUNICATE_MAX_DGRAM_LEN);
		}
	}COMMUNICATESTRUCT, *PCOMMUNICATESTRUCT;
	
	typedef CTypedPtrList<CPtrList, PCOMMUNICATESTRUCT>CCommList;


	int m_nTimeID;
	CWnd *m_pRecWnd;

	BYTE m_packBuf[COMMUNICATE_MAX_DGRAM_LEN];
	CCommList m_commList; 

	BYTE m_frameBuf[COMMUNICATE_MAX_DGRAM_LEN];

	char m_szSysFlag[4];		//ͨѶ�е�ϵͳ��־�����ڰ�ͷ�з�ֹͨѶ�н��յ������ݰ�

private:
	PCOMMUNICATESTRUCT GetFreeCommstruct();
	int ReceivePack(BYTE *pPack, int nPackLen, CString strTargetAddr,UINT uTargetPort);


// Operations
public:
	CCommunicate();
	virtual ~CCommunicate();

	int CommunicateTimer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);


	BOOL Initialize(UINT uUDPPort = 0); 
	void UnInitialize();
	void SetRecWnd(CWnd *pWnd);
	
	/////////////////////////////������Э��ʵ�ֺ�����Э���ǰ����ֽ���Ϊ��Ӧ��Ԥ����//////////////////////////////////

	//	�����豸��Monitor����Server��ʵʱ����ֵ����(���豸Ϊ��λ);
	int AppProtocol_1 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo);

	//Server�˸��������͸��豸����ͨ����״̬������ֵ�ͱ�������;
	int AppProtocol_2 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChCnt, int nDataBufLen, byte *pbyDataBuf);

	//ICM��Server�˱���������ÿ��10s����һ������;
	int AppProtocol_3 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo);

	//Monitor��Server�˷��Ϳ�������;
	int AppProtocol_4 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChNo, byte byCtrType, byte byCodeNo);

	//Server�˸��ݿ����豸����ICMת���������
	int AppProtocol_5 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChNo, byte byCtrType, byte byCodeNo);

	//ICM�����豸�յ���������ظ�Server���յ������Ӧ��;
	int AppProtocol_6 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChNo);

	//Server�˽����������Ӧ��ת����Monitor�ˣ�
	int AppProtocol_7 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChNo);


	//��Server��������֤��¼���
	int AppProtocol_21 (LPCTSTR lpszTargetAddr, UINT wTargetPort, const CString &strUserName, const CString &strPassword, byte byOptCode);

	//Server�˻ظ���֤��¼���
	int AppProtocol_22 (LPCTSTR lpszTargetAddr, UINT wTargetPort, byte byOptCode, byte byPass, int nUserID, byte byUserRight);

	//��Server�����󵼺�����(����Ϣ)
	int AppProtocol_23 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nUserID);

	//Server�˻ظ���������
	int AppProtocol_24 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nUserID, int nDataBufLen, byte *pbyDataBuf);

	//��Server������ͼ����
	int AppProtocol_25 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nPlotID);

	//Server�˻ظ�ͼ����
	int AppProtocol_26 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nPlotID, int nDataBufLen, byte *pbyDataBuf);

	//��Server������ͨ����Ϣ
	int AppProtocol_27 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChNo);

	//Server�˻ظ�ͨ����Ϣ
	int AppProtocol_28 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChNo, int nDeviceType, TCHAR szDeviceName[], 
						byte byChType, TCHAR szChName[], byte byWorkEnable, int nDataBufLen, byte *pbyDataBuf);

	//�ƶ�APP����Server��ʵʱ����ֵ����(���豸Ϊ��λ);
	int AppProtocol_29 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo);

	//Server�˸��������͸��豸����ͨ����״̬������ֵ�ͱ�������;
	int AppProtocol_30 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChCnt, int nDataBufLen, byte *pbyDataBuf);


// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommunicate)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CCommunicate)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMUNICATE_H__FBA19E89_4366_4456_96A5_6E4FA481834D__INCLUDED_)
