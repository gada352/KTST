#include "StdAfx.h"
#include "RelayChannel.h"

#import <msxml3.dll>


RelayChannel::RelayChannel(void)
{
	m_nOutputDelay = 0;
	m_bEnergized = FALSE;
	m_bBypass = FALSE;
	m_bLatching = FALSE;

	m_bDelayStarting = FALSE;
	m_dwDelayStartTime = 0;

	m_nLastOutputValue = 0;

	m_bExpressionUpdate = TRUE;
}


RelayChannel::~RelayChannel(void)
{
	ClearMemory();
}

BOOL RelayChannel::Initialize ()
{
	if (!Channel::Initialize())
		return FALSE;

	AnalyzeRelayChannelBuffer();

	return TRUE;
}

BOOL RelayChannel::UnInitialize ()
{
	if (!Channel::UnInitialize())
		return FALSE;

	return TRUE;
}

//����֪ͨ
void RelayChannel::OnNotify (NotifyCode code, WPARAM wParam, LPARAM lParam)
{
	Channel::OnNotify(code, wParam, lParam);

	switch (code)
	{
// 	case ncChannelStateChange:		//ͨ��״̬�����ı�
	case ncAlarmLevelChange:		//ͨ�������������ı�
		{
			CollectionChannel *pChannel = (CollectionChannel *)wParam;
			if (pChannel == NULL)
				return;

			RelayExpression *pExpression = NULL;
			POSITION pos = m_listExpression.GetHeadPosition();
			while (pos)
			{
				pExpression = m_listExpression.GetNext(pos);

				if (pExpression	&& pExpression->eExpressionType == etChannel 
					&& pExpression->nDeviceNo == pChannel->m_nDeviceNo 
					&& pExpression->nChannelNo == pChannel->m_nChannelNo)
				{
					pExpression->byAlarmLevel = pChannel->m_byAlarmLevel;
					m_bExpressionUpdate = TRUE;		//���ʽ���б���״̬����
				}
			}
		}
		break;

	default:
		break;
	}
}

void RelayChannel::OnTimer ()
{
	Channel::OnTimer();

	//�ж��Ƿ��������ʱʱ���ѵ�
	if (m_bDelayStarting)
	{
		int nPassedTime = GetTickCount() - m_dwDelayStartTime;
		if (nPassedTime >= m_nOutputDelay*1000 && m_nOutputDelay > 0)
			m_bExpressionUpdate = TRUE;
	}
}

//�����̵����豸��Ϣ�����������豸�Զ��建�����е���Ϣ
BOOL RelayChannel::AnalyzeRelayChannelBuffer ()
{
	FreeObjPtrList(&m_listExpression);

	int nLen = m_bdDefBuf.GetCount();
	if (nLen <= 0)
		return TRUE;

	//�ӻ���������XML
	CString strXML((wchar_t *)m_bdDefBuf.GetData(), nLen/sizeof(wchar_t));


	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr;

	hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if (FAILED (hr))
		return FALSE;

	pDoc->loadXML((_bstr_t )strXML);

	MSXML2::IXMLDOMNodeListPtr pNodeList;
	pNodeList = pDoc->GetchildNodes();

	if (pNodeList->Getlength() == 0)
		return FALSE;

	MSXML2::IXMLDOMNodePtr rootNode;
	rootNode = pNodeList->Getitem(0);

	CString rootName = rootNode->GetnodeName();
	if (rootName != _T("Channel"))
		return FALSE;

	MSXML2::IXMLDOMNodeListPtr pChildNodeList;
	pChildNodeList = rootNode->GetchildNodes();
	int childNodeCount = pChildNodeList->Getlength();

	for (int i = 0; i < childNodeCount; i++)
	{
		MSXML2::IXMLDOMNodePtr childNode;
		childNode = pChildNodeList->Getitem(i);

		CString childNodeName = childNode->GetnodeName();
		CString childNodeXml = childNode->Getxml();

		if (childNodeName == _T("OutputDelay"))				//�����ʱ
		{
			CString strText = childNode->Gettext();
			m_nOutputDelay = _ttoi(strText);
		}

		if (childNodeName == _T("Energized"))				//���
		{
			CString strText = childNode->Gettext();
			m_bEnergized = strText.Compare(_T("0"));
		}

		if (childNodeName == _T("Bypass"))					//��·
		{
			CString strText = childNode->Gettext();
			m_bBypass = strText.Compare(_T("0"));
		}

		if (childNodeName == _T("Latching"))				//����
		{
			CString strText = childNode->Gettext();
			m_bLatching = strText.Compare(_T("0"));
		}

		if (childNodeName == _T("Expression"))				//���ʽ
		{
			MSXML2::IXMLDOMNodeListPtr pChannelNodeList;
			pChannelNodeList = childNode->GetchildNodes();

			int expNodeCount = pChannelNodeList->Getlength();

			for (int j = 0; j < expNodeCount; j++)
			{
				MSXML2::IXMLDOMNodePtr expNode;
				expNode = pChannelNodeList->Getitem(j);

				CString expNodeName = expNode->GetnodeName();

				if (expNodeName == _T("Operator"))				//����
				{
					RelayExpression *pExpression = new RelayExpression();
					pExpression->eExpressionType = etOperator;		//������
					CString strText = expNode->Gettext();
 					pExpression->strOperator = strText;
					m_listExpression.AddTail(pExpression);
				}

				if (expNodeName == _T("FollowCh"))
				{
					MSXML2::IXMLDOMNamedNodeMapPtr pAttrs = expNode->Getattributes();

					MSXML2::IXMLDOMNodePtr pAttrDeviceNo = pAttrs->getNamedItem(_T("DeviceNo"));
					CString strDeviceNo = pAttrDeviceNo->GetnodeValue().bstrVal;

					MSXML2::IXMLDOMNodePtr pAttrChannelNo = pAttrs->getNamedItem(_T("ChannelNo"));
					CString strChannelNo = pAttrChannelNo->GetnodeValue().bstrVal;

					int nDeviceNo = _ttoi(strDeviceNo);
					int nChannelNo = _ttoi(strChannelNo);

					RelayExpression *pExpression = new RelayExpression();
					pExpression->eExpressionType = etChannel;		//��ע��ͨ��
					pExpression->nDeviceNo = nDeviceNo;
					pExpression->nChannelNo = nChannelNo;
					m_listExpression.AddTail(pExpression);
				}
			}
		}
	}

	return TRUE;
}

//����̵����豸��Ϣ�����������豸�Զ��建�����е���Ϣ
BOOL RelayChannel::SaveRelayChannelBuffer ()
{
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr;
	CString strText;

	hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if (FAILED (hr))
		return FALSE;

	//ͨ��
	MSXML2::IXMLDOMElementPtr rootChannel = pDoc->createElement((_bstr_t )_T("Channel"));
	pDoc->appendChild(rootChannel);

	//�����ʱ
	MSXML2::IXMLDOMElementPtr docOutputDelay = pDoc->createElement((_bstr_t )_T("OutputDelay"));
	strText.Format(_T("%d"), m_nOutputDelay);
	docOutputDelay->Puttext((_bstr_t)strText);
	rootChannel->appendChild(docOutputDelay);

	//���
	MSXML2::IXMLDOMElementPtr docEnergized = pDoc->createElement((_bstr_t )_T("Energized"));
	strText = m_bEnergized ? _T("1"):_T("0");
	docEnergized->Puttext((_bstr_t)strText);
	rootChannel->appendChild(docEnergized);

	//��·
	MSXML2::IXMLDOMElementPtr docBypass = pDoc->createElement((_bstr_t )_T("Bypass"));
	strText = m_bBypass ? _T("1"):_T("0");
	docBypass->Puttext((_bstr_t)strText);
	rootChannel->appendChild(docBypass);

	//����
	MSXML2::IXMLDOMElementPtr docLatching = pDoc->createElement((_bstr_t )_T("Latching"));
	strText = m_bLatching ? _T("1"):_T("0");
	docLatching->Puttext((_bstr_t)strText);
	rootChannel->appendChild(docLatching);

	//���ʽ
	MSXML2::IXMLDOMElementPtr docExpression = pDoc->createElement((_bstr_t )_T("Expression"));
	rootChannel->appendChild(docExpression);

	MSXML2::IXMLDOMElementPtr docExpressionNode;
	RelayExpression *pExpression = NULL;
	POSITION pos = m_listExpression.GetHeadPosition();
	while (pos)
	{
		pExpression = m_listExpression.GetNext(pos);
		if (pExpression == NULL)
			continue;

		switch (pExpression->eExpressionType)
		{
		case etOperator:
			{
				docExpressionNode = pDoc->createElement((_bstr_t )_T("Operator"));
				docExpressionNode->Puttext((_bstr_t)pExpression->strOperator);
				docExpression->appendChild(docExpressionNode);
			}
			break;

		case etChannel:
			{
				docExpressionNode = pDoc->createElement((_bstr_t )_T("FollowCh"));
				strText.Format(_T("%d"), pExpression->nDeviceNo);
				docExpressionNode->setAttribute(_T("DeviceNo"), (_bstr_t )strText);
				strText.Format(_T("%d"), pExpression->nChannelNo);
				docExpressionNode->setAttribute(_T("ChannelNo"), (_bstr_t )strText);
				docExpression->appendChild(docExpressionNode);
			}
			break;
		}
	}


	//�������豸���Զ��建������
	CStringW strXML = pDoc->Getxml();
	m_bdDefBuf.SetData((byte *)(LPCWSTR )strXML, strXML.GetLength()*sizeof(wchar_t));

	return TRUE;
}

//�õ���ǰ�̵���ͨ��Ӧ�������ֵ
int RelayChannel::GetOutputValue ()
{
	int nCurOutputValue = 0;

	//ֻ��do while()ѭ��ִ��һ�Σ�Ϊ����;����ִ��break����
	do
	{
		if (m_bBypass)
		{
			nCurOutputValue = 0;
			break;
		}

		CString strExpression = GetExpressionString();
		if (strExpression.IsEmpty())
		{
			nCurOutputValue = 0;
			break;
		}

		int nExpressionValue;
		if (!Cal (strExpression, nExpressionValue))	//������ʽ��ֵ
		{
			nCurOutputValue = 0;
			break;
		}

		if (m_bLatching)	//����
		{
			if (nExpressionValue == 1)
			{
				nCurOutputValue = 1;

				if (m_nLastOutputValue == 0 && nCurOutputValue == 1)	//ֵ��0���1
				{
					m_bDelayStarting = TRUE;
					m_dwDelayStartTime = GetTickCount();
				}

				m_nLastOutputValue = nCurOutputValue;
			}
			else if (nExpressionValue == 0)
			{
				nCurOutputValue = m_nLastOutputValue;
			}
		}
		else	//������
		{
			nCurOutputValue = nExpressionValue;

			if (m_nLastOutputValue == 0 && nCurOutputValue == 1)	//ֵ��0���1
			{
				m_bDelayStarting = TRUE;
				m_dwDelayStartTime = GetTickCount();
			}

			if (m_nLastOutputValue == 1 && nCurOutputValue == 0)	//ֵ��1���0
				m_bDelayStarting = FALSE;

			m_nLastOutputValue = nCurOutputValue;
		}
	}while (0);

	//�ж��Ƿ�ﵽ��ʱʱ��
	if (nCurOutputValue == 1 && m_bDelayStarting)
	{
		int nPassedTime = GetTickCount() - m_dwDelayStartTime;
		if (nPassedTime >= m_nOutputDelay*1000)
			m_bDelayStarting = FALSE;
		else
			nCurOutputValue = 0;
	}

	//���
	if (m_bEnergized)
	{
		if (nCurOutputValue == 0)
			nCurOutputValue = 1;
		else if (nCurOutputValue == 1)
			nCurOutputValue = 0;
	}

	return nCurOutputValue;
}

//��λ����ͨ��
void RelayChannel::Reset ()
{
	if (m_bLatching)
		m_nLastOutputValue = 0;

	m_bExpressionUpdate = TRUE;
}

//����ڴ�
void RelayChannel::ClearMemory ()
{
	FreeObjPtrList(&m_listExpression);
}

//�������й�ע��ͨ������
void RelayChannel::RequestAllFollowingChannelData ()
{
	CDataCenter *pDataCenter = CDataCenter::GetInstance();
	if (pDataCenter == NULL)
		return;

	RelayExpression *pExpression = NULL;
	POSITION pos = m_listExpression.GetHeadPosition();
	while (pos)
	{
		pExpression = m_listExpression.GetNext(pos);
		if (pExpression && pExpression->eExpressionType == etChannel)
			pDataCenter->Request(pExpression->nDeviceNo, pExpression->nChannelNo);
	}
}

void RelayChannel::SetExpressionUpdate (BOOL bUpdate)
{
	m_bExpressionUpdate = bUpdate;
}

//�жϼ̵���ͨ���Ƿ������ݸ���
BOOL RelayChannel::IsExpressionUpdated ()
{
	return m_bExpressionUpdate;
}

//���̵���ͨ�����ʽƴ���ַ���
CString RelayChannel::GetExpressionString ()
{
	CString strExpression, strText;

	RelayExpression *pExpression = NULL;
	POSITION pos = m_listExpression.GetHeadPosition();
	while (pos)
	{
		pExpression = m_listExpression.GetNext(pos);
		if (pExpression == NULL)
			continue;

		switch (pExpression->eExpressionType)
		{
		case etOperator:
			strExpression += pExpression->strOperator;
			break;

		case etChannel:
			if (pExpression->byAlarmLevel > 0)
				strExpression += _T("1");
			else
				strExpression += _T("0");
			break;
		}
	}

	return strExpression;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BOOL Cal(CString str,int& nReturnValue)
//����ʱ�䣺
//		2006-8-21 12:48:00
//������
//		CString str[in]����ʾ����ı��ʽ��
//		int& nReturnValue[out]����ʾ������ֵ��
//���ܣ�
//		����̵���ͨ�����ʽ�ַ���(����ֵΪ�����Ƿ�ɹ� ���ɹ�����ܱ��ʽ����)(�ڲ�����)��
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL RelayChannel::Cal(CString str,int& nReturnValue)
{
	int i=0;//ѭ������

	int ExpStack[300];//յ����
	int State[300];//״̬����
	int ExpPre=-1,ExpNext=-1,curPos=-1;//���ʽ��ʼ�ͽ�����λ�� ��ǰյλ��
	bool IsPreUse=false;
	int khNum=0;//���ŵ�����


	for(i=0;i<300;i++)//��ʼ��
	{
		ExpStack[i]=0;
		State[i]=0;
	}

	int strLenth=str.GetLength();
	for(i=0;i<strLenth;i++)
	{	
		//ѹյ
		curPos=curPos+1;
		//��յ -1��ʾ�ǡ� ~ ��  ;-2��ʾ��" & "  ;-3��ʾ�� " | " ; -4��ʾ�� ; -5��ʾ��
		switch(str[i])
		{			
		case '~':
			if(curPos>0 && (ExpStack[curPos-1]==-5 || State[curPos-1]==0))//( ����
				return FALSE;
			ExpStack[curPos]=-1;
			State[curPos]=2;
			break;
		case '&':
			if(curPos>0 && (ExpStack[curPos-1]==-4 || State[curPos-1]==1 || State[curPos-1]==2))//( | & ~
				return FALSE;
			ExpStack[curPos]=-2;
			State[curPos]=1;
			break;
		case '|':
			if(curPos>0 && (ExpStack[curPos-1]==-4 || State[curPos-1]==1 || State[curPos-1]==2))//( | & ~
				return FALSE;
			ExpStack[curPos]=-3;
			State[curPos]=1;
			break;
		case '(':
			if(curPos>0 && (ExpStack[curPos-1]==-5 || State[curPos-1]==0))//) ����
				return FALSE;
			ExpStack[curPos]=-4;
			State[curPos]=3;
			khNum++;
			break;
		case ')':
			if(khNum==0)
				return FALSE;
			if(curPos>0 && ExpStack[curPos-1]==-4)//(
				return FALSE;						
			if(curPos>0 && (ExpStack[curPos-1]==-3 || ExpStack[curPos-1]==-2 || ExpStack[curPos-1]==-1))
				return FALSE;						//|						 //&						//~
			ExpStack[curPos]=-5;
			State[curPos]=4;
			khNum--;

			break;
		default :	
			if(curPos>0 && (State[curPos-1]==0 || ExpStack[curPos-1]==-5))//(
				return FALSE;
			ExpStack[curPos]=(char)str[i]-'0';
			State[curPos]=0;
			break;
		}

		if((ExpStack[0]==-3 || ExpStack[0]==-2 || ExpStack[0]==-5))//��һλ����Ϊ &|)
			return FALSE;

		//����
		if(State[curPos]>0)//�����ǰ���Ƿ���
		{
			if(!IsPreUse)//��ǰ��û�з���
			{	
				ExpPre=curPos;
				IsPreUse=TRUE;
			}
			else//��ǰ���������ſ��������Ƚ�����
			{
				ExpNext=curPos;
LOGO:	if(State[ExpPre]>=State[ExpNext])//�������ķ�������С�ڵ���ǰ���
		{	
			//	if(ExpNext-ExpPre==1 && ((State[ExpPre]==1) && (State[ExpNext]==1)))
			//		return FALSE;
			//	if(ExpNext-ExpPre==1 && ((State[ExpPre]==2) && (State[ExpNext]==1)))
			//		return FALSE;
			if(ExpNext-ExpPre==1 &&((State[ExpPre]==2) && (State[ExpNext]==2)))
			{
				ExpPre=ExpNext;
				continue;
			}

			switch(ExpStack[ExpPre])//�����ż���
			{
			case -1:
				ExpStack[ExpPre]=!ExpStack[ExpPre+1];	//~
				ExpStack[ExpPre+1]=ExpStack[ExpNext];
				ExpStack[ExpNext]=0;
				State[ExpPre]=0;
				State[ExpPre+1]=State[ExpNext];
				State[ExpNext]=0;
				curPos-=1;
				ExpNext=curPos;
				while(ExpPre>=0)//��ǰһ������
				{						
					if(State[ExpPre]>0)
						break;
					ExpPre-=1;
				}

				if(ExpPre<0)//���ǰ��û�з����˽�����ǰ���ֱ��ʽ�ļ��� ָ���һ�����ʽ
					ExpPre=curPos;
				else goto LOGO;
				break;
			case -2:
				ExpStack[ExpPre-1]=ExpStack[ExpPre-1] & ExpStack[ExpPre+1];//&
				ExpStack[ExpPre]=ExpStack[ExpNext];
				ExpStack[ExpNext]=0;
				ExpStack[ExpPre+1]=0;
				State[ExpPre-1]=0;
				State[ExpPre]=State[ExpNext];
				State[ExpNext]=0;
				State[ExpPre+1]=0;
				curPos-=2;
				ExpNext=curPos;
				ExpPre-=1;
				while(ExpPre>=0)//��ǰһ������
				{						
					if(State[ExpPre]>0)
						break;
					ExpPre-=1;
				}

				if(ExpPre<0)//���ǰ��û�з����˽�����ǰ���ֱ��ʽ�ļ��� ָ���һ�����ʽ
					ExpPre=curPos;
				else goto LOGO;
				break;
			case -3:
				ExpStack[ExpPre-1]=ExpStack[ExpPre-1] | ExpStack[ExpPre+1];//|
				ExpStack[ExpPre]=ExpStack[ExpNext];
				ExpStack[ExpNext]=0;
				ExpStack[ExpPre+1]=0;
				State[ExpPre-1]=0;
				State[ExpPre]=State[ExpNext];
				State[ExpNext]=0;
				State[ExpPre+1]=0;
				curPos-=2;
				ExpNext=curPos;	
				ExpPre-=1;
				while(ExpPre>=0)//��ǰһ������
				{						
					if(State[ExpPre]>0)
						break;
					ExpPre-=1;
				}

				if(ExpPre<0)//���ǰ��û�з����˽�����ǰ���ֱ��ʽ�ļ��� ָ���һ�����ʽ
					ExpPre=curPos;
				else goto LOGO;
				break;
			case -4:
				ExpPre=ExpNext;
				//	IsPreUse=false;
				break;
			}


		}
		else//��������ȸ���ǰ��� 
		{
			if(ExpStack[ExpNext]==-5)//����ǣ�
			{
				while(ExpStack[ExpPre]!=-4)//�����ǣ�ʱ
				{
					switch(ExpStack[ExpPre])//����
					{
					case -1:
						ExpStack[ExpPre]=!ExpStack[ExpPre+1];	//~
						ExpStack[ExpPre+1]=ExpStack[ExpNext];
						ExpStack[ExpNext]=0;
						State[ExpPre]=0;
						State[ExpPre+1]=State[ExpNext];
						State[ExpNext]=0;
						curPos-=1;
						ExpNext=curPos;

						break;
					case -2:
						ExpStack[ExpPre-1]=ExpStack[ExpPre-1] & ExpStack[ExpPre+1];//&
						ExpStack[ExpPre]=ExpStack[ExpNext];
						ExpStack[ExpNext]=0;
						ExpStack[ExpPre+1]=0;
						State[ExpPre-1]=0;
						State[ExpPre]=State[ExpNext];
						State[ExpNext]=0;
						State[ExpPre+1]=0;
						curPos-=2;
						ExpNext=curPos;
						ExpPre-=1;

						break;
					case -3:
						ExpStack[ExpPre-1]=ExpStack[ExpPre-1] | ExpStack[ExpPre+1];//|
						ExpStack[ExpPre]=ExpStack[ExpNext];
						ExpStack[ExpNext]=0;
						ExpStack[ExpPre+1]=0;
						State[ExpPre-1]=0;
						State[ExpPre]=State[ExpNext];
						State[ExpNext]=0;
						State[ExpPre+1]=0;
						curPos-=2;
						ExpNext=curPos;	
						ExpPre-=1;

						break;
					}

					while(ExpPre>=0)//��ǰһ������
					{						
						if(State[ExpPre]>0)
							break;
						ExpPre-=1;
					}

					if(ExpPre<0)//����Ҳ��������ش�����Ϣ
						return FALSE;
				}

				if(ExpStack[ExpPre]==-4)//��ǰ�����Ŷ�Ӧ�������� ����յ����
				{
					ExpStack[ExpPre]=ExpStack[ExpPre+1];
					State[ExpPre]=0;
					ExpStack[ExpPre+1]=0;
					ExpStack[ExpNext]=0;
					curPos-=2;
					ExpNext=curPos;	
					while(ExpPre>=0)
					{						
						if(State[ExpPre]>0)
							break;
						ExpPre-=1;
					}
					if(ExpPre<=0)
					{
						ExpPre=curPos;
						IsPreUse=false;
					}
				}
			}
			else//�ǣ������
				ExpPre=curPos;
		}
			}
		}
	}
	if(ExpStack[ExpPre]==-1)//���һ������Ϊ�ǵ����
	{
		ExpStack[ExpPre]=!ExpStack[ExpPre+1];	//~
		ExpStack[ExpPre+1]=0;		
		State[ExpPre]=0;
		State[ExpPre+1]=0;		
		curPos-=1;
		ExpNext=curPos;
	}
	if(khNum!=0)
		return FALSE;
	if(State[curPos]>0)
		return FALSE;
	while(curPos>=0)//ȫ��ѹյ���ļ���
	{	
		if(curPos==0)	//��ǰָ��Ϊ0����
		{
			if(ExpStack[curPos]<0)
				return FALSE;
			nReturnValue=ExpStack[curPos];//����
			return TRUE;
		}
		ExpPre=curPos;
		while(ExpPre>=0)//��ǰһ������
		{						
			if(State[ExpPre]>0)
				break;
			ExpPre-=1;
		}
		//	ExpPre=0;
		//	while(ExpPre<=0)//��ǰһ������
		//	{						
		//		if(State[ExpPre]>0)
		//			break;
		//		ExpPre+=1;
		//	}
		switch(ExpStack[ExpPre])//���з�������
		{
		case -1:
			ExpStack[ExpPre]=!ExpStack[ExpPre+1];	//~
			State[ExpPre]=0;
			curPos-=1;			
			break;
		case -2:
			ExpStack[ExpPre-1]=ExpStack[ExpPre-1] & ExpStack[ExpPre+1];//&	
			State[ExpPre]=0;
			curPos-=2;		
			ExpPre-=1;
			break;
		case -3:
			ExpStack[ExpPre-1]=ExpStack[ExpPre-1] | ExpStack[ExpPre+1];//|	
			State[ExpPre]=0;
			curPos-=2;			
			ExpPre-=1;
			break;
		case -4:
			ExpStack[ExpPre]=ExpStack[ExpPre+1];
			State[ExpPre]=0;
			curPos-=2;
			break;
		case -5:
			State[ExpPre]=0;
			break;
		default:
			return FALSE;
		}
	}

	return FALSE;
}
