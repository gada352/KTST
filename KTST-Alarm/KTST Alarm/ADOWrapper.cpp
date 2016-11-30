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

//���м�ʱ������Ͽ������Խ�������һ�Σ�
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

//�ͷ���Դ���Ͽ�����
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

//�ж������Ƿ�ɹ����ɹ������棬ʧ�ܷ��ؼ�
BOOL CADOConnection::IsConnectSuccess ()
{
	if (m_pConnection!=NULL && (m_pConnection->State & adStateOpen))
		return TRUE;
	else
		return FALSE;
}

//�ͷ���Դ
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

//����Connection�����ؼ�¼����
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

//�����¼�����õ�����
void CADORecordset::SetConnection (CADOConnection *pADOConnection)
{
	m_pADOConnection = pADOConnection;
}

//�õ���¼�����õ�����
CADOConnection *CADORecordset::GetConnection ()
{
	return m_pADOConnection;
}

//�رռ�¼��
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

//�ͷ���Դ
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

//ִ��strSQLָ�����������Ϊ��ѯ��䣬���ؼ�¼������¼��ָ������pADORecordset��
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
			lt = adLockBatchOptimistic;		//����

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

//��ȡ��ǰ�򿪵ļ�¼���ļ�¼����;
int CADORecordset::GetRecordCount ()
{
	if (m_pRecordset == NULL)
		return 0;

	int nCount = 0;

	if (!this->IsEmpty())
		nCount = m_pRecordset->GetRecordCount();

	return nCount;
}

//��¼���Ƶ���һ����¼
BOOL CADORecordset::MoveFirst ()
{
	if (m_pRecordset == NULL)
		return FALSE;

	if (!this->IsEmpty())
		m_pRecordset->MoveFirst();

	return TRUE;
}

//��¼�����¹���һ����¼
BOOL CADORecordset::MoveNext ()
{
	if (m_pRecordset == NULL)
		return FALSE;

	if (!m_pRecordset->EndOfFile)
		m_pRecordset->MoveNext();

	return TRUE;
}

//��¼���շ����棬�ǿշ��ؼ�
BOOL CADORecordset::IsEmpty ()
{
	if (m_pRecordset == NULL)
		return FALSE;

	if (m_pRecordset->BOF && m_pRecordset->EndOfFile)
		return TRUE;
	else
		return FALSE;
}

//�ж���¼���Ƿ񵽴�β��;
BOOL CADORecordset::IsReachEnd ()
{
	if (m_pRecordset == NULL)
		return FALSE;

	if (m_pRecordset->EndOfFile)
		return TRUE;
	else
		return FALSE;
}

//�жϼ�¼��ָ�뵱ǰ��ָ��¼�Ƿ���Ч
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

//�жϵ�ǰ��¼���Ƿ�Ϊ��״̬
BOOL CADORecordset::IsOpen ()
{
	if (m_pRecordset == NULL)
		return FALSE;

	if ((m_pRecordset->State & adStateOpen) == adStateOpen)
		return TRUE;

	return FALSE;
}

//�˺���Ϊ�������еļ�¼�д��ֶε�ֵ �ڵ��ú���ǰȷ����¼���ѷ��ص���Ҫ���ļ�¼������ϴ洢���̴����¼�¼���ɸ�����Ҫ�޸ģ�
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

		//���ڸ��¼�¼������������Update Resync��̬����ΪadResyncAll����¼������ͬ�������б��ֶ�
		//���������Update ResyncҲ����ͬ���������ֶΣ��������ֶβ�һ����ͬ������
		//������ο�Update Resync ��̬���� (ADO)����ĵ�
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
		//�����쳣��Ӧ����CancelUpdateȡ�����£�������¼����������Close������Closeʱ���쳣��
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
