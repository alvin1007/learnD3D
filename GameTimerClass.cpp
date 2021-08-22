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
	  중단과 시작 사이에 흐른 시간을 구한다.

	                           |<----------------d--------------->|
	  -------------------------*----------------------------------*-----------------------> 시간
	                       mStopTime                          startTime
    */

	// 중단 상태에서 타이머를 재개하는 경우라면..
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
	// 이미 정지 상태면 아무 일도 하지 않는다.
	if (!mStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		// 중단 시간(일시 정지된 시점의 시간)을 저장하고, 타이머가 중단된다
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

	// 이번 프레임의 시간을 얻는다
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	// 이 시간과 이전 프레임의 시간의 차이를 구한다
	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

	//다음 프레임을 준비한다
	mPrevTime = mCurrTime;

	/*
	  음수가 되지 않도록 한다.
	  SDK 문서화의 CDXUTTimer 항목에 따르면, 프로세서가 절전 모드로 들어가거나 다른 프로세서와 엉키는 경우 mDeltaTime이 음수가 될 수 있다.
	*/
	if (mDeltaTime < 0.0f)
	{
		mDeltaTime = 0.0f;
	}
}