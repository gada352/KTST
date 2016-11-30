#pragma once

#pragma warning(disable:4146)
#import "..\Lib\msado15.dll" no_namespace \
	rename("EOF","EndOfFile") \
	rename("LockTypeEnum","newLockTypeEnum")\
	rename("DataTypeEnum","newDataTypeEnum")\
	rename("FieldAttributeEnum","newFieldAttributeEnum")\
	rename("EditModeEnum","newEditModeEnum")\
	rename("RecordStatusEnum","newRecordStatusEnum")\
	rename("ParameterDirectionEnum","newParameterDirectionEnum")

#define CHUNKSIZE					8192
#define ADO_CONNECTION_TIMEOUT		10		//_ConnectionPtr���ӳ�ʱ
#define ADO_COMMAND_TIMEOUT			0		//_ConnectionPtrִ�г�ʱ��0Ϊ�ȴ�ִ����ɻ򷵻ش���

#define DEF_SERVER					_T("localhost")
#define DEF_PORT					_T("1433")
#define DEF_SQLSVR_USER				_T("sa")
#define DEF_SQLSVR_PWD				_T("")
#define DB_MASTER					_T("master")

///////////////////////////////////////////////////////////////////////////////////////////////////
//		CADOConnection

class CADOConnection
{
	friend class CADORecordset;

public:
	CADOConnection(void);
	~CADOConnection(void);

public:
	BOOL Connect (CString strServerName, CString strPort, CString strDBName, CString strUserID, CString strPassword);
	BOOL Disconnect ();
	BOOL IsConnectSuccess ();

	BOOL ConnectExecute (LPCTSTR lpszSQL, long lOptions=adCmdText);

private:
	BOOL ConnectAgain ();
	BOOL Release ();

protected:
	_ConnectionPtr	m_pConnection;

	CString m_strConnection;		//�����ַ���
	CString m_strServerName;		//����������
	CString	m_strPort;				//�������˿ں�
	CString m_strDBName;			//���ݿ�����
	CString m_strUserID;			//���ݿ��û���
	CString m_strPassword;			//���ݿ�����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//		CADORecordset

class CADORecordset
{
public:
	CADORecordset(void);
	CADORecordset(CADOConnection *pADOConnection);
	~CADORecordset(void);

public:
	void SetConnection (CADOConnection *pADOConnection);
	CADOConnection *GetConnection ();

public:
	BOOL RecordExecute (LPCTSTR lpszSQL, BOOL bBatch=FALSE);
	int GetRecordCount ();
	BOOL AddNewRecord ();
	BOOL UpdateRecord (BOOL bBatch=FALSE);
	BOOL UpdateBinaryFldVal (LPCTSTR lpszFldName, byte *pbyBuffer, int DataLen);

	BOOL MoveFirst ();
	BOOL MoveNext ();
	BOOL IsReachEnd ();
	BOOL IsEmpty ();
	BOOL IsCurRecordValid ();
	BOOL IsOpen ();
	BOOL Close ();

	void SetRecordsetUseFlag (BOOL bUsing);

private:
	BOOL Release ();

protected:
	_RecordsetPtr	m_pRecordset;			//ADO��¼��ָ��

	CADOConnection	*m_pADOConnection;		//���������ָ��
	BOOL			m_bRecordsetUsing;		//��ʶ��¼��ָ���Ƿ�����ʹ��
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//		CADOError

class CADOError
{
public:
	CADOError(void);
	~CADOError(void);

	static void OutputErrorString (const _com_error &e, CString strClassName=_T(""), CString strFuctionName=_T(""), CString strSQL=_T(""));
};
