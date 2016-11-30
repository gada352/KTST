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

//Timer�Ļص�������
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
//	this->SetSockOpt(SO_BROADCAST, &bBroadcast, sizeof(BOOL)); //���ô�����Socket֧�ֹ㲥��

	//���ջ�����;
	int nRecvBuf=MAX_RECV_BUFFER_SIZE;
	this->SetSockOpt(SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));

	//���ͻ�����;
	int nSendBuf=MAX_SEND_BUFFER_SIZE;
	this->SetSockOpt(SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));	

	//���óɲ��������շ����������Socket�ر�ʱʹ��ǿ�йر�;
	BOOL bDontLinger = TRUE; //�����ϴ󲿷ֶ���bDontLinger���ó�FALSE�ģ����Է���û�����ϱϸ���MSDN�Ľ���Ӧ�ý��������ı������ó�TRUE������⣻
	this->SetSockOpt(SO_DONTLINGER,(const char*)&bDontLinger,sizeof(BOOL));

	g_pCommunicate = this;

	if(!(m_nTimeID = ::SetTimer(NULL, m_nTimeID, COMMUNICATE_TIMER_ELAPSE, CommTimerProc))) //�ص�������
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
//		AsyncSelect(0); //UDP���͵�Socket���Ϻ��������⣬������Tcp��̬���ڹرյĴ���ü��ϣ�����������⣻

		this->Close();
	}
	if(m_nTimeID)
	{
		::KillTimer(NULL, m_nTimeID);
		m_nTimeID = 0;
	}
}

//�������ͨѶ�ṹ�����
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
	if(pComm) //�����־Ϊ��ʼ״̬��֤����Ч�����Լ������ã����ٿ�����
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

//	�����豸��Monitor����Server��ʵʱ����ֵ����(���豸Ϊ��λ);
int CCommunicate::AppProtocol_1(LPCTSTR lpszTargetAddr,UINT wTargetPort, int nDeviceNo)
{
	if(this->m_hSocket == INVALID_SOCKET )
		return COMMUNICATE_ERROR_FAILURE;	


	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//��ϵͳ��ʶ
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//Ԥ��1���ֽ�

	//�Ӱ�ͷ��
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_REQUIRE_ANSWER;//��Ҫ�Է�Ӧ��ģ�
	nPackLen++;
	DWORD dwContext = ::GetTickCount();   //1��Э����Ҫһ���Է�ȷ�ϵ�ʱ�����Ҫ��Է���Ӧʱ��������
	*(DWORD *)(m_packBuf + nPackLen) = dwContext;
	nPackLen += 4;



	m_packBuf[nPackLen] = 1; //1��Э��
	nPackLen++;

	*(int *)(m_packBuf + nPackLen) = nDeviceNo;
	nPackLen += 4;

	int iRes = SendTo(m_packBuf, nPackLen, wTargetPort, lpszTargetAddr);	
	
	if( iRes < 0)   
	{
		iRes = ::GetLastError();
		return COMMUNICATE_ERROR_FAILURE;
	}

	//Ϊ���ط�����������Ϣ��
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

//Server�˸��������͸��豸����ͨ����״̬������ֵ�ͱ�������;
int CCommunicate::AppProtocol_2(LPCTSTR lpszTargetAddr,UINT wTargetPort, int nDeviceNo,byte byChCnt, int nDataBufLen, byte *pbyDataBuf)
{
	if(this->m_hSocket == INVALID_SOCKET )
		return COMMUNICATE_ERROR_FAILURE;

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//��ϵͳ��ʶ
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//Ԥ��1���ֽ�

	//�Ӱ�ͷ��
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //����ҪӦ��ģ�
	nPackLen++;
	nPackLen += 4; //Ԥ��4���ֽڣ�


	if (nDataBufLen > 0 && pbyDataBuf == NULL)
		return COMMUNICATE_ERROR_FAILURE;

	m_packBuf[nPackLen] = 2; //2��Э��
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

//ICM��Server�˱���������ÿ��10s����һ������;
int CCommunicate::AppProtocol_3(LPCTSTR lpszTargetAddr,UINT wTargetPort, int nDeviceNo)
{
	if(this->m_hSocket == INVALID_SOCKET )
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//��ϵͳ��ʶ
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//Ԥ��1���ֽ�

	//�Ӱ�ͷ��
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //����ҪӦ��ģ�
	nPackLen++;
	nPackLen += 4; //Ԥ��4���ֽڣ�



	m_packBuf[nPackLen] = 3; //3��Э��
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

//Monitor��Server�˷��Ϳ�������;
int CCommunicate::AppProtocol_4(LPCTSTR lpszTargetAddr,UINT wTargetPort, int nDeviceNo,byte byChNo,byte byCtrType,byte byCodeNo)
{
	if(this->m_hSocket == INVALID_SOCKET )
		return COMMUNICATE_ERROR_FAILURE;

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//��ϵͳ��ʶ
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//Ԥ��1���ֽ�

	//�Ӱ�ͷ��
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //����ҪӦ��ģ�
	nPackLen++;
	nPackLen += 4; //Ԥ��4���ֽڣ�



	m_packBuf[nPackLen] = 4; //4��Э��
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

//Server�˸��ݿ����豸����ICMת���������
int CCommunicate::AppProtocol_5(LPCTSTR lpszTargetAddr,UINT wTargetPort, int nDeviceNo,byte byChNo,byte byCtrType,byte byCodeNo)
{
	if(this->m_hSocket == INVALID_SOCKET )
		return COMMUNICATE_ERROR_FAILURE;

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//��ϵͳ��ʶ
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//Ԥ��1���ֽ�

	//�Ӱ�ͷ��
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //����ҪӦ��ģ�
	nPackLen++;
	nPackLen += 4; //Ԥ��4���ֽڣ�




	m_packBuf[nPackLen] = 5; //5��Э��
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

//ICM�����豸�յ���������ظ�Server���յ������Ӧ��;
int CCommunicate::AppProtocol_6(LPCTSTR lpszTargetAddr,UINT wTargetPort, int nDeviceNo,byte byChNo)
{
	if(this->m_hSocket == INVALID_SOCKET )
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//��ϵͳ��ʶ
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//Ԥ��1���ֽ�

	//�Ӱ�ͷ��
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //����ҪӦ��ģ�
	nPackLen++;
	nPackLen += 4; //Ԥ��4���ֽڣ�




	m_packBuf[nPackLen] = 6; //6��Э��
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

//Server�˽����������Ӧ��ת����Monitor�ˣ�
int CCommunicate::AppProtocol_7(LPCTSTR lpszTargetAddr,UINT wTargetPort, int nDeviceNo,byte byChNo)
{
	if(this->m_hSocket == INVALID_SOCKET )
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//��ϵͳ��ʶ
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//Ԥ��1���ֽ�

	//�Ӱ�ͷ��
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //����ҪӦ��ģ�
	nPackLen++;
	nPackLen += 4; //Ԥ��4���ֽڣ�




	m_packBuf[nPackLen] = 7; //7��Э��
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

//��Server��������֤��¼���
int CCommunicate::AppProtocol_21(LPCTSTR lpszTargetAddr, UINT wTargetPort, const CString &strUserName, const CString &strPassword, byte byOptCode)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	int nLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//��ϵͳ��ʶ
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//Ԥ��1���ֽ�

	//�Ӱ�ͷ��
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //����ҪӦ��ģ�
	nPackLen ++;
	nPackLen += 4; //Ԥ��4���ֽڣ�


	//Э���
	m_packBuf[nPackLen] = 21;
	nPackLen ++;

	//�û�����
	nLen = 64;
	memset (m_packBuf + nPackLen, 0x0, nLen*sizeof(byte));
	memcpy (m_packBuf + nPackLen, (LPCTSTR )strUserName, min(nLen, int (strUserName.GetLength()*sizeof(TCHAR))));
	nPackLen += nLen;

	//�û�����
	nLen = 64;
	memset (m_packBuf + nPackLen, 0x0, nLen*sizeof(byte));
	memcpy (m_packBuf + nPackLen, (LPCTSTR )strPassword, min(nLen, int (strPassword.GetLength()*sizeof(TCHAR))));
	nPackLen += nLen;

	//���β�����
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

//Server�˻ظ���֤��¼���
int CCommunicate::AppProtocol_22(LPCTSTR lpszTargetAddr, UINT wTargetPort, byte byOptCode, byte byPass, int nUserID, byte byUserRight)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//��ϵͳ��ʶ
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//Ԥ��1���ֽ�

	//�Ӱ�ͷ��
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //����ҪӦ��ģ�
	nPackLen ++;
	nPackLen += 4; //Ԥ��4���ֽڣ�


	//Э���
	m_packBuf[nPackLen] = 22;
	nPackLen ++;

	//���β�����
	m_packBuf[nPackLen] = byOptCode;
	nPackLen += sizeof(byte);

	//��֤��¼�Ƿ�ͨ��
	m_packBuf[nPackLen] = byPass;
	nPackLen += sizeof(byte);

	//�û�ID
	*(int *)(m_packBuf + nPackLen) = nUserID;
	nPackLen += sizeof(int);

	//�û�Ȩ��
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

//��Server�����󵼺�����(����Ϣ)
int CCommunicate::AppProtocol_23(LPCTSTR lpszTargetAddr, UINT wTargetPort, int nUserID)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//��ϵͳ��ʶ
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//Ԥ��1���ֽ�

	//�Ӱ�ͷ��
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //����ҪӦ��ģ�
	nPackLen ++;
	nPackLen += 4; //Ԥ��4���ֽڣ�


	//Э���
	m_packBuf[nPackLen] = 23;
	nPackLen ++;

	//�û�ID
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

//Server�˻ظ���������
int CCommunicate::AppProtocol_24(LPCTSTR lpszTargetAddr, UINT wTargetPort, int nUserID, int nDataBufLen, byte *pbyDataBuf)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//��ϵͳ��ʶ
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//Ԥ��1���ֽ�

	//�Ӱ�ͷ��
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //����ҪӦ��ģ�
	nPackLen ++;
	nPackLen += 4; //Ԥ��4���ֽڣ�


	if (nDataBufLen > 0 && pbyDataBuf == NULL)
		return COMMUNICATE_ERROR_FAILURE;

	//Э���
	m_packBuf[nPackLen] = 24;
	nPackLen ++;

	//�û�ID
	*(int *)(m_packBuf + nPackLen) = nUserID;
	nPackLen += sizeof(int);

	//���ݳ���
	*(int *)(m_packBuf + nPackLen) = nDataBufLen;
	nPackLen += sizeof(int);

	//����
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

//��Server������ͼ����
int CCommunicate::AppProtocol_25(LPCTSTR lpszTargetAddr, UINT wTargetPort, int nPlotID)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//��ϵͳ��ʶ
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//Ԥ��1���ֽ�

	//�Ӱ�ͷ��
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //����ҪӦ��ģ�
	nPackLen ++;
	nPackLen += 4; //Ԥ��4���ֽڣ�


	//Э���
	m_packBuf[nPackLen] = 25;
	nPackLen ++;

	//ͼID
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

//Server�˻ظ�ͼ����
int CCommunicate::AppProtocol_26(LPCTSTR lpszTargetAddr, UINT wTargetPort, int nPlotID, int nDataBufLen, byte *pbyDataBuf)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//��ϵͳ��ʶ
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//Ԥ��1���ֽ�

	//�Ӱ�ͷ��
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //����ҪӦ��ģ�
	nPackLen ++;
	nPackLen += 4; //Ԥ��4���ֽڣ�


	if (nDataBufLen > 0 && pbyDataBuf == NULL)
		return COMMUNICATE_ERROR_FAILURE;

	//Э���
	m_packBuf[nPackLen] = 26;
	nPackLen ++;

	//ͼID
	*(int *)(m_packBuf + nPackLen) = nPlotID;
	nPackLen += sizeof(int);

	//���ݳ���
	*(int *)(m_packBuf + nPackLen) = nDataBufLen;
	nPackLen += sizeof(int);

	//����
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

//��Server������ͨ����Ϣ
int CCommunicate::AppProtocol_27(LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChNo)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//��ϵͳ��ʶ
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//Ԥ��1���ֽ�

	//�Ӱ�ͷ��
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //����ҪӦ��ģ�
	nPackLen ++;
	nPackLen += 4; //Ԥ��4���ֽڣ�


	//Э���
	m_packBuf[nPackLen] = 27;
	nPackLen ++;

	//�豸���
	*(int *)(m_packBuf + nPackLen) = nDeviceNo;
	nPackLen += sizeof(int);

	//ͨ����
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

//Server�˻ظ�ͨ����Ϣ
int CCommunicate::AppProtocol_28(LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChNo, int nDeviceType, 
								TCHAR szDeviceName[], byte byChType, TCHAR szChName[], byte byWorkEnable, int nDataBufLen, 
								byte *pbyDataBuf)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	int nLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//��ϵͳ��ʶ
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//Ԥ��1���ֽ�

	//�Ӱ�ͷ��
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //����ҪӦ��ģ�
	nPackLen ++;
	nPackLen += 4; //Ԥ��4���ֽڣ�


	if (nDataBufLen > 0 && pbyDataBuf == NULL)
		return COMMUNICATE_ERROR_FAILURE;

	//Э���
	m_packBuf[nPackLen] = 28;
	nPackLen ++;

	//�豸���
	*(int *)(m_packBuf + nPackLen) = nDeviceNo;
	nPackLen += sizeof(int);

	//ͨ����
	m_packBuf[nPackLen] = byChNo;
	nPackLen += sizeof(byte);

	//�豸����
	*(int *)(m_packBuf + nPackLen) = nDeviceType;
	nPackLen += sizeof(int);

	//�豸����
	nLen = 64;
	memset (m_packBuf + nPackLen, 0x0, nLen*sizeof(byte));
	memcpy (m_packBuf + nPackLen, szDeviceName, nLen);
	nPackLen += nLen;

	//ͨ������
	m_packBuf[nPackLen] = byChType;
	nPackLen += sizeof(byte);

	//ͨ������
	nLen = 64;
	memset (m_packBuf + nPackLen, 0x0, nLen*sizeof(byte));
	memcpy (m_packBuf + nPackLen, szChName, nLen);
	nPackLen += nLen;

	//��������
	m_packBuf[nPackLen] = byWorkEnable;
	nPackLen += sizeof(byte);
	
	//���ݳ���
	*(int *)(m_packBuf + nPackLen) = nDataBufLen;
	nPackLen += sizeof(int);

	//����
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

//�ƶ�APP����Server��ʵʱ����ֵ����(���豸Ϊ��λ);
int CCommunicate::AppProtocol_29(LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//��ϵͳ��ʶ
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//Ԥ��1���ֽ�

	//�Ӱ�ͷ��
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //����ҪӦ��ģ�
	nPackLen ++;
	nPackLen += 4; //Ԥ��4���ֽڣ�


	//Э���
	m_packBuf[nPackLen] = 29;
	nPackLen ++;

	//�豸���
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

//Server�˸��������͸��豸����ͨ����״̬������ֵ�ͱ�������;
int CCommunicate::AppProtocol_30(LPCTSTR lpszTargetAddr, UINT wTargetPort, int nDeviceNo, byte byChCnt, int nDataBufLen, byte *pbyDataBuf)
{
	if (this->m_hSocket == INVALID_SOCKET)
		return COMMUNICATE_ERROR_FAILURE;	

	int nPackLen = 0;
	memset (m_packBuf, 0x0, sizeof(m_packBuf));

	//��ϵͳ��ʶ
	memcpy (m_packBuf, m_szSysFlag, 4);
	nPackLen += 4;
	nPackLen ++;		//Ԥ��1���ֽ�

	//�Ӱ�ͷ��
	m_packBuf[nPackLen] = COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER; //����ҪӦ��ģ�
	nPackLen ++;
	nPackLen += 4; //Ԥ��4���ֽڣ�


	if (nDataBufLen > 0 && pbyDataBuf == NULL)
		return COMMUNICATE_ERROR_FAILURE;

	//Э���
	m_packBuf[nPackLen] = 30;
	nPackLen ++;

	//�豸���
	*(int *)(m_packBuf + nPackLen) = nDeviceNo;
	nPackLen += sizeof(int);

	//ͨ������
	m_packBuf[nPackLen] = byChCnt;
	nPackLen += sizeof(byte);

	//����
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

	if (nPackLen < COMMUNICATE_MAX_HEAD_LEN)	//��ͷ������
		return COMMUNICATE_ERROR_PACK_LEN;

	for (int i=0; i<4; i++)		//ͨѶ���е�ϵͳ��ʶ����
	{
		if (pPack[i] != m_szSysFlag[i])
			return COMMUNICATE_ERROR_SYS_FLAG;
	}

	PCOMMUNICATESTRUCT pComm;
	POSITION pos;

	switch(pPack[5])
	{
	case COMMUNICATE_PACK_PROPERTY_DEFAULT: //����Ҫ���ģ�
		break;

	case COMMUNICATE_PACK_PROPERTY_UNREQUIRE_ANSWER: //����ҪӦ��
		break;

	case COMMUNICATE_PACK_PROPERTY_REQUIRE_ANSWER: //��ҪӦ��
		{		
			BYTE bySend[COMMUNICATE_MAX_HEAD_LEN];
			memset (bySend, 0x0, sizeof(bySend));

			//��ϵͳ��ʶ
			memcpy (bySend, m_szSysFlag, 4);

			//Ӧ���־��ʱ���
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

	case COMMUNICATE_PACK_PROPERTY_ANSWER: //�Է�Ӧ��ģ�		
		{
			pos = m_commList.GetHeadPosition();
			while(pos)
			{
				pComm = m_commList.GetNext(pos);
				
				if(pComm->nFlag != COMMUNICATE_STRUCT_FLAG_SEND)
					continue;
				
				if(pComm->dwContext != *(DWORD *)(pPack + 6)) //Ҫ��Ӧ���������Ƿ��Ӧ�ϣ�
					continue;
				
				if(pComm->strTargetAddr == strTargetAddr && pComm->uTargetPort == uTargetPort) //�Ƿ�Ϊ������Ŀ���ַ��Ӧ�ģ�
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
	::memcpy(g_UdpCommObj.byRecBuff,pPack+COMMUNICATE_MAX_HEAD_LEN,nPackLen-COMMUNICATE_MAX_HEAD_LEN); //��COMMUNICATE_MAX_HEAD_LEN���ֽڵİ�ͷȥ����

	nPackLen = nPackLen - COMMUNICATE_MAX_HEAD_LEN;
	LRESULT lResult = ::SendMessage(m_pRecWnd->m_hWnd, WM_UDP_RECEIVE, (WPARAM)&g_UdpCommObj, (LPARAM)nPackLen);

	return COMMUNICATE_SUCCESS;
}
