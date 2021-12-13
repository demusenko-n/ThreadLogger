#pragma once
#include <memory>
#include <thread>
#include <queue>

#include "QueueThreadSafe.h"

namespace active_object
{
	using std::queue;
	using std::thread;

	template <typename T>
	concept Executable = requires(T c) {
		c.Execute();
		{c.IsStopMessage()} -> std::same_as<bool>;
		{T::GetStopMessage() } -> std::convertible_to<const T&>;
	};



	template<Executable T>
	class Active
	{
	public:

		Active() : thd_([this]() {Run(); })
		{}

		~Active() {
			try
			{
				Interrupt();
				thd_.join();
			}
			catch (...)
			{
			}
		}

		template<Executable U>
		void Send(U&& m)
		{
			if (!m.IsStopMessage())
			{
				mq_.push(std::forward<U>(m));
			}
		}

		Active& operator= (const Active& other) = delete;
		Active& operator= (Active&& other) = delete;
	private:
		QueueThreadSafe<T> mq_;
		thread thd_;

		void Run() {

			while (true)
			{
				T msg = mq_.dequeue();
				if (msg.IsStopMessage())
				{
					break;
				}
				msg.Execute();
			}
		}

		void Interrupt()
		{
			mq_.push(T::GetStopMessage());
		}

	};
}
