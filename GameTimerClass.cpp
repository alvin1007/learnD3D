#include "stdafx.h"
#include "GameTimerClass.h"

GameTimerClass::GameTimerClass() : mSecondsPerCount(0.0f), mDeltaTime(-1.0f), mBaseTime(0), mPausedTime(0), mPrevTime(0), mCurrTime(0), mStopped(false)
{
	__int64 countsPersec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPersec);
	mSecondsPerCount = 1.0 / (double)countsPersec;
}


GameTimerClass::GameTimerClass(const GameTimerClass& other)
{
}


GameTimerClass::~GameTimerClass()
{
}

float GameTimerClass::TotalTime() const
{
	if (mStopped)
	{
		return (float)(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
	else
	{
		return (float)(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
}

float GameTimerClass::DeltaTime() const
{
	return (float)mDeltaTime;
}

void GameTimerClass::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void GameTimerClass::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	/*
	  �ߴܰ� ���� ���̿� �帥 �ð��� ���Ѵ�.

	                           |<----------------d--------------->|
	  -------------------------*----------------------------------*-----------------------> �ð�
	                       mStopTime                          startTime
    */

	// �ߴ� ���¿��� Ÿ�̸Ӹ� �簳�ϴ� �����..
	if (mStopped)
	{
		mPausedTime += (startTime - mStopTime);

		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

void GameTimerClass::Stop()
{
	// �̹� ���� ���¸� �ƹ� �ϵ� ���� �ʴ´�.
	if (!mStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		// �ߴ� �ð�(�Ͻ� ������ ������ �ð�)�� �����ϰ�, Ÿ�̸Ӱ� �ߴܵȴ�
		mStopTime = currTime;
		mStopped = true;
	}
}

void GameTimerClass::Tick()
{
	if (mStopped)
	{
		mDeltaTime = 0.0f;
		return;
	}

	// �̹� �������� �ð��� ��´�
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	// �� �ð��� ���� �������� �ð��� ���̸� ���Ѵ�
	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

	//���� �������� �غ��Ѵ�
	mPrevTime = mCurrTime;

	/*
	  ������ ���� �ʵ��� �Ѵ�.
	  SDK ����ȭ�� CDXUTTimer �׸� ������, ���μ����� ���� ���� ���ų� �ٸ� ���μ����� ��Ű�� ��� mDeltaTime�� ������ �� �� �ִ�.
	*/
	if (mDeltaTime < 0.0f)
	{
		mDeltaTime = 0.0f;
	}
}