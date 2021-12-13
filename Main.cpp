#include <thread>
#include <chrono>
#include "Logger.h"



void myFunc()
{
	LOG("Other thread1");
	{
		LOG("Other thread2");
		{
			LOG("Other thread3");
		}
	}
}
int main()
{
	using namespace std::chrono_literals;

	std::jthread thr(myFunc);
	{
		LOG("Main thread1");
		{
			LOG("Main thread2");
			{
				LOG("Main thread3");
			}
		}
	}
	std::this_thread::sleep_for(3000ms);
	{
		LOG("main thread after pause");
	}
	std::jthread thr2(myFunc);
	
}