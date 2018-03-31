#pragma once

template<class T>
class Thread
{
	typedef DWORD(T::* Method)(void);
public:
	explicit Thread(T* object, DWORD(T::* method)(void));
	~Thread(void);

	bool start();
	void join();
	bool interrupt();

	inline bool isInterrupted()
	{
		return this->isInterrupted(0);
	}

	inline bool isInterrupted(DWORD millisec)
	{
		if (WaitForSingleObject(hInterrupt, millisec) == WAIT_TIMEOUT)
		{
			return false;
		}
		ReleaseSemaphore(hInterrupt, 1, NULL);
		return true;
	}

	inline bool isRunning()
	{
		DWORD exitCode = 0;
		if (hThread)
		{
			GetExitCodeThread(hThread, &exitCode);
		}

		return exitCode == STILL_ACTIVE;
	}

	__declspec(property(get = getThreadHandle)) HANDLE ThreadHandle;
	inline HANDLE getThreadHandle()
	{
		return hThread;
	}

	__declspec(property(get = getThreadID)) DWORD ThreadID;
	inline DWORD getThreadID()
	{
		return threadID;
	}

private:
	Thread(const Thread<T>& other) {}
	Thread<T>& operator =(const Thread<T>& other) {}

	static DWORD run(LPVOID thread_obj)
	{
		Thread<T>* thread = (Thread<T>*)thread_obj;
		return (thread->object->*thread->method) ();
	}

private:
	HANDLE  hThread;      // unique handle to the thread
	DWORD   threadID;     // thread id - 0 until started
	T*      object;       // the object which owns the method
	Method  method;       // the method of the object
	HANDLE  hInterrupt;   // mutex to signal an interrupt via ReleaseSemaphore()
	HANDLE  hSingleStart; // only one thread allowed to call start() mutex
};

template<class T>
Thread<T>::Thread(T* object, DWORD(T::* method)(void))
{
	this->hThread = NULL;
	this->object = object;
	this->method = method;
	this->threadID = 0;
	this->hInterrupt = CreateSemaphore(NULL, 1, 1, NULL);
	this->hSingleStart = CreateMutex(NULL, FALSE, NULL);
}


template<class T>
Thread<T>::~Thread(void)
{
	if (hInterrupt)
	{
		CloseHandle(hInterrupt);
	}
	if (hThread)
	{
		CloseHandle(hThread);
	}
}

template<class T>
bool Thread<T>::start()
{
	__try {
		if (WaitForSingleObject(hSingleStart, 0) != WAIT_OBJECT_0)
		{
			return false;
		}
		if (hThread)    // Thread had been started sometime in the past
		{
			if (WaitForSingleObject(hThread, 0) == WAIT_TIMEOUT)
			{   // if thread's still running deny new start
				return false;
			}
			CloseHandle(hThread);
		}
		// (Re-)Set not interrupted semaphore state
		WaitForSingleObject(hInterrupt, 0);

		hThread = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)Thread<T>::run,
			this,
			0,
			&this->threadID
		);
		return hThread;
	}
	__finally
	{
		ReleaseMutex(hSingleStart);
	}
}

template<class T>
void Thread<T>::join()
{
	WaitForSingleObject(hThread, INFINITE);
}

template<class T>
bool Thread<T>::interrupt()
{
	if (hInterrupt)
	{
		return ((ReleaseSemaphore(hInterrupt, 1, NULL) == FALSE) ? false : true);
	}
	return false;
}