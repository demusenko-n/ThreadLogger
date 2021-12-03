#pragma once
#include <memory>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <queue>

#include "QueueThreadSafe.h"

namespace active_object
{
	using std::unique_ptr;
	using std::queue;
	using std::thread;


	class Active
	{
	public:
		class Message
		{
		public:
			virtual ~Message();
			virtual void Execute();
		};

		Active();

		~Active();

		void Send(unique_ptr<Message> m);

		//Active(Active&& other) = delete;
		//Active(const Active& other) = delete;
		Active& operator= (const Active& other) = delete;
		Active& operator= (Active&& other) = delete;
	private:
		QueueThreadSafe<unique_ptr<Message>> mq_;
		unique_ptr<thread> thd_;

		void Run();

		void Interrupt();
	};
}
