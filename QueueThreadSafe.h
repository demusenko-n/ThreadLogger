#pragma once
#include <queue>
#include <shared_mutex>


namespace active_object
{
	using std::unique_lock;
	using std::shared_lock;
	using std::shared_mutex;
	using std::queue;
	using std::condition_variable_any;

	template<class Message>
	class QueueThreadSafe
	{
	public:
		QueueThreadSafe();

		template<class U>
		void push(U&& obj);

		template<class... Ts>
		void emplace(Ts&&... args);

		[[nodiscard]] const Message& front()const;

		[[nodiscard]] const Message& back()const;

		Message& front();

		Message& back();

		[[nodiscard]] bool empty()const;

		[[nodiscard]] size_t size()const;

		void pop();

		[[nodiscard]] Message dequeue();

	private:
		queue<Message> queue_;
		mutable shared_mutex mutex_;
		mutable condition_variable_any cv_enq_;
	};

	template <class Message>
	QueueThreadSafe<Message>::QueueThreadSafe() = default;

	template <class Message>
	template <class U>
	void QueueThreadSafe<Message>::push(U&& obj)
	{
		unique_lock l(mutex_);
		queue_.push(std::forward<U>(obj));
		l.unlock();
		cv_enq_.notify_one();
	}

	template <class Message>
	template <class ... Ts>
	void QueueThreadSafe<Message>::emplace(Ts&&... args)
	{
		unique_lock l(mutex_);
		queue_.emplace(std::forward<Ts>(args)...);
		l.unlock();
		cv_enq_.notify_one();
	}

	template <class Message>
	const Message& QueueThreadSafe<Message>::front() const
	{
		shared_lock l(mutex_);
		cv_enq_.wait(l, [this] { return !queue_.empty(); });
		return queue_.front();
	}

	template <class Message>
	const Message& QueueThreadSafe<Message>::back() const
	{
		shared_lock l(mutex_);
		cv_enq_.wait(l, [this] { return !queue_.empty(); });
		return queue_.back();
	}

	template <class Message>
	Message& QueueThreadSafe<Message>::front()
	{
		shared_lock l(mutex_);
		cv_enq_.wait(l, [this] { return !queue_.empty(); });
		return queue_.front();
	}

	template <class Message>
	Message& QueueThreadSafe<Message>::back()
	{
		shared_lock l(mutex_);
		cv_enq_.wait(l, [this] { return !queue_.empty(); });
		return queue_.back();
	}

	template <class Message>
	bool QueueThreadSafe<Message>::empty() const
	{
		shared_lock l(mutex_);
		return queue_.empty();
	}

	template <class Message>
	size_t QueueThreadSafe<Message>::size() const
	{
		shared_lock l(mutex_);
		return queue_.size();
	}

	template <class Message>
	void QueueThreadSafe<Message>::pop()
	{
		unique_lock l(mutex_);

		cv_enq_.wait(l, [this] { return !queue_.empty(); });

		queue_.pop();
	}

	template <class Message>
	Message QueueThreadSafe<Message>::dequeue()
	{
		unique_lock l(mutex_);
		cv_enq_.wait(l, [this] { return !queue_.empty(); });
		Message res = std::move(queue_.front());
		queue_.pop();
		return res;
	}
}
