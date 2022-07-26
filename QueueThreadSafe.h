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

	template<class T>
	class QueueThreadSafe
	{
	public:
		QueueThreadSafe();

		template<class U>
		void push(U&& obj);

		template<class... Ts>
		void emplace(Ts&&... args);

		[[nodiscard]] const T& front()const;

		[[nodiscard]] const T& back()const;

		T& front();

		T& back();

		[[nodiscard]] bool empty()const;

		[[nodiscard]] size_t size()const;

		void pop();

		[[nodiscard]] T dequeue();

	private:
		queue<T> queue_;
		mutable shared_mutex mutex_;
		mutable condition_variable_any cv_enq_;
		mutable std::condition_variable cv;
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
	const T& QueueThreadSafe<T>::front() const
	{
		shared_lock l(mutex_);
		cv_enq_.wait(l, [this] { return !queue_.empty(); });
		return queue_.front();
	}

	template <class T>
	const T& QueueThreadSafe<T>::back() const
	{
		shared_lock l(mutex_);
		cv_enq_.wait(l, [this] { return !queue_.empty(); });
		return queue_.back();
	}

	template <class T>
	T& QueueThreadSafe<T>::front()
	{
		shared_lock l(mutex_);
		cv_enq_.wait(l, [this] { return !queue_.empty(); });
		return queue_.front();
	}

	template <class T>
	T& QueueThreadSafe<T>::back()
	{
		shared_lock l(mutex_);
		cv_enq_.wait(l, [this] { return !queue_.empty(); });
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

		cv_enq_.wait(l, [this] { return !queue_.empty(); });

		queue_.pop();
	}

	template <class T>
	T QueueThreadSafe<T>::dequeue()
	{
		unique_lock l(mutex_);
		cv_enq_.wait(l, [this] { return !queue_.empty(); });
		T res = std::move(queue_.front());
		queue_.pop();
		return res;
	}
}
