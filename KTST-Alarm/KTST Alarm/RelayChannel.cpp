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

//接收通知
void RelayChannel::OnNotify (NotifyCode code, WPARAM wParam, LPARAM lParam)
{
	Channel::OnNotify(code, wParam, lParam);

	switch (code)
	{
// 	case ncChannelStateChange:		//通道状态发生改变
	case ncAlarmLevelChange:		//通道报警级别发生改变
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
					m_bExpressionUpdate = TRUE;		//表达式中有报警状态更新
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

	//判断是否有输出延时时间已到
	if (m_bDelayStarting)
	{
		int nPassedTime = GetTickCount() - m_dwDelayStartTime;
		if (nPassedTime >= m_nOutputDelay*1000 && m_nOutputDelay > 0)
			m_bExpressionUpdate = TRUE;
	}
}

//解析继电器设备信息，包括解析设备自定义缓冲区中的信息
BOOL RelayChannel::AnalyzeRelayChannelBuffer ()
{
	FreeObjPtrList(&m_listExpression);

	int nLen = m_bdDefBuf.GetCount();
	if (nLen <= 0)
		return TRUE;

	//从缓冲区读出XML
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

		if (childNodeName == _T("OutputDelay"))				//输出延时
		{
			CString strText = childNode->Gettext();
			m_nOutputDelay = _ttoi(strText);
		}

		if (childNodeName == _T("Energized"))				//充电
		{
			CString strText = childNode->Gettext();
			m_bEnergized = strText.Compare(_T("0"));
		}

		if (childNodeName == _T("Bypass"))					//旁路
		{
			CString strText = childNode->Gettext();
			m_bBypass = strText.Compare(_T("0"));
		}

		if (childNodeName == _T("Latching"))				//锁定
		{
			CString strText = childNode->Gettext();
			m_bLatching = strText.Compare(_T("0"));
		}

		if (childNodeName == _T("Expression"))				//表达式
		{
			MSXML2::IXMLDOMNodeListPtr pChannelNodeList;
			pChannelNodeList = childNode->GetchildNodes();

			int expNodeCount = pChannelNodeList->Getlength();

			for (int j = 0; j < expNodeCount; j++)
			{
				MSXML2::IXMLDOMNodePtr expNode;
				expNode = pChannelNodeList->Getitem(j);

				CString expNodeName = expNode->GetnodeName();

				if (expNodeName == _T("Operator"))				//锁定
				{
					RelayExpression *pExpression = new RelayExpression();
					pExpression->eExpressionType = etOperator;		//操作符
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
					pExpression->eExpressionType = etChannel;		//关注的通道
					pExpression->nDeviceNo = nDeviceNo;
					pExpression->nChannelNo = nChannelNo;
					m_listExpression.AddTail(pExpression);
				}
			}
		}
	}

	return TRUE;
}

//保存继电器设备信息，包括保存设备自定义缓冲区中的信息
BOOL RelayChannel::SaveRelayChannelBuffer ()
{
	MSXML2::IXMLDOMDocumentPtr pDoc;
	HRESULT hr;
	CString strText;

	hr = pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
	if (FAILED (hr))
		return FALSE;

	//通道
	MSXML2::IXMLDOMElementPtr rootChannel = pDoc->createElement((_bstr_t )_T("Channel"));
	pDoc->appendChild(rootChannel);

	//输出延时
	MSXML2::IXMLDOMElementPtr docOutputDelay = pDoc->createElement((_bstr_t )_T("OutputDelay"));
	strText.Format(_T("%d"), m_nOutputDelay);
	docOutputDelay->Puttext((_bstr_t)strText);
	rootChannel->appendChild(docOutputDelay);

	//充电
	MSXML2::IXMLDOMElementPtr docEnergized = pDoc->createElement((_bstr_t )_T("Energized"));
	strText = m_bEnergized ? _T("1"):_T("0");
	docEnergized->Puttext((_bstr_t)strText);
	rootChannel->appendChild(docEnergized);

	//旁路
	MSXML2::IXMLDOMElementPtr docBypass = pDoc->createElement((_bstr_t )_T("Bypass"));
	strText = m_bBypass ? _T("1"):_T("0");
	docBypass->Puttext((_bstr_t)strText);
	rootChannel->appendChild(docBypass);

	//锁定
	MSXML2::IXMLDOMElementPtr docLatching = pDoc->createElement((_bstr_t )_T("Latching"));
	strText = m_bLatching ? _T("1"):_T("0");
	docLatching->Puttext((_bstr_t)strText);
	rootChannel->appendChild(docLatching);

	//表达式
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


	//保存至设备的自定义缓冲区中
	CStringW strXML = pDoc->Getxml();
	m_bdDefBuf.SetData((byte *)(LPCWSTR )strXML, strXML.GetLength()*sizeof(wchar_t));

	return TRUE;
}

//得到当前继电器通道应当输出的值
int RelayChannel::GetOutputValue ()
{
	int nCurOutputValue = 0;

	//只让do while()循环执行一次，为了中途可以执行break跳出
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
		if (!Cal (strExpression, nExpressionValue))	//计算表达式的值
		{
			nCurOutputValue = 0;
			break;
		}

		if (m_bLatching)	//锁定
		{
			if (nExpressionValue == 1)
			{
				nCurOutputValue = 1;

				if (m_nLastOutputValue == 0 && nCurOutputValue == 1)	//值由0变成1
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
		else	//非锁定
		{
			nCurOutputValue = nExpressionValue;

			if (m_nLastOutputValue == 0 && nCurOutputValue == 1)	//值由0变成1
			{
				m_bDelayStarting = TRUE;
				m_dwDelayStartTime = GetTickCount();
			}

			if (m_nLastOutputValue == 1 && nCurOutputValue == 0)	//值由1变成0
				m_bDelayStarting = FALSE;

			m_nLastOutputValue = nCurOutputValue;
		}
	}while (0);

	//判断是否达到延时时间
	if (nCurOutputValue == 1 && m_bDelayStarting)
	{
		int nPassedTime = GetTickCount() - m_dwDelayStartTime;
		if (nPassedTime >= m_nOutputDelay*1000)
			m_bDelayStarting = FALSE;
		else
			nCurOutputValue = 0;
	}

	//充电
	if (m_bEnergized)
	{
		if (nCurOutputValue == 0)
			nCurOutputValue = 1;
		else if (nCurOutputValue == 1)
			nCurOutputValue = 0;
	}

	return nCurOutputValue;
}

//复位锁定通道
void RelayChannel::Reset ()
{
	if (m_bLatching)
		m_nLastOutputValue = 0;

	m_bExpressionUpdate = TRUE;
}

//清空内存
void RelayChannel::ClearMemory ()
{
	FreeObjPtrList(&m_listExpression);
}

//请求所有关注的通道数据
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

//判断继电器通道是否有数据更新
BOOL RelayChannel::IsExpressionUpdated ()
{
	return m_bExpressionUpdate;
}

//将继电器通道表达式拼成字符串
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
//创建时间：
//		2006-8-21 12:48:00
//参数：
//		CString str[in]：表示输入的表达式。
//		int& nReturnValue[out]：表示传出的值。
//功能：
//		计算继电器通道表达式字符串(返回值为计算是否成功 不成功则可能表达式出错)(内部调用)。
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL RelayChannel::Cal(CString str,int& nReturnValue)
{
	int i=0;//循环变量

	int ExpStack[300];//盏数组
	int State[300];//状态数组
	int ExpPre=-1,ExpNext=-1,curPos=-1;//表达式开始和结束的位置 当前盏位置
	bool IsPreUse=false;
	int khNum=0;//括号的数量


	for(i=0;i<300;i++)//初始化
	{
		ExpStack[i]=0;
		State[i]=0;
	}

	int strLenth=str.GetLength();
	for(i=0;i<strLenth;i++)
	{	
		//压盏
		curPos=curPos+1;
		//入盏 -1表示非“ ~ ”  ;-2表示与" & "  ;-3表示或 " | " ; -4表示（ ; -5表示）
		switch(str[i])
		{			
		case '~':
			if(curPos>0 && (ExpStack[curPos-1]==-5 || State[curPos-1]==0))//( 数字
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
			if(curPos>0 && (ExpStack[curPos-1]==-5 || State[curPos-1]==0))//) 数字
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

		if((ExpStack[0]==-3 || ExpStack[0]==-2 || ExpStack[0]==-5))//第一位不能为 &|)
			return FALSE;

		//计算
		if(State[curPos]>0)//如果当前的是符号
		{
			if(!IsPreUse)//当前还没有符号
			{	
				ExpPre=curPos;
				IsPreUse=TRUE;
			}
			else//当前有两个符号可以用来比较优先
			{
				ExpNext=curPos;
LOGO:	if(State[ExpPre]>=State[ExpNext])//如果后面的符号优先小于等于前面的
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

			switch(ExpStack[ExpPre])//按符号计算
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
				while(ExpPre>=0)//找前一个符号
				{						
					if(State[ExpPre]>0)
						break;
					ExpPre-=1;
				}

				if(ExpPre<0)//如果前面没有符号了结束当前部分表达式的计算 指向后一个表达式
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
				while(ExpPre>=0)//找前一个符号
				{						
					if(State[ExpPre]>0)
						break;
					ExpPre-=1;
				}

				if(ExpPre<0)//如果前面没有符号了结束当前部分表达式的计算 指向后一个表达式
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
				while(ExpPre>=0)//找前一个符号
				{						
					if(State[ExpPre]>0)
						break;
					ExpPre-=1;
				}

				if(ExpPre<0)//如果前面没有符号了结束当前部分表达式的计算 指向后一个表达式
					ExpPre=curPos;
				else goto LOGO;
				break;
			case -4:
				ExpPre=ExpNext;
				//	IsPreUse=false;
				break;
			}


		}
		else//后面的优先高于前面的 
		{
			if(ExpStack[ExpNext]==-5)//如果是）
			{
				while(ExpStack[ExpPre]!=-4)//当不是（时
				{
					switch(ExpStack[ExpPre])//计算
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

					while(ExpPre>=0)//找前一个符号
					{						
						if(State[ExpPre]>0)
							break;
						ExpPre-=1;
					}

					if(ExpPre<0)//如果找不到（返回错误信息
						return FALSE;
				}

				if(ExpStack[ExpPre]==-4)//当前反括号对应的正括号 做消盏处理
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
			else//非）的情况
				ExpPre=curPos;
		}
			}
		}
	}
	if(ExpStack[ExpPre]==-1)//最后一个符号为非的情况
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
	while(curPos>=0)//全部压盏完后的计算
	{	
		if(curPos==0)	//当前指针为0返回
		{
			if(ExpStack[curPos]<0)
				return FALSE;
			nReturnValue=ExpStack[curPos];//返回
			return TRUE;
		}
		ExpPre=curPos;
		while(ExpPre>=0)//找前一个符号
		{						
			if(State[ExpPre]>0)
				break;
			ExpPre-=1;
		}
		//	ExpPre=0;
		//	while(ExpPre<=0)//找前一个符号
		//	{						
		//		if(State[ExpPre]>0)
		//			break;
		//		ExpPre+=1;
		//	}
		switch(ExpStack[ExpPre])//进行符号运算
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
