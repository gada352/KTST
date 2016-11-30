

//************************************************************************************
//		���ú궨��
//************************************************************************************
#define SOFTWARE_TYPE_ID			4							//�������KTSTΪ4
#define SOFTWARE_NAME				_T("KTST-Alarm")			//�������
#define SOFTWARE_REG_NAME			_T("\\KTST-Alarm.reg")		//ע������ļ�����
#define LOCAL_SERVER_NAME			_T("localhost")				//���ط���������
#define DB_NAME						_T("KTSTDB")				//���ݿ�����
#define	DB_NAME_PREFIX				_T("KTSTDB_")				//ϵͳ���ݿ���ǰ׺
#define	HELP_FILE_NAME				_T("KTST-Alarm.chm")		//�����ĵ���ȫ��
#define	CFG_FILE_NAME				_T("KTST.xml")				//�����װ�����ݿ��������Ϣ�ļ��������û�������ȣ�

#define AUTO_RUN_CMD_LINE			_T("-AUTORUN")				//�����Զ����������в���
#define AUTO_RUN_SLEEP_TIME			90000						//�����Զ�������ʱʱ��(����)

#define NORMAL_COLOR				RGB(100, 220, 10)			//ͨ������״̬(�Ǳ���)����ɫ

//************************************************************************************
//		�û�Ȩ�޺��û���
//************************************************************************************
#define USER_NAME_ADMIN				_T("administrator")			//����Ա�û���
#define USERRIGHT_NORMAL			1							//��ͨ�û�
#define USERRIGHT_ADMIN				0							//administrator�û�

//************************************************************************************
//		����������
//************************************************************************************
#define BUF_LEN_S					32		//����������:С��
#define BUF_LEN_M					128		//����������:����
#define BUF_LEN_L					1024	//����������:����

//************************************************************************************
//		ͨ��״̬
//************************************************************************************
enum ChannelState
{
	csNormal = 0,			//����
	csNotWorking = 1,		//������
	csFault = 2,			//����
};

//************************************************************************************
//		�豸����(��λ����)
//************************************************************************************
enum DeviceType
{
	devTypeProgrammableController = 0x01,	//�ɱ�̿������豸
	devTypeDriver = 0x02,					//�����豸
	devTypeProcessSample = 0x04,			//�������ɼ��豸
	devTypeVideoSample = 0x08,				//ͼ��ɼ��豸
	devTypeRelay = 0x10,					//�̵����豸
};

//************************************************************************************
//		��������
//************************************************************************************
enum DriverType
{
	dvrTypeModbusRTU = 0,					//Modbus RTU
	dvrTypeModbusTCP = 1,					//Modbus TCP
	dvrTypeController = 2,					//Controller
	dvrTypeSMSDevice = 3,					//SMS Device
	dvrTypeCameraDevice = 4,				//Camera Device
};

//************************************************************************************
//		ͨ����������
//************************************************************************************
enum ChannelDataType
{
	datTypeProcess = 0,						//������
	datTypeSwitch = 1,						//������
	datTypeCamera = 2,						//ͼ��ͼ��
};

//************************************************************************************
//		�����������	MaxLimit
//************************************************************************************
#define _MAX_DEVICE					65535		//����豸��
#define _MAX_ONEDEVICE_CHL			32			//һ���豸����ͨ����
#define _MAX_CHANNLE				(_MAX_DEVICE*_MAX_ONEDEVICE_CHL)	//����ͨ����
#define _MAX_DEVICE_IN_LIST			_MAX_DEVICE	//���豸����Ի���������ӵ�����豸��
#define _MAX_DEVICE_TYPE			128			//����豸���͸���
#define _MAX_RELAY_EXPRESSION_LEN	300			//���̵������ʽ����
#define DEF_CH_CNT_PROCESS			4			//�������ɼ��豸Ĭ��ͨ����
#define DEF_CH_CNT_RELAY			8			//�̵����豸Ĭ��ͨ����

#define MIN_DEV_SAMPLE_INTERVAL		5			//�豸�ɼ���Сʱ����(��)
#define MAX_DEV_SAMPLE_INTERVAL		86400		//�豸�ɼ����ʱ����(��)
#define DEF_DEV_SAMPLE_INTERVAL		60			//�豸�ɼ�Ĭ��ʱ����(��)

#define MAX_ALARM_LEVEL_CNT			5			//��󱨾��������
#define MAX_ALARM_RANGE_CNT			10			//��󱨾���Χ����
#define MAX_ALARM_RANGE_ONE_LEVEL	2			//һ�������������ӵ���󱨾���Χ����

//*************************************************************************************
//		�Զ�����Ϣ		UserMessage
//*************************************************************************************
#define WM_UDP_RECEIVE				WM_USER + 2001  //UDP���յ��������ܷ��͵���Ϣ��
#define WM_TRAY_NOTIFY_ICON			WM_USER + 2002	//��������ͼ����Ϣ

//************************************************************************************
//		����Ĺ��������־
//************************************************************************************
#define	OPT_NULL					0			//�޲���
#define	OPT_ADD						1			//���
#define	OPT_MODIFY					2			//�޸�
#define	OPT_DELETE					3			//ɾ��

//************************************************************************************
//		֪ͨ����
//************************************************************************************
enum NotifyCode
{
	ncChannelStateChange = 1,					//ͨ��״̬�����ı�
	ncAlarmLevelChange = 2,						//ͨ�������������ı�
};

//////////////////////////////////////////////////////////////////////////

#define FreeObjPtrList(objPtrList) {\
	if ((objPtrList))\
	{\
		ASSERT_VALID((objPtrList));\
		while ((objPtrList)->GetCount())\
			delete (objPtrList)->RemoveHead ();\
	}\
}

#define FreeObjPtrArray(objPtrArr) {\
	if ((objPtrArr))\
	{\
		ASSERT_VALID((objPtrArr));\
		int nCnt = (objPtrArr)->GetCount();\
		for (int i=0; i<nCnt; i++)\
			delete (* objPtrArr)[i];\
		(objPtrArr)->RemoveAll();}\
}

#define FreeAndNullPtr(ptr) {\
	if (ptr)\
	{\
		delete ptr;\
		ptr = NULL;\
	}\
}

#define FreeAndNullArray(arrPtr) {\
	if (arrPtr)\
	{\
		delete [] arrPtr;\
		arrPtr = NULL;\
	}\
}

#define SWITCH_VALUE(x) (x<0.5 ? 0 : 1)

//////////////////////////////////////////////////////////////////////////

//��������
typedef struct AlarmLevel
{
	byte byAlarmLevel;
	TCHAR szAlarmLevelName[BUF_LEN_S];
	COLORREF clrAlarmColor;

	AlarmLevel ()
	{
		byAlarmLevel = 0;
		memset (szAlarmLevelName, 0x0, sizeof(szAlarmLevelName));
		clrAlarmColor = RGB(255, 0, 0);
	}

	void Copy (const AlarmLevel &al)
	{
		byAlarmLevel = al.byAlarmLevel;
		memcpy (szAlarmLevelName, al.szAlarmLevelName, sizeof(szAlarmLevelName));
		clrAlarmColor = al.clrAlarmColor;
	}
}AlarmLevel;
typedef CTypedPtrList<CPtrList, AlarmLevel*> CAlarmLevelList;
typedef CTypedPtrArray<CPtrArray, AlarmLevel*> CAlarmLevelArray;


//������Χ
typedef struct AlarmRange
{
	byte byAlarmLevel;
	TCHAR szAlarmRangeName[BUF_LEN_S];
	float fAlarmRangeLow;
	float fAlarmRangeHigh;

	AlarmRange ()
	{
		byAlarmLevel = 0;
		memset (szAlarmRangeName, 0x0, sizeof(szAlarmRangeName));
		fAlarmRangeLow = 0.0f;
		fAlarmRangeHigh = 1.0f;
	}

	void Copy (const AlarmRange &ar)
	{
		byAlarmLevel = ar.byAlarmLevel;
		memcpy (szAlarmRangeName, ar.szAlarmRangeName, sizeof(szAlarmRangeName));
		fAlarmRangeLow = ar.fAlarmRangeLow;
		fAlarmRangeHigh = ar.fAlarmRangeHigh;
	}
}AlarmRange;
typedef CTypedPtrList<CPtrList, AlarmRange*> CAlarmRangeList;
typedef CTypedPtrArray<CPtrArray, AlarmRange*> CAlarmRangeArray;


//��ע��ͨ��
typedef struct FollowingChannel
{
	int nDeviceNo;
	int nChannelNo;

	FollowingChannel()
	{
		nDeviceNo = -1;
		nChannelNo = -1;
	}
}FollowingChannel;
typedef CTypedPtrList<CPtrList, FollowingChannel*> CFollowingChannelList;
typedef CTypedPtrArray<CPtrArray, FollowingChannel*> CFollowingChannelArray;


//////////////////////////////////////////////////////////////////////////

//ϵͳ��
typedef struct DBSystem
{
	int nRecordID;
	BlobData<byte> bdDefBuf;

	int nOptFlag;

	DBSystem ()
	{
		nRecordID = 0;
		bdDefBuf.Reset();

		nOptFlag = OPT_NULL;
	}
}DBSystem;


//�û���Ϣ��
typedef struct UserInfo
{
	int nUserID;
	CString strUserName;
	CString strPassword;
	int nUserRight;
	BlobData<byte> bdDefBuf;

	int nOptFlag;

	UserInfo ()
	{
		nUserID = 0;
		strUserName.Empty();
		strPassword.Empty();
		nUserRight = 1;
		bdDefBuf.Reset();

		nOptFlag = OPT_NULL;
	}
}UserInfo;


//ͼ��
typedef struct Plot
{
	int nPlotID;
	CString strPlotName;
	byte byPlotType;
	BlobData<byte> bdData;
	BlobData<byte> bdDefBuf;

	int nOptFlag;

	Plot ()
	{
		nPlotID = 0;
		strPlotName.Empty();
		byPlotType = 0;
		bdData.Reset();
		bdDefBuf.Reset();

		nOptFlag = OPT_NULL;
	}
}Plot;


//�豸��
typedef struct DeviceInfo
{
	int nDeviceNo;
	CString strDeviceName;
	int nDeviceType;
	int nDriverType;
	int nChannelCount;
	CString strDeviceGUID;
	int nSampleInterval;
	BlobData<byte> bdDefBuf;

	int nOptFlag;

	DeviceInfo ()
	{
		nDeviceNo = 0;
		strDeviceName.Empty();
		nDeviceType = 0;
		nDriverType = 0;
		nChannelCount = 0;
		strDeviceGUID.Empty();
		nSampleInterval = 60;
		bdDefBuf.Reset();

		nOptFlag = OPT_NULL;
	}
	DeviceInfo (const DeviceInfo &device)
	{
		Copy (device);

		nOptFlag = OPT_NULL;
	}
	void Copy (const DeviceInfo &device)
	{
		nDeviceNo = device.nDeviceNo;
		strDeviceName = device.strDeviceName;
		nDeviceType = device.nDeviceType;
		nDriverType = device.nDriverType;
		nChannelCount = device.nChannelCount;
		strDeviceGUID = device.strDeviceGUID;
		nSampleInterval = device.nSampleInterval;
		bdDefBuf = device.bdDefBuf;
	}
}DeviceInfo;
typedef CTypedPtrList<CPtrList, DeviceInfo*> CDeviceInfoList;
typedef CTypedPtrArray<CPtrArray, DeviceInfo*> CDeviceInfoArray;


//ͨ����
typedef struct ChannelInfo
{
	int nDeviceNo;
	int nChannelNo;
	CString strChannelName;
	BOOL bWorkEnable;
	float fFullScaleHigh;
	float fFullScaleLow;
	float fCalibrationHigh;
	float fCalibrationLow;
	byte byDataType;
	byte byDecimalDigits;
	CString strUnitString;
	BlobData<byte> bdAlarmLevelData;
	BlobData<byte> bdAlarmRangeData;
	BlobData<byte> bdDefBuf;

	//�������ı�������ͱ�����Χ����������
	COLORREF clrNormalColor;				//����״̬���Ǳ�����ͨ������ɫ
	CAlarmLevelArray arrAlarmLevel;
	CAlarmRangeArray arrAlarmRange;

	//�������ı�������ͱ�����Χ����������
	byte bySwitchEnableAlarm;				//�������Ƿ�������
	byte bySwitchAlarmValue;				//������Ϊ0ʱ��������Ϊ1ʱ����

	//ͨ�����öԻ����õ�
	byte byOldDataType;			//����ԭʼͨ����������
	BOOL bMsgBoxDelData;		//��ʶ��ʾɾ�������Ƿ�ѡ���ǡ�

	int nOptFlag;

	ChannelInfo ()
	{
		nDeviceNo = 0;
		nChannelNo = 0;
		strChannelName.Empty();
		bWorkEnable = FALSE;
		fFullScaleHigh = 0.0f;
		fFullScaleLow = 0.0f;
		fCalibrationHigh = 0.0f;
		fCalibrationLow = 0.0f;
		byDataType = 0;
		byDecimalDigits = 0;
		strUnitString.Empty();
		bdAlarmLevelData.Reset();
		bdAlarmRangeData.Reset();
		bdDefBuf.Reset();

		clrNormalColor = NORMAL_COLOR;

		bySwitchEnableAlarm = 0;
		bySwitchAlarmValue = 0;

		byOldDataType = 0;
		bMsgBoxDelData = FALSE;

		nOptFlag = OPT_NULL;
	}

	ChannelInfo (const ChannelInfo &channel)
	{
		Copy (channel);

		nOptFlag = OPT_NULL;
	}

	void Copy (const ChannelInfo &channel)
	{
		nDeviceNo = channel.nDeviceNo;
		nChannelNo = channel.nChannelNo;
		strChannelName = channel.strChannelName;
		bWorkEnable = channel.bWorkEnable;
		fFullScaleHigh = channel.fFullScaleHigh;
		fFullScaleLow = channel.fFullScaleLow;
		fCalibrationHigh = channel.fCalibrationHigh;
		fCalibrationLow = channel.fCalibrationLow;
		byDataType = channel.byDataType;
		byDecimalDigits = channel.byDecimalDigits;
		strUnitString = channel.strUnitString;
		bdAlarmLevelData = channel.bdAlarmLevelData;
		bdAlarmRangeData = channel.bdAlarmRangeData;
		bdDefBuf = channel.bdDefBuf;
	}
}ChannelInfo;
typedef CTypedPtrList<CPtrList, ChannelInfo*> CChannelInfoList;
typedef CTypedPtrArray<CPtrArray, ChannelInfo*> CChannelInfoArray;


//ͨ�����ݱ�
typedef struct ChannelData
{
	int nDataID;
	int nDeviceNo;
	int nChannelNo;
	COleDateTime odtSampleTime;
	float fMeasureValue;
	byte byAlarmLevel;
	BlobData<byte> bdData;

	int nOptFlag;

	ChannelData ()
	{
		nDataID = 0;
		nDeviceNo = 0;
		nChannelNo = 0;
		odtSampleTime = COleDateTime(2000, 1, 1, 1, 1, 1);
		fMeasureValue = 0.0f;
		byAlarmLevel = 0;
		bdData.Reset();

		nOptFlag = OPT_NULL;
	}
}ChannelData;


//�������ݱ�
typedef struct AlarmData
{
	int nAlarmDataID;
	int nDataID;
	int nDeviceNo;
	int nChannelNo;
	COleDateTime odtSampleTime;
	float fMeasureValue;
	byte byAlarmLevel;
	BlobData<byte> bdDefBuf;

	int nOptFlag;

	AlarmData ()
	{
		nAlarmDataID = 0;
		nDataID = 0;
		nDeviceNo = 0;
		nChannelNo = 0;
		odtSampleTime = COleDateTime(2000, 1, 1, 1, 1, 1);
		fMeasureValue = 0.0f;
		byAlarmLevel = 0;
		bdDefBuf.Reset();

		nOptFlag = OPT_NULL;
	}
}AlarmData;


//��������
typedef struct Controller
{
	int nDeviceNo;
	int nControllerNo;
	CString strControllerName;
	byte byControllerType;
	BlobData<byte> bdData;
	BlobData<byte> bdDefBuf;

	int nOptFlag;

	Controller ()
	{
		nDeviceNo = 0;
		nControllerNo = 0;
		strControllerName.Empty();
		byControllerType = 0;
		bdData.Reset();
		bdDefBuf.Reset();

		nOptFlag = OPT_NULL;
	}
}Controller;
