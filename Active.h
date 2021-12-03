#pragma once
#include <memory>
#include <thread>
#include <queue>

#include "QueueThreadSafe.h"

namespace active_object
{
	//I'm not sure if it's okay to do so, because
	//if I write lines below, after that in any file I can do following:
	// using active_object::unique_ptr
	// using active_object::queue
	// etc.
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
			virtual void Execute() = 0;
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
