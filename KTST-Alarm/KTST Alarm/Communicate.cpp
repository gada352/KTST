// Communicate.cpp : implementation file
//

#include "stdafx.h"
#include "Communicate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

tagUdpComm g_UdpCommObj;

//Timer的回调函数；
CCommunicate *g_pCommunicate = NULL;
void CALLBACK CommTimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if(g_pCommunicate)
		g_pCommunicate->CommunicateTimer(hWnd, uMsg, idEvent, dwTime);
}


CCommunicate::CCommunicate()
{
	m_nTimeID = 0;
	m_pRecWnd = NULL;

	m_szSysFlag[0] = 'K';
	m_szSysFlag[1] = 'T';
	m_szSysFlag[2] = 'S';
	m_szSysFlag[3] = 'T';
}

CCommunicate::~CCommunicate()
{
	UnInitialize();
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CCommunicate, CAsyncSocket)
	//{{AFX_MSG_MAP(CCommunicate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CCommunicate member functions
BOOL CCommunicate::Initialize(UINT uUDPPort)
{
	if(!AfxSocketInit())
	{
		::AfxMessageBox(_T("Initialize socket failure!"));
		return FALSE;
	}
	

	if(!Create(uUDPPort, SOCK_DGRAM))
	{
		::AfxMessageBox(_T("The current UDP port is being used,\n please login CFG software to reset it again!"));
		return FALSE;
	}

//	BOOL bBroadcast = TRUE;
//	this->SetSockOpt(SO_BROADCAST, &bBroadcast, sizeof(BOOL)); //设置创建的Socket支持广播；

	//接收缓冲区;
	int nRecvBuf=MAX_RECV_BUFFER_SIZE;
	this->SetSockOpt(SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));

	//发送缓冲区;
	int nSendBuf=MAX_SEND_BUFFER_SIZE;
	this->SetSockOpt(SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));	

	//设置成不论正在收发数据与否在Socket关闭时使用强行关闭;
	BOOL bDontLinger = TRUE; //在网上大部分都将bDontLinger设置成FALSE的，测试发现没区别，老毕根据MSDN的解释应该将不逗留的变量设置成TRUE更好理解；
	this->SetSockOpt(SO_DONTLINGER,(const char*)&bDontLinger,sizeof(BOOL));

	g_pCommunicate = this;

	if(!(m_nTimeID = ::SetTimer(NULL, m_nTimeID, COMMUNICATE_TIMER_ELAPSE, CommTimerProc))) //回调函数；
	{
		m_nTimeID = 0;
		return FALSE;
	}

	return TRUE;
}

void CCommunicate::UnInitialize()
{
	PCOMMUNICATESTRUCT pComm;
	while(m_commList.GetCount() > 0)
	{
		pComm = m_commList.RemoveHead();
		delete pComm;
	}

	if(this->m_hSocket != INVALID_SOCKET ) 
	{
//		AsyncSelect(0); //UDP类型的Socket加上好象有问题，但是在Tcp动态库内关闭的代码得加上，否则会有问题；

		this->Close();
	}
	if(m_nTimeID)
	{
		::KillTimer(NULL, m_nTimeID);
		m_nTimeID = 0;
	}
}

//申请接收通讯结构体对象；
CCommunicate::PCOMMUNICATESTRUCT CCommunicate::GetFreeCommstruct()
{
	PCOMMUNICATESTRUCT pComm = NULL;
	POSITION pos = m_commList.GetHeadPosition();
	while(pos)
	{
		pComm = m_commList.GetNext(pos);
		if(pComm->nFlag == COMMUNICATE_STRUCT_FLAG_DEFAULT)
		{
			break;
		}
		pComm = NULL;
	}
	if(pComm) //如果标志为初始状态，证明无效，可以继续利用，减少开销；
	{
		pComm->strTargetAddr = _T("");
		pComm->uTargetPort = COMMUNICATE_SOCKET_PORT;
		pComm->nRepeatTimes = COMMUNICATE_REPEAT_TIMES;
		pComm->dwStartingTime = 0;
		pComm->nFlag = COMMUNICATE_STRUCT_FLAG_DEFAULT;
		memset(pComm->buff, 0,COMMUNICATE_MAX_DGRAM_LEN);
	}
	else
	{
		pComm = new COMMUNICATESTRUCT;
		m_commList.AddTail(pComm);
	}

	return pComm;
}


void CCommunicate::SetRecWnd(CWnd *pWnd)
{
	m_pRecWnd = pWnd; 
}

int CCommunicate::CommunicateTimer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	int iRes;

	PCOMMUNICATESTRUCT pComm;
	POSITION pos = m_commList.GetHeadPosition();
	while(pos)
	{
		pComm = m_commList.GetNext(pos);

		switch(pComm->nFlag)
		{
		case COMMUNICATE_STRUCT_FLAG_SEND:
			{
				if(int(dwTime - pComm->dwStartingTime) > COMMUNICATE_SEND_WAIT_TIME)
				{
					if(pComm->nRepeatTimes > 0)
					{
						iRes = SendTo(pComm->buff, pComm->nBuffLen, pComm->uTargetPort, pComm->strTargetAddr);
						if(iRes <0)
						{
							pComm->nFlag = COMMUNICATE_STRUCT_FLAG_DEFAULT;
							
							break;
						}						
						
						pComm->dwStartingTime = ::GetTickCount();
						pComm->nRepeatTimes--;
					}
					else
						pComm->nFlag = COMMUNICATE_STRUCT_FLAG_DEFAULT;
					
				}
				
				break;
			}
			
		case COMMUNICATE_STRUCT_FLAG_DEFAULT:
		default:
			;
		}
	}

	return COMMUNICATE_SUCCESS;
}

//	控制设备或Monitor端向Server端实时测量值请求(以设备为单位);
int CCommunicate::AppProtocol_1(LPCTSTR lpszTargetAddr,UINT wTargetPort, int nDeviceNo)
{
	if(this->m_hSocket == INVALID_SOCKET )
		return COMMUNICATE_ERROR_FAILURE;	


	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//加系统标识
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//预留1个字节

	//加包头；
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_REQUIRE_ANSWER;//需要对方应答的；
	nPackLen++;
	DWORD dwContext = ::GetTickCount();   //1号协议需要一个对方确认的时间戳，要求对方回应时带回来；
	*(DWORD *)(m_packBuf + nPackLen) = dwContext;
	nPackLen += 4;



	m_packBuf[nPackLen] = 1; //1号协议
	nPackLen++;

	*(int *)(m_packBuf + nPackLen) = nDeviceNo;
	nPackLen += 4;

	int iRes = SendTo(m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	
	
	if( iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}

	//为了重发保留发送信息；
	PCOMMUNICATESTRUCT pComm = NULL;
	pComm = GetFreeCommstruct();
	VERIFY(pComm);
	pComm->strTargetAddr = lpszTargetAddr;
	pComm->uTargetPort = wTargetPort;
	pComm->nFlag = COMMUNICATE_STRUCT_FLAG_SEND;
	pComm->dwStartingTime = ::GetTickCount();
	pComm->dwContext = dwContext;
	pComm->nBuffLen = nPackLen;
	::memcpy(pComm->buff,m_packBuf,nPackLen);

	return iRes;
}

//Server端根据请求发送该设备所有通道的状态、测量值和报警级别;
int CCommunicate::AppProtocol_2(LPCTSTR lpszTargetAddr,UINT wTargetPort, int nDeviceNo,byte byChCnt, int nDataBufLen, byte *pbyDataBuf)
{
	if(this->m_hSocket == INVALID_SOCKET )
		return COMMUNICATE_ERROR_FAILURE;

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//加系统标识
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//预留1个字节

	//加包头；
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //不需要应答的；
	nPackLen++;
	nPackLen += 4; //预留4个字节；


	if (nDataBufLen > 0 && pbyDataBuf == NULL)
		return COMMUNICATE_ERROR_FAILURE;

	m_packBuf[nPackLen] = 2; //2号协议
	nPackLen++;

	*(int *)(m_packBuf + nPackLen) = nDeviceNo;
	nPackLen += 4;

	*(byte *)(m_packBuf + nPackLen) = byChCnt;
	nPackLen += 1;

	if (nDataBufLen > 0)
	{
		memcpy(m_packBuf + nPackLen, pbyDataBuf, nDataBufLen*sizeof(byte));
		nPackLen += nDataBufLen*sizeof(byte);
	}

	int iRes = SendTo(m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	
	
	if( iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}


	return iRes;
}

//ICM向Server端报告心跳，每隔10s发送一次心跳;
int CCommunicate::AppProtocol_3(LPCTSTR lpszTargetAddr,UINT wTargetPort, int nDeviceNo)
{
	if(this->m_hSocket == INVALID_SOCKET )
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//加系统标识
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//预留1个字节

	//加包头；
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //不需要应答的；
	nPackLen++;
	nPackLen += 4; //预留4个字节；



	m_packBuf[nPackLen] = 3; //3号协议
	nPackLen++;

	*(int *)(m_packBuf + nPackLen) = nDeviceNo;
	nPackLen += 4;

	int iRes = SendTo(m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	
	
	if( iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}


	return iRes;
}

//Monitor向Server端发送控制命令;
int CCommunicate::AppProtocol_4(LPCTSTR lpszTargetAddr,UINT wTargetPort, int nDeviceNo,byte byChNo,byte byCtrType,byte byCodeNo)
{
	if(this->m_hSocket == INVALID_SOCKET )
		return COMMUNICATE_ERROR_FAILURE;

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//加系统标识
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//预留1个字节

	//加包头；
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //不需要应答的；
	nPackLen++;
	nPackLen += 4; //预留4个字节；



	m_packBuf[nPackLen] = 4; //4号协议
	nPackLen++;

	*(int *)(m_packBuf + nPackLen) = nDeviceNo;
	nPackLen += 4;

	*(byte *)(m_packBuf + nPackLen) = byChNo;
	nPackLen += 1;

	*(byte *)(m_packBuf + nPackLen) = byCtrType;
	nPackLen += 1;

	*(byte *)(m_packBuf + nPackLen) = byCodeNo;
	nPackLen += 1;

	int iRes = SendTo(m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	

	if( iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}


	return iRes;
}

//Server端根据控制设备号向ICM转发控制命令；
int CCommunicate::AppProtocol_5(LPCTSTR lpszTargetAddr,UINT wTargetPort, int nDeviceNo,byte byChNo,byte byCtrType,byte byCodeNo)
{
	if(this->m_hSocket == INVALID_SOCKET )
		return COMMUNICATE_ERROR_FAILURE;

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//加系统标识
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//预留1个字节

	//加包头；
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //不需要应答的；
	nPackLen++;
	nPackLen += 4; //预留4个字节；




	m_packBuf[nPackLen] = 5; //5号协议
	nPackLen++;

	*(int *)(m_packBuf + nPackLen) = nDeviceNo;
	nPackLen += 4;

	*(byte *)(m_packBuf + nPackLen) = byChNo;
	nPackLen += 1;

	*(byte *)(m_packBuf + nPackLen) = byCtrType;
	nPackLen += 1;

	*(byte *)(m_packBuf + nPackLen) = byCodeNo;
	nPackLen += 1;

	int iRes = SendTo(m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	

	if( iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}


	return iRes;
}

//ICM控制设备收到控制命令返回给Server端收到命令的应答;
int CCommunicate::AppProtocol_6(LPCTSTR lpszTargetAddr,UINT wTargetPort, int nDeviceNo,byte byChNo)
{
	if(this->m_hSocket == INVALID_SOCKET )
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//加系统标识
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//预留1个字节

	//加包头；
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //不需要应答的；
	nPackLen++;
	nPackLen += 4; //预留4个字节；




	m_packBuf[nPackLen] = 6; //6号协议
	nPackLen++;

	*(int *)(m_packBuf + nPackLen) = nDeviceNo;
	nPackLen += 4;

	*(byte *)(m_packBuf + nPackLen) = byChNo;
	nPackLen += 1;

	int iRes = SendTo(m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	
	
	if( iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}


	return iRes;
}

//Server端将控制命令的应答转交给Monitor端；
int CCommunicate::AppProtocol_7(LPCTSTR lpszTargetAddr,UINT wTargetPort, int nDeviceNo,byte byChNo)
{
	if(this->m_hSocket == INVALID_SOCKET )
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//加系统标识
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//预留1个字节

	//加包头；
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //不需要应答的；
	nPackLen++;
	nPackLen += 4; //预留4个字节；




	m_packBuf[nPackLen] = 7; //7号协议
	nPackLen++;

	*(int *)(m_packBuf + nPackLen) = nDeviceNo;
	nPackLen += 4;

	*(byte *)(m_packBuf + nPackLen) = byChNo;
	nPackLen += 1;

	int iRes = SendTo(m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	
	
	if( iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}


	return iRes;
}

//向Server端请求验证登录身份
int CCommunicate::AppProtocol_21(LPCTSTR lpszTargetAddr, UINT wTargetPort, const CString &strUserName, const CString &strPassword, byte byOptCode)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	int nLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//加系统标识
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//预留1个字节

	//加包头；
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //不需要应答的；
	nPackLen ++;
	nPackLen += 4; //预留4个字节；


	//协议号
	m_packBuf[nPackLen] = 21;
	nPackLen ++;

	//用户名称
	nLen = 64;
	memset (m_packBuf + nPackLen, 0x0, nLen*sizeof(byte));
	memcpy (m_packBuf + nPackLen, (LPCTSTR )strUserName, min(nLen, int (strUserName.GetLength()*sizeof(TCHAR))));
	nPackLen += nLen;

	//用户密码
	nLen = 64;
	memset (m_packBuf + nPackLen, 0x0, nLen*sizeof(byte));
	memcpy (m_packBuf + nPackLen, (LPCTSTR )strPassword, min(nLen, int (strPassword.GetLength()*sizeof(TCHAR))));
	nPackLen += nLen;

	//本次操作码
	m_packBuf[nPackLen] = byOptCode;
	nPackLen += sizeof(byte);


	int iRes = SendTo (m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	

	if (iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}

	return iRes;
}

//Server端回复验证登录身份
int CCommunicate::AppProtocol_22(LPCTSTR lpszTargetAddr, UINT wTargetPort, byte byOptCode, byte byPass, int nUserID, byte byUserRight)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//加系统标识
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//预留1个字节

	//加包头；
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //不需要应答的；
	nPackLen ++;
	nPackLen += 4; //预留4个字节；


	//协议号
	m_packBuf[nPackLen] = 22;
	nPackLen ++;

	//本次操作码
	m_packBuf[nPackLen] = byOptCode;
	nPackLen += sizeof(byte);

	//验证登录是否通过
	m_packBuf[nPackLen] = byPass;
	nPackLen += sizeof(byte);

	//用户ID
	*(int *)(m_packBuf + nPackLen) = nUserID;
	nPackLen += sizeof(int);

	//用户权限
	m_packBuf[nPackLen] = byUserRight;
	nPackLen += sizeof(byte);


	int iRes = SendTo (m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	

	if (iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}

	return iRes;
}

//向Server端请求导航数据(树信息)
int CCommunicate::AppProtocol_23(LPCTSTR lpszTargetAddr, UINT wTargetPort, int nUserID)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//加系统标识
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//预留1个字节

	//加包头；
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //不需要应答的；
	nPackLen ++;
	nPackLen += 4; //预留4个字节；


	//协议号
	m_packBuf[nPackLen] = 23;
	nPackLen ++;

	//用户ID
	*(int *)(m_packBuf + nPackLen) = nUserID;
	nPackLen += sizeof(int);


	int iRes = SendTo (m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	

	if (iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}

	return iRes;
}

//Server端回复导航数据
int CCommunicate::AppProtocol_24(LPCTSTR lpszTargetAddr, UINT wTargetPort, int nUserID, int nDataBufLen, byte *pbyDataBuf)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//加系统标识
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//预留1个字节

	//加包头；
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //不需要应答的；
	nPackLen ++;
	nPackLen += 4; //预留4个字节；


	if (nDataBufLen > 0 && pbyDataBuf == NULL)
		return COMMUNICATE_ERROR_FAILURE;

	//协议号
	m_packBuf[nPackLen] = 24;
	nPackLen ++;

	//用户ID
	*(int *)(m_packBuf + nPackLen) = nUserID;
	nPackLen += sizeof(int);

	//数据长度
	*(int *)(m_packBuf + nPackLen) = nDataBufLen;
	nPackLen += sizeof(int);

	//数据
	if (nDataBufLen > 0)
	{
		memcpy (m_packBuf + nPackLen, pbyDataBuf, nDataBufLen*sizeof(byte));
		nPackLen += nDataBufLen*sizeof(byte);
	}


	int iRes = SendTo (m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	

	if (iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}

	return iRes;
}

//向Server端请求图数据
int CCommunicate::AppProtocol_25(LPCTSTR lpszTargetAddr, UINT wTargetPort, int nPlotID)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//加系统标识
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//预留1个字节

	//加包头；
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //不需要应答的；
	nPackLen ++;
	nPackLen += 4; //预留4个字节；


	//协议号
	m_packBuf[nPackLen] = 25;
	nPackLen ++;

	//图ID
	*(int *)(m_packBuf + nPackLen) = nPlotID;
	nPackLen += sizeof(int);


	int iRes = SendTo (m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	

	if (iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}

	return iRes;
}

//Server端回复图数据
int CCommunicate::AppProtocol_26(LPCTSTR lpszTargetAddr, UINT wTargetPort, int nPlotID, int nDataBufLen, byte *pbyDataBuf)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//加系统标识
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//预留1个字节

	//加包头；
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //不需要应答的；
	nPackLen ++;
	nPackLen += 4; //预留4个字节；


	if (nDataBufLen > 0 && pbyDataBuf == NULL)
		return COMMUNICATE_ERROR_FAILURE;

	//协议号
	m_packBuf[nPackLen] = 26;
	nPackLen ++;

	//图ID
	*(int *)(m_packBuf + nPackLen) = nPlotID;
	nPackLen += sizeof(int);

	//数据长度
	*(int *)(m_packBuf + nPackLen) = nDataBufLen;
	nPackLen += sizeof(int);

	//数据
	if (nDataBufLen > 0)
	{
		memcpy (m_packBuf + nPackLen, pbyDataBuf, nDataBufLen*sizeof(byte));
		nPackLen += nDataBufLen*sizeof(byte);
	}


	int iRes = SendTo (m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	

	if (iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}

	return iRes;
}

//向Server端请求通道信息
int CCommunicate::AppProtocol_27(LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChNo)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//加系统标识
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//预留1个字节

	//加包头；
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //不需要应答的；
	nPackLen ++;
	nPackLen += 4; //预留4个字节；


	//协议号
	m_packBuf[nPackLen] = 27;
	nPackLen ++;

	//设备编号
	*(int *)(m_packBuf + nPackLen) = nDeviceNo;
	nPackLen += sizeof(int);

	//通道号
	m_packBuf[nPackLen] = byChNo;
	nPackLen += sizeof(byte);


	int iRes = SendTo (m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	

	if (iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}

	return iRes;
}

//Server端回复通道信息
int CCommunicate::AppProtocol_28(LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChNo, int nDeviceType, 
								TCHAR szDeviceName[], byte byChType, TCHAR szChName[], byte byWorkEnable, int nDataBufLen, 
								byte *pbyDataBuf)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	int nLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//加系统标识
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//预留1个字节

	//加包头；
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //不需要应答的；
	nPackLen ++;
	nPackLen += 4; //预留4个字节；


	if (nDataBufLen > 0 && pbyDataBuf == NULL)
		return COMMUNICATE_ERROR_FAILURE;

	//协议号
	m_packBuf[nPackLen] = 28;
	nPackLen ++;

	//设备编号
	*(int *)(m_packBuf + nPackLen) = nDeviceNo;
	nPackLen += sizeof(int);

	//通道号
	m_packBuf[nPackLen] = byChNo;
	nPackLen += sizeof(byte);

	//设备类型
	*(int *)(m_packBuf + nPackLen) = nDeviceType;
	nPackLen += sizeof(int);

	//设备名称
	nLen = 64;
	memset (m_packBuf + nPackLen, 0x0, nLen*sizeof(byte));
	memcpy (m_packBuf + nPackLen, szDeviceName, nLen);
	nPackLen += nLen;

	//通道类型
	m_packBuf[nPackLen] = byChType;
	nPackLen += sizeof(byte);

	//通道名称
	nLen = 64;
	memset (m_packBuf + nPackLen, 0x0, nLen*sizeof(byte));
	memcpy (m_packBuf + nPackLen, szChName, nLen);
	nPackLen += nLen;

	//工作允许
	m_packBuf[nPackLen] = byWorkEnable;
	nPackLen += sizeof(byte);
	
	//数据长度
	*(int *)(m_packBuf + nPackLen) = nDataBufLen;
	nPackLen += sizeof(int);

	//数据
	if (nDataBufLen > 0)
	{
		memcpy (m_packBuf + nPackLen, pbyDataBuf, nDataBufLen*sizeof(byte));
		nPackLen += nDataBufLen*sizeof(byte);
	}


	int iRes = SendTo (m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	

	if (iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}

	return iRes;
}

//移动APP端向Server端实时测量值请求(以设备为单位);
int CCommunicate::AppProtocol_29(LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//加系统标识
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//预留1个字节

	//加包头；
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //不需要应答的；
	nPackLen ++;
	nPackLen += 4; //预留4个字节；


	//协议号
	m_packBuf[nPackLen] = 29;
	nPackLen ++;

	//设备编号
	*(int *)(m_packBuf + nPackLen) = nDeviceNo;
	nPackLen += sizeof(int);


	int iRes = SendTo (m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	

	if (iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}

	return iRes;
}

//Server端根据请求发送该设备所有通道的状态、测量值和报警级别;
int CCommunicate::AppProtocol_30(LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChCnt, int nDataBufLen, byte *pbyDataBuf)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//加系统标识
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//预留1个字节

	//加包头；
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //不需要应答的；
	nPackLen ++;
	nPackLen += 4; //预留4个字节；


	if (nDataBufLen > 0 && pbyDataBuf == NULL)
		return COMMUNICATE_ERROR_FAILURE;

	//协议号
	m_packBuf[nPackLen] = 30;
	nPackLen ++;

	//设备编号
	*(int *)(m_packBuf + nPackLen) = nDeviceNo;
	nPackLen += sizeof(int);

	//通道个数
	m_packBuf[nPackLen] = byChCnt;
	nPackLen += sizeof(byte);

	//数据
	if (nDataBufLen > 0)
	{
		memcpy (m_packBuf + nPackLen, pbyDataBuf, nDataBufLen*sizeof(byte));
		nPackLen += nDataBufLen*sizeof(byte);
	}


	int iRes = SendTo (m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	

	if (iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}

	return iRes;
}

void CCommunicate::OnReceive(int nErrorCode) 
{
	int iRes;
	CString strIPAddr;
	UINT uPort;
	CString str, str1;

	if((iRes = ReceiveFrom(m_frameBuf, COMMUNICATE_MAX_DGRAM_LEN, strIPAddr, uPort)) >= 0)
	{
	//	str.Format(_T("%03d    "), iRes);

		iRes = ReceivePack(m_frameBuf, iRes, strIPAddr,uPort);
		if(iRes != COMMUNICATE_SUCCESS)
		{
		//	str.Format(_T("Receive error: %d"), iRes);
			//::AfxMessageBox(str);
		}
	}
	else
	{
		iRes = ::GetLastError();
	}
	
	CAsyncSocket::OnReceive(nErrorCode);
}


int CCommunicate::ReceivePack(BYTE *pPack, int nPackLen, CString strTargetAddr,UINT uTargetPort)
{
	if (pPack == NULL)
		return COMMUNICATE_ERROR_FAILURE;

	if (nPackLen < COMMUNICATE_MAX_HEAD_LEN)	//包头长错误
		return COMMUNICATE_ERROR_PACK_LEN;

	for (int i=0; i<4; i++)		//通讯包中的系统标识错误
	{
		if (pPack[i] != m_szSysFlag[i])
			return COMMUNICATE_ERROR_SYS_FLAG;
	}

	PCOMMUNICATESTRUCT pComm;
	POSITION pos;

	switch(pPack[5])
	{
	case COMMUNICATE_PACK_PROPERTY_DEFAULT: //不需要理会的；
		break;

	case COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER: //不需要应答
		break;

	case COMMUNICATE_PACK_PROPERTY_REQUIRE_ANSWER: //需要应答；
		{		
			BYTE bySend[COMMUNICATE_MAX_HEAD_LEN];
			memset (bySend, 0x0, sizeof(bySend));

			//加系统标识
			memcpy (bySend, m_szSysFlag, 4);

			//应答标志和时间戳
			bySend[5] = COMMUNICATE_PACK_PROPERTY_ANSWER;
			::memcpy(bySend+6,pPack+6,sizeof(DWORD));

			int iRes = SendTo(bySend, COMMUNICATE_MAX_HEAD_LEN, uTargetPort, strTargetAddr);	
			
			if( iRes < 0)   
			{
				iRes = ::GetLastError();
				return COMMUNICATE_ERROR_FAILURE;
			}
			
		}	
		break;

	case COMMUNICATE_PACK_PROPERTY_ANSWER: //对方应答的；		
		{
			pos = m_commList.GetHeadPosition();
			while(pos)
			{
				pComm = m_commList.GetNext(pos);
				
				if(pComm->nFlag != COMMUNICATE_STRUCT_FLAG_SEND)
					continue;
				
				if(pComm->dwContext != *(DWORD *)(pPack + 6)) //要回应的上下文是否对应上；
					continue;
				
				if(pComm->strTargetAddr == strTargetAddr && pComm->uTargetPort == uTargetPort) //是否为所发送目标地址回应的；
				{
					pComm->nFlag = COMMUNICATE_STRUCT_FLAG_DEFAULT;
					
					return COMMUNICATE_SUCCESS;
				}
			}
		}		
		
		break;		
	}

	if(!m_pRecWnd)
		return COMMUNICATE_ERROR_FAILURE;

	g_UdpCommObj.strRequstIP = strTargetAddr;
	g_UdpCommObj.uRequstPort = uTargetPort;
	::memcpy(g_UdpCommObj.byRecBuff,pPack+COMMUNICATE_MAX_HEAD_LEN,nPackLen-COMMUNICATE_MAX_HEAD_LEN); //将COMMUNICATE_MAX_HEAD_LEN个字节的包头去掉；

	nPackLen = nPackLen - COMMUNICATE_MAX_HEAD_LEN;
	LRESULT lResult = ::SendMessage(m_pRecWnd->m_hWnd, WM_UDP_RECEIVE, (WPARAM)&g_UdpCommObj, (LPARAM)nPackLen);

	return COMMUNICATE_SUCCESS;
}
