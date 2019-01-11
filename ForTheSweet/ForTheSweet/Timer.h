#pragma once

const ULONG MAX_SAMPLE_COUNT = 50; // 50ȸ�� ������ ó���ð��� �����Ͽ� ����Ѵ�.

class CGameTimer
{
public:
	CGameTimer();
	virtual ~CGameTimer();

	void Tick(float fLockFPS = 0.0f);	// Ÿ�̸� �ð� ����
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);	//�����ӷ���Ʈ ��ȯ

	void Start();
	void Stop();
	void Reset();

	float GetTimeElapsed();				// �������� ��սð� ��ȯ
private:
	bool			m_bHardwareHasPerformanceCounter;	// ��ǻ�Ͱ� Performance Counter�� ������ �ִ���
	float			m_fTimeScale;						// Scale Counter�� ��
	float			m_fTimeElapsed;						// ������ ������ ���� ������ �ð�
	__int64			m_nCurrentTime;						// ������ �ð�
	__int64			m_nLastTime;						// ������ �������� �ð�
	__int64			m_nPerformanceFrequency;			// ��ǻ���� Performance Frequency

	__int64			m_nBasePerformanceCounter;
	__int64			m_nPausedPerformanceCounter;
	__int64			m_nStopPerformanceCounter;
	__int64			m_nLastPerformanceCounter;

	float			m_fFrameTime[MAX_SAMPLE_COUNT];		// ������ �ð��� �����ϱ� ���� �迭
	ULONG			m_nSampleCount;						// ������ ������ Ƚ��
	unsigned long	m_nCurrentFrameRate;				// ������ ������ ����Ʈ
	unsigned long	m_nFramesPerSecond;					// �ʴ� ������ ��
	float			m_fFPSTimeElapsed;					// ������ ����Ʈ ��� �ҿ� �ð�

	bool			m_bStopped;
};

