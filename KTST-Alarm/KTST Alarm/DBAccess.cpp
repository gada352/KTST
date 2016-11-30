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
功能：为指定表添加、修改、删除记录
参数：[IN]  int nTableNo, 表编号
参数：[OUT] CString strTable, 表名称
参数：[IN]  CString strKeyID, 记录的主键名称
参数：[IN]  CString strKeyExID, 如果表主键由两个字段构成，这是第二个字段名称
返回值：TRUE成功，FALSE失败。
/*/
BOOL CDBAccess::GetTableInfo (int nTableNo, CString &strTable, CString &strKeyID, CString &strKeyExID)
{
	CString strTableName, strKeyIDName, strKeyExIDName;

	strTableName = _T("");
	strKeyIDName = _T("");
	strKeyExIDName = _T("");

	switch (nTableNo)
	{
	case TABLE_DB_SYSTEM:				//系统表
		strTableName = table_DBSystem;
		strKeyIDName = DBSystem_RecordID;
		break;

	case TABLE_USER_INFO:				//用户信息表
		strTableName = table_UserInfo;
		strKeyIDName = UserInfo_UserID;
		break;

	case TABLE_PLOT:					//图表
		strTableName = table_Plot;
		strKeyIDName = Plot_PlotID;
		break;

	case TABLE_DEVICE:					//设备表
		strTableName = table_Device;
		strKeyIDName = Device_DeviceNo;
		break;

	case TABLE_CHANNEL:					//通道表
		strTableName = table_Channel;
		strKeyIDName = Channel_DeviceNo;
		strKeyExIDName = Channel_ChannelNo;
		break;

	case TABLE_CHANNEL_DATA:			//通道数据表
		strTableName = table_ChannelData;
		strKeyIDName = ChannelData_DataID;
		break;

	case TABLE_ALARM_DATA:				//报警数据表
		strTableName = table_AlarmData;
		strKeyIDName = AlarmData_AlarmDataID;
		break;

	case TABLE_CONTROLLER:				//控制器表
		strTableName = table_Controller;
		strKeyIDName = Controller_DeviceNo;
		strKeyExIDName = Controller_ControllerNo;
		break;

	case TABLE_ASSET:					//资产表
		strTableName = table_Asset;
		strKeyIDName = Asset_AssetID;
		break;

	case TABLE_ASSET_MAINTENANCE:		//资产维护表
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
功能：为指定表添加、修改、删除记录
参数：[IN]  int nDBOptFlag, 操作标志，添加、修改、删除
参数：[IN]  int nTableNo, 表编号
参数：[IN]  int nKeyID, 记录的主键
参数：[IN]  int nKeyExID, 如果表主键由两个字段构成，这是第二个字段
返回值：RET_DB_SUCCESS成功，其它失败。
/*/
int CDBAccess::RecordOperate (int nDBOptFlag, int nTableNo, int nKeyID/*=-1*/, int nKeyExID/*=-1*/)
{
	CString strSQL, strDBName, strTableName, strKeyIDName, strKeyExIDName;

	if (!this->GetTableInfo (nTableNo, strTableName, strKeyIDName, strKeyExIDName))
		return RET_DB_PARAM_INVALID;

	return this->RecordOperate(nDBOptFlag, strTableName, strKeyIDName, strKeyExIDName, nKeyID, nKeyExID);
}

/*/
功能：为指定表添加、修改、删除记录
参数：[IN]  int nDBOptFlag, 操作标志，添加、修改、删除
参数：[IN]  CString strTableName, 表名称
参数：[IN]  CString strKeyIDName, 表主键名称
参数：[IN]  CString strKeyExIDName, 如果表主键由两个字段构成，这是第二个字段名称
参数：[IN]  int nKeyID, 记录的主键
参数：[IN]  int nKeyExID, 如果表主键由两个字段构成，这是第二个字段
返回值：RET_DB_SUCCESS成功，其它失败。
/*/
int CDBAccess::RecordOperate (int nDBOptFlag, CString strTableName, CString strKeyIDName, CString strKeyExIDName, 
								int nKeyID/*=-1*/, int nKeyExID/*=-1*/)
{
	CString strSQL, strDBName;

	//如果数据库名称不空，则在表名前加上数据库名称
	this->AddOptDBName (strTableName);

	switch (nDBOptFlag)
	{
	case DB_OPT_ADD:		//添加		//添加记录时只需要出空记录集
		if (strKeyExIDName.IsEmpty())
			strSQL.Format(_T("SELECT * FROM %s WHERE %s = -1"), strTableName, strKeyIDName);
		else
			strSQL.Format(_T("SELECT * FROM %s WHERE %s = -1 AND %s = -1"), strTableName, strKeyIDName, strKeyExIDName);
		break;

	case DB_OPT_MODIFY:		//修改
		if (strKeyExIDName.IsEmpty())
			strSQL.Format(_T("SELECT * FROM %s WHERE %s = %d"), strTableName, strKeyIDName, nKeyID);
		else
			strSQL.Format(_T("SELECT * FROM %s WHERE %s = %d AND %s = %d"), strTableName, strKeyIDName, nKeyID, strKeyExIDName, nKeyExID);
		break;

	case DB_OPT_DELETE:		//删除
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
		if (this->GetRecordCount() > 0)				//添加时必须为空记录集
			return RET_DB_RECORD_NOT_NULL;

		if (!this->AddNewRecord ())
			return RET_DB_ADD_RECORD_FAIL;
		break;

	case DB_OPT_MODIFY:
		if (this->GetRecordCount() <= 0)			//修改时不能为空记录集
			return RET_DB_RECORD_NULL;
		break;
	}

	return RET_DB_SUCCESS;
}

/*/
功能：为指定表添加、修改、删除记录
参数：[IN]  int nDBOptFlag, 操作标志，添加、修改、删除
参数：[IN]  int nTableNo, 表编号
参数：[IN]  int *pnKeyIDArr, 记录的主键数组
参数：[IN]  int *pnKeyExIDArr, 如果表主键由两个字段构成，这是第二个字段数组
参数：[IN]  int nArrLen, 表主键数组长度
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：为指定表添加、修改、删除记录
参数：[IN]  int nDBOptFlag, 操作标志，添加、修改、删除
参数：[IN]  CString strTableName, 表名称
参数：[IN]  CString strKeyIDName, 表主键名称
参数：[IN]  CString strKeyExIDName, 如果表主键由两个字段构成，这是第二个字段名称
参数：[IN]  int *pnKeyIDArr, 记录的主键数组
参数：[IN]  int *pnKeyExIDArr, 如果表主键由两个字段构成，这是第二个字段数组
参数：[IN]  int nArrLen, 表主键数组长度
返回值：RET_DB_SUCCESS成功，其它失败。
/*/
int CDBAccess::RecordOperateBatch (int nDBOptFlag, CString strTableName, CString strKeyIDName, CString strKeyExIDName, 
									int *pnKeyIDArr/*=NULL*/, int *pnKeyExIDArr/*=NULL*/, int nArrLen/*=0*/)
{
	CString strSQL, strDBName, strKeyIDSet, strKeyExIDSet, strKeyComboSet1, strKeyComboSet2;

	//如果数据库名称不空，则在表名前加上数据库名称
	this->AddOptDBName (strTableName);


	//拼出ID集合(1, 2, 3, ...)
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
		//两种方法实现，方法一的效率可能高一些
		//************************************************************************

		//方法一：将键值对插入到一个临时表中
		if (pnKeyIDArr != NULL && pnKeyExIDArr != NULL)
		{
			strKeyComboSet1.Format(_T("(%d, %d)"), pnKeyIDArr[0], pnKeyExIDArr[0]);
			for (int i=1; i<nArrLen; i++)
			{
				strID.Format(_T(", (%d, %d)"), pnKeyIDArr[i], pnKeyExIDArr[i]);
				strKeyComboSet1 += strID;
			}
		}

		//方法二：将多个键值用特殊字符“&, @, |, -, #, %”连接成一个字段进行查询，此处用‘&’字符
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
	case DB_OPT_BATCH_ADD:			//批量添加		//添加记录时只需要出空记录集
		if (strKeyExIDName.IsEmpty())
			strSQL.Format(_T("SELECT * FROM %s WHERE %s = -1"), strTableName, strKeyIDName);
		else
			strSQL.Format(_T("SELECT * FROM %s WHERE %s = -1 AND %s = -1"), strTableName, strKeyIDName, strKeyExIDName);

		if (!this->RecordExecute(strSQL, TRUE))
			return RET_DB_EXE_FAIL;

		if (this->GetRecordCount() > 0)				//添加时必须为空记录集
			return RET_DB_RECORD_NOT_NULL;
		break;

	case DB_OPT_BATCH_MODIFY:		//批量修改
		if (strKeyExIDName.IsEmpty())
			strSQL.Format(_T("SELECT * FROM %s WHERE %s IN (%s)"), strTableName, strKeyIDName, strKeyIDSet);
		else
		{
			//方法一：
			strSQL.Format(_T("DECLARE @temp table(keyID int, keyExID int) \
							INSERT INTO @temp VALUES %s \
							SELECT t.* \
								FROM %s t INNER JOIN @temp tt \
								ON t.%s=tt.keyID \
									AND t.%s=tt.keyExID"), 
							strKeyComboSet1, strTableName, strKeyIDName, strKeyExIDName);

			//方法二：
// 			strSQL.Format(_T("SELECT * \
// 								FROM %s \
// 								WHERE CAST(%s AS nvarchar) + \'&\' + CAST(%s AS nvarchar) \
// 									IN (%s)"), 
// 							strTableName, strKeyIDName, strKeyExIDName, strKeyComboSet2);
		}

		if (!this->RecordExecute(strSQL, TRUE))
			return RET_DB_EXE_FAIL;

		if (this->GetRecordCount() <= 0)			//修改时不能为空记录集
			return RET_DB_RECORD_NULL;
		break;

	case DB_OPT_BATCH_DELETE:		//批量删除
		if (strKeyExIDName.IsEmpty())
			strSQL.Format(_T("DELETE %s WHERE %s IN (%s)"), strTableName, strKeyIDName, strKeyIDSet);
		else
		{
			//方法一：
			strSQL.Format(_T("DECLARE @temp table(keyID int, keyExID int) \
							 INSERT INTO @temp VALUES %s \
							 DELETE t \
								 FROM %s t INNER JOIN @temp tt \
								 ON t.%s=tt.keyID \
									 AND t.%s=tt.keyExID"), 
							 strKeyComboSet1, strTableName, strKeyIDName, strKeyExIDName);

			//方法二：
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

//设置不同类型字段的值
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

//得到不同类型字段的值
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

//要服务器日期时间
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
*****************		下面的函数都是涉及具体操作数据库表的		***************************
/*/

//*************************************************************************************************
//数据库表记录用户自定义字段的获取和修改

/*/
功能：得到记录集指针当前所指记录的自定义缓冲区字段
参数：[OUT] int nDefBufLen, 自定义缓冲区长度
参数：[OUT] BlobData<byte> *pbdDefBuf, 自定义缓冲区
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到对应表记录用户自定义字段的内容；适用于数据库中所有表的记录的用户自定义字段；
参数：[IN]  int nTableNo, 表的编号
参数：[IN]  int nKeyID, 记录的主键
参数：[IN]  int nKeyExID, 如果表主键由两个字段构成，这是第二个字段
参数：[OUT] int nDefBufLen, 自定义缓冲区长度
参数：[OUT] BlobData<byte> *pbdDefBuf, 自定义缓冲区
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：修改对应表记录用户自定义字段的内容；适用于数据库中所有表的记录的用户自定义字段；
参数：[IN]  int nTableNo, 表的编号
参数：[IN]  int nKeyID, 记录的主键
参数：[IN]  int nKeyExID, 如果表主键由两个字段构成，这是第二个字段
参数：[IN]  int nDefBufLen, 自定义缓冲区长度
参数：[IN]  BlobData<byte> *pbdDefBuf, 自定义缓冲区
返回值：RET_DB_SUCCESS成功，其它失败。
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
//系统表

/*/
功能：得到数据库版本
参数：[OUT] int nDBVersion, 数据库版本号
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：修改数据库版本
参数：[IN]  int nDBVersion, 数据库版本号
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到数据库类型
参数：[OUT] int nDBType, 数据库类型：0公有云数据库、1私有云数据库
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：修改数据库类型
参数：[IN]  int nDBType, 数据库类型：0公有云数据库、1私有云数据库
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到服务器IP地址和端口号
参数：[OUT] byte byIPAddress[], IP地址缓冲区
参数：[OUT] int nPortNo, 端口号
返回值：RET_DB_SUCCESS成功，其它失败。
/*/
int CDBAccess::GetServerIPandPort (byte byIPAddress[], int &nPortNo)
{
	int nDefBuffLen;
	BlobData<byte> bdDefBuf;
	int nDBRet = GetDefBufData(TABLE_DB_SYSTEM, SYSTEM_ID_SERVER_IP_PORT, 0, nDefBuffLen, &bdDefBuf);
	if (nDBRet != RET_DB_SUCCESS)
		return nDBRet;

	int nDataLen = 4*sizeof(byte) + sizeof(int);	//4个byte表示IP地址、1个int表示端口号
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
功能：修改服务器IP地址和端口号
参数：[IN]  byte byIPAddress[], IP地址缓冲区
参数：[IN]  int nPortNo, 端口号
返回值：RET_DB_SUCCESS成功，其它失败。
/*/
int CDBAccess::ModiServerIPandPort (byte byIPAddress[], int nPortNo)
{
	int nDataLen = 4*sizeof(byte) + sizeof(int);	//4个byte表示IP地址、1个int表示端口号

	int nDefBuffLen = 4*sizeof(byte) + sizeof(int);	//4个byte表示IP地址、1个int表示端口号
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
功能：得到项目数据
参数：[OUT] BlobData<byte> *pbdDefBuf, 项目数据
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：修改项目数据
参数：[IN]  BlobData<byte> *pbdDefBuf, 项目数据
返回值：RET_DB_SUCCESS成功，其它失败。
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
//用户信息表

/*/
功能：得到所有用户，形成记录集
参数：无
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到记录集中当前用户信息
参数：[OUT] int nUserID, 用户ID
参数：[OUT] TCHAR szUserName[], 用户名称
参数：[OUT] TCHAR szPassword[], 用户密码
参数：[OUT] int nUserRight, 用户权限
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到指定用户的信息
参数：[IN]  int nUserID, 用户ID
参数：[OUT] TCHAR szUserName[], 用户名称
参数：[OUT] TCHAR szPassword[], 用户密码
参数：[OUT] int nUserRight, 用户权限
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：添加一个用户
参数：[OUT] int nUserID, 用户ID
参数：[IN]  TCHAR szUserName[], 用户名称
参数：[IN]  TCHAR szPassword[], 用户密码
参数：[IN]  int nUserRight, 用户权限
参数：[IN]  int nDefBufLen, 自定义缓冲区长度
参数：[IN]  BlobData<byte> *pbdDefBuf, 自定义缓冲区
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：修改指定的用户信息
参数：[IN]  int nUserID, 用户ID
参数：[IN]  TCHAR szUserName[], 用户名称
参数：[IN]  TCHAR szPassword[], 用户密码
参数：[IN]  int nUserRight, 用户权限
参数：[IN]  int nDefBufLen, 自定义缓冲区长度
参数：[IN]  BlobData<byte> *pbdDefBuf, 自定义缓冲区
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：删除指定的用户
参数：[IN]  int nUserID, 用户ID
返回值：RET_DB_SUCCESS成功，其它失败。
/*/
int CDBAccess::DelUser (int nUserID)
{
	int nRet = this->RecordOperate (DB_OPT_DELETE, TABLE_USER_INFO, nUserID);

	return nRet;
}

/*/
功能：根据用户名和密码判断用户是否存在
参数：[IN]  TCHAR szUserName[], 用户名称
参数：[IN]  TCHAR szPassword[], 用户密码
参数：[OUT] BOOL bExist, 用户是否存在
参数：[OUT] int nUserID, 用户ID
参数：[OUT] int nUserRight, 用户权限
返回值：RET_DB_SUCCESS成功，其它失败。
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
//图表

/*/
功能：得到所有图，形成记录集
参数：无
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到记录集中当前图信息
参数：[OUT] int nPlotID, 图ID
参数：[OUT] TCHAR szPlotName[], 图名称
参数：[OUT] byte byPlotType, 图类型
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到记录集中当前图信息
参数：[OUT] int nPlotID, 图ID
参数：[OUT] TCHAR szPlotName[], 图名称
参数：[OUT] byte byPlotType, 图类型
参数：[OUT] int nDataLen, 图数据长度
参数：[OUT] BlobData<byte> *pbdData, 图数据
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到指定图的信息
参数：[IN]  int nPlotID, 图ID
参数：[OUT] TCHAR szPlotName[], 图名称
参数：[OUT] byte byPlotType, 图类型
参数：[OUT] int nDataLen, 图数据长度
参数：[OUT] BlobData<byte> *pbdData, 图数据
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：添加一个图
参数：[OUT] int nPlotID, 图ID
参数：[IN]  TCHAR szPlotName[], 图名称
参数：[IN]  byte byPlotType, 图类型
参数：[IN]  int nDataLen, 图数据长度
参数：[IN]  BlobData<byte> *pbdData, 图数据
参数：[IN]  int nDefBufLen, 自定义缓冲区长度
参数：[IN]  BlobData<byte> *pbdDefBuf, 自定义缓冲区
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：修改指定的图信息
参数：[IN]  int nPlotID, 图ID
参数：[IN]  TCHAR szPlotName[], 图名称
参数：[IN]  byte byPlotType, 图类型
参数：[IN]  int nDataLen, 图数据长度
参数：[IN]  BlobData<byte> *pbdData, 图数据
参数：[IN]  int nDefBufLen, 自定义缓冲区长度
参数：[IN]  BlobData<byte> *pbdDefBuf, 自定义缓冲区
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：修改指定的图名称
参数：[IN]  int nPlotID, 图ID
参数：[IN]  TCHAR szPlotName[], 图名称
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：删除指定的图
参数：[IN]  int nPlotID, 图ID
返回值：RET_DB_SUCCESS成功，其它失败。
/*/
int CDBAccess::DelPlot (int nPlotID)
{
	int nRet = this->RecordOperate (DB_OPT_DELETE, TABLE_PLOT, nPlotID);

	return nRet;
}

/*/
功能：批量删除图
参数：[IN]  int *pnPlotIDArr, 图ID数组
参数：[IN]  int nArrLen, 数组长度
返回值：RET_DBOPT_SUCCESS成功，其它失败。
/*/
int CDBAccess::DelPlotBatch (int *pnPlotIDArr, int nArrLen)
{
	if (pnPlotIDArr == NULL || nArrLen <= 0)
		return RET_DB_PARAM_INVALID;

	int nRet = this->RecordOperateBatch (DB_OPT_BATCH_DELETE, TABLE_PLOT, pnPlotIDArr, NULL, nArrLen);

	return nRet;
}


//*************************************************************************************************
//设备表

/*/
功能：得到所有设备，形成记录集
参数：[IN]  int nDeviceType, 设备类型，为-1时要全部设备，否则要指定类型的设备
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到记录集中当前设备信息
参数：[OUT] int nDeviceNo, 设备编号
参数：[OUT] TCHAR szDeviceName[], 设备名称
参数：[OUT] int nDeviceType, 设备类型
参数：[OUT] int nDriverType, 驱动类型
参数：[OUT] int nChannelCount, 通道个数
参数：[OUT] TCHAR szDeviceGUID[], 设备GUID
参数：[OUT] int nSampleInterval, 采集间隔
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到指定设备信息
参数：[IN]  int nDeviceNo, 设备编号
参数：[OUT] TCHAR szDeviceName[], 设备名称
参数：[OUT] int nDeviceType, 设备类型
参数：[OUT] int nDriverType, 驱动类型
参数：[OUT] int nChannelCount, 通道个数
参数：[OUT] TCHAR szDeviceGUID[], 设备GUID
参数：[OUT] int nSampleInterval, 采集间隔
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：添加一个设备
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  TCHAR szDeviceName[], 设备名称
参数：[IN]  int nDeviceType, 设备类型
参数：[IN]  int nDriverType, 驱动类型
参数：[IN]  int nChannelCount, 通道个数
参数：[IN]  TCHAR szDeviceGUID[], 设备GUID
参数：[IN]  int nSampleInterval, 采集间隔
参数：[IN]  int nDefBufLen, 自定义缓冲区长度
参数：[IN]  BlobData<byte> *pbdDefBuf, 自定义缓冲区
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：修改指定设备
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  TCHAR szDeviceName[], 设备名称
参数：[IN]  int nDeviceType, 设备类型
参数：[IN]  int nDriverType, 驱动类型
参数：[IN]  int nChannelCount, 通道个数
参数：[IN]  TCHAR szDeviceGUID[], 设备GUID
参数：[IN]  int nSampleInterval, 采集间隔
参数：[IN]  int nDefBufLen, 自定义缓冲区长度
参数：[IN]  BlobData<byte> *pbdDefBuf, 自定义缓冲区
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：删除指定设备
参数：[IN]  int nDeviceNo, 设备编号
返回值：RET_DB_SUCCESS成功，其它失败。
/*/
int CDBAccess::DelDevice (int nDeviceNo)
{
	int nRet = this->RecordOperate (DB_OPT_DELETE, TABLE_DEVICE, nDeviceNo);

	return nRet;
}


//*************************************************************************************************
//通道表

/*/
功能：得到所有通道，形成记录集
参数：[IN]  int nDeviceNo, 设备编号。默认值为-1时要所有通道，否则要指定设备的通道。
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到记录集中当前通道信息
参数：[OUT] int nDeviceNo, 设备编号
参数：[OUT] int nChannelNo, 通道编号
参数：[OUT] TCHAR szChannelName[], 通道名称
参数：[OUT] BOOL bWorkEnable, 工作允许
参数：[OUT] float fFullScaleHigh, 满量程高
参数：[OUT] float fFullScaleHigh, 满量程低
参数：[OUT] float fFullScaleHigh, 校验高
参数：[OUT] float fFullScaleHigh, 校验低
参数：[OUT] byte byDataType, 数据类型
参数：[OUT] byte byDecimalDigits, 测量值小数位数
参数：[OUT] TCHAR szUnitString[], 单位字符串
参数：[OUT] int nAlarmLevelDataLen, 报警级别数据长度
参数：[OUT] BlobData<byte> *pbdAlarmLevelData, 报警级别数据
参数：[OUT] int nAlarmRangeDataLen, 报警范围数据长度
参数：[OUT] BlobData<byte> *pbdAlarmRangeData, 报警范围数据
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到指定通道信息
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nChannelNo, 通道编号
参数：[OUT] TCHAR szChannelName[], 通道名称
参数：[OUT] BOOL bWorkEnable, 工作允许
参数：[OUT] float fFullScaleHigh, 满量程高
参数：[OUT] float fFullScaleHigh, 满量程低
参数：[OUT] float fFullScaleHigh, 校验高
参数：[OUT] float fFullScaleHigh, 校验低
参数：[OUT] byte byDataType, 数据类型
参数：[OUT] byte byDecimalDigits, 测量值小数位数
参数：[OUT] TCHAR szUnitString[], 单位字符串
参数：[OUT] int nAlarmLevelDataLen, 报警级别数据长度
参数：[OUT] BlobData<byte> *pbdAlarmLevelData, 报警级别数据
参数：[OUT] int nAlarmRangeDataLen, 报警范围数据长度
参数：[OUT] BlobData<byte> *pbdAlarmRangeData, 报警范围数据
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：添加一个通道
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nChannelNo, 通道编号
参数：[IN]  TCHAR szChannelName[], 通道名称
参数：[IN]  BOOL bWorkEnable, 工作允许
参数：[IN]  float fFullScaleHigh, 满量程高
参数：[IN]  float fFullScaleHigh, 满量程低
参数：[IN]  float fFullScaleHigh, 校验高
参数：[IN]  float fFullScaleHigh, 校验低
参数：[IN]  byte byDataType, 数据类型
参数：[IN]  byte byDecimalDigits, 测量值小数位数
参数：[IN]  TCHAR szUnitString[], 单位字符串
参数：[IN]  int nAlarmLevelDataLen, 报警级别数据长度
参数：[IN]  BlobData<byte> *pbdAlarmLevelData, 报警级别数据
参数：[IN]  int nAlarmRangeDataLen, 报警范围数据长度
参数：[IN]  BlobData<byte> *pbdAlarmRangeData, 报警范围数据
参数：[IN]  int nDefBufLen, 自定义缓冲区长度
参数：[IN]  BlobData<byte> *pbdDefBuf, 自定义缓冲区
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：修改指定通道
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nChannelNo, 通道编号
参数：[IN]  TCHAR szChannelName[], 通道名称
参数：[IN]  BOOL bWorkEnable, 工作允许
参数：[IN]  float fFullScaleHigh, 满量程高
参数：[IN]  float fFullScaleHigh, 满量程低
参数：[IN]  float fFullScaleHigh, 校验高
参数：[IN]  float fFullScaleHigh, 校验低
参数：[IN]  byte byDataType, 数据类型
参数：[IN]  byte byDecimalDigits, 测量值小数位数
参数：[IN]  TCHAR szUnitString[], 单位字符串
参数：[IN]  int nAlarmLevelDataLen, 报警级别数据长度
参数：[IN]  BlobData<byte> *pbdAlarmLevelData, 报警级别数据
参数：[IN]  int nAlarmRangeDataLen, 报警范围数据长度
参数：[IN]  BlobData<byte> *pbdAlarmRangeData, 报警范围数据
参数：[IN]  int nDefBufLen, 自定义缓冲区长度
参数：[IN]  BlobData<byte> *pbdDefBuf, 自定义缓冲区
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：删除指定通道
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nChannelNo, 通道编号
返回值：RET_DB_SUCCESS成功，其它失败。
/*/
int CDBAccess::DelChannel (int nDeviceNo, int nChannelNo)
{
	int nRet = this->RecordOperate (DB_OPT_DELETE, TABLE_CHANNEL, nDeviceNo, nChannelNo);

	return nRet;
}


//*************************************************************************************************
//通道数据表

/*/
功能：得到指定通道的所有通道数据，形成记录集
参数：[IN]  int nDeviceNo, 设备编号。
参数：[IN]  int nChannelNo, 通道编号。
参数：[IN]  BOOL bIsGetAll, 是否要通道的全部数据。TRUE为要全部数据，FALSE为指定时间范围要。
参数：[IN]  COleDateTime odtBeginTime, 时间范围起始时间。
参数：[IN]  COleDateTime odtEndTime, 时间范围结束时间。
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到指定设备的所有通道的最后一个数据，形成记录集
参数：[IN]  int nDeviceNo, 设备编号。
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到记录集中当前通道数据信息
参数：[OUT] int nDataID, 数据ID
参数：[OUT] int nDeviceNo, 设备编号
参数：[OUT] int nChannelNo, 通道编号
参数：[OUT] COleDateTime odtSampleTime, 采集时间
参数：[OUT] float fMeasureValue, 测量值
参数：[OUT] byte byAlarmLevel, 报警级别
参数：[OUT] int nDataLen, 数据长度
参数：[OUT] BlobData<byte> *pbdData, 数据
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到指定通道数据信息
参数：[IN]  int nDataID, 数据ID
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nChannelNo, 通道编号
参数：[OUT] COleDateTime odtSampleTime, 采集时间
参数：[OUT] float fMeasureValue, 测量值
参数：[OUT] byte byAlarmLevel, 报警级别
参数：[OUT] int nDataLen, 数据长度
参数：[OUT] BlobData<byte> *pbdData, 数据
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到指定通道数据信息
参数：[OUT] int nDataID, 数据ID
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nChannelNo, 通道编号
参数：[OUT] COleDateTime odtSampleTime, 采集时间
参数：[OUT] float fMeasureValue, 测量值
参数：[OUT] byte byAlarmLevel, 报警级别
参数：[OUT] int nDataLen, 数据长度
参数：[OUT] BlobData<byte> *pbdData, 数据
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：添加一个通道数据
参数：[OUT] int nDataID, 数据ID
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nChannelNo, 通道编号
参数：[IN]  COleDateTime odtSampleTime, 采集时间
参数：[IN]  float fMeasureValue, 测量值
参数：[IN]  byte byAlarmLevel, 报警级别
参数：[IN]  int nDataLen, 数据长度
参数：[IN]  BlobData<byte> *pbdData, 数据
返回值：RET_DB_SUCCESS成功，其它失败。
/*/
int CDBAccess::AddChannelData (int &nDataID, int nDeviceNo, int nChannelNo, COleDateTime odtSampleTime, float fMeasureValue, 
								byte byAlarmLevel, int nDataLen, BlobData<byte> *pbdData)
{
	if (nDataLen < 0 || (nDataLen > 0 && pbdData == NULL))
		return RET_DB_PARAM_INVALID;

	CString strTableName, strKeyIDName, strKeyExIDName;

	if (!this->GetTableInfo (TABLE_CHANNEL_DATA, strTableName, strKeyIDName, strKeyExIDName))
		return RET_DB_PARAM_INVALID;

	//通道数据表的表名称要动态的拼
	strTableName = this->GetChannelDataTableName (nDeviceNo, nChannelNo);

	int nRet = this->RecordOperate (DB_OPT_ADD, strTableName, strKeyIDName, strKeyExIDName);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	//注意：
	//向数据库服务器添加数据记录时，由服务器自动获得当前时间，这样保证所有DPU模块采集的数据都以服务器时间为准

	this->SetFeildValue (ChannelData_DeviceNo, nDeviceNo);
	this->SetFeildValue (ChannelData_ChannelNo, nChannelNo);
// 	this->SetFeildValue (ChannelData_SampleTime, odtSampleTime);
	this->SetFeildValue (ChannelData_MeasureValue, fMeasureValue);
	this->SetFeildValue (ChannelData_AlarmLevel, byAlarmLevel);

	this->SetFeildValue (ChannelData_DataLen, nDataLen);
	this->SetFeildValue (ChannelData_Data, pbdData, nDataLen);

	this->UpdateRecord ();

	this->GetFeildValue (ChannelData_DataID, nDataID);

/*	//由数据表的插入触发器来判断报警级别，如果报警则插入到报警列表中
	//报警数据要加到报警数据表中
	if (byAlarmLevel > 0)
	{
		//由于数据的采集统一由数据库服务器来打时间戳，在此打采集时间要出来
		COleDateTime odtSampleTimeTemp;
		this->GetFeildValue (ChannelData_SampleTime, odtSampleTimeTemp);

		int nAlarmDataID;
		this->AddAlarmData (nAlarmDataID, nDataID, nDeviceNo, nChannelNo, odtSampleTimeTemp, fMeasureValue, byAlarmLevel);
	}
*/
	return RET_DB_SUCCESS;
}

/*/
功能：修改指定通道数据
参数：[IN]  int nDataID, 数据ID
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nChannelNo, 通道编号
参数：[IN]  COleDateTime odtSampleTime, 采集时间
参数：[IN]  float fMeasureValue, 测量值
参数：[IN]  byte byAlarmLevel, 报警级别
参数：[IN]  int nDataLen, 数据长度
参数：[IN]  BlobData<byte> *pbdData, 数据
返回值：RET_DB_SUCCESS成功，其它失败。
/*/
int CDBAccess::ModiChannelData (int nDataID, int nDeviceNo, int nChannelNo, COleDateTime odtSampleTime, float fMeasureValue, 
								byte byAlarmLevel, int nDataLen, BlobData<byte> *pbdData)
{
	if (nDataLen < 0 || (nDataLen > 0 && pbdData == NULL))
		return RET_DB_PARAM_INVALID;

	CString strTableName, strKeyIDName, strKeyExIDName;

	if (!this->GetTableInfo (TABLE_CHANNEL_DATA, strTableName, strKeyIDName, strKeyExIDName))
		return RET_DB_PARAM_INVALID;

	//通道数据表的表名称要动态的拼
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
功能：删除指定通道数据
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nChannelNo, 通道编号
返回值：RET_DB_SUCCESS成功，其它失败。
/*/
int CDBAccess::DelChannelData (int nDataID, int nDeviceNo, int nChannelNo)
{
	CString strTableName, strKeyIDName, strKeyExIDName;

	if (!this->GetTableInfo (TABLE_CHANNEL_DATA, strTableName, strKeyIDName, strKeyExIDName))
		return RET_DB_PARAM_INVALID;

	//通道数据表的表名称要动态的拼
	strTableName = this->GetChannelDataTableName (nDeviceNo, nChannelNo);

	int nRet = this->RecordOperate (DB_OPT_DELETE, strTableName, strKeyIDName, strKeyExIDName, nDataID);

	return nRet;
}

/*/
功能：批量删除指定通道数据表的数据
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nChannelNo, 通道编号
参数：[IN]  BOOL bIsDelAll, 是否删除通道的全部数据。TRUE为要全部数据，FALSE为指定时间范围要。
参数：[IN]  COleDateTime odtBeginTime, 时间范围起始时间。
参数：[IN]  COleDateTime odtEndTime, 时间范围结束时间。
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：根据设备号、通道号得到通道数据表的表名称
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nChannelNo, 通道编号
返回值：表名称。
/*/
CString CDBAccess::GetChannelDataTableName (int nDeviceNo, int nChannelNo)
{
	CString strTableName;
	strTableName.Format(_T("%s_%d_%d"), table_ChannelData, nDeviceNo, nChannelNo);
	return strTableName;
}

/*/
功能：批量添加通道数据开始
参数：无
返回值：RET_DBOPT_SUCCESS成功，其它失败。
/*/
int CDBAccess::BeginAddChannelDataBatch ()
{
	int nRet = this->RecordOperateBatch (DB_OPT_BATCH_ADD, TABLE_CHANNEL_DATA);
	if (nRet != RET_DB_SUCCESS)
		return nRet;

	return RET_DB_SUCCESS;
}

/*/
功能：批量操作，添加通道数据
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nChannelNo, 通道编号
参数：[IN]  COleDateTime odtSampleTime, 采集时间
参数：[IN]  float fMeasureValue, 测量值
参数：[IN]  byte byAlarmLevel, 报警级别
参数：[IN]  int nDataLen, 数据长度
参数：[IN]  BlobData<byte> *pbdData, 数据
返回值：RET_DBOPT_SUCCESS成功，其它失败。
/*/
int CDBAccess::AddChannelDataBatch (int nDeviceNo, int nChannelNo, COleDateTime odtSampleTime, float fMeasureValue, byte byAlarmLevel, 
									int nDataLen, BlobData<byte> *pbdData)
{
	if (!this->AddNewRecord ())
		return RET_DB_ADD_RECORD_FAIL;

	//注意：
	//向数据库服务器添加数据记录时，由服务器自动获得当前时间，这样保证所有DPU模块采集的数据都以服务器时间为准

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
功能：批量添加通道数据结束
参数：[INOUT]  int *pnDataIDArr, 数据ID数组
参数：[INOUT]  int &nArrLen, 数组长度
返回值：RET_DBOPT_SUCCESS成功，其它失败。
/*/
int CDBAccess::EndAddChannelDataBatch (int *pnDataIDArr, int &nArrLen)
{
	if (!this->UpdateRecord (TRUE))
		return RET_DB_EXE_FAIL;

	if (pnDataIDArr != NULL && nArrLen > 0)
	{
		//循环要回ID
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

		nArrLen = nCount;	//保存记录个数
	}

	return RET_DB_SUCCESS;
}


//*************************************************************************************************
//报警数据表

/*/
功能：得到指定通道的所有报警数据，形成记录集
参数：[IN]  int nDeviceNo, 设备编号。
参数：[IN]  int nChannelNo, 通道编号。
参数：[IN]  BOOL bIsGetAll, 是否要通道的全部数据。TRUE为要全部数据，FALSE为指定时间范围要。
参数：[IN]  COleDateTime odtBeginTime, 时间范围起始时间。
参数：[IN]  COleDateTime odtEndTime, 时间范围结束时间。
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到指定多个通道的所有报警数据，形成记录集
参数：[IN]  int nDeviceNoArr[], 设备编号数组。
参数：[IN]  int nChannelNoArr[], 通道编号数组。
参数：[IN]  BOOL bIsGetAll, 是否要通道的全部数据。TRUE为要全部数据，FALSE为指定时间范围要。
参数：[IN]  COleDateTime odtBeginTime, 时间范围起始时间。
参数：[IN]  COleDateTime odtEndTime, 时间范围结束时间。
返回值：RET_DB_SUCCESS成功，其它失败。
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

	//形成(设备号, 通道号)集合
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
功能：得到记录集中当前报警数据信息
参数：[OUT] int nAlarmDataID, 报警数据ID
参数：[OUT] int nDataID, 数据ID
参数：[OUT] int nDeviceNo, 设备编号
参数：[OUT] TCHAR szDeviceName[], 设备名称
参数：[OUT] int nChannelNo, 通道编号
参数：[OUT] TCHAR szChannelName[], 通道名称
参数：[OUT] COleDateTime odtSampleTime, 采集时间
参数：[OUT] float fMeasureValue, 测量值
参数：[OUT] byte byAlarmLevel, 报警级别
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到指定报警数据信息
参数：[IN]  int nAlarmDataID, 报警数据ID
参数：[OUT] int nDataID, 数据ID
参数：[OUT] int nDeviceNo, 设备编号
参数：[OUT] int nChannelNo, 通道编号
参数：[OUT] COleDateTime odtSampleTime, 采集时间
参数：[OUT] float fMeasureValue, 测量值
参数：[OUT] byte byAlarmLevel, 报警级别
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：添加一个报警数据
参数：[OUT] int nAlarmDataID, 报警数据ID
参数：[IN]  int nDataID, 数据ID
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nChannelNo, 通道编号
参数：[IN]  COleDateTime odtSampleTime, 采集时间
参数：[IN]  float fMeasureValue, 测量值
参数：[IN]  byte byAlarmLevel, 报警级别
参数：[IN]  int nDefBufLen, 自定义缓冲区长度
参数：[IN]  BlobData<byte> *pbdDefBuf, 自定义缓冲区
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：修改指定报警数据
参数：[IN]  int nAlarmDataID, 报警数据ID
参数：[IN]  int nDataID, 数据ID
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nChannelNo, 通道编号
参数：[IN]  COleDateTime odtSampleTime, 采集时间
参数：[IN]  float fMeasureValue, 测量值
参数：[IN]  byte byAlarmLevel, 报警级别
参数：[IN]  int nDefBufLen, 自定义缓冲区长度
参数：[IN]  BlobData<byte> *pbdDefBuf, 自定义缓冲区
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：删除指定报警数据
参数：[IN]  int nAlarmDataID, 报警数据ID
返回值：RET_DB_SUCCESS成功，其它失败。
/*/
int CDBAccess::DelAlarmData (int nAlarmDataID)
{
	int nRet = this->RecordOperate (DB_OPT_DELETE, TABLE_ALARM_DATA, nAlarmDataID);

	return nRet;
}

/*/
功能：批量删除指定报警数据表的数据
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nChannelNo, 通道编号
参数：[IN]  BOOL bIsDelAll, 是否删除通道的全部数据。TRUE为要全部数据，FALSE为指定时间范围要。
参数：[IN]  COleDateTime odtBeginTime, 时间范围起始时间。
参数：[IN]  COleDateTime odtEndTime, 时间范围结束时间。
返回值：RET_DB_SUCCESS成功，其它失败。
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
//控制器表

/*/
功能：得到所有控制器，形成记录集
参数：[IN]  int nDeviceNo, 设备编号。默认值为-1时要所有控制器，否则要指定设备的控制器。
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到记录集中当前控制器信息
参数：[OUT] int nDeviceNo, 设备编号
参数：[OUT] int nControllerNo, 控制器编号
参数：[OUT] TCHAR szControllerName[], 控制器名称
参数：[OUT] byte byControllerType, 控制器类型
参数：[OUT] int nDataLen, 数据长度
参数：[OUT] BlobData<byte> *pbdData, 数据
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到指定控制器信息
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nControllerNo, 控制器编号
参数：[OUT] TCHAR szControllerName[], 控制器名称
参数：[OUT] byte byControllerType, 控制器类型
参数：[OUT] int nDataLen, 数据长度
参数：[OUT] BlobData<byte> *pbdData, 数据
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：添加一个控制器
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nControllerNo, 控制器编号
参数：[IN]  TCHAR szControllerName[], 控制器名称
参数：[IN]  byte byControllerType, 控制器类型
参数：[IN]  int nDataLen, 数据长度
参数：[IN]  BlobData<byte> *pbdData, 数据
参数：[IN]  int nDefBufLen, 自定义缓冲区长度
参数：[IN]  BlobData<byte> *pbdDefBuf, 自定义缓冲区
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：修改指定控制器
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nControllerNo, 控制器编号
参数：[IN]  TCHAR szControllerName[], 控制器名称
参数：[IN]  byte byControllerType, 控制器类型
参数：[IN]  int nDataLen, 数据长度
参数：[IN]  BlobData<byte> *pbdData, 数据
参数：[IN]  int nDefBufLen, 自定义缓冲区长度
参数：[IN]  BlobData<byte> *pbdDefBuf, 自定义缓冲区
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：删除指定控制器
参数：[IN]  int nDeviceNo, 设备编号
参数：[IN]  int nControllerNo, 控制器编号
返回值：RET_DB_SUCCESS成功，其它失败。
/*/
int CDBAccess::DelController (int nDeviceNo, int nControllerNo)
{
	int nRet = this->RecordOperate (DB_OPT_DELETE, TABLE_CONTROLLER, nDeviceNo, nControllerNo);

	return nRet;
}

/*/
功能：删除指定设备的所有控制器
参数：[IN]  int nDeviceNo, 设备编号
返回值：RET_DB_SUCCESS成功，其它失败。
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
//资产表

/*/
功能：得到所有资产，形成记录集
参数：无
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到记录集中当前资产信息
参数：[OUT] int nAssetID, 资产ID
参数：[OUT] TCHAR szAssetNumber[], 资产编号
参数：[OUT] TCHAR szProjectNumber[], 项目编号
参数：[OUT] TCHAR szAssetName[], 资产名称
参数：[OUT] TCHAR szBrand[], 品牌
参数：[OUT] TCHAR szSpecification[], 规格型号
参数：[OUT] TCHAR szSerialNumber[], 序列号
参数：[OUT] TCHAR szUsage[], 资产用途
参数：[OUT] COleDateTime odtPurchaseDate, 采购日期
参数：[OUT] TCHAR szPurchasePrice[], 采购价格
参数：[OUT] TCHAR szRemarks[], 备注
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到指定资产信息
参数：[IN]  int nAssetID, 资产ID
参数：[OUT] TCHAR szAssetNumber[], 资产编号
参数：[OUT] TCHAR szProjectNumber[], 项目编号
参数：[OUT] TCHAR szAssetName[], 资产名称
参数：[OUT] TCHAR szBrand[], 品牌
参数：[OUT] TCHAR szSpecification[], 规格型号
参数：[OUT] TCHAR szSerialNumber[], 序列号
参数：[OUT] TCHAR szUsage[], 资产用途
参数：[OUT] COleDateTime odtPurchaseDate, 采购日期
参数：[OUT] TCHAR szPurchasePrice[], 采购价格
参数：[OUT] TCHAR szRemarks[], 备注
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：添加资产信息
参数：[OUT] int nAssetID, 资产ID
参数：[IN]  TCHAR szAssetNumber[], 资产编号
参数：[IN]  TCHAR szProjectNumber[], 项目编号
参数：[IN]  TCHAR szAssetName[], 资产名称
参数：[IN]  TCHAR szBrand[], 品牌
参数：[IN]  TCHAR szSpecification[], 规格型号
参数：[IN]  TCHAR szSerialNumber[], 序列号
参数：[IN]  TCHAR szUsage[], 资产用途
参数：[IN]  COleDateTime odtPurchaseDate, 采购日期
参数：[IN]  TCHAR szPurchasePrice[], 采购价格
参数：[IN]  TCHAR szRemarks[], 备注
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：修改指定资产信息
参数：[IN]  int nAssetID, 资产ID
参数：[IN]  TCHAR szAssetNumber[], 资产编号
参数：[IN]  TCHAR szProjectNumber[], 项目编号
参数：[IN]  TCHAR szAssetName[], 资产名称
参数：[IN]  TCHAR szBrand[], 品牌
参数：[IN]  TCHAR szSpecification[], 规格型号
参数：[IN]  TCHAR szSerialNumber[], 序列号
参数：[IN]  TCHAR szUsage[], 资产用途
参数：[IN]  COleDateTime odtPurchaseDate, 采购日期
参数：[IN]  TCHAR szPurchasePrice[], 采购价格
参数：[IN]  TCHAR szRemarks[], 备注
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：删除指定资产信息
参数：[IN]  int nAssetID, 资产ID
返回值：RET_DB_SUCCESS成功，其它失败。
/*/
int CDBAccess::DelAsset (int nAssetID)
{
	int nRet = this->RecordOperate (DB_OPT_DELETE, TABLE_ASSET, nAssetID);

	return nRet;
}


//*************************************************************************************************
//资产维护表

/*/
功能：得到所有资产维护记录，形成记录集
参数：[IN]  int nAssetID, 资产ID，为-1时要全部资产维护记录，否则要指定资产的维护记录
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到记录集中当资产维护信息
参数：[OUT] int nMaintenanceID, 资产维护ID
参数：[OUT] int nAssetID, 资产ID
参数：[OUT] COleDateTime odtMaintenanceDate[], 维护日期
参数：[OUT] TCHAR szMaintenanceReason[], 维护原因
参数：[OUT] TCHAR szMaintenanceContent[], 维护内容
参数：[OUT] TCHAR szMaintenancePersonnel[], 维护人员
参数：[OUT] TCHAR szRemarks[], 备注
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：得到指定资产维护信息
参数：[IN]  int nMaintenanceID, 资产维护ID
参数：[OUT] int nAssetID, 资产ID
参数：[OUT] COleDateTime odtMaintenanceDate[], 维护日期
参数：[OUT] TCHAR szMaintenanceReason[], 维护原因
参数：[OUT] TCHAR szMaintenanceContent[], 维护内容
参数：[OUT] TCHAR szMaintenancePersonnel[], 维护人员
参数：[OUT] TCHAR szRemarks[], 备注
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：添加资产维护信息
参数：[OUT] int nMaintenanceID, 资产维护ID
参数：[IN]  int nAssetID, 资产ID
参数：[IN]  COleDateTime odtMaintenanceDate[], 维护日期
参数：[IN]  TCHAR szMaintenanceReason[], 维护原因
参数：[IN]  TCHAR szMaintenanceContent[], 维护内容
参数：[IN]  TCHAR szMaintenancePersonnel[], 维护人员
参数：[IN]  TCHAR szRemarks[], 备注
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：修改资产维护信息
参数：[IN]  int nMaintenanceID, 资产维护ID
参数：[IN]  int nAssetID, 资产ID
参数：[IN]  COleDateTime odtMaintenanceDate[], 维护日期
参数：[IN]  TCHAR szMaintenanceReason[], 维护原因
参数：[IN]  TCHAR szMaintenanceContent[], 维护内容
参数：[IN]  TCHAR szMaintenancePersonnel[], 维护人员
参数：[IN]  TCHAR szRemarks[], 备注
返回值：RET_DB_SUCCESS成功，其它失败。
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
功能：删除资产维护信息
参数：[IN]  int nMaintenanceID, 资产维护ID
返回值：RET_DB_SUCCESS成功，其它失败。
/*/
int CDBAccess::DelAssetMaintenance (int nMaintenanceID)
{
	int nRet = this->RecordOperate (DB_OPT_DELETE, TABLE_ASSET_MAINTENANCE, nMaintenanceID);

	return nRet;
}
