#include "StdAfx.h"
#include "ADOWrapper.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//		CADOConnection

CADOConnection::CADOConnection(void)
{
// 	if(FAILED(::CoInitialize(NULL)))
// 		return;

	m_pConnection.CreateInstance(__uuidof(Connection));
	m_strConnection = _T("");
	m_strServerName = DEF_SERVER;
	m_strPort = DEF_PORT;
	m_strDBName = DB_MASTER;
	m_strUserID = DEF_SQLSVR_USER;
	m_strPassword = DEF_SQLSVR_PWD;
}

CADOConnection::~CADOConnection(void)
{
	this->Disconnect ();
	this->Release ();

// 	::CoUninitialize();
}

BOOL CADOConnection::Connect (CString strServerName, CString strPort, CString strDBName, CString strUserID, CString strPassword)
{
	CString strConnection = _T("Provider=SQLOLEDB.1;Persist Security Info = false;Network Library=DBMSSOCN;Data Source=");
	strConnection += strServerName;
	strConnection += _T(",");
	strConnection += strPort;
	strConnection += _T("; Initial Catalog=");
	strConnection += strDBName;
	strConnection += _T("; User ID=");
	strConnection += strUserID;
	strConnection += _T("; PWD=");
	strConnection += strPassword;

	if (this->IsConnectSuccess ())
		this->Disconnect ();

	try
	{
		m_pConnection->ConnectionTimeout = ADO_CONNECTION_TIMEOUT;
		m_pConnection->CommandTimeout = ADO_COMMAND_TIMEOUT;
		m_pConnection->Open(_bstr_t(strConnection), _T(""), _T(""), adConnectUnspecified);
	}
	catch (_com_error &e)
	{
		CADOError::OutputErrorString (e, _T("CADOConnection"), _T("Connect"));

		return FALSE;
	}

	m_strConnection = strConnection;
	m_strServerName = strServerName;
	m_strPort = strPort;
	m_strDBName = strDBName;
	m_strUserID = strUserID;
	m_strPassword = strPassword;

	return TRUE;
}

//当中间时刻网络断开，尝试进行连接一次；
BOOL CADOConnection::ConnectAgain ()
{
	this->Disconnect ();

	try
	{
		m_pConnection->ConnectionTimeout = ADO_CONNECTION_TIMEOUT;
		m_pConnection->CommandTimeout = ADO_COMMAND_TIMEOUT; 
		m_pConnection->Open(_bstr_t(m_strConnection), _T(""), _T(""), adConnectUnspecified);	
	}
	catch (_com_error &e)
	{
		CADOError::OutputErrorString (e, _T("CADOConnection"), _T("ConnectAgain"));

		return FALSE;
	}

	return TRUE;
}

//释放资源，断开连接
BOOL CADOConnection::Disconnect ()
{
	if (m_pConnection != NULL)
	{
		try
		{
			if (m_pConnection->State & adStateOpen)
				m_pConnection->Close();
		}
		catch (_com_error &e)
		{
			CADOError::OutputErrorString (e, _T("CADOConnection"), _T("Disconnect"));

			return FALSE;
		}
	}

	return TRUE;
}

//判定连接是否成功，成功返回真，失败返回假
BOOL CADOConnection::IsConnectSuccess ()
{
	if (m_pConnection!=NULL && (m_pConnection->State & adStateOpen))
		return TRUE;
	else
		return FALSE;
}

//释放资源
BOOL CADOConnection::Release ()
{
	if (m_pConnection != NULL)
	{
		try
		{
			m_pConnection.Release();
			m_pConnection = NULL;
		}
		catch (_com_error &e)
		{
			CADOError::OutputErrorString (e, _T("CADOConnection"), _T("Release"));

			return FALSE;
		}
	}

	return TRUE;
}

//利用Connection不返回记录集；
BOOL CADOConnection::ConnectExecute (LPCTSTR lpszSQL, long lOptions/*=adCmdText*/)
{
	if (m_pConnection == NULL)
		return FALSE;

	if (!this->IsConnectSuccess ())
	{
		if (!this->ConnectAgain ())
			return FALSE;
	}

	try
	{
		_variant_t RecordsAffected;
		m_pConnection->Execute(lpszSQL, &RecordsAffected, lOptions);
	}
	catch(_com_error e)
	{
		if (e.Error() == E_FAIL)
			this->Disconnect ();

		CADOError::OutputErrorString (e, _T("CADOConnection"), _T("ConnectExecute"), lpszSQL);

		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//		CADORecordset

CADORecordset::CADORecordset(void)
{
// 	if (FAILED(::CoInitialize(NULL)))
// 		return;

	m_pRecordset.CreateInstance(__uuidof(Recordset));
	m_pADOConnection = NULL;
	m_bRecordsetUsing = FALSE;
}

CADORecordset::CADORecordset(CADOConnection *pADOConnection)
{
// 	if (FAILED(::CoInitialize(NULL)))
// 		return;

	m_pRecordset.CreateInstance(__uuidof(Recordset));
	m_bRecordsetUsing = FALSE;

	this->SetConnection (pADOConnection);
}

CADORecordset::~CADORecordset(void)
{
	this->Close ();
	this->Release ();

	m_pADOConnection = NULL;

// 	::CoUninitialize();
}

//传入记录集所用的连接
void CADORecordset::SetConnection (CADOConnection *pADOConnection)
{
	m_pADOConnection = pADOConnection;
}

//得到记录集所用的连接
CADOConnection *CADORecordset::GetConnection ()
{
	return m_pADOConnection;
}

//关闭记录集
BOOL CADORecordset::Close ()
{
	if (m_pRecordset != NULL)
	{
		try
		{
			if (this->IsOpen ())	
				m_pRecordset->Close();
		}
		catch (_com_error &e)
		{
			CADOError::OutputErrorString (e, _T("CADORecordset"), _T("Close"));

			return FALSE;
		}
	}

	return TRUE;
}

//释放资源
BOOL CADORecordset::Release ()
{
	if (m_pRecordset != NULL)
	{
		try
		{
			m_pRecordset.Release();
			m_pRecordset = NULL;
		}
		catch (_com_error &e)
		{
			CADOError::OutputErrorString (e, _T("CADORecordset"), _T("Release"));

			return FALSE;
		}
	}

	return TRUE;
}

//执行strSQL指定的命令，若它为查询语句，返回记录集，记录集指针存放在pADORecordset里
BOOL CADORecordset::RecordExecute (LPCTSTR lpszSQL, BOOL bBatch/*=FALSE*/)
{
	if (m_bRecordsetUsing)
		return FALSE;

	if (m_pRecordset == NULL || m_pADOConnection == NULL)
		return FALSE;

	if (!m_pADOConnection->IsConnectSuccess ())
	{
		if (!m_pADOConnection->ConnectAgain ())
			return FALSE;
	}

	this->Close ();

	try
	{
		newLockTypeEnum lt = adLockOptimistic;
		if (bBatch)
			lt = adLockBatchOptimistic;		//批量

		m_pRecordset->CursorLocation = adUseClient;
		HRESULT hRet = m_pRecordset->Open(lpszSQL, m_pADOConnection->m_pConnection.GetInterfacePtr(), adOpenStatic, lt, adCmdText);
		if (hRet != S_OK)
			return FALSE;
	}
	catch (_com_error &e)
	{
		if (e.Error() == E_FAIL)
			m_pADOConnection->Disconnect ();

		CADOError::OutputErrorString (e, _T("CADORecordset"), _T("RecordExecute"), lpszSQL);

		return FALSE;
	}

	return TRUE;
}

//获取当前打开的记录集的记录总数;
int CADORecordset::GetRecordCount ()
{
	if (m_pRecordset == NULL)
		return 0;

	int nCount = 0;

	if (!this->IsEmpty())
		nCount = m_pRecordset->GetRecordCount();

	return nCount;
}

//记录集移到第一条记录
BOOL CADORecordset::MoveFirst ()
{
	if (m_pRecordset == NULL)
		return FALSE;

	if (!this->IsEmpty())
		m_pRecordset->MoveFirst();

	return TRUE;
}

//记录集向下滚动一条记录
BOOL CADORecordset::MoveNext ()
{
	if (m_pRecordset == NULL)
		return FALSE;

	if (!m_pRecordset->EndOfFile)
		m_pRecordset->MoveNext();

	return TRUE;
}

//记录集空返回真，非空返回假
BOOL CADORecordset::IsEmpty ()
{
	if (m_pRecordset == NULL)
		return FALSE;

	if (m_pRecordset->BOF && m_pRecordset->EndOfFile)
		return TRUE;
	else
		return FALSE;
}

//判定记录集是否到达尾部;
BOOL CADORecordset::IsReachEnd ()
{
	if (m_pRecordset == NULL)
		return FALSE;

	if (m_pRecordset->EndOfFile)
		return TRUE;
	else
		return FALSE;
}

//判断记录集指针当前所指记录是否有效
BOOL CADORecordset::IsCurRecordValid ()
{
	if (m_pRecordset == NULL)
		return FALSE;

	if (!this->IsOpen ())
		return FALSE;

	if (m_pRecordset->BOF || m_pRecordset->EndOfFile)
		return FALSE;

	return TRUE;
}

//判断当前记录集是否为打开状态
BOOL CADORecordset::IsOpen ()
{
	if (m_pRecordset == NULL)
		return FALSE;

	if ((m_pRecordset->State & adStateOpen) == adStateOpen)
		return TRUE;

	return FALSE;
}

//此函数为更新以有的记录中大字段的值 在调用函数前确保记录集已返回到需要更改记录，可配合存储过程创建新记录（可根据需要修改）
BOOL CADORecordset::UpdateBinaryFldVal (LPCTSTR lpszFldName, byte *pbyBuffer, int nDataLen)
{
	if (!this->IsCurRecordValid ())
		return FALSE;

	if (nDataLen < 0 || (nDataLen > 0 && pbyBuffer == NULL))
		return FALSE;

	CByteArray bta;
	int dwSize;
	int nCount = 0;
	if (nDataLen == 0)
	{
		bta.SetSize(0);
		COleVariant vv(bta);

		m_pRecordset->GetFields()->GetItem(COleVariant(lpszFldName))->AppendChunk(vv);
	}
	else
	{
		while (nDataLen > 0)
		{
			if (nDataLen >= CHUNKSIZE)
				dwSize = CHUNKSIZE;
			else
				dwSize = nDataLen;
			nDataLen -= dwSize;

			bta.SetSize(dwSize);

			for (int i=0; i<dwSize; i++)
				bta.SetAt(i, pbyBuffer[i+nCount]);
			COleVariant vv(bta);

			m_pRecordset->GetFields()->GetItem(COleVariant(lpszFldName))->AppendChunk(vv);
			nCount += dwSize;
		}
	}

	return TRUE;
}

void CADORecordset::SetRecordsetUseFlag (BOOL bUsing)
{
	m_bRecordsetUsing = bUsing;
}

BOOL CADORecordset::AddNewRecord ()
{
	if (m_pRecordset == NULL)
		return FALSE;

	if (!this->IsOpen ())
		return FALSE;

	try
	{
		HRESULT hRet = m_pRecordset->AddNew ();
		if (hRet != S_OK)
			return FALSE;
	}
	catch (_com_error &e)
	{
		if (e.Error() == E_FAIL)
			m_pADOConnection->Disconnect ();

		CADOError::OutputErrorString (e, _T("CADORecordset"), _T("AddNewRecord"));

		return FALSE;
	}

	return TRUE;
}

BOOL CADORecordset::UpdateRecord (BOOL bBatch/*=FALSE*/)
{
	if (m_pRecordset == NULL)
		return FALSE;

	if (!this->IsOpen ())
		return FALSE;

	try
	{
		HRESULT hRet = S_OK;

		//对于更新记录集，可以设置Update Resync动态属性为adResyncAll，记录集可以同步回所有表字段
		//如果不设置Update Resync也可以同步回主键字段，但其它字段不一定能同步回来
		//具体请参考Update Resync 动态属性 (ADO)相关文档
// 		m_pRecordset->Properties->GetItem(COleVariant(_T("Update Resync")))->put_Value(COleVariant(long(adResyncAll)));

		if (bBatch)
			hRet = m_pRecordset->UpdateBatch (adAffectAll);
		else
			hRet = m_pRecordset->Update ();

		if (hRet != S_OK)
			return FALSE;
	}
	catch (_com_error &e)
	{
		//更新异常后应调用CancelUpdate取消更新，这样记录集才能正常Close，否则Close时会异常。
		if (bBatch)
			m_pRecordset->CancelBatch (adAffectAll);
		else
			m_pRecordset->CancelUpdate ();

		if (e.Error() == E_FAIL)
			m_pADOConnection->Disconnect ();

		CADOError::OutputErrorString (e, _T("CADORecordset"), _T("UpdateRecord"));

		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//		CADOError

CADOError::CADOError(void)
{
}

CADOError::~CADOError(void)
{
}

void CADOError::OutputErrorString (const _com_error &e, CString strClassName/*=_T("")*/, CString strFuctionName/*=_T("")*/, CString strSQL/*=_T("")*/)
{
	CString strError;
	strError.Format(_T("\nADO Error: %s\n"), (LPTSTR )e.Description());
	OutputDebugString (strError);

	if (!strClassName.IsEmpty())
	{
		strError.Format(_T("\tClass Name: %s\n"), strClassName);
		OutputDebugString (strError);
	}

	if (!strFuctionName.IsEmpty())
	{
		strError.Format(_T("\tFuction Name: %s\n"), strFuctionName);
		OutputDebugString (strError);
	}

	if (!strSQL.IsEmpty())
	{
		strError.Format(_T("\tSQL: %s\n"), strSQL);
		OutputDebugString (strError);
	}
}
