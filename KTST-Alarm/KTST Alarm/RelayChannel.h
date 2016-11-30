#pragma once

#include "channel.h"

enum ExpressionType		//�̵������ʽ����
{
	etOperator = 0, 
	etChannel = 1, 
};

//�̵������ʽ
typedef struct RelayExpression
{
	ExpressionType eExpressionType;	//���ʽ���ͣ�0��������1��ע��ͨ��

// 	byte byOperatorType;	//���������ͣ�0 End��1 "("��2 ")"��3 "&"��4 "|"��5 "~"
	CString strOperator;	//������

	int nDeviceNo;			//��עͨ�����豸��
	int nChannelNo;			//��עͨ����ͨ����
	byte byAlarmLevel;		//��������

	CString strDisplay;		//�ڱ��ʽ�༭������ʾ���ַ���
	int nIndexBegin;		//�ڱ��ʽ�༭������ʼ����
	int nIndexEnd;			//�ڱ��ʽ�༭���н�������

	RelayExpression ()
	{
		eExpressionType = etOperator;
// 		byOperatorType = 0;
		strOperator.Empty();
		nDeviceNo = 0;
		nChannelNo = 0;
		byAlarmLevel = 0;

		strDisplay.Empty();
		nIndexBegin = 0;
		nIndexEnd = 0;
	}
}RelayExpression;
typedef CTypedPtrList<CPtrList, RelayExpression*> CRelayExpressionList;

class RelayChannel :
	public Channel
{
public:
	RelayChannel(void);
	virtual ~RelayChannel(void);

public:
	virtual BOOL Initialize ();
	virtual BOOL UnInitialize ();
	virtual void OnNotify (NotifyCode code, WPARAM wParam, LPARAM lParam);
	virtual void OnTimer ();

public:
	BOOL AnalyzeRelayChannelBuffer ();
	BOOL SaveRelayChannelBuffer ();

public:
	int GetOutputValue ();
	void Reset ();
	void RequestAllFollowingChannelData ();

	void SetExpressionUpdate (BOOL bUpdate);
	BOOL IsExpressionUpdated ();

public:
	CString GetExpressionString ();
	BOOL Cal(CString str,int& nReturnValue);

private:
	void ClearMemory ();

public:
	int m_nOutputDelay;						//�����ʱ(��)
	BOOL m_bEnergized;						//���
	BOOL m_bBypass;							//��·
	BOOL m_bLatching;						//����
	CRelayExpressionList m_listExpression;	//���ʽ����

private:
	BOOL m_bDelayStarting;					//�����ʱ����
	DWORD m_dwDelayStartTime;				//�����ʱ�Ѿ���ʱʱ��

	int m_nLastOutputValue;					//ǰһ�����ֵ

	BOOL m_bExpressionUpdate;				//��ʶ���ʽ�Ƿ��б���״̬����

public:
	BOOL m_bExpressionValid;				//��ʶ���ʽ�Ƿ���Ч����Ч��������Ϊѡ��Ĳɼ�ͨ�������ڣ�


};
typedef CTypedPtrList<CPtrList, RelayChannel*> CRelayChannelList;

