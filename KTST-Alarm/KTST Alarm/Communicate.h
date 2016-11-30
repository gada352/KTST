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
#define COMMUNICATE_ERROR_PACK_LEN					3002	//包长错误
#define COMMUNICATE_ERROR_SYS_FLAG					3003	//通讯包中系统标识错误

#define COMMUNICATE_MAX_DGRAM_LEN					17000   //最大包长；
#define COMMUNICATE_MAX_HEAD_LEN					10		//包头长； 

#define COMMUNICATE_SOCKET_PORT						8111
#define MAX_RECV_BUFFER_SIZE						0x7fffffff //必须大，否则开实时图形多了，通道会长时间丢数;      
#define MAX_SEND_BUFFER_SIZE						0x7fffffff 

#define COMMUNICATE_REPEAT_TIMES					1

#define COMMUNICATE_SEND_WAIT_TIME					500	//原来是200，考虑到网页客户端比较慢；
#define COMMUNICATE_RECEIVE_WAIT_TIME				1000
#define COMMUNICATE_TIMER_ELAPSE					50

#define MESUREVALUE_NODE_LEN						7 //[ChNo(1byte)、通道状态（1byte，0 正常、1不工作、2故障  ）、测量值（4byte的float）、报警级别（1byte）]


//接收通讯结构体对象类型；
#define COMMUNICATE_STRUCT_FLAG_DEFAULT				0
#define COMMUNICATE_STRUCT_FLAG_SEND				1
#define COMMUNICATE_STRUCT_FLAG_RECEIVE				2

#define COMMUNICATE_PACK_PROPERTY_DEFAULT			0
#define COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER	1 //不需要对方回应；
#define COMMUNICATE_PACK_PROPERTY_REQUIRE_ANSWER	2 //需要对方回应；
#define COMMUNICATE_PACK_PROPERTY_ANSWER			3 //对方来的回应；


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

	char m_szSysFlag[4];		//通讯中的系统标志，放在包头中防止通讯中接收到乱数据包

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
	
	/////////////////////////////下面是协议实现函数，协议的前五个字节是为了应答预留的//////////////////////////////////

	//	控制设备或Monitor端向Server端实时测量值请求(以设备为单位);
	int AppProtocol_1 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo);

	//Server端根据请求发送该设备所有通道的状态、测量值和报警级别;
	int AppProtocol_2 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChCnt, int nDataBufLen, byte *pbyDataBuf);

	//ICM向Server端报告心跳，每隔10s发送一次心跳;
	int AppProtocol_3 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo);

	//Monitor向Server端发送控制命令;
	int AppProtocol_4 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChNo, byte byCtrType, byte byCodeNo);

	//Server端根据控制设备号向ICM转发控制命令；
	int AppProtocol_5 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChNo, byte byCtrType, byte byCodeNo);

	//ICM控制设备收到控制命令返回给Server端收到命令的应答;
	int AppProtocol_6 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChNo);

	//Server端将控制命令的应答转交给Monitor端；
	int AppProtocol_7 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChNo);


	//向Server端请求验证登录身份
	int AppProtocol_21 (LPCTSTR lpszTargetAddr, UINT wTargetPort, const CString &strUserName, const CString &strPassword, byte byOptCode);

	//Server端回复验证登录身份
	int AppProtocol_22 (LPCTSTR lpszTargetAddr, UINT wTargetPort, byte byOptCode, byte byPass, int nUserID, byte byUserRight);

	//向Server端请求导航数据(树信息)
	int AppProtocol_23 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nUserID);

	//Server端回复导航数据
	int AppProtocol_24 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nUserID, int nDataBufLen, byte *pbyDataBuf);

	//向Server端请求图数据
	int AppProtocol_25 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nPlotID);

	//Server端回复图数据
	int AppProtocol_26 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nPlotID, int nDataBufLen, byte *pbyDataBuf);

	//向Server端请求通道信息
	int AppProtocol_27 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChNo);

	//Server端回复通道信息
	int AppProtocol_28 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChNo, int nDeviceType, TCHAR szDeviceName[], 
						byte byChType, TCHAR szChName[], byte byWorkEnable, int nDataBufLen, byte *pbyDataBuf);

	//移动APP端向Server端实时测量值请求(以设备为单位);
	int AppProtocol_29 (LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo);

	//Server端根据请求发送该设备所有通道的状态、测量值和报警级别;
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
