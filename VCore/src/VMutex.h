#pragma once
#include <atomic>
#include <mutex>
#include "VObject.h"


class VMutex : public std::mutex, public VObject
{
public:
	VMutex(){}
	virtual ~VMutex(){}

	void lock()
	{	
		local_lock.lock();
		is_lock = true;
		std::mutex::lock();
	}

	bool try_lock()
	{	
		if (!local_lock.try_lock())
		{
			return false;
		}
		
		if (std::mutex::try_lock())
		{
			is_lock = true;
			return is_lock;
		}
		
		return false;
	}

	void unlock()
	{	
		local_lock.unlock();
		is_lock = true;
		std::mutex::unlock();
		
	}

	bool isLock(){
		return is_lock;
	}
protected:
private:
	std::mutex local_lock;
	std::atomic<bool> is_lock;
};
