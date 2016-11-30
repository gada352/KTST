#include "StdAfx.h"
#include "DBAccess.h"

CDBAccess::CDBAccess(void)
{
	m_strOptDBName.Empty();
}

CDBAccess::CDBAccess(CADOConnection *pADOConnection, CString strOptDBName/*=_T("")*/)
	:CADORecordset(pADOConnection)
{
	SetOptDBName (strOptDBName);
}

CDBAccess::~CDBAccess(void)
{
}

void CDBAccess::SetOptDBName (CString strOptDBName)
{
	m_strOptDBName = strOptDBName;
}

CString CDBAccess::GetOptDBName ()
{
	return m_strOptDBName;
}

void CDBAccess::ResetOptDBName ()
{
	m_strOptDBName.Empty();
}

/*/
���ܣ�Ϊָ������ӡ��޸ġ�ɾ����¼
������[IN]  int nTableNo, ����
������[OUT] CString strTable, ������
������[IN]  CString strKeyID, ��¼����������
������[IN]  CString strKeyExID, ����������������ֶι��ɣ����ǵڶ����ֶ�����
����ֵ��TRUE�ɹ���FALSEʧ�ܡ�
/*/
BOOL CDBAccess::GetTableInfo (int nTableNo, CString &strTable, CString &strKeyID, CString &strKeyExID)
{
	CString strTableName, strKeyIDName, strKeyExIDName;

	strTableName = _T("");
	strKeyIDName = _T("");
	strKeyExIDName = _T("");

	switch (nTableNo)
	{
	case TABLE_DB_SYSTEM:				//ϵͳ��
		strTableName = table_DBSystem;
		strKeyIDName = DBSystem_RecordID;
		break;

	case TABLE_USER_INFO:				//�û���Ϣ��
		strTableName = table_UserInfo;
		strKeyIDName = UserInfo_UserID;
		break;

	case TABLE_PLOT:					//ͼ��
		strTableName = table_Plot;
		strKeyIDName = Plot_PlotID;
		break;

	case TABLE_DEVICE:					//�豸��
		strTableName = table_Device;
		strKeyIDName = Device_DeviceNo;
		break;

	case TABLE_CHANNEL:					//ͨ����
		strTableName = table_Channel;
		strKeyIDName = Channel_DeviceNo;
		strKeyExIDName = Channel_ChannelNo;
		break;

	case TABLE_CHANNEL_DATA:			//ͨ�����ݱ�
		strTableName = table_ChannelData;
		strKeyIDName = ChannelData_DataID;
		break;

	case TABLE_ALARM_DATA:				//�������ݱ�
		strTableName = table_AlarmData;
		strKeyIDName = AlarmData_AlarmDataID;
		break;

	case TABLE_CONTROLLER:				//��������
		strTableName = table_Controller;
		strKeyIDName = Controller_DeviceNo;
		strKeyExIDName = Controller_ControllerNo;
		break;

	case TABLE_ASSET:					//�ʲ���
		strTableName = table_Asset;
		strKeyIDName = Asset_AssetID;
		break;

	case TABLE_ASSET_MAINTENANCE:		//�ʲ�ά����
		strTableName = table_AssetMaintenance;
		strKeyIDName = AssetMaintenance_MaintenanceID;
		break;

	default:
		return FALSE;
	}

	strTable = strTableName;
	strKeyID = strKeyIDName;
	strKeyExID = strKeyExIDName;

	return TRUE;
}

/*/
���ܣ�Ϊָ������ӡ��޸ġ�ɾ����¼
������[IN]  int nDBOptFlag, ������־����ӡ��޸ġ�ɾ��
������[IN]  int nTableNo, ����
������[IN]  int nKeyID, ��¼������
������[IN]  int nKeyExID, ����������������ֶι��ɣ����ǵڶ����ֶ�
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::RecordOperate (int nDBOptFlag, int nTableNo, int nKeyID/*=-1*/, int nKeyExID/*=-1*/)
{
	CString strSQL, strDBName, strTableName, strKeyIDName, strKeyExIDName;

	if (!this->GetTableInfo (nTableNo, strTableName, strKeyIDName, strKeyExIDName))
		return RET_DB_PARAM_INVALID;

	return this->RecordOperate(nDBOptFlag, strTableName, strKeyIDName, strKeyExIDName, nKeyID, nKeyExID);
}

/*/
���ܣ�Ϊָ������ӡ��޸ġ�ɾ����¼
������[IN]  int nDBOptFlag, ������־����ӡ��޸ġ�ɾ��
������[IN]  CString strTableName, ������
������[IN]  CString strKeyIDName, ����������
������[IN]  CString strKeyExIDName, ����������������ֶι��ɣ����ǵڶ����ֶ�����
������[IN]  int nKeyID, ��¼������
������[IN]  int nKeyExID, ����������������ֶι��ɣ����ǵڶ����ֶ�
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::RecordOperate (int nDBOptFlag, CString strTableName, CString strKeyIDName, CString strKeyExIDName, 
								int nKeyID/*=-1*/, int nKeyExID/*=-1*/)
{
	CString strSQL, strDBName;

	//������ݿ����Ʋ��գ����ڱ���ǰ�������ݿ�����
	this->AddOptDBName (strTableName);

	switch (nDBOptFlag)
	{
	case DB_OPT_ADD:		//���		//��Ӽ�¼ʱֻ��Ҫ���ռ�¼��
		if (strKeyExIDName.IsEmpty())
			strSQL.Format(_T("SELECT * FROM %s WHERE %s = -1"), strTableName, strKeyIDName);
		else
			strSQL.Format(_T("SELECT * FROM %s WHERE %s = -1 AND %s = -1"), strTableName, strKeyIDName, strKeyExIDName);
		break;

	case DB_OPT_MODIFY:		//�޸�
		if (strKeyExIDName.IsEmpty())
			strSQL.Format(_T("SELECT * FROM %s WHERE %s = %d"), strTableName, strKeyIDName, nKeyID);
		else
			strSQL.Format(_T("SELECT * FROM %s WHERE %s = %d AND %s = %d"), strTableName, strKeyIDName, nKeyID, strKeyExIDName, nKeyExID);
		break;

	case DB_OPT_DELETE:		//ɾ��
		if (strKeyExIDName.IsEmpty())
			strSQL.Format(_T("DELETE %s WHERE %s = %d"), strTableName, strKeyIDName, nKeyID);
		else
			strSQL.Format(_T("DELETE %s WHERE %s = %d AND %s = %d"), strTableName, strKeyIDName, nKeyID, strKeyExIDName, nKeyExID);
		break;

	default:
		return RET_DB_PARAM_INVALID;
		break;
	}

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	switch (nDBOptFlag)
	{
	case DB_OPT_ADD:
		if (this->GetRecordCount() > 0)				//���ʱ����Ϊ�ռ�¼��
			return RET_DB_RECORD_NOT_NULL;

		if (!this->AddNewRecord ())
			return RET_DB_ADD_RECORD_FAIL;
		break;

	case DB_OPT_MODIFY:
		if (this->GetRecordCount() <= 0)			//�޸�ʱ����Ϊ�ռ�¼��
			return RET_DB_RECORD_NULL;
		break;
	}

	return RET_DB_SUCCESS;
}

/*/
���ܣ�Ϊָ������ӡ��޸ġ�ɾ����¼
������[IN]  int nDBOptFlag, ������־����ӡ��޸ġ�ɾ��
������[IN]  int nTableNo, ����
������[IN]  int *pnKeyIDArr, ��¼����������
������[IN]  int *pnKeyExIDArr, ����������������ֶι��ɣ����ǵڶ����ֶ�����
������[IN]  int nArrLen, ���������鳤��
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::RecordOperateBatch (int nDBOptFlag, int nTableNo, int *pnKeyIDArr/*=NULL*/, int *pnKeyExIDArr/*=NULL*/, 
									int nArrLen/*=0*/)
{
	CString strSQL, strDBName, strTableName, strKeyIDName, strKeyExIDName;

	if (!this->GetTableInfo (nTableNo, strTableName, strKeyIDName, strKeyExIDName))
		return RET_DB_PARAM_INVALID;

	return this->RecordOperateBatch(nDBOptFlag, strTableName, strKeyIDName, strKeyExIDName, pnKeyIDArr, pnKeyExIDArr, nArrLen);
}

/*/
���ܣ�Ϊָ������ӡ��޸ġ�ɾ����¼
������[IN]  int nDBOptFlag, ������־����ӡ��޸ġ�ɾ��
������[IN]  CString strTableName, ������
������[IN]  CString strKeyIDName, ����������
������[IN]  CString strKeyExIDName, ����������������ֶι��ɣ����ǵڶ����ֶ�����
������[IN]  int *pnKeyIDArr, ��¼����������
������[IN]  int *pnKeyExIDArr, ����������������ֶι��ɣ����ǵڶ����ֶ�����
������[IN]  int nArrLen, ���������鳤��
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::RecordOperateBatch (int nDBOptFlag, CString strTableName, CString strKeyIDName, CString strKeyExIDName, 
									int *pnKeyIDArr/*=NULL*/, int *pnKeyExIDArr/*=NULL*/, int nArrLen/*=0*/)
{
	CString strSQL, strDBName, strKeyIDSet, strKeyExIDSet, strKeyComboSet1, strKeyComboSet2;

	//������ݿ����Ʋ��գ����ڱ���ǰ�������ݿ�����
	this->AddOptDBName (strTableName);


	//ƴ��ID����(1, 2, 3, ...)
	if (nArrLen > 0)
	{
		CString strID;

		if (pnKeyIDArr != NULL)
		{
			strKeyIDSet.Format(_T("%d"), pnKeyIDArr[0]);
			for (int i=1; i<nArrLen; i++)
			{
				strID.Format(_T(", %d"), pnKeyIDArr[i]);
				strKeyIDSet += strID;
			}
		}

		if (pnKeyExIDArr != NULL)
		{
			strKeyExIDSet.Format(_T("%d"), pnKeyExIDArr[0]);
			for (int i=1; i<nArrLen; i++)
			{
				strID.Format(_T(", %d"), pnKeyExIDArr[i]);
				strKeyExIDSet += strID;
			}
		}


		//************************************************************************
		//���ַ���ʵ�֣�����һ��Ч�ʿ��ܸ�һЩ
		//************************************************************************

		//����һ������ֵ�Բ��뵽һ����ʱ����
		if (pnKeyIDArr != NULL && pnKeyExIDArr != NULL)
		{
			strKeyComboSet1.Format(_T("(%d, %d)"), pnKeyIDArr[0], pnKeyExIDArr[0]);
			for (int i=1; i<nArrLen; i++)
			{
				strID.Format(_T(", (%d, %d)"), pnKeyIDArr[i], pnKeyExIDArr[i]);
				strKeyComboSet1 += strID;
			}
		}

		//���������������ֵ�������ַ���&, @, |, -, #, %�����ӳ�һ���ֶν��в�ѯ���˴��á�&���ַ�
		if (pnKeyIDArr != NULL && pnKeyExIDArr != NULL)
		{
			strKeyComboSet2.Format(_T("\'%d&%d\'"), pnKeyIDArr[0], pnKeyExIDArr[0]);
			for (int i=1; i<nArrLen; i++)
			{
				strID.Format(_T(", \'%d&%d\'"), pnKeyIDArr[i], pnKeyExIDArr[i]);
				strKeyComboSet2 += strID;
			}
		}
	}


	switch (nDBOptFlag)
	{
	case DB_OPT_BATCH_ADD:			//�������		//��Ӽ�¼ʱֻ��Ҫ���ռ�¼��
		if (strKeyExIDName.IsEmpty())
			strSQL.Format(_T("SELECT * FROM %s WHERE %s = -1"), strTableName, strKeyIDName);
		else
			strSQL.Format(_T("SELECT * FROM %s WHERE %s = -1 AND %s = -1"), strTableName, strKeyIDName, strKeyExIDName);

		if (!this->RecordExecute(strSQL, TRUE))
			return RET_DB_EXE_FAIL;

		if (this->GetRecordCount() > 0)				//���ʱ����Ϊ�ռ�¼��
			return RET_DB_RECORD_NOT_NULL;
		break;

	case DB_OPT_BATCH_MODIFY:		//�����޸�
		if (strKeyExIDName.IsEmpty())
			strSQL.Format(_T("SELECT * FROM %s WHERE %s IN (%s)"), strTableName, strKeyIDName, strKeyIDSet);
		else
		{
			//����һ��
			strSQL.Format(_T("DECLARE @temp table(keyID int, keyExID int) \
							INSERT INTO @temp VALUES %s \
							SELECT t.* \
								FROM %s t INNER JOIN @temp tt \
								ON t.%s=tt.keyID \
									AND t.%s=tt.keyExID"), 
							strKeyComboSet1, strTableName, strKeyIDName, strKeyExIDName);

			//��������
// 			strSQL.Format(_T("SELECT * \
// 								FROM %s \
// 								WHERE CAST(%s AS nvarchar) + \'&\' + CAST(%s AS nvarchar) \
// 									IN (%s)"), 
// 							strTableName, strKeyIDName, strKeyExIDName, strKeyComboSet2);
		}

		if (!this->RecordExecute(strSQL, TRUE))
			return RET_DB_EXE_FAIL;

		if (this->GetRecordCount() <= 0)			//�޸�ʱ����Ϊ�ռ�¼��
			return RET_DB_RECORD_NULL;
		break;

	case DB_OPT_BATCH_DELETE:		//����ɾ��
		if (strKeyExIDName.IsEmpty())
			strSQL.Format(_T("DELETE %s WHERE %s IN (%s)"), strTableName, strKeyIDName, strKeyIDSet);
		else
		{
			//����һ��
			strSQL.Format(_T("DECLARE @temp table(keyID int, keyExID int) \
							 INSERT INTO @temp VALUES %s \
							 DELETE t \
								 FROM %s t INNER JOIN @temp tt \
								 ON t.%s=tt.keyID \
									 AND t.%s=tt.keyExID"), 
							 strKeyComboSet1, strTableName, strKeyIDName, strKeyExIDName);

			//��������
// 			strSQL.Format(_T("DELETE %s \
// 								WHERE CAST(%s AS nvarchar) + \'&\' + CAST(%s AS nvarchar) \
// 									IN (%s)"), 
// 							strTableName, strKeyIDName, strKeyExIDName, strKeyComboSet2);
		}

		if (!this->RecordExecute(strSQL))
			return RET_DB_EXE_FAIL;
		break;

	default:
		return RET_DB_PARAM_INVALID;
		break;
	}

	return RET_DB_SUCCESS;
}

void CDBAccess::AddOptDBName (CString &strObjectName)
{
	CString strDBName = GetOptDBName();
	if (!strDBName.IsEmpty())
	{
		CString strName;
		strName.Format(_T("%s.dbo.%s"), strDBName, strObjectName);
		strObjectName = strName;
	}
}

//���ò�ͬ�����ֶε�ֵ
int CDBAccess::SetFeildValue (CString strName, int nValue)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	m_pRecordset->PutCollect((LPCTSTR )strName, (long )nValue);

	return RET_DB_SUCCESS;
}

int CDBAccess::SetFeildValue (CString strName, float fValue)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	m_pRecordset->PutCollect((LPCTSTR )strName, fValue);

	return RET_DB_SUCCESS;
}

int CDBAccess::SetFeildValue (CString strName, double dValue)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	m_pRecordset->PutCollect((LPCTSTR )strName, dValue);

	return RET_DB_SUCCESS;
}

int CDBAccess::SetFeildValue (CString strName, byte byValue)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	m_pRecordset->PutCollect((LPCTSTR )strName, byValue);

	return RET_DB_SUCCESS;
}

int CDBAccess::SetFeildValue (CString strName, TCHAR szValue[])
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	m_pRecordset->PutCollect((LPCTSTR )strName, (LPCTSTR )szValue);

	return RET_DB_SUCCESS;
}

int CDBAccess::SetFeildValue (CString strName, const CString &strValue)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	m_pRecordset->PutCollect((LPCTSTR )strName, (LPCTSTR )strValue);

	return RET_DB_SUCCESS;
}

int CDBAccess::SetFeildValue (CString strName, const COleDateTime &odtValue)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	_variant_t Holder;
	Holder.vt = VT_DATE;
	Holder.date = odtValue.m_dt;

	m_pRecordset->PutCollect((LPCTSTR )strName, Holder);

	return RET_DB_SUCCESS;
}

int CDBAccess::SetFeildValue (CString strName, BlobData<byte> *pbdValue, int nValueLen)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	this->UpdateBinaryFldVal(strName, (pbdValue!=NULL)?pbdValue->GetData():NULL, nValueLen);

	return RET_DB_SUCCESS;
}

//�õ���ͬ�����ֶε�ֵ
int CDBAccess::GetFeildValue (CString strName, int &nValue)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	_variant_t Holder;

	Holder = m_pRecordset->GetCollect((LPCTSTR )strName);
	nValue = Holder.intVal;

	return RET_DB_SUCCESS;
}

int CDBAccess::GetFeildValue (CString strName, float &fValue)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	_variant_t Holder;

	Holder = m_pRecordset->GetCollect((LPCTSTR )strName);
	fValue = Holder.fltVal;

	return RET_DB_SUCCESS;
}

int CDBAccess::GetFeildValue (CString strName, double &dValue)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	_variant_t Holder;

	Holder = m_pRecordset->GetCollect((LPCTSTR )strName);
	dValue = Holder.dblVal;

	return RET_DB_SUCCESS;
}

int CDBAccess::GetFeildValue (CString strName, byte &byValue)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	_variant_t Holder;

	Holder = m_pRecordset->GetCollect((LPCTSTR )strName);
	byValue = Holder.bVal;

	return RET_DB_SUCCESS;
}

int CDBAccess::GetFeildValue (CString strName, TCHAR szValue[])
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	CString str;
	_variant_t Holder;

	Holder = m_pRecordset->GetCollect((LPCTSTR )strName);
	str = Holder.vt==VT_NULL ? _T("") : (LPTSTR )(_bstr_t)Holder;
	str.TrimLeft();
	str.TrimRight();
	::memcpy (szValue, (LPCTSTR )str, str.GetLength()*sizeof(TCHAR));

	return RET_DB_SUCCESS;
}

int CDBAccess::GetFeildValue (CString strName, CString &strValue)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	_variant_t Holder;

	Holder = m_pRecordset->GetCollect((LPCTSTR )strName);
	strValue = Holder.vt==VT_NULL ? _T("") : (LPTSTR )(_bstr_t)Holder;

	return RET_DB_SUCCESS;
}

int CDBAccess::GetFeildValue (CString strName, COleDateTime &odtValue)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	_variant_t Holder;

	Holder = m_pRecordset->GetCollect((LPCTSTR )strName);
	odtValue = Holder.date;

	return RET_DB_SUCCESS;
}

int CDBAccess::GetFeildValue (CString strName, BlobData<byte> *pbdValue)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	int nLen;
	_variant_t Holder;

	Holder = m_pRecordset->GetCollect((LPCTSTR )strName);
	nLen = Holder.parray->rgsabound[0].cElements;
	if (Holder.vt != VT_NULL && nLen > 0)
	{
		if (pbdValue != NULL)
			pbdValue->SetData((byte *)Holder.parray->pvData, nLen);
	}
	else
	{
		if (pbdValue != NULL)
			pbdValue->Reset();
	}

	return RET_DB_SUCCESS;
}

//Ҫ����������ʱ��
int CDBAccess::GetServerDateTime (COleDateTime &odtServerDateTime)
{
	CString strSQL;

	strSQL.Format(_T("SELECT GETDATE() AS 'ServerDateTime'"));

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	if (this->GetRecordCount() <= 0)
		return RET_DB_RECORD_NULL;

	this->GetFeildValue (_T("ServerDateTime"), odtServerDateTime);

	return RET_DB_SUCCESS;
}


/*/////////////////////////////////////////////////////////////////////////////////////////////////
*****************		����ĺ��������漰����������ݿ���		***************************
/*/

//*************************************************************************************************
//���ݿ���¼�û��Զ����ֶεĻ�ȡ���޸�

/*/
���ܣ��õ���¼��ָ�뵱ǰ��ָ��¼���Զ��建�����ֶ�
������[OUT] int nDefBufLen, �Զ��建��������
������[OUT] BlobData<byte> *pbdDefBuf, �Զ��建����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetCurDefBufData (int &nDefBufLen, BlobData<byte> *pbdDefBuf)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	this->GetFeildValue (field_DefBufLen, nDefBufLen);
	this->GetFeildValue (field_DefBuf, pbdDefBuf);
	if (pbdDefBuf != NULL && nDefBufLen != pbdDefBuf->GetSize())
		nDefBufLen = pbdDefBuf->GetSize();

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ���Ӧ���¼�û��Զ����ֶε����ݣ����������ݿ������б�ļ�¼���û��Զ����ֶΣ�
������[IN]  int nTableNo, ��ı��
������[IN]  int nKeyID, ��¼������
������[IN]  int nKeyExID, ����������������ֶι��ɣ����ǵڶ����ֶ�
������[OUT] int nDefBufLen, �Զ��建��������
������[OUT] BlobData<byte> *pbdDefBuf, �Զ��建����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetDefBufData (int nTableNo, int nKeyID, int nKeyExID, int &nDefBufLen, BlobData<byte> *pbdDefBuf)
{
	if (pbdDefBuf == NULL)
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperate (DB_OPT_MODIFY, nTableNo, nKeyID, nKeyExID);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->GetFeildValue (field_DefBufLen, nDefBufLen);
	this->GetFeildValue (field_DefBuf, pbdDefBuf);
	if (pbdDefBuf != NULL && nDefBufLen != pbdDefBuf->GetSize())
		nDefBufLen = pbdDefBuf->GetSize();

	return RET_DB_SUCCESS;
}

/*/
���ܣ��޸Ķ�Ӧ���¼�û��Զ����ֶε����ݣ����������ݿ������б�ļ�¼���û��Զ����ֶΣ�
������[IN]  int nTableNo, ��ı��
������[IN]  int nKeyID, ��¼������
������[IN]  int nKeyExID, ����������������ֶι��ɣ����ǵڶ����ֶ�
������[IN]  int nDefBufLen, �Զ��建��������
������[IN]  BlobData<byte> *pbdDefBuf, �Զ��建����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ModiDefBufData (int nTableNo, int nKeyID, int nKeyExID, int nDefBufLen, BlobData<byte> *pbdDefBuf)
{
	if (nDefBufLen < 0 || (nDefBufLen > 0 && pbdDefBuf == NULL))
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperate (DB_OPT_MODIFY, nTableNo, nKeyID, nKeyExID);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (field_DefBufLen, nDefBufLen);
	this->SetFeildValue (field_DefBuf, pbdDefBuf, nDefBufLen);

	this->UpdateRecord ();

	return RET_DB_SUCCESS;
}

//*************************************************************************************************
//ϵͳ��

/*/
���ܣ��õ����ݿ�汾
������[OUT] int nDBVersion, ���ݿ�汾��
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetDBVersion (int &nDBVersion)
{
	int nDefBuffLen;
	BlobData<byte> bdDefBuf;
	int nDBRet = GetDefBufData(TABLE_DB_SYSTEM, SYSTEM_ID_DB_VERSION, 0, nDefBuffLen, &bdDefBuf);
	if (nDBRet != RET_DB_SUCCESS)
		return nDBRet;

	if (nDefBuffLen != 0)
		nDBVersion = *(int *)bdDefBuf.GetData();
	else
		nDBVersion = DB_VERSION_UNKNOWN;

	return RET_DB_SUCCESS;
}

/*/
���ܣ��޸����ݿ�汾
������[IN]  int nDBVersion, ���ݿ�汾��
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ModiDBVersion (int nDBVersion)
{
	BlobData<byte> bdDefBuf;
	byte *pbyData = bdDefBuf.SetData(NULL, sizeof(int));
	*(int *)pbyData = nDBVersion;
	int nDefBuffLen = sizeof(int);

	int nDBRet = ModiDefBufData(TABLE_DB_SYSTEM, SYSTEM_ID_DB_VERSION, 0, nDefBuffLen, &bdDefBuf);

	return nDBRet;
}

/*/
���ܣ��õ����ݿ�����
������[OUT] int nDBType, ���ݿ����ͣ�0���������ݿ⡢1˽�������ݿ�
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetDBType (int &nDBType)
{
	int nDefBuffLen;
	BlobData<byte> bdDefBuf;
	int nDBRet = GetDefBufData(TABLE_DB_SYSTEM, SYSTEM_ID_DB_TYPE, 0, nDefBuffLen, &bdDefBuf);
	if (nDBRet != RET_DB_SUCCESS)
		return nDBRet;

	if (nDefBuffLen != 0)
		nDBType = *(int *)bdDefBuf.GetData();
	else
		nDBType = DB_TYPE_PUBLIC_CLOUD;

	return RET_DB_SUCCESS;
}

/*/
���ܣ��޸����ݿ�����
������[IN]  int nDBType, ���ݿ����ͣ�0���������ݿ⡢1˽�������ݿ�
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ModiDBType (int nDBType)
{
	BlobData<byte> bdDefBuf;
	byte *pbyData = bdDefBuf.SetData(NULL, sizeof(int));
	*(int *)pbyData = nDBType;
	int nDefBuffLen = sizeof(int);

	int nDBRet = ModiDefBufData(TABLE_DB_SYSTEM, SYSTEM_ID_DB_TYPE, 0, nDefBuffLen, &bdDefBuf);

	return nDBRet;
}

/*/
���ܣ��õ�������IP��ַ�Ͷ˿ں�
������[OUT] byte byIPAddress[], IP��ַ������
������[OUT] int nPortNo, �˿ں�
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetServerIPandPort (byte byIPAddress[], int &nPortNo)
{
	int nDefBuffLen;
	BlobData<byte> bdDefBuf;
	int nDBRet = GetDefBufData(TABLE_DB_SYSTEM, SYSTEM_ID_SERVER_IP_PORT, 0, nDefBuffLen, &bdDefBuf);
	if (nDBRet != RET_DB_SUCCESS)
		return nDBRet;

	int nDataLen = 4*sizeof(byte) + sizeof(int);	//4��byte��ʾIP��ַ��1��int��ʾ�˿ں�
	if (nDefBuffLen != nDataLen || bdDefBuf.GetSize() != nDataLen)
		return RET_DB_RECORD_INVALID;

	byte *pbyData = bdDefBuf.GetData();
	int nLen;

	nLen = 4*sizeof(byte);
	memcpy (byIPAddress, pbyData, nLen);
	pbyData += nLen;

	nLen = sizeof(int);
	memcpy (&nPortNo, pbyData, sizeof(int));
	pbyData += nLen;

	return RET_DB_SUCCESS;
}

/*/
���ܣ��޸ķ�����IP��ַ�Ͷ˿ں�
������[IN]  byte byIPAddress[], IP��ַ������
������[IN]  int nPortNo, �˿ں�
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ModiServerIPandPort (byte byIPAddress[], int nPortNo)
{
	int nDataLen = 4*sizeof(byte) + sizeof(int);	//4��byte��ʾIP��ַ��1��int��ʾ�˿ں�

	int nDefBuffLen = 4*sizeof(byte) + sizeof(int);	//4��byte��ʾIP��ַ��1��int��ʾ�˿ں�
	BlobData<byte> bdDefBuf;
	byte *pbyData = bdDefBuf.SetData(NULL, nDefBuffLen);
	int nLen;

	nLen = 4*sizeof(byte);
	memcpy (pbyData, byIPAddress, nLen);
	pbyData += nLen;

	nLen = sizeof(int);
	memcpy (pbyData, &nPortNo, sizeof(int));
	pbyData += nLen;

	int nDBRet = ModiDefBufData(TABLE_DB_SYSTEM, SYSTEM_ID_SERVER_IP_PORT, 0, nDefBuffLen, &bdDefBuf);

	return nDBRet;
}

/*/
���ܣ��õ���Ŀ����
������[OUT] BlobData<byte> *pbdDefBuf, ��Ŀ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetProjectData (BlobData<byte> *pbdDefBuf)
{
	if (pbdDefBuf == NULL)
		return RET_DB_PARAM_INVALID;

	int nDefBuffLen;

	int nDBRet = GetDefBufData(TABLE_DB_SYSTEM, SYSTEM_ID_PROJECT, 0, nDefBuffLen, pbdDefBuf);

	return nDBRet;
}

/*/
���ܣ��޸���Ŀ����
������[IN]  BlobData<byte> *pbdDefBuf, ��Ŀ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ModiProjectData (BlobData<byte> *pbdDefBuf)
{
	if (pbdDefBuf == NULL)
		return RET_DB_PARAM_INVALID;

	int nDefBuffLen = pbdDefBuf->GetSize();

	int nDBRet = ModiDefBufData(TABLE_DB_SYSTEM, SYSTEM_ID_PROJECT, 0, nDefBuffLen, pbdDefBuf);

	return nDBRet;
}

//*************************************************************************************************
//�û���Ϣ��

/*/
���ܣ��õ������û����γɼ�¼��
��������
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ExecGetAllUser ()
{
	CString strSQL, strSP;

	strSP = _T("proc_ExecGetAllUser");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s"), strSP);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ���¼���е�ǰ�û���Ϣ
������[OUT] int nUserID, �û�ID
������[OUT] TCHAR szUserName[], �û�����
������[OUT] TCHAR szPassword[], �û�����
������[OUT] int nUserRight, �û�Ȩ��
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetCurUser (int &nUserID, TCHAR szUserName[], TCHAR szPassword[], int &nUserRight)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	if (szUserName==NULL || szPassword==NULL)
		return RET_DB_PARAM_INVALID;

	this->GetFeildValue (UserInfo_UserID, nUserID);
	this->GetFeildValue (UserInfo_UserName, szUserName);
	this->GetFeildValue (UserInfo_Password, szPassword);
	this->GetFeildValue (UserInfo_UserRight, nUserRight);

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ�ָ���û�����Ϣ
������[IN]  int nUserID, �û�ID
������[OUT] TCHAR szUserName[], �û�����
������[OUT] TCHAR szPassword[], �û�����
������[OUT] int nUserRight, �û�Ȩ��
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetOneUserInfo (int nUserID, TCHAR szUserName[], TCHAR szPassword[], int &nUserRight)
{
	if (szUserName==NULL || szPassword==NULL)
		return RET_DB_PARAM_INVALID;

	CString strSQL, strSP;

	strSP = _T("proc_GetUserInfo");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d"), strSP, nUserID);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	if (this->GetRecordCount() <= 0)
		return RET_DB_RECORD_NULL;

	this->GetFeildValue (UserInfo_UserName, szUserName);
	this->GetFeildValue (UserInfo_Password, szPassword);
	this->GetFeildValue (UserInfo_UserRight, nUserRight);

	return RET_DB_SUCCESS;
}

/*/
���ܣ����һ���û�
������[OUT] int nUserID, �û�ID
������[IN]  TCHAR szUserName[], �û�����
������[IN]  TCHAR szPassword[], �û�����
������[IN]  int nUserRight, �û�Ȩ��
������[IN]  int nDefBufLen, �Զ��建��������
������[IN]  BlobData<byte> *pbdDefBuf, �Զ��建����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::AddUser (int &nUserID, TCHAR szUserName[], TCHAR szPassword[], int nUserRight, int nDefBufLen/*=-1*/, 
							BlobData<byte> *pbdDefBuf/*=NULL*/)
{
	if (szUserName==NULL || szPassword==NULL)
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperate (DB_OPT_ADD, TABLE_USER_INFO);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (UserInfo_UserName, szUserName);
	this->SetFeildValue (UserInfo_Password, szPassword);
	this->SetFeildValue (UserInfo_UserRight, nUserRight);

	if (nDefBufLen >= 0)
	{
		this->SetFeildValue (field_DefBufLen, nDefBufLen);
		this->SetFeildValue (field_DefBuf, pbdDefBuf, nDefBufLen);
	}

	this->UpdateRecord ();

	this->GetFeildValue (UserInfo_UserID, nUserID);

	return RET_DB_SUCCESS;
}

/*/
���ܣ��޸�ָ�����û���Ϣ
������[IN]  int nUserID, �û�ID
������[IN]  TCHAR szUserName[], �û�����
������[IN]  TCHAR szPassword[], �û�����
������[IN]  int nUserRight, �û�Ȩ��
������[IN]  int nDefBufLen, �Զ��建��������
������[IN]  BlobData<byte> *pbdDefBuf, �Զ��建����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ModiUser (int nUserID, TCHAR szUserName[], TCHAR szPassword[], int nUserRight, int nDefBufLen/*=-1*/, 
							BlobData<byte> *pbdDefBuf/*=NULL*/)
{
	if (szUserName==NULL || szPassword==NULL)
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperate (DB_OPT_MODIFY, TABLE_USER_INFO, nUserID);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (UserInfo_UserName, szUserName);
	this->SetFeildValue (UserInfo_Password, szPassword);
	this->SetFeildValue (UserInfo_UserRight, nUserRight);

	if (nDefBufLen >= 0)
	{
		this->SetFeildValue (field_DefBufLen, nDefBufLen);
		this->SetFeildValue (field_DefBuf, pbdDefBuf, nDefBufLen);
	}

	this->UpdateRecord ();

	return RET_DB_SUCCESS;
}

/*/
���ܣ�ɾ��ָ�����û�
������[IN]  int nUserID, �û�ID
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::DelUser (int nUserID)
{
	int nRet = this->RecordOperate (DB_OPT_DELETE, TABLE_USER_INFO, nUserID);

	return nRet;
}

/*/
���ܣ������û����������ж��û��Ƿ����
������[IN]  TCHAR szUserName[], �û�����
������[IN]  TCHAR szPassword[], �û�����
������[OUT] BOOL bExist, �û��Ƿ����
������[OUT] int nUserID, �û�ID
������[OUT] int nUserRight, �û�Ȩ��
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::CheckUserExist (TCHAR szUserName[], TCHAR szPassword[], BOOL &bExist, int &nUserID, int &nUserRight)
{
	CString strSQL, strSP;

	strSP = _T("proc_CheckUserExist");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s '%s', '%s'"), 
				strSP, szUserName, szPassword);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	if (this->GetRecordCount() > 0)
	{
		bExist = TRUE;

		this->GetFeildValue (UserInfo_UserID, nUserID);
		this->GetFeildValue (UserInfo_UserRight, nUserRight);
	}
	else
		bExist = FALSE;

	return RET_DB_SUCCESS;
}

//*************************************************************************************************
//ͼ��

/*/
���ܣ��õ�����ͼ���γɼ�¼��
��������
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ExecGetAllPlot ()
{
	CString strSQL, strSP;

	strSP = _T("proc_ExecGetAllPlot");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s"), strSP);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ���¼���е�ǰͼ��Ϣ
������[OUT] int nPlotID, ͼID
������[OUT] TCHAR szPlotName[], ͼ����
������[OUT] byte byPlotType, ͼ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetCurPlot (int &nPlotID, TCHAR szPlotName[], byte &byPlotType)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	if (szPlotName==NULL)
		return RET_DB_PARAM_INVALID;

	this->GetFeildValue (Plot_PlotID, nPlotID);
	this->GetFeildValue (Plot_PlotName, szPlotName);
	this->GetFeildValue (Plot_PlotType, byPlotType);

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ���¼���е�ǰͼ��Ϣ
������[OUT] int nPlotID, ͼID
������[OUT] TCHAR szPlotName[], ͼ����
������[OUT] byte byPlotType, ͼ����
������[OUT] int nDataLen, ͼ���ݳ���
������[OUT] BlobData<byte> *pbdData, ͼ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetCurPlot (int &nPlotID, TCHAR szPlotName[], byte &byPlotType, int &nDataLen, BlobData<byte> *pbdData)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	if (szPlotName == NULL)
		return RET_DB_PARAM_INVALID;

	this->GetFeildValue (Plot_PlotID, nPlotID);
	this->GetFeildValue (Plot_PlotName, szPlotName);
	this->GetFeildValue (Plot_PlotType, byPlotType);

	this->GetFeildValue (Plot_DataLen, nDataLen);
	this->GetFeildValue (Plot_Data, pbdData);
	if (pbdData != NULL && nDataLen != pbdData->GetSize())
		nDataLen = pbdData->GetSize();

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ�ָ��ͼ����Ϣ
������[IN]  int nPlotID, ͼID
������[OUT] TCHAR szPlotName[], ͼ����
������[OUT] byte byPlotType, ͼ����
������[OUT] int nDataLen, ͼ���ݳ���
������[OUT] BlobData<byte> *pbdData, ͼ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetOnePlotInfo (int nPlotID, TCHAR szPlotName[], byte &byPlotType, int &nDataLen, BlobData<byte> *pbdData)
{
	if (szPlotName == NULL)
		return RET_DB_PARAM_INVALID;

	CString strSQL, strSP;

	strSP = _T("proc_GetPlotInfo");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d"), strSP, nPlotID);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	if (this->GetRecordCount() <= 0)
		return RET_DB_RECORD_NULL;

	this->GetFeildValue (Plot_PlotName, szPlotName);
	this->GetFeildValue (Plot_PlotType, byPlotType);

	this->GetFeildValue (Plot_DataLen, nDataLen);
	this->GetFeildValue (Plot_Data, pbdData);
	if (pbdData != NULL && nDataLen != pbdData->GetSize())
		nDataLen = pbdData->GetSize();

	return RET_DB_SUCCESS;
}

/*/
���ܣ����һ��ͼ
������[OUT] int nPlotID, ͼID
������[IN]  TCHAR szPlotName[], ͼ����
������[IN]  byte byPlotType, ͼ����
������[IN]  int nDataLen, ͼ���ݳ���
������[IN]  BlobData<byte> *pbdData, ͼ����
������[IN]  int nDefBufLen, �Զ��建��������
������[IN]  BlobData<byte> *pbdDefBuf, �Զ��建����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::AddPlot (int &nPlotID, TCHAR szPlotName[], byte byPlotType, int nDataLen, BlobData<byte> *pbdData, 
						int nDefBufLen/*=-1*/, BlobData<byte> *pbdDefBuf/*=NULL*/)
{
	if (szPlotName==NULL)
		return RET_DB_PARAM_INVALID;

	if (nDataLen < 0 || (nDataLen > 0 && pbdData == NULL))
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperate (DB_OPT_ADD, TABLE_PLOT);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (Plot_PlotName, szPlotName);
	this->SetFeildValue (Plot_PlotType, byPlotType);

	this->SetFeildValue (Plot_DataLen, nDataLen);
	this->SetFeildValue (Plot_Data, pbdData, nDataLen);

	if (nDefBufLen >= 0)
	{
		this->SetFeildValue (field_DefBufLen, nDefBufLen);
		this->SetFeildValue (field_DefBuf, pbdDefBuf, nDefBufLen);
	}

	this->UpdateRecord ();

	this->GetFeildValue (Plot_PlotID, nPlotID);

	return RET_DB_SUCCESS;
}

/*/
���ܣ��޸�ָ����ͼ��Ϣ
������[IN]  int nPlotID, ͼID
������[IN]  TCHAR szPlotName[], ͼ����
������[IN]  byte byPlotType, ͼ����
������[IN]  int nDataLen, ͼ���ݳ���
������[IN]  BlobData<byte> *pbdData, ͼ����
������[IN]  int nDefBufLen, �Զ��建��������
������[IN]  BlobData<byte> *pbdDefBuf, �Զ��建����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ModiPlot (int nPlotID, TCHAR szPlotName[], byte byPlotType, int nDataLen, BlobData<byte> *pbdData, 
							int nDefBufLen/*=-1*/, BlobData<byte> *pbdDefBuf/*=NULL*/)
{
	if (szPlotName == NULL)
		return RET_DB_PARAM_INVALID;

	if (nDataLen < 0 || (nDataLen > 0 && pbdData == NULL))
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperate (DB_OPT_MODIFY, TABLE_PLOT, nPlotID);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (Plot_PlotName, szPlotName);
	this->SetFeildValue (Plot_PlotType, byPlotType);

	this->SetFeildValue (Plot_DataLen, nDataLen);
	this->SetFeildValue (Plot_Data, pbdData, nDataLen);

	if (nDefBufLen >= 0)
	{
		this->SetFeildValue (field_DefBufLen, nDefBufLen);
		this->SetFeildValue (field_DefBuf, pbdDefBuf, nDefBufLen);
	}

	this->UpdateRecord ();

	return RET_DB_SUCCESS;
}

/*/
���ܣ��޸�ָ����ͼ����
������[IN]  int nPlotID, ͼID
������[IN]  TCHAR szPlotName[], ͼ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ModiPlotName (int nPlotID, TCHAR szPlotName[])
{
	if (szPlotName == NULL)
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperate (DB_OPT_MODIFY, TABLE_PLOT, nPlotID);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (Plot_PlotName, szPlotName);

	this->UpdateRecord ();

	return RET_DB_SUCCESS;
}

/*/
���ܣ�ɾ��ָ����ͼ
������[IN]  int nPlotID, ͼID
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::DelPlot (int nPlotID)
{
	int nRet = this->RecordOperate (DB_OPT_DELETE, TABLE_PLOT, nPlotID);

	return nRet;
}

/*/
���ܣ�����ɾ��ͼ
������[IN]  int *pnPlotIDArr, ͼID����
������[IN]  int nArrLen, ���鳤��
����ֵ��RET_DBOPT_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::DelPlotBatch (int *pnPlotIDArr, int nArrLen)
{
	if (pnPlotIDArr == NULL || nArrLen <= 0)
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperateBatch (DB_OPT_BATCH_DELETE, TABLE_PLOT, pnPlotIDArr, NULL, nArrLen);

	return nRet;
}


//*************************************************************************************************
//�豸��

/*/
���ܣ��õ������豸���γɼ�¼��
������[IN]  int nDeviceType, �豸���ͣ�Ϊ-1ʱҪȫ���豸������Ҫָ�����͵��豸
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ExecGetAllDevice (int nDeviceType/*=-1*/)
{
	CString strSQL, strSP;

	strSP = _T("proc_ExecGetAllDevice");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d"), strSP, nDeviceType);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ���¼���е�ǰ�豸��Ϣ
������[OUT] int nDeviceNo, �豸���
������[OUT] TCHAR szDeviceName[], �豸����
������[OUT] int nDeviceType, �豸����
������[OUT] int nDriverType, ��������
������[OUT] int nChannelCount, ͨ������
������[OUT] TCHAR szDeviceGUID[], �豸GUID
������[OUT] int nSampleInterval, �ɼ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetCurDevice (int &nDeviceNo, TCHAR szDeviceName[], int &nDeviceType, int &nDriverType, 
								int &nChannelCount, TCHAR szDeviceGUID[], int &nSampleInterval)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	if (szDeviceName == NULL || szDeviceGUID == NULL)
		return RET_DB_PARAM_INVALID;

	this->GetFeildValue (Device_DeviceNo, nDeviceNo);
	this->GetFeildValue (Device_DeviceName, szDeviceName);
	this->GetFeildValue (Device_DeviceType, nDeviceType);
	this->GetFeildValue (Device_DriverType, nDriverType);
	this->GetFeildValue (Device_ChannelCount, nChannelCount);
	this->GetFeildValue (Device_DeviceGUID, szDeviceGUID);
	this->GetFeildValue (Device_SampleInterval, nSampleInterval);

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ�ָ���豸��Ϣ
������[IN]  int nDeviceNo, �豸���
������[OUT] TCHAR szDeviceName[], �豸����
������[OUT] int nDeviceType, �豸����
������[OUT] int nDriverType, ��������
������[OUT] int nChannelCount, ͨ������
������[OUT] TCHAR szDeviceGUID[], �豸GUID
������[OUT] int nSampleInterval, �ɼ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetOneDeviceInfo (int nDeviceNo, TCHAR szDeviceName[], int &nDeviceType, int &nDriverType, 
									int &nChannelCount, TCHAR szDeviceGUID[], int &nSampleInterval)
{
	if (szDeviceName == NULL || szDeviceGUID == NULL)
		return RET_DB_PARAM_INVALID;

	CString strSQL, strSP;

	strSP = _T("proc_GetDeviceInfo");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d"), strSP, nDeviceNo);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	if (this->GetRecordCount() <= 0)
		return RET_DB_RECORD_NULL;

	this->GetFeildValue (Device_DeviceName, szDeviceName);
	this->GetFeildValue (Device_DeviceType, nDeviceType);
	this->GetFeildValue (Device_DriverType, nDriverType);
	this->GetFeildValue (Device_ChannelCount, nChannelCount);
	this->GetFeildValue (Device_DeviceGUID, szDeviceGUID);
	this->GetFeildValue (Device_SampleInterval, nSampleInterval);

	return RET_DB_SUCCESS;
}

/*/
���ܣ����һ���豸
������[IN]  int nDeviceNo, �豸���
������[IN]  TCHAR szDeviceName[], �豸����
������[IN]  int nDeviceType, �豸����
������[IN]  int nDriverType, ��������
������[IN]  int nChannelCount, ͨ������
������[IN]  TCHAR szDeviceGUID[], �豸GUID
������[IN]  int nSampleInterval, �ɼ����
������[IN]  int nDefBufLen, �Զ��建��������
������[IN]  BlobData<byte> *pbdDefBuf, �Զ��建����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::AddDevice (int &nDeviceNo, TCHAR szDeviceName[], int nDeviceType, int nDriverType, 
							int nChannelCount, TCHAR szDeviceGUID[], int nSampleInterval, 
							int nDefBufLen/*=-1*/, BlobData<byte> *pbdDefBuf/*=NULL*/)
{
	if (szDeviceName == NULL || szDeviceGUID == NULL)
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperate (DB_OPT_ADD, TABLE_DEVICE);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (Device_DeviceNo, nDeviceNo);
	this->SetFeildValue (Device_DeviceName, szDeviceName);
	this->SetFeildValue (Device_DeviceType, nDeviceType);
	this->SetFeildValue (Device_DriverType, nDriverType);
	this->SetFeildValue (Device_ChannelCount, nChannelCount);
 	this->SetFeildValue (Device_DeviceGUID, szDeviceGUID);
	this->SetFeildValue (Device_SampleInterval, nSampleInterval);

	if (nDefBufLen >= 0)
	{
		this->SetFeildValue (field_DefBufLen, nDefBufLen);
		this->SetFeildValue (field_DefBuf, pbdDefBuf, nDefBufLen);
	}

	this->UpdateRecord ();

	return RET_DB_SUCCESS;
}

/*/
���ܣ��޸�ָ���豸
������[IN]  int nDeviceNo, �豸���
������[IN]  TCHAR szDeviceName[], �豸����
������[IN]  int nDeviceType, �豸����
������[IN]  int nDriverType, ��������
������[IN]  int nChannelCount, ͨ������
������[IN]  TCHAR szDeviceGUID[], �豸GUID
������[IN]  int nSampleInterval, �ɼ����
������[IN]  int nDefBufLen, �Զ��建��������
������[IN]  BlobData<byte> *pbdDefBuf, �Զ��建����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ModiDevice (int nDeviceNo, TCHAR szDeviceName[], int nDeviceType, int nDriverType, 
							int nChannelCount, TCHAR szDeviceGUID[], int nSampleInterval, 
							int nDefBufLen/*=-1*/, BlobData<byte> *pbdDefBuf/*=NULL*/)
{
	if (szDeviceName == NULL || szDeviceGUID == NULL)
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperate (DB_OPT_MODIFY, TABLE_DEVICE, nDeviceNo);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (Device_DeviceName, szDeviceName);
	this->SetFeildValue (Device_DeviceType, nDeviceType);
	this->SetFeildValue (Device_DriverType, nDriverType);
	this->SetFeildValue (Device_ChannelCount, nChannelCount);
	this->SetFeildValue (Device_DeviceGUID, szDeviceGUID);
	this->SetFeildValue (Device_SampleInterval, nSampleInterval);

	if (nDefBufLen >= 0)
	{
		this->SetFeildValue (field_DefBufLen, nDefBufLen);
		this->SetFeildValue (field_DefBuf, pbdDefBuf, nDefBufLen);
	}

	this->UpdateRecord ();

	return RET_DB_SUCCESS;
}

/*/
���ܣ�ɾ��ָ���豸
������[IN]  int nDeviceNo, �豸���
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::DelDevice (int nDeviceNo)
{
	int nRet = this->RecordOperate (DB_OPT_DELETE, TABLE_DEVICE, nDeviceNo);

	return nRet;
}


//*************************************************************************************************
//ͨ����

/*/
���ܣ��õ�����ͨ�����γɼ�¼��
������[IN]  int nDeviceNo, �豸��š�Ĭ��ֵΪ-1ʱҪ����ͨ��������Ҫָ���豸��ͨ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ExecGetAllChannel (int nDeviceNo/*=-1*/)
{
	CString strSQL, strSP;

	strSP = _T("proc_ExecGetAllChannel");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d"), strSP, nDeviceNo);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ���¼���е�ǰͨ����Ϣ
������[OUT] int nDeviceNo, �豸���
������[OUT] int nChannelNo, ͨ�����
������[OUT] TCHAR szChannelName[], ͨ������
������[OUT] BOOL bWorkEnable, ��������
������[OUT] float fFullScaleHigh, �����̸�
������[OUT] float fFullScaleHigh, �����̵�
������[OUT] float fFullScaleHigh, У���
������[OUT] float fFullScaleHigh, У���
������[OUT] byte byDataType, ��������
������[OUT] byte byDecimalDigits, ����ֵС��λ��
������[OUT] TCHAR szUnitString[], ��λ�ַ���
������[OUT] int nAlarmLevelDataLen, �����������ݳ���
������[OUT] BlobData<byte> *pbdAlarmLevelData, ������������
������[OUT] int nAlarmRangeDataLen, ������Χ���ݳ���
������[OUT] BlobData<byte> *pbdAlarmRangeData, ������Χ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetCurChannel (int &nDeviceNo, int &nChannelNo, TCHAR szChannelName[], BOOL &bWorkEnable, float &fFullScaleHigh, 
								float &fFullScaleLow, float &fCalibrationHigh, float &fCalibrationLow, 
								byte &byDataType, byte &byDecimalDigits, TCHAR szUnitString[], int &nAlarmLevelDataLen, 
								BlobData<byte> *pbdAlarmLevelData, int &nAlarmRangeDataLen, BlobData<byte> *pbdAlarmRangeData)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	if (szChannelName == NULL || szUnitString == NULL)
		return RET_DB_PARAM_INVALID;

	this->GetFeildValue (Channel_DeviceNo, nDeviceNo);
	this->GetFeildValue (Channel_ChannelNo, nChannelNo);
	this->GetFeildValue (Channel_ChannelName, szChannelName);
	byte byWorkEnable;
	this->GetFeildValue (Channel_WorkEnable, byWorkEnable);
	bWorkEnable = (BOOL )byWorkEnable;
	this->GetFeildValue (Channel_FullScaleHigh, fFullScaleHigh);
	this->GetFeildValue (Channel_FullScaleLow, fFullScaleLow);
	this->GetFeildValue (Channel_CalibrationHigh, fCalibrationHigh);
	this->GetFeildValue (Channel_CalibrationLow, fCalibrationLow);
	this->GetFeildValue (Channel_DataType, byDataType);
	this->GetFeildValue (Channel_DecimalDigits, byDecimalDigits);
	this->GetFeildValue (Channel_UnitString, szUnitString);

	this->GetFeildValue (Channel_AlarmLevelDataLen, nAlarmLevelDataLen);
	this->GetFeildValue (Channel_AlarmLevelData, pbdAlarmLevelData);

	this->GetFeildValue (Channel_AlarmRangeDataLen, nAlarmRangeDataLen);
	this->GetFeildValue (Channel_AlarmRangeData, pbdAlarmRangeData);

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ�ָ��ͨ����Ϣ
������[IN]  int nDeviceNo, �豸���
������[IN]  int nChannelNo, ͨ�����
������[OUT] TCHAR szChannelName[], ͨ������
������[OUT] BOOL bWorkEnable, ��������
������[OUT] float fFullScaleHigh, �����̸�
������[OUT] float fFullScaleHigh, �����̵�
������[OUT] float fFullScaleHigh, У���
������[OUT] float fFullScaleHigh, У���
������[OUT] byte byDataType, ��������
������[OUT] byte byDecimalDigits, ����ֵС��λ��
������[OUT] TCHAR szUnitString[], ��λ�ַ���
������[OUT] int nAlarmLevelDataLen, �����������ݳ���
������[OUT] BlobData<byte> *pbdAlarmLevelData, ������������
������[OUT] int nAlarmRangeDataLen, ������Χ���ݳ���
������[OUT] BlobData<byte> *pbdAlarmRangeData, ������Χ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetOneChannelInfo (int nDeviceNo, int nChannelNo, TCHAR szChannelName[], BOOL &bWorkEnable, float &fFullScaleHigh, 
									float &fFullScaleLow, float &fCalibrationHigh, float &fCalibrationLow, 
									byte &byDataType, byte &byDecimalDigits, TCHAR szUnitString[], int &nAlarmLevelDataLen, 
									BlobData<byte> *pbdAlarmLevelData, int &nAlarmRangeDataLen, BlobData<byte> *pbdAlarmRangeData)
{
	if (szChannelName == NULL || szUnitString == NULL)
		return RET_DB_PARAM_INVALID;

	CString strSQL, strSP;

	strSP = _T("proc_GetChannelInfo");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d, %d"), strSP, nDeviceNo, nChannelNo);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	if (this->GetRecordCount() <= 0)
		return RET_DB_RECORD_NULL;

	this->GetFeildValue (Channel_ChannelName, szChannelName);
	byte byWorkEnable;
	this->GetFeildValue (Channel_WorkEnable, byWorkEnable);
	bWorkEnable = (BOOL )byWorkEnable;
	this->GetFeildValue (Channel_FullScaleHigh, fFullScaleHigh);
	this->GetFeildValue (Channel_FullScaleLow, fFullScaleLow);
	this->GetFeildValue (Channel_CalibrationHigh, fCalibrationHigh);
	this->GetFeildValue (Channel_CalibrationLow, fCalibrationLow);
	this->GetFeildValue (Channel_DataType, byDataType);
	this->GetFeildValue (Channel_DecimalDigits, byDecimalDigits);
	this->GetFeildValue (Channel_UnitString, szUnitString);

	this->GetFeildValue (Channel_AlarmLevelDataLen, nAlarmLevelDataLen);
	this->GetFeildValue (Channel_AlarmLevelData, pbdAlarmLevelData);

	this->GetFeildValue (Channel_AlarmRangeDataLen, nAlarmRangeDataLen);
	this->GetFeildValue (Channel_AlarmRangeData, pbdAlarmRangeData);

	return RET_DB_SUCCESS;
}

/*/
���ܣ����һ��ͨ��
������[IN]  int nDeviceNo, �豸���
������[IN]  int nChannelNo, ͨ�����
������[IN]  TCHAR szChannelName[], ͨ������
������[IN]  BOOL bWorkEnable, ��������
������[IN]  float fFullScaleHigh, �����̸�
������[IN]  float fFullScaleHigh, �����̵�
������[IN]  float fFullScaleHigh, У���
������[IN]  float fFullScaleHigh, У���
������[IN]  byte byDataType, ��������
������[IN]  byte byDecimalDigits, ����ֵС��λ��
������[IN]  TCHAR szUnitString[], ��λ�ַ���
������[IN]  int nAlarmLevelDataLen, �����������ݳ���
������[IN]  BlobData<byte> *pbdAlarmLevelData, ������������
������[IN]  int nAlarmRangeDataLen, ������Χ���ݳ���
������[IN]  BlobData<byte> *pbdAlarmRangeData, ������Χ����
������[IN]  int nDefBufLen, �Զ��建��������
������[IN]  BlobData<byte> *pbdDefBuf, �Զ��建����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::AddChannel (int nDeviceNo, int nChannelNo, TCHAR szChannelName[], BOOL bWorkEnable, float fFullScaleHigh, 
							float fFullScaleLow, float fCalibrationHigh, float fCalibrationLow, 
							byte byDataType, byte byDecimalDigits, TCHAR szUnitString[], int nAlarmLevelDataLen, 
							BlobData<byte> *pbdAlarmLevelData, int nAlarmRangeDataLen, BlobData<byte> *pbdAlarmRangeData, 
							int nDefBufLen/*=-1*/, BlobData<byte> *pbdDefBuf/*=NULL*/)
{
	if (szChannelName == NULL || szUnitString == NULL)
		return RET_DB_PARAM_INVALID;

	if (nAlarmLevelDataLen < 0 || (nAlarmLevelDataLen > 0 && pbdAlarmLevelData == NULL))
		return RET_DB_PARAM_INVALID;

	if (nAlarmRangeDataLen < 0 || (nAlarmRangeDataLen > 0 && pbdAlarmRangeData == NULL))
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperate (DB_OPT_ADD, TABLE_CHANNEL);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (Channel_DeviceNo, nDeviceNo);
	this->SetFeildValue (Channel_ChannelNo, nChannelNo);
	this->SetFeildValue (Channel_ChannelName, szChannelName);
	this->SetFeildValue (Channel_WorkEnable, (byte )bWorkEnable);
	this->SetFeildValue (Channel_FullScaleHigh, fFullScaleHigh);
	this->SetFeildValue (Channel_FullScaleLow, fFullScaleLow);
	this->SetFeildValue (Channel_CalibrationHigh, fCalibrationHigh);
	this->SetFeildValue (Channel_CalibrationLow, fCalibrationLow);
	this->SetFeildValue (Channel_DataType, byDataType);
	this->SetFeildValue (Channel_DecimalDigits, byDecimalDigits);
	this->SetFeildValue (Channel_UnitString, szUnitString);

	this->SetFeildValue (Channel_AlarmLevelDataLen, nAlarmLevelDataLen);
	this->SetFeildValue (Channel_AlarmLevelData, pbdAlarmLevelData, nAlarmLevelDataLen);

	this->SetFeildValue (Channel_AlarmRangeDataLen, nAlarmRangeDataLen);
	this->SetFeildValue (Channel_AlarmRangeData, pbdAlarmRangeData, nAlarmRangeDataLen);

	if (nDefBufLen >= 0)
	{
		this->SetFeildValue (field_DefBufLen, nDefBufLen);
		this->SetFeildValue (field_DefBuf, pbdDefBuf, nDefBufLen);
	}

	this->UpdateRecord ();

	return RET_DB_SUCCESS;
}

/*/
���ܣ��޸�ָ��ͨ��
������[IN]  int nDeviceNo, �豸���
������[IN]  int nChannelNo, ͨ�����
������[IN]  TCHAR szChannelName[], ͨ������
������[IN]  BOOL bWorkEnable, ��������
������[IN]  float fFullScaleHigh, �����̸�
������[IN]  float fFullScaleHigh, �����̵�
������[IN]  float fFullScaleHigh, У���
������[IN]  float fFullScaleHigh, У���
������[IN]  byte byDataType, ��������
������[IN]  byte byDecimalDigits, ����ֵС��λ��
������[IN]  TCHAR szUnitString[], ��λ�ַ���
������[IN]  int nAlarmLevelDataLen, �����������ݳ���
������[IN]  BlobData<byte> *pbdAlarmLevelData, ������������
������[IN]  int nAlarmRangeDataLen, ������Χ���ݳ���
������[IN]  BlobData<byte> *pbdAlarmRangeData, ������Χ����
������[IN]  int nDefBufLen, �Զ��建��������
������[IN]  BlobData<byte> *pbdDefBuf, �Զ��建����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ModiChannel (int nDeviceNo, int nChannelNo, TCHAR szChannelName[], BOOL bWorkEnable, float fFullScaleHigh, 
							float fFullScaleLow, float fCalibrationHigh, float fCalibrationLow, byte byDataType, 
							byte byDecimalDigits, TCHAR szUnitString[], int nAlarmLevelDataLen, 
							BlobData<byte> *pbdAlarmLevelData, int nAlarmRangeDataLen, BlobData<byte> *pbdAlarmRangeData, 
							int nDefBufLen/*=-1*/, BlobData<byte> *pbdDefBuf/*=NULL*/)
{
	if (szChannelName == NULL || szUnitString == NULL)
		return RET_DB_PARAM_INVALID;

	if (nAlarmLevelDataLen < 0 || (nAlarmLevelDataLen > 0 && pbdAlarmLevelData == NULL))
		return RET_DB_PARAM_INVALID;

	if (nAlarmRangeDataLen < 0 || (nAlarmRangeDataLen > 0 && pbdAlarmRangeData == NULL))
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperate (DB_OPT_MODIFY, TABLE_CHANNEL, nDeviceNo, nChannelNo);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (Channel_ChannelName, szChannelName);
	this->SetFeildValue (Channel_WorkEnable, (byte )bWorkEnable);
	this->SetFeildValue (Channel_FullScaleHigh, fFullScaleHigh);
	this->SetFeildValue (Channel_FullScaleLow, fFullScaleLow);
	this->SetFeildValue (Channel_CalibrationHigh, fCalibrationHigh);
	this->SetFeildValue (Channel_CalibrationLow, fCalibrationLow);
	this->SetFeildValue (Channel_DataType, byDataType);
	this->SetFeildValue (Channel_DecimalDigits, byDecimalDigits);
	this->SetFeildValue (Channel_UnitString, szUnitString);

	this->SetFeildValue (Channel_AlarmLevelDataLen, nAlarmLevelDataLen);
	this->SetFeildValue (Channel_AlarmLevelData, pbdAlarmLevelData, nAlarmLevelDataLen);

	this->SetFeildValue (Channel_AlarmRangeDataLen, nAlarmRangeDataLen);
	this->SetFeildValue (Channel_AlarmRangeData, pbdAlarmRangeData, nAlarmRangeDataLen);

	if (nDefBufLen >= 0)
	{
		this->SetFeildValue (field_DefBufLen, nDefBufLen);
		this->SetFeildValue (field_DefBuf, pbdDefBuf, nDefBufLen);
	}

	this->UpdateRecord ();

	return RET_DB_SUCCESS;
}

/*/
���ܣ�ɾ��ָ��ͨ��
������[IN]  int nDeviceNo, �豸���
������[IN]  int nChannelNo, ͨ�����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::DelChannel (int nDeviceNo, int nChannelNo)
{
	int nRet = this->RecordOperate (DB_OPT_DELETE, TABLE_CHANNEL, nDeviceNo, nChannelNo);

	return nRet;
}


//*************************************************************************************************
//ͨ�����ݱ�

/*/
���ܣ��õ�ָ��ͨ��������ͨ�����ݣ��γɼ�¼��
������[IN]  int nDeviceNo, �豸��š�
������[IN]  int nChannelNo, ͨ����š�
������[IN]  BOOL bIsGetAll, �Ƿ�Ҫͨ����ȫ�����ݡ�TRUEΪҪȫ�����ݣ�FALSEΪָ��ʱ�䷶ΧҪ��
������[IN]  COleDateTime odtBeginTime, ʱ�䷶Χ��ʼʱ�䡣
������[IN]  COleDateTime odtEndTime, ʱ�䷶Χ����ʱ�䡣
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ExecGetAllChannelData (int nDeviceNo, int nChannelNo, BOOL bIsGetAll/*=TRUE*/, 
										COleDateTime odtBeginTime/*=DT_RANGE_BEGIN*/, 
										COleDateTime odtEndTime/*=DT_RANGE_END*/)
{
	CString strSQL, strSP, strBeginTime, strEndTime;

	strBeginTime.Format(_T("%d-%d-%d %d:%d:%d"),
						odtBeginTime.GetYear(), odtBeginTime.GetMonth(), odtBeginTime.GetDay(),
						odtBeginTime.GetHour(), odtBeginTime.GetMinute(), odtBeginTime.GetSecond());
	strEndTime.Format(_T("%d-%d-%d %d:%d:%d"),
						odtEndTime.GetYear(), odtEndTime.GetMonth(), odtEndTime.GetDay(),
						odtEndTime.GetHour(), odtEndTime.GetMinute(), odtEndTime.GetSecond());

	strSP = _T("proc_ExecGetAllChannelData");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d, %d, %d, '%s', '%s'"), 
					strSP, nDeviceNo, nChannelNo, bIsGetAll, strBeginTime, strEndTime);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ�ָ���豸������ͨ�������һ�����ݣ��γɼ�¼��
������[IN]  int nDeviceNo, �豸��š�
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ExecGetAllLastChannelData (int nDeviceNo)
{
	CString strSQL, strSP;

	strSP = _T("proc_ExecGetAllLastChannelData");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d"), strSP, nDeviceNo);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ���¼���е�ǰͨ��������Ϣ
������[OUT] int nDataID, ����ID
������[OUT] int nDeviceNo, �豸���
������[OUT] int nChannelNo, ͨ�����
������[OUT] COleDateTime odtSampleTime, �ɼ�ʱ��
������[OUT] float fMeasureValue, ����ֵ
������[OUT] byte byAlarmLevel, ��������
������[OUT] int nDataLen, ���ݳ���
������[OUT] BlobData<byte> *pbdData, ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetCurChannelData (int &nDataID, int &nDeviceNo, int &nChannelNo, COleDateTime &odtSampleTime, 
									float &fMeasureValue, byte &byAlarmLevel, int &nDataLen, BlobData<byte> *pbdData)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	this->GetFeildValue (ChannelData_DataID, nDataID);
	this->GetFeildValue (ChannelData_DeviceNo, nDeviceNo);
	this->GetFeildValue (ChannelData_ChannelNo, nChannelNo);
	this->GetFeildValue (ChannelData_SampleTime, odtSampleTime);
	this->GetFeildValue (ChannelData_MeasureValue, fMeasureValue);
	this->GetFeildValue (ChannelData_AlarmLevel, byAlarmLevel);

	this->GetFeildValue (ChannelData_DataLen, nDataLen);
	this->GetFeildValue (ChannelData_Data, pbdData);
	if (pbdData != NULL && nDataLen != pbdData->GetSize())
		nDataLen = pbdData->GetSize();

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ�ָ��ͨ��������Ϣ
������[IN]  int nDataID, ����ID
������[IN]  int nDeviceNo, �豸���
������[IN]  int nChannelNo, ͨ�����
������[OUT] COleDateTime odtSampleTime, �ɼ�ʱ��
������[OUT] float fMeasureValue, ����ֵ
������[OUT] byte byAlarmLevel, ��������
������[OUT] int nDataLen, ���ݳ���
������[OUT] BlobData<byte> *pbdData, ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetOneChannelDataInfo (int nDataID, int nDeviceNo, int nChannelNo, COleDateTime &odtSampleTime, 
										float &fMeasureValue, byte &byAlarmLevel, int &nDataLen, BlobData<byte> *pbdData)
{
	CString strSQL, strSP;

	strSP = _T("proc_GetChannelDataInfo");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d, %d, %d"), strSP, nDataID, nDeviceNo, nChannelNo);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	if (this->GetRecordCount() <= 0)
		return RET_DB_RECORD_NULL;

	this->GetFeildValue (ChannelData_SampleTime, odtSampleTime);
	this->GetFeildValue (ChannelData_MeasureValue, fMeasureValue);
	this->GetFeildValue (ChannelData_AlarmLevel, byAlarmLevel);

	this->GetFeildValue (ChannelData_DataLen, nDataLen);
	this->GetFeildValue (ChannelData_Data, pbdData);
	if (pbdData != NULL && nDataLen != pbdData->GetSize())
		nDataLen = pbdData->GetSize();

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ�ָ��ͨ��������Ϣ
������[OUT] int nDataID, ����ID
������[IN]  int nDeviceNo, �豸���
������[IN]  int nChannelNo, ͨ�����
������[OUT] COleDateTime odtSampleTime, �ɼ�ʱ��
������[OUT] float fMeasureValue, ����ֵ
������[OUT] byte byAlarmLevel, ��������
������[OUT] int nDataLen, ���ݳ���
������[OUT] BlobData<byte> *pbdData, ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetLastChannelDataInfo (int &nDataID, int nDeviceNo, int nChannelNo, COleDateTime &odtSampleTime, 
										float &fMeasureValue, byte &byAlarmLevel, int &nDataLen, BlobData<byte> *pbdData)
{
	CString strSQL, strSP;

	strSP = _T("proc_GetLastChannelDataInfo");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d, %d"), strSP, nDeviceNo, nChannelNo);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	if (this->GetRecordCount() <= 0)
		return RET_DB_RECORD_NULL;

	this->GetFeildValue (ChannelData_DataID, nDataID);
	this->GetFeildValue (ChannelData_SampleTime, odtSampleTime);
	this->GetFeildValue (ChannelData_MeasureValue, fMeasureValue);
	this->GetFeildValue (ChannelData_AlarmLevel, byAlarmLevel);

	this->GetFeildValue (ChannelData_DataLen, nDataLen);
	this->GetFeildValue (ChannelData_Data, pbdData);
	if (pbdData != NULL && nDataLen != pbdData->GetSize())
		nDataLen = pbdData->GetSize();

	return RET_DB_SUCCESS;
}

/*/
���ܣ����һ��ͨ������
������[OUT] int nDataID, ����ID
������[IN]  int nDeviceNo, �豸���
������[IN]  int nChannelNo, ͨ�����
������[IN]  COleDateTime odtSampleTime, �ɼ�ʱ��
������[IN]  float fMeasureValue, ����ֵ
������[IN]  byte byAlarmLevel, ��������
������[IN]  int nDataLen, ���ݳ���
������[IN]  BlobData<byte> *pbdData, ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::AddChannelData (int &nDataID, int nDeviceNo, int nChannelNo, COleDateTime odtSampleTime, float fMeasureValue, 
								byte byAlarmLevel, int nDataLen, BlobData<byte> *pbdData)
{
	if (nDataLen < 0 || (nDataLen > 0 && pbdData == NULL))
		return RET_DB_PARAM_INVALID;

	CString strTableName, strKeyIDName, strKeyExIDName;

	if (!this->GetTableInfo (TABLE_CHANNEL_DATA, strTableName, strKeyIDName, strKeyExIDName))
		return RET_DB_PARAM_INVALID;

	//ͨ�����ݱ�ı�����Ҫ��̬��ƴ
	strTableName = this->GetChannelDataTableName (nDeviceNo, nChannelNo);

	int nRet = this->RecordOperate (DB_OPT_ADD, strTableName, strKeyIDName, strKeyExIDName);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	//ע�⣺
	//�����ݿ������������ݼ�¼ʱ���ɷ������Զ���õ�ǰʱ�䣬������֤����DPUģ��ɼ������ݶ��Է�����ʱ��Ϊ׼

	this->SetFeildValue (ChannelData_DeviceNo, nDeviceNo);
	this->SetFeildValue (ChannelData_ChannelNo, nChannelNo);
// 	this->SetFeildValue (ChannelData_SampleTime, odtSampleTime);
	this->SetFeildValue (ChannelData_MeasureValue, fMeasureValue);
	this->SetFeildValue (ChannelData_AlarmLevel, byAlarmLevel);

	this->SetFeildValue (ChannelData_DataLen, nDataLen);
	this->SetFeildValue (ChannelData_Data, pbdData, nDataLen);

	this->UpdateRecord ();

	this->GetFeildValue (ChannelData_DataID, nDataID);

/*	//�����ݱ�Ĳ��봥�������жϱ������������������뵽�����б���
	//��������Ҫ�ӵ��������ݱ���
	if (byAlarmLevel > 0)
	{
		//�������ݵĲɼ�ͳһ�����ݿ����������ʱ������ڴ˴�ɼ�ʱ��Ҫ����
		COleDateTime odtSampleTimeTemp;
		this->GetFeildValue (ChannelData_SampleTime, odtSampleTimeTemp);

		int nAlarmDataID;
		this->AddAlarmData (nAlarmDataID, nDataID, nDeviceNo, nChannelNo, odtSampleTimeTemp, fMeasureValue, byAlarmLevel);
	}
*/
	return RET_DB_SUCCESS;
}

/*/
���ܣ��޸�ָ��ͨ������
������[IN]  int nDataID, ����ID
������[IN]  int nDeviceNo, �豸���
������[IN]  int nChannelNo, ͨ�����
������[IN]  COleDateTime odtSampleTime, �ɼ�ʱ��
������[IN]  float fMeasureValue, ����ֵ
������[IN]  byte byAlarmLevel, ��������
������[IN]  int nDataLen, ���ݳ���
������[IN]  BlobData<byte> *pbdData, ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ModiChannelData (int nDataID, int nDeviceNo, int nChannelNo, COleDateTime odtSampleTime, float fMeasureValue, 
								byte byAlarmLevel, int nDataLen, BlobData<byte> *pbdData)
{
	if (nDataLen < 0 || (nDataLen > 0 && pbdData == NULL))
		return RET_DB_PARAM_INVALID;

	CString strTableName, strKeyIDName, strKeyExIDName;

	if (!this->GetTableInfo (TABLE_CHANNEL_DATA, strTableName, strKeyIDName, strKeyExIDName))
		return RET_DB_PARAM_INVALID;

	//ͨ�����ݱ�ı�����Ҫ��̬��ƴ
	strTableName = this->GetChannelDataTableName (nDeviceNo, nChannelNo);

	int nRet = this->RecordOperate (DB_OPT_MODIFY, strTableName, strKeyIDName, strKeyExIDName, nDataID);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (ChannelData_SampleTime, odtSampleTime);
	this->SetFeildValue (ChannelData_MeasureValue, fMeasureValue);
	this->SetFeildValue (ChannelData_AlarmLevel, byAlarmLevel);

	this->SetFeildValue (ChannelData_DataLen, nDataLen);
	this->SetFeildValue (ChannelData_Data, pbdData, nDataLen);

	this->UpdateRecord ();

	return RET_DB_SUCCESS;
}

/*/
���ܣ�ɾ��ָ��ͨ������
������[IN]  int nDeviceNo, �豸���
������[IN]  int nChannelNo, ͨ�����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::DelChannelData (int nDataID, int nDeviceNo, int nChannelNo)
{
	CString strTableName, strKeyIDName, strKeyExIDName;

	if (!this->GetTableInfo (TABLE_CHANNEL_DATA, strTableName, strKeyIDName, strKeyExIDName))
		return RET_DB_PARAM_INVALID;

	//ͨ�����ݱ�ı�����Ҫ��̬��ƴ
	strTableName = this->GetChannelDataTableName (nDeviceNo, nChannelNo);

	int nRet = this->RecordOperate (DB_OPT_DELETE, strTableName, strKeyIDName, strKeyExIDName, nDataID);

	return nRet;
}

/*/
���ܣ�����ɾ��ָ��ͨ�����ݱ������
������[IN]  int nDeviceNo, �豸���
������[IN]  int nChannelNo, ͨ�����
������[IN]  BOOL bIsDelAll, �Ƿ�ɾ��ͨ����ȫ�����ݡ�TRUEΪҪȫ�����ݣ�FALSEΪָ��ʱ�䷶ΧҪ��
������[IN]  COleDateTime odtBeginTime, ʱ�䷶Χ��ʼʱ�䡣
������[IN]  COleDateTime odtEndTime, ʱ�䷶Χ����ʱ�䡣
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::DelAllChannelData (int nDeviceNo, int nChannelNo, BOOL bIsDelAll/*=TRUE*/, 
									COleDateTime odtBeginTime/*=DT_RANGE_BEGIN*/, 
									COleDateTime odtEndTime/*=DT_RANGE_END*/)
{
	CString strSQL, strSP, strBeginTime, strEndTime;

	strBeginTime.Format(_T("%d-%d-%d %d:%d:%d"),
						odtBeginTime.GetYear(), odtBeginTime.GetMonth(), odtBeginTime.GetDay(),
						odtBeginTime.GetHour(), odtBeginTime.GetMinute(), odtBeginTime.GetSecond());
	strEndTime.Format(_T("%d-%d-%d %d:%d:%d"),
						odtEndTime.GetYear(), odtEndTime.GetMonth(), odtEndTime.GetDay(),
						odtEndTime.GetHour(), odtEndTime.GetMinute(), odtEndTime.GetSecond());

	strSP = _T("proc_DeleteAllChannelData");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d, %d, %d, '%s', '%s'"), 
					strSP, nDeviceNo, nChannelNo, bIsDelAll, strBeginTime, strEndTime);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	return RET_DB_SUCCESS;
}

/*/
���ܣ������豸�š�ͨ���ŵõ�ͨ�����ݱ�ı�����
������[IN]  int nDeviceNo, �豸���
������[IN]  int nChannelNo, ͨ�����
����ֵ�������ơ�
/*/
CString CDBAccess::GetChannelDataTableName (int nDeviceNo, int nChannelNo)
{
	CString strTableName;
	strTableName.Format(_T("%s_%d_%d"), table_ChannelData, nDeviceNo, nChannelNo);
	return strTableName;
}

/*/
���ܣ��������ͨ�����ݿ�ʼ
��������
����ֵ��RET_DBOPT_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::BeginAddChannelDataBatch ()
{
	int nRet = this->RecordOperateBatch (DB_OPT_BATCH_ADD, TABLE_CHANNEL_DATA);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	return RET_DB_SUCCESS;
}

/*/
���ܣ��������������ͨ������
������[IN]  int nDeviceNo, �豸���
������[IN]  int nChannelNo, ͨ�����
������[IN]  COleDateTime odtSampleTime, �ɼ�ʱ��
������[IN]  float fMeasureValue, ����ֵ
������[IN]  byte byAlarmLevel, ��������
������[IN]  int nDataLen, ���ݳ���
������[IN]  BlobData<byte> *pbdData, ����
����ֵ��RET_DBOPT_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::AddChannelDataBatch (int nDeviceNo, int nChannelNo, COleDateTime odtSampleTime, float fMeasureValue, byte byAlarmLevel, 
									int nDataLen, BlobData<byte> *pbdData)
{
	if (!this->AddNewRecord ())
		return RET_DB_ADD_RECORD_FAIL;

	//ע�⣺
	//�����ݿ������������ݼ�¼ʱ���ɷ������Զ���õ�ǰʱ�䣬������֤����DPUģ��ɼ������ݶ��Է�����ʱ��Ϊ׼

	this->SetFeildValue (ChannelData_DeviceNo, nDeviceNo);
	this->SetFeildValue (ChannelData_ChannelNo, nChannelNo);
// 	this->SetFeildValue (ChannelData_SampleTime, odtSampleTime);
	this->SetFeildValue (ChannelData_MeasureValue, fMeasureValue);
	this->SetFeildValue (ChannelData_AlarmLevel, byAlarmLevel);

	this->SetFeildValue (ChannelData_DataLen, nDataLen);
	this->SetFeildValue (ChannelData_Data, pbdData, nDataLen);

	return RET_DB_SUCCESS;
}

/*/
���ܣ��������ͨ�����ݽ���
������[INOUT]  int *pnDataIDArr, ����ID����
������[INOUT]  int &nArrLen, ���鳤��
����ֵ��RET_DBOPT_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::EndAddChannelDataBatch (int *pnDataIDArr, int &nArrLen)
{
	if (!this->UpdateRecord (TRUE))
		return RET_DB_EXE_FAIL;

	if (pnDataIDArr != NULL && nArrLen > 0)
	{
		//ѭ��Ҫ��ID
		int nDataID;
		int nCount = 0;

		this->MoveFirst ();
		while (!this->IsReachEnd())
		{
			this->GetFeildValue (ChannelData_DataID, nDataID);
			pnDataIDArr[nCount] = nDataID;

			nCount ++;
			if (nCount >= nArrLen)
				break;

			this->MoveNext();
		}

		nArrLen = nCount;	//�����¼����
	}

	return RET_DB_SUCCESS;
}


//*************************************************************************************************
//�������ݱ�

/*/
���ܣ��õ�ָ��ͨ�������б������ݣ��γɼ�¼��
������[IN]  int nDeviceNo, �豸��š�
������[IN]  int nChannelNo, ͨ����š�
������[IN]  BOOL bIsGetAll, �Ƿ�Ҫͨ����ȫ�����ݡ�TRUEΪҪȫ�����ݣ�FALSEΪָ��ʱ�䷶ΧҪ��
������[IN]  COleDateTime odtBeginTime, ʱ�䷶Χ��ʼʱ�䡣
������[IN]  COleDateTime odtEndTime, ʱ�䷶Χ����ʱ�䡣
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ExecGetAllAlarmData (int nDeviceNo, int nChannelNo, BOOL bIsGetAll/*=TRUE*/, 
										COleDateTime odtBeginTime/*=DT_RANGE_BEGIN*/, 
										COleDateTime odtEndTime/*=DT_RANGE_END*/)
{
	CString strSQL, strSP, strBeginTime, strEndTime;

	strBeginTime.Format(_T("%d-%d-%d %d:%d:%d"),
						odtBeginTime.GetYear(), odtBeginTime.GetMonth(), odtBeginTime.GetDay(),
						odtBeginTime.GetHour(), odtBeginTime.GetMinute(), odtBeginTime.GetSecond());
	strEndTime.Format(_T("%d-%d-%d %d:%d:%d"),
						odtEndTime.GetYear(), odtEndTime.GetMonth(), odtEndTime.GetDay(),
						odtEndTime.GetHour(), odtEndTime.GetMinute(), odtEndTime.GetSecond());

	strSP = _T("proc_ExecGetAlarmData");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d, %d, %d, '%s', '%s'"), 
					strSP, nDeviceNo, nChannelNo, bIsGetAll, strBeginTime, strEndTime);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ�ָ�����ͨ�������б������ݣ��γɼ�¼��
������[IN]  int nDeviceNoArr[], �豸������顣
������[IN]  int nChannelNoArr[], ͨ��������顣
������[IN]  BOOL bIsGetAll, �Ƿ�Ҫͨ����ȫ�����ݡ�TRUEΪҪȫ�����ݣ�FALSEΪָ��ʱ�䷶ΧҪ��
������[IN]  COleDateTime odtBeginTime, ʱ�䷶Χ��ʼʱ�䡣
������[IN]  COleDateTime odtEndTime, ʱ�䷶Χ����ʱ�䡣
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ExecGetAllAlarmDataBatch (int nDeviceNoArr[], int nChannelNoArr[], int nArrLen, BOOL bIsGetAll/*=TRUE*/, 
										COleDateTime odtBeginTime/*=DT_RANGE_BEGIN*/, 
										COleDateTime odtEndTime/*=DT_RANGE_END*/)
{
	if (nDeviceNoArr == NULL || nChannelNoArr == NULL || nArrLen <= 0)
		return RET_DB_PARAM_INVALID;

	CString strSQL, strSP, strBeginTime, strEndTime, strDevNoAndChNoSet, strID;

	strBeginTime.Format(_T("%d-%d-%d %d:%d:%d"),
						odtBeginTime.GetYear(), odtBeginTime.GetMonth(), odtBeginTime.GetDay(),
						odtBeginTime.GetHour(), odtBeginTime.GetMinute(), odtBeginTime.GetSecond());
	strEndTime.Format(_T("%d-%d-%d %d:%d:%d"),
						odtEndTime.GetYear(), odtEndTime.GetMonth(), odtEndTime.GetDay(),
						odtEndTime.GetHour(), odtEndTime.GetMinute(), odtEndTime.GetSecond());

	strSP = _T("proc_ExecGetAlarmDataBatch");
	this->AddOptDBName (strSP);

	//�γ�(�豸��, ͨ����)����
	strDevNoAndChNoSet.Format(_T("(%d, %d)"), nDeviceNoArr[0], nChannelNoArr[0]);
	for (int i=1; i<nArrLen; i++)
	{
		strID.Format(_T(", (%d, %d)"), nDeviceNoArr[i], nChannelNoArr[i]);
		strDevNoAndChNoSet += strID;
	}

	strSQL.Format(_T("EXEC %s '%s', %d, '%s', '%s'"), 
				strSP, strDevNoAndChNoSet, bIsGetAll, strBeginTime, strEndTime);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ���¼���е�ǰ����������Ϣ
������[OUT] int nAlarmDataID, ��������ID
������[OUT] int nDataID, ����ID
������[OUT] int nDeviceNo, �豸���
������[OUT] TCHAR szDeviceName[], �豸����
������[OUT] int nChannelNo, ͨ�����
������[OUT] TCHAR szChannelName[], ͨ������
������[OUT] COleDateTime odtSampleTime, �ɼ�ʱ��
������[OUT] float fMeasureValue, ����ֵ
������[OUT] byte byAlarmLevel, ��������
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetCurAlarmData (int &nAlarmDataID, int &nDataID, int &nDeviceNo, TCHAR szDeviceName[], int &nChannelNo, 
								TCHAR szChannelName[], COleDateTime &odtSampleTime, float &fMeasureValue, byte &byAlarmLevel)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	this->GetFeildValue (AlarmData_AlarmDataID, nDataID);
	this->GetFeildValue (AlarmData_DataID, nDataID);
	this->GetFeildValue (AlarmData_DeviceNo, nDeviceNo);
	this->GetFeildValue (AlarmData_ChannelNo, nChannelNo);
	this->GetFeildValue (AlarmData_SampleTime, odtSampleTime);
	this->GetFeildValue (AlarmData_MeasureValue, fMeasureValue);
	this->GetFeildValue (AlarmData_AlarmLevel, byAlarmLevel);

	this->GetFeildValue (Device_DeviceName, szDeviceName);
	this->GetFeildValue (Channel_ChannelName, szChannelName);

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ�ָ������������Ϣ
������[IN]  int nAlarmDataID, ��������ID
������[OUT] int nDataID, ����ID
������[OUT] int nDeviceNo, �豸���
������[OUT] int nChannelNo, ͨ�����
������[OUT] COleDateTime odtSampleTime, �ɼ�ʱ��
������[OUT] float fMeasureValue, ����ֵ
������[OUT] byte byAlarmLevel, ��������
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetOneAlarmDataInfo (int nAlarmDataID, int &nDataID, int &nDeviceNo, int &nChannelNo, 
									COleDateTime &odtSampleTime, float &fMeasureValue, byte &byAlarmLevel)
{
	CString strSQL, strSP;

	strSP = _T("proc_GetAlarmDataInfo");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d"), strSP, nAlarmDataID);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	if (this->GetRecordCount() <= 0)
		return RET_DB_RECORD_NULL;

	this->GetFeildValue (AlarmData_DataID, nDataID);
	this->GetFeildValue (AlarmData_DeviceNo, nDeviceNo);
	this->GetFeildValue (AlarmData_ChannelNo, nChannelNo);
	this->GetFeildValue (AlarmData_SampleTime, odtSampleTime);
	this->GetFeildValue (AlarmData_MeasureValue, fMeasureValue);
	this->GetFeildValue (AlarmData_AlarmLevel, byAlarmLevel);

	return RET_DB_SUCCESS;
}

/*/
���ܣ����һ����������
������[OUT] int nAlarmDataID, ��������ID
������[IN]  int nDataID, ����ID
������[IN]  int nDeviceNo, �豸���
������[IN]  int nChannelNo, ͨ�����
������[IN]  COleDateTime odtSampleTime, �ɼ�ʱ��
������[IN]  float fMeasureValue, ����ֵ
������[IN]  byte byAlarmLevel, ��������
������[IN]  int nDefBufLen, �Զ��建��������
������[IN]  BlobData<byte> *pbdDefBuf, �Զ��建����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::AddAlarmData (int &nAlarmDataID, int nDataID, int nDeviceNo, int nChannelNo, COleDateTime odtSampleTime, 
							float fMeasureValue, byte byAlarmLevel, int nDefBufLen/*=-1*/, BlobData<byte> *pbdDefBuf/*=NULL*/)
{
	int nRet = this->RecordOperate (DB_OPT_ADD, TABLE_ALARM_DATA);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (AlarmData_DataID, nDataID);
	this->SetFeildValue (AlarmData_DeviceNo, nDeviceNo);
	this->SetFeildValue (AlarmData_ChannelNo, nChannelNo);
	this->SetFeildValue (AlarmData_SampleTime, odtSampleTime);
	this->SetFeildValue (AlarmData_MeasureValue, fMeasureValue);
	this->SetFeildValue (AlarmData_AlarmLevel, byAlarmLevel);

	if (nDefBufLen >= 0)
	{
		this->SetFeildValue (field_DefBufLen, nDefBufLen);
		this->SetFeildValue (field_DefBuf, pbdDefBuf, nDefBufLen);
	}

	this->UpdateRecord ();

	this->GetFeildValue (AlarmData_AlarmDataID, nAlarmDataID);

	return RET_DB_SUCCESS;
}

/*/
���ܣ��޸�ָ����������
������[IN]  int nAlarmDataID, ��������ID
������[IN]  int nDataID, ����ID
������[IN]  int nDeviceNo, �豸���
������[IN]  int nChannelNo, ͨ�����
������[IN]  COleDateTime odtSampleTime, �ɼ�ʱ��
������[IN]  float fMeasureValue, ����ֵ
������[IN]  byte byAlarmLevel, ��������
������[IN]  int nDefBufLen, �Զ��建��������
������[IN]  BlobData<byte> *pbdDefBuf, �Զ��建����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ModiAlarmData (int nAlarmDataID, int nDataID, int nDeviceNo, int nChannelNo, COleDateTime odtSampleTime, 
								float fMeasureValue, byte byAlarmLevel, int nDefBufLen/*=-1*/, BlobData<byte> *pbdDefBuf/*=NULL*/)
{
	int nRet = this->RecordOperate (DB_OPT_MODIFY, TABLE_ALARM_DATA, nAlarmDataID);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (AlarmData_DataID, nDataID);
	this->SetFeildValue (AlarmData_DeviceNo, nDeviceNo);
	this->SetFeildValue (AlarmData_ChannelNo, nChannelNo);
	this->SetFeildValue (AlarmData_SampleTime, odtSampleTime);
	this->SetFeildValue (AlarmData_MeasureValue, fMeasureValue);
	this->SetFeildValue (AlarmData_AlarmLevel, byAlarmLevel);

	if (nDefBufLen >= 0)
	{
		this->SetFeildValue (field_DefBufLen, nDefBufLen);
		this->SetFeildValue (field_DefBuf, pbdDefBuf, nDefBufLen);
	}

	this->UpdateRecord ();

	return RET_DB_SUCCESS;
}

/*/
���ܣ�ɾ��ָ����������
������[IN]  int nAlarmDataID, ��������ID
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::DelAlarmData (int nAlarmDataID)
{
	int nRet = this->RecordOperate (DB_OPT_DELETE, TABLE_ALARM_DATA, nAlarmDataID);

	return nRet;
}

/*/
���ܣ�����ɾ��ָ���������ݱ������
������[IN]  int nDeviceNo, �豸���
������[IN]  int nChannelNo, ͨ�����
������[IN]  BOOL bIsDelAll, �Ƿ�ɾ��ͨ����ȫ�����ݡ�TRUEΪҪȫ�����ݣ�FALSEΪָ��ʱ�䷶ΧҪ��
������[IN]  COleDateTime odtBeginTime, ʱ�䷶Χ��ʼʱ�䡣
������[IN]  COleDateTime odtEndTime, ʱ�䷶Χ����ʱ�䡣
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::DelAllAlarmData (int nDeviceNo, int nChannelNo, BOOL bIsDelAll/*=TRUE*/, 
									COleDateTime odtBeginTime/*=DT_RANGE_BEGIN*/, 
									COleDateTime odtEndTime/*=DT_RANGE_END*/)
{
	CString strSQL, strSP, strBeginTime, strEndTime;

	strBeginTime.Format(_T("%d-%d-%d %d:%d:%d"),
						odtBeginTime.GetYear(), odtBeginTime.GetMonth(), odtBeginTime.GetDay(),
						odtBeginTime.GetHour(), odtBeginTime.GetMinute(), odtBeginTime.GetSecond());
	strEndTime.Format(_T("%d-%d-%d %d:%d:%d"),
						odtEndTime.GetYear(), odtEndTime.GetMonth(), odtEndTime.GetDay(),
						odtEndTime.GetHour(), odtEndTime.GetMinute(), odtEndTime.GetSecond());

	strSP = _T("proc_DeleteAllAlarmData");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d, %d, %d, '%s', '%s'"), 
					strSP, nDeviceNo, nChannelNo, bIsDelAll, strBeginTime, strEndTime);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	return RET_DB_SUCCESS;
}


//*************************************************************************************************
//��������

/*/
���ܣ��õ����п��������γɼ�¼��
������[IN]  int nDeviceNo, �豸��š�Ĭ��ֵΪ-1ʱҪ���п�����������Ҫָ���豸�Ŀ�������
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ExecGetAllController (int nDeviceNo/*=-1*/)
{
	CString strSQL, strSP;

	strSP = _T("proc_ExecGetAllController");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d"), strSP, nDeviceNo);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ���¼���е�ǰ��������Ϣ
������[OUT] int nDeviceNo, �豸���
������[OUT] int nControllerNo, ���������
������[OUT] TCHAR szControllerName[], ����������
������[OUT] byte byControllerType, ����������
������[OUT] int nDataLen, ���ݳ���
������[OUT] BlobData<byte> *pbdData, ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetCurController (int &nDeviceNo, int &nControllerNo, TCHAR szControllerName[], byte &byControllerType, 
									int &nDataLen, BlobData<byte> *pbdData)
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	if (szControllerName == NULL)
		return RET_DB_PARAM_INVALID;

	this->GetFeildValue (Controller_DeviceNo, nDeviceNo);
	this->GetFeildValue (Controller_ControllerNo, nControllerNo);
	this->GetFeildValue (Controller_ControllerName, szControllerName);
	this->GetFeildValue (Controller_ControllerType, byControllerType);
	this->GetFeildValue (Controller_DataLen, nDataLen);
	this->GetFeildValue (Controller_Data, pbdData);
	if (pbdData != NULL && nDataLen != pbdData->GetSize())
		nDataLen = pbdData->GetSize();

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ�ָ����������Ϣ
������[IN]  int nDeviceNo, �豸���
������[IN]  int nControllerNo, ���������
������[OUT] TCHAR szControllerName[], ����������
������[OUT] byte byControllerType, ����������
������[OUT] int nDataLen, ���ݳ���
������[OUT] BlobData<byte> *pbdData, ����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetOneControllerInfo (int nDeviceNo, int nControllerNo, TCHAR szControllerName[], byte &byControllerType, 
										int &nDataLen, BlobData<byte> *pbdData)
{
	if (szControllerName == NULL)
		return RET_DB_PARAM_INVALID;

	CString strSQL, strSP;

	strSP = _T("proc_GetControllerInfo");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d, %d"), strSP, nDeviceNo, nControllerNo);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	if (this->GetRecordCount() <= 0)
		return RET_DB_RECORD_NULL;

	this->GetFeildValue (Controller_ControllerName, szControllerName);
	this->GetFeildValue (Controller_ControllerType, byControllerType);
	this->GetFeildValue (Controller_DataLen, nDataLen);
	this->GetFeildValue (Controller_Data, pbdData);
	if (pbdData != NULL && nDataLen != pbdData->GetSize())
		nDataLen = pbdData->GetSize();

	return RET_DB_SUCCESS;
}

/*/
���ܣ����һ��������
������[IN]  int nDeviceNo, �豸���
������[IN]  int nControllerNo, ���������
������[IN]  TCHAR szControllerName[], ����������
������[IN]  byte byControllerType, ����������
������[IN]  int nDataLen, ���ݳ���
������[IN]  BlobData<byte> *pbdData, ����
������[IN]  int nDefBufLen, �Զ��建��������
������[IN]  BlobData<byte> *pbdDefBuf, �Զ��建����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::AddController (int nDeviceNo, int nControllerNo, TCHAR szControllerName[], byte byControllerType, 
								int nDataLen, BlobData<byte> *pbdData, int nDefBufLen/*=-1*/, 
								BlobData<byte> *pbdDefBuf/*=NULL*/)
{
	if (szControllerName == NULL)
		return RET_DB_PARAM_INVALID;

	if (nDataLen < 0 || (nDataLen > 0 && pbdData == NULL))
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperate (DB_OPT_ADD, TABLE_CONTROLLER);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (Controller_DeviceNo, nDeviceNo);
	this->SetFeildValue (Controller_ControllerNo, nControllerNo);
	this->SetFeildValue (Controller_ControllerName, szControllerName);
	this->SetFeildValue (Controller_ControllerType, byControllerType);

	this->SetFeildValue (Controller_DataLen, nDataLen);
	this->SetFeildValue (Controller_Data, pbdData, nDataLen);

	if (nDefBufLen >= 0)
	{
		this->SetFeildValue (field_DefBufLen, nDefBufLen);
		this->SetFeildValue (field_DefBuf, pbdDefBuf, nDefBufLen);
	}

	this->UpdateRecord ();

	return RET_DB_SUCCESS;
}

/*/
���ܣ��޸�ָ��������
������[IN]  int nDeviceNo, �豸���
������[IN]  int nControllerNo, ���������
������[IN]  TCHAR szControllerName[], ����������
������[IN]  byte byControllerType, ����������
������[IN]  int nDataLen, ���ݳ���
������[IN]  BlobData<byte> *pbdData, ����
������[IN]  int nDefBufLen, �Զ��建��������
������[IN]  BlobData<byte> *pbdDefBuf, �Զ��建����
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ModiController (int nDeviceNo, int nControllerNo, TCHAR szControllerName[], byte byControllerType, 
								int nDataLen, BlobData<byte> *pbdData, int nDefBufLen/*=-1*/, BlobData<byte> *pbdDefBuf/*=NULL*/)
{
	if (szControllerName == NULL)
		return RET_DB_PARAM_INVALID;

	if (nDataLen < 0 || (nDataLen > 0 && pbdData == NULL))
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperate (DB_OPT_MODIFY, TABLE_CONTROLLER, nDeviceNo, nControllerNo);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (Controller_ControllerName, szControllerName);
	this->SetFeildValue (Controller_ControllerType, byControllerType);

	this->SetFeildValue (Controller_DataLen, nDataLen);
	this->SetFeildValue (Controller_Data, pbdData, nDataLen);

	if (nDefBufLen >= 0)
	{
		this->SetFeildValue (field_DefBufLen, nDefBufLen);
		this->SetFeildValue (field_DefBuf, pbdDefBuf, nDefBufLen);
	}

	this->UpdateRecord ();

	return RET_DB_SUCCESS;
}

/*/
���ܣ�ɾ��ָ��������
������[IN]  int nDeviceNo, �豸���
������[IN]  int nControllerNo, ���������
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::DelController (int nDeviceNo, int nControllerNo)
{
	int nRet = this->RecordOperate (DB_OPT_DELETE, TABLE_CONTROLLER, nDeviceNo, nControllerNo);

	return nRet;
}

/*/
���ܣ�ɾ��ָ���豸�����п�����
������[IN]  int nDeviceNo, �豸���
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::DelAllController (int nDeviceNo)
{
	CString strSQL, strSP;

	strSP = _T("proc_DeleteAllController");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d"), strSP, nDeviceNo);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	return RET_DB_SUCCESS;
}

//*************************************************************************************************
//�ʲ���

/*/
���ܣ��õ������ʲ����γɼ�¼��
��������
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ExecGetAllAsset ()
{
	CString strSQL, strSP;

	strSP = _T("proc_ExecGetAllAsset");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s"), strSP);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ���¼���е�ǰ�ʲ���Ϣ
������[OUT] int nAssetID, �ʲ�ID
������[OUT] TCHAR szAssetNumber[], �ʲ����
������[OUT] TCHAR szProjectNumber[], ��Ŀ���
������[OUT] TCHAR szAssetName[], �ʲ�����
������[OUT] TCHAR szBrand[], Ʒ��
������[OUT] TCHAR szSpecification[], ����ͺ�
������[OUT] TCHAR szSerialNumber[], ���к�
������[OUT] TCHAR szUsage[], �ʲ���;
������[OUT] COleDateTime odtPurchaseDate, �ɹ�����
������[OUT] TCHAR szPurchasePrice[], �ɹ��۸�
������[OUT] TCHAR szRemarks[], ��ע
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetCurAsset (int &nAssetID, TCHAR szAssetNumber[], TCHAR szProjectNumber[], TCHAR szAssetName[], 
							TCHAR szBrand[], TCHAR szSpecification[], TCHAR szSerialNumber[], TCHAR szUsage[], 
							COleDateTime &odtPurchaseDate, TCHAR szPurchasePrice[], TCHAR szRemarks[])
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	if (szAssetNumber==NULL || szProjectNumber==NULL || szAssetName==NULL || szBrand==NULL || szSpecification==NULL 
		|| szSerialNumber==NULL || szUsage==NULL || szPurchasePrice==NULL || szRemarks==NULL)
		return RET_DB_PARAM_INVALID;

	this->GetFeildValue (Asset_AssetID, nAssetID);
	this->GetFeildValue (Asset_AssetNumber, szAssetNumber);
	this->GetFeildValue (Asset_ProjectNumber, szProjectNumber);
	this->GetFeildValue (Asset_AssetName, szAssetName);
	this->GetFeildValue (Asset_Brand, szBrand);
	this->GetFeildValue (Asset_Specification, szSpecification);
	this->GetFeildValue (Asset_SerialNumber, szSerialNumber);
	this->GetFeildValue (Asset_Usage, szUsage);
	this->GetFeildValue (Asset_PurchaseDate, odtPurchaseDate);
	this->GetFeildValue (Asset_PurchasePrice, szPurchasePrice);
	this->GetFeildValue (Asset_Remarks, szRemarks);

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ�ָ���ʲ���Ϣ
������[IN]  int nAssetID, �ʲ�ID
������[OUT] TCHAR szAssetNumber[], �ʲ����
������[OUT] TCHAR szProjectNumber[], ��Ŀ���
������[OUT] TCHAR szAssetName[], �ʲ�����
������[OUT] TCHAR szBrand[], Ʒ��
������[OUT] TCHAR szSpecification[], ����ͺ�
������[OUT] TCHAR szSerialNumber[], ���к�
������[OUT] TCHAR szUsage[], �ʲ���;
������[OUT] COleDateTime odtPurchaseDate, �ɹ�����
������[OUT] TCHAR szPurchasePrice[], �ɹ��۸�
������[OUT] TCHAR szRemarks[], ��ע
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetOneAssetInfo (int nAssetID, TCHAR szAssetNumber[], TCHAR szProjectNumber[], TCHAR szAssetName[], 
							TCHAR szBrand[], TCHAR szSpecification[], TCHAR szSerialNumber[], TCHAR szUsage[], 
							COleDateTime &odtPurchaseDate, TCHAR szPurchasePrice[], TCHAR szRemarks[])
{
	if (szAssetNumber==NULL || szProjectNumber==NULL || szAssetName==NULL || szBrand==NULL || szSpecification==NULL 
		|| szSerialNumber==NULL || szUsage==NULL || szPurchasePrice==NULL || szRemarks==NULL)
		return RET_DB_PARAM_INVALID;

	CString strSQL, strSP;

	strSP = _T("proc_GetAssetInfo");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d"), strSP, nAssetID);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	if (this->GetRecordCount() <= 0)
		return RET_DB_RECORD_NULL;

	this->GetFeildValue (Asset_AssetNumber, szAssetNumber);
	this->GetFeildValue (Asset_ProjectNumber, szProjectNumber);
	this->GetFeildValue (Asset_AssetName, szAssetName);
	this->GetFeildValue (Asset_Brand, szBrand);
	this->GetFeildValue (Asset_Specification, szSpecification);
	this->GetFeildValue (Asset_SerialNumber, szSerialNumber);
	this->GetFeildValue (Asset_Usage, szUsage);
	this->GetFeildValue (Asset_PurchaseDate, odtPurchaseDate);
	this->GetFeildValue (Asset_PurchasePrice, szPurchasePrice);
	this->GetFeildValue (Asset_Remarks, szRemarks);

	return RET_DB_SUCCESS;
}

/*/
���ܣ�����ʲ���Ϣ
������[OUT] int nAssetID, �ʲ�ID
������[IN]  TCHAR szAssetNumber[], �ʲ����
������[IN]  TCHAR szProjectNumber[], ��Ŀ���
������[IN]  TCHAR szAssetName[], �ʲ�����
������[IN]  TCHAR szBrand[], Ʒ��
������[IN]  TCHAR szSpecification[], ����ͺ�
������[IN]  TCHAR szSerialNumber[], ���к�
������[IN]  TCHAR szUsage[], �ʲ���;
������[IN]  COleDateTime odtPurchaseDate, �ɹ�����
������[IN]  TCHAR szPurchasePrice[], �ɹ��۸�
������[IN]  TCHAR szRemarks[], ��ע
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::AddAsset (int &nAssetID, TCHAR szAssetNumber[], TCHAR szProjectNumber[], TCHAR szAssetName[], 
						TCHAR szBrand[], TCHAR szSpecification[], TCHAR szSerialNumber[], TCHAR szUsage[], 
						COleDateTime odtPurchaseDate, TCHAR szPurchasePrice[], TCHAR szRemarks[], 
						int nDefBufLen/*=-1*/, BlobData<byte> *pbdDefBuf/*=NULL*/)
{
	if (szAssetNumber==NULL || szProjectNumber==NULL || szAssetName==NULL || szBrand==NULL || szSpecification==NULL 
		|| szSerialNumber==NULL || szUsage==NULL || szPurchasePrice==NULL || szRemarks==NULL)
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperate (DB_OPT_ADD, TABLE_ASSET);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (Asset_AssetNumber, szAssetNumber);
	this->SetFeildValue (Asset_ProjectNumber, szProjectNumber);
	this->SetFeildValue (Asset_AssetName, szAssetName);
	this->SetFeildValue (Asset_Brand, szBrand);
	this->SetFeildValue (Asset_Specification, szSpecification);
	this->SetFeildValue (Asset_SerialNumber, szSerialNumber);
	this->SetFeildValue (Asset_Usage, szUsage);
	this->SetFeildValue (Asset_PurchaseDate, odtPurchaseDate);
	this->SetFeildValue (Asset_PurchasePrice, szPurchasePrice);
	this->SetFeildValue (Asset_Remarks, szRemarks);

	if (nDefBufLen >= 0)
	{
		this->SetFeildValue (field_DefBufLen, nDefBufLen);
		this->SetFeildValue (field_DefBuf, pbdDefBuf, nDefBufLen);
	}

	this->UpdateRecord ();

	this->GetFeildValue (Asset_AssetID, nAssetID);

	return RET_DB_SUCCESS;
}

/*/
���ܣ��޸�ָ���ʲ���Ϣ
������[IN]  int nAssetID, �ʲ�ID
������[IN]  TCHAR szAssetNumber[], �ʲ����
������[IN]  TCHAR szProjectNumber[], ��Ŀ���
������[IN]  TCHAR szAssetName[], �ʲ�����
������[IN]  TCHAR szBrand[], Ʒ��
������[IN]  TCHAR szSpecification[], ����ͺ�
������[IN]  TCHAR szSerialNumber[], ���к�
������[IN]  TCHAR szUsage[], �ʲ���;
������[IN]  COleDateTime odtPurchaseDate, �ɹ�����
������[IN]  TCHAR szPurchasePrice[], �ɹ��۸�
������[IN]  TCHAR szRemarks[], ��ע
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ModiAsset (int nAssetID, TCHAR szAssetNumber[], TCHAR szProjectNumber[], TCHAR szAssetName[], 
						TCHAR szBrand[], TCHAR szSpecification[], TCHAR szSerialNumber[], TCHAR szUsage[], 
						COleDateTime odtPurchaseDate, TCHAR szPurchasePrice[], TCHAR szRemarks[], 
						int nDefBufLen/*=-1*/, BlobData<byte> *pbdDefBuf/*=NULL*/)
{
	if (szAssetNumber==NULL || szProjectNumber==NULL || szAssetName==NULL || szBrand==NULL || szSpecification==NULL 
		|| szSerialNumber==NULL || szUsage==NULL || szPurchasePrice==NULL || szRemarks==NULL)
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperate (DB_OPT_MODIFY, TABLE_ASSET, nAssetID);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (Asset_AssetNumber, szAssetNumber);
	this->SetFeildValue (Asset_ProjectNumber, szProjectNumber);
	this->SetFeildValue (Asset_AssetName, szAssetName);
	this->SetFeildValue (Asset_Brand, szBrand);
	this->SetFeildValue (Asset_Specification, szSpecification);
	this->SetFeildValue (Asset_SerialNumber, szSerialNumber);
	this->SetFeildValue (Asset_Usage, szUsage);
	this->SetFeildValue (Asset_PurchaseDate, odtPurchaseDate);
	this->SetFeildValue (Asset_PurchasePrice, szPurchasePrice);
	this->SetFeildValue (Asset_Remarks, szRemarks);

	if (nDefBufLen >= 0)
	{
		this->SetFeildValue (field_DefBufLen, nDefBufLen);
		this->SetFeildValue (field_DefBuf, pbdDefBuf, nDefBufLen);
	}

	this->UpdateRecord ();

	return RET_DB_SUCCESS;
}

/*/
���ܣ�ɾ��ָ���ʲ���Ϣ
������[IN]  int nAssetID, �ʲ�ID
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::DelAsset (int nAssetID)
{
	int nRet = this->RecordOperate (DB_OPT_DELETE, TABLE_ASSET, nAssetID);

	return nRet;
}


//*************************************************************************************************
//�ʲ�ά����

/*/
���ܣ��õ������ʲ�ά����¼���γɼ�¼��
������[IN]  int nAssetID, �ʲ�ID��Ϊ-1ʱҪȫ���ʲ�ά����¼������Ҫָ���ʲ���ά����¼
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ExecGetAllAssetMaintenance (int nAssetID/*=-1*/)
{
	CString strSQL, strSP;

	strSP = _T("proc_ExecGetAllAssetMaintenance");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d"), strSP, nAssetID);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ���¼���е��ʲ�ά����Ϣ
������[OUT] int nMaintenanceID, �ʲ�ά��ID
������[OUT] int nAssetID, �ʲ�ID
������[OUT] COleDateTime odtMaintenanceDate[], ά������
������[OUT] TCHAR szMaintenanceReason[], ά��ԭ��
������[OUT] TCHAR szMaintenanceContent[], ά������
������[OUT] TCHAR szMaintenancePersonnel[], ά����Ա
������[OUT] TCHAR szRemarks[], ��ע
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetCurAssetMaintenance (int &nMaintenanceID, int &nAssetID, COleDateTime &odtMaintenanceDate, 
								TCHAR szMaintenanceReason[], TCHAR szMaintenanceContent[], TCHAR szMaintenancePersonnel[],
								TCHAR szRemarks[])
{
	if (!this->IsCurRecordValid ())
		return RET_DB_RECORD_INVALID;

	if (szMaintenanceReason == NULL || szMaintenanceContent == NULL || szMaintenancePersonnel == NULL || szRemarks == NULL)
		return RET_DB_PARAM_INVALID;

	this->GetFeildValue (AssetMaintenance_MaintenanceID, nMaintenanceID);
	this->GetFeildValue (AssetMaintenance_AssetID, nAssetID);
	this->GetFeildValue (AssetMaintenance_MaintenanceDate, odtMaintenanceDate);
	this->GetFeildValue (AssetMaintenance_MaintenanceReason, szMaintenanceReason);
	this->GetFeildValue (AssetMaintenance_MaintenanceContent, szMaintenanceContent);
	this->GetFeildValue (AssetMaintenance_MaintenancePersonnel, szMaintenancePersonnel);
	this->GetFeildValue (AssetMaintenance_Remarks, szRemarks);

	return RET_DB_SUCCESS;
}

/*/
���ܣ��õ�ָ���ʲ�ά����Ϣ
������[IN]  int nMaintenanceID, �ʲ�ά��ID
������[OUT] int nAssetID, �ʲ�ID
������[OUT] COleDateTime odtMaintenanceDate[], ά������
������[OUT] TCHAR szMaintenanceReason[], ά��ԭ��
������[OUT] TCHAR szMaintenanceContent[], ά������
������[OUT] TCHAR szMaintenancePersonnel[], ά����Ա
������[OUT] TCHAR szRemarks[], ��ע
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::GetOneAssetMaintenanceInfo (int nMaintenanceID, int &nAssetID, COleDateTime &odtMaintenanceDate, 
								TCHAR szMaintenanceReason[], TCHAR szMaintenanceContent[], TCHAR szMaintenancePersonnel[], 
								TCHAR szRemarks[])
{
	if (szMaintenanceReason == NULL || szMaintenanceContent == NULL || szMaintenancePersonnel == NULL || szRemarks == NULL)
		return RET_DB_PARAM_INVALID;

	CString strSQL, strSP;

	strSP = _T("proc_GetAssetMaintenanceInfo");
	this->AddOptDBName (strSP);

	strSQL.Format(_T("EXEC %s %d"), strSP, nMaintenanceID);

	if (!this->RecordExecute(strSQL))
		return RET_DB_EXE_FAIL;

	if (this->GetRecordCount() <= 0)
		return RET_DB_RECORD_NULL;

	this->GetFeildValue (AssetMaintenance_AssetID, nAssetID);
	this->GetFeildValue (AssetMaintenance_MaintenanceDate, odtMaintenanceDate);
	this->GetFeildValue (AssetMaintenance_MaintenanceReason, szMaintenanceReason);
	this->GetFeildValue (AssetMaintenance_MaintenanceContent, szMaintenanceContent);
	this->GetFeildValue (AssetMaintenance_MaintenancePersonnel, szMaintenancePersonnel);
	this->GetFeildValue (AssetMaintenance_Remarks, szRemarks);

	return RET_DB_SUCCESS;
}

/*/
���ܣ�����ʲ�ά����Ϣ
������[OUT] int nMaintenanceID, �ʲ�ά��ID
������[IN]  int nAssetID, �ʲ�ID
������[IN]  COleDateTime odtMaintenanceDate[], ά������
������[IN]  TCHAR szMaintenanceReason[], ά��ԭ��
������[IN]  TCHAR szMaintenanceContent[], ά������
������[IN]  TCHAR szMaintenancePersonnel[], ά����Ա
������[IN]  TCHAR szRemarks[], ��ע
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::AddAssetMaintenance (int &nMaintenanceID, int nAssetID, COleDateTime odtMaintenanceDate, 
								TCHAR szMaintenanceReason[], TCHAR szMaintenanceContent[], TCHAR szMaintenancePersonnel[], 
								TCHAR szRemarks[], int nDefBufLen/*=-1*/, BlobData<byte> *pbdDefBuf/*=NULL*/)
{
	if (szMaintenanceReason == NULL || szMaintenanceContent == NULL || szMaintenancePersonnel == NULL || szRemarks == NULL)
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperate (DB_OPT_ADD, TABLE_ASSET_MAINTENANCE);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (AssetMaintenance_AssetID, nAssetID);
	this->SetFeildValue (AssetMaintenance_MaintenanceDate, odtMaintenanceDate);
	this->SetFeildValue (AssetMaintenance_MaintenanceReason, szMaintenanceReason);
	this->SetFeildValue (AssetMaintenance_MaintenanceContent, szMaintenanceContent);
	this->SetFeildValue (AssetMaintenance_MaintenancePersonnel, szMaintenancePersonnel);
	this->SetFeildValue (AssetMaintenance_Remarks, szRemarks);

	if (nDefBufLen >= 0)
	{
		this->SetFeildValue (field_DefBufLen, nDefBufLen);
		this->SetFeildValue (field_DefBuf, pbdDefBuf, nDefBufLen);
	}

	this->UpdateRecord ();

	this->GetFeildValue (AssetMaintenance_MaintenanceID, nMaintenanceID);

	return RET_DB_SUCCESS;
}

/*/
���ܣ��޸��ʲ�ά����Ϣ
������[IN]  int nMaintenanceID, �ʲ�ά��ID
������[IN]  int nAssetID, �ʲ�ID
������[IN]  COleDateTime odtMaintenanceDate[], ά������
������[IN]  TCHAR szMaintenanceReason[], ά��ԭ��
������[IN]  TCHAR szMaintenanceContent[], ά������
������[IN]  TCHAR szMaintenancePersonnel[], ά����Ա
������[IN]  TCHAR szRemarks[], ��ע
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::ModiAssetMaintenance (int nMaintenanceID, int nAssetID, COleDateTime odtMaintenanceDate, 
								TCHAR szMaintenanceReason[], TCHAR szMaintenanceContent[], TCHAR szMaintenancePersonnel[], 
								TCHAR szRemarks[], int nDefBufLen/*=-1*/, BlobData<byte> *pbdDefBuf/*=NULL*/)
{
	if (szMaintenanceReason == NULL || szMaintenanceContent == NULL || szMaintenancePersonnel == NULL || szRemarks == NULL)
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperate (DB_OPT_MODIFY, TABLE_ASSET_MAINTENANCE, nMaintenanceID);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	this->SetFeildValue (AssetMaintenance_AssetID, nAssetID);
	this->SetFeildValue (AssetMaintenance_MaintenanceDate, odtMaintenanceDate);
	this->SetFeildValue (AssetMaintenance_MaintenanceReason, szMaintenanceReason);
	this->SetFeildValue (AssetMaintenance_MaintenanceContent, szMaintenanceContent);
	this->SetFeildValue (AssetMaintenance_MaintenancePersonnel, szMaintenancePersonnel);
	this->SetFeildValue (AssetMaintenance_Remarks, szRemarks);

	if (nDefBufLen >= 0)
	{
		this->SetFeildValue (field_DefBufLen, nDefBufLen);
		this->SetFeildValue (field_DefBuf, pbdDefBuf, nDefBufLen);
	}

	this->UpdateRecord ();

	return RET_DB_SUCCESS;
}

/*/
���ܣ�ɾ���ʲ�ά����Ϣ
������[IN]  int nMaintenanceID, �ʲ�ά��ID
����ֵ��RET_DB_SUCCESS�ɹ�������ʧ�ܡ�
/*/
int CDBAccess::DelAssetMaintenance (int nMaintenanceID)
{
	int nRet = this->RecordOperate (DB_OPT_DELETE, TABLE_ASSET_MAINTENANCE, nMaintenanceID);

	return nRet;
}
