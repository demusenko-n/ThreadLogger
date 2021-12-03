#pragma once
#include <queue>
#include <shared_mutex>


namespace active_object
{
	//I'm not sure if it's okay to do so, because
	//if I write lines below, after that in any file I can do following:
	// using active_object::unique_lock
	// using active_object::shared_lock
	// etc.

	using std::unique_lock;
	using std::shared_lock;
	using std::shared_mutex;
	using std::queue;
	using std::condition_variable_any;

	template<class T>
	class QueueThreadSafe
	{
	public:
		QueueThreadSafe();

		template<class U>
		void push(U&& obj);

		template<class... Ts>
		void emplace(Ts&&... args);

		[[nodiscard]] T& front()const;

		[[nodiscard]] T& back()const;

		T& front();

		T& back();

		[[nodiscard]] bool empty()const;

		[[nodiscard]] size_t size()const;

		void pop();

		[[nodiscard]] T/*&&*/ dequeue();

	private:
		queue<T> queue_;
		shared_mutex mutex_;
		mutable condition_variable_any cv_enq_;
	};

	template <class T>
	QueueThreadSafe<T>::QueueThreadSafe() = default;

	template <class T>
	template <class U>
	void QueueThreadSafe<T>::push(U&& obj)
	{
		unique_lock l(mutex_);
		queue_.push(std::forward<U>(obj));
		l.unlock();
		cv_enq_.notify_one();
	}

	template <class T>
	template <class ... Ts>
	void QueueThreadSafe<T>::emplace(Ts&&... args)
	{
		unique_lock l(mutex_);
		queue_.emplace(std::forward<Ts>(args)...);
		l.unlock();
		cv_enq_.notify_one();
	}

	template <class T>
	T& QueueThreadSafe<T>::front() const
	{
		shared_lock l(mutex_);
		while (queue_.empty())
		{
			cv_enq_.wait(l);
		}
		return queue_.front();
	}

	template <class T>
	T& QueueThreadSafe<T>::back() const
	{
		shared_lock l(mutex_);
		while (queue_.empty())
		{
			cv_enq_.wait(l);
		}
		return queue_.back();
	}

	template <class T>
	T& QueueThreadSafe<T>::front()
	{
		shared_lock l(mutex_);
		while (queue_.empty())
		{
			cv_enq_.wait(l);
		}
		return queue_.front();
	}

	template <class T>
	T& QueueThreadSafe<T>::back()
	{
		shared_lock l(mutex_);
		while (queue_.empty())
		{
			cv_enq_.wait(l);
		}
		return queue_.back();
	}

	template <class T>
	bool QueueThreadSafe<T>::empty() const
	{
		shared_lock l(mutex_);
		return queue_.empty();
	}

	template <class T>
	size_t QueueThreadSafe<T>::size() const
	{
		shared_lock l(mutex_);
		return queue_.size();
	}

	template <class T>
	void QueueThreadSafe<T>::pop()
	{
		unique_lock l(mutex_);

		while (queue_.empty())
		{
			cv_enq_.wait(l);
		}

		queue_.pop();
	}

	template <class T>
	T QueueThreadSafe<T>::dequeue()
	{
		unique_lock l(mutex_);
		while (queue_.empty())
		{
			cv_enq_.wait(l);
		}
		T res = std::move(queue_.front());
		queue_.pop();
		//return std::move(res);
		return res;		//I believe it's clever enough to optimize it best way...
	}
}
