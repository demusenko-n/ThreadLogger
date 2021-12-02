#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <chrono>
#include <queue>

#define LOG(...) logger lg(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define STOP_LOGGER() logger::stop()

std::string get_current_timestamp()
{
	//isn't thread-safe, so I don't put time information into logs :(
	const auto time = std::chrono::system_clock::now();
	const std::time_t end_time = std::chrono::system_clock::to_time_t(time);
	char* str = std::ctime(&end_time);
	str[strlen(str) - 1] = '\0';
	return str;
}




class logger
{
public:
	template<class... Ts>
	logger(const char* file, const char* function, int line, Ts&&... params)
	{
		using namespace std::chrono_literals;

		static std::queue<std::unique_ptr<std::stringstream>> log_queue;
		static std::mutex queue_mutex;
		static std::condition_variable cv_queue_not_full;
		static std::jthread consumer([]() {
			while (true)
			{
				std::unique_lock lock(queue_mutex);

				while (log_queue.empty()  && !is_stopped_)
				{
					cv_to_log.wait(lock, []() {return !log_queue.empty() || is_stopped_; });
				}

				while(!log_queue.empty())
				{
					const auto s = std::move(log_queue.front());
					log_queue.pop();
					//std::cout << "taken first elem for writing...\n";
					lock.unlock();
					cv_queue_not_full.notify_one();

					std::this_thread::sleep_for(100ms);
					stream << s->rdbuf();
					
					lock.lock();
				}

				if (is_stopped_)
					return;
			}
			});


		++indent_count_;

		const std::string indent(indent_count_, '\t');

		auto message_stream_ptr = std::make_unique<std::stringstream>();

		*message_stream_ptr << indent << "tid [" << std::hex << std::this_thread::get_id() << "] [" << " " << "] ["
			<< file << '|' << function << ':' << std::dec << line << "] ";
		(*message_stream_ptr << ... << std::forward<Ts>(params)) << std::endl;


		{
			std::unique_lock l(queue_mutex);
			while (log_queue.size() >= max_queue_size)
			{
				cv_queue_not_full.wait(l);
			}
			//std::cout << "pushing\n";
			log_queue.push(std::move(message_stream_ptr));
		}
		cv_to_log.notify_one();
	}

	logger(const logger& other) = delete;
	logger& operator=(const logger& other) = delete;

	static void stop()
	{
		is_stopped_ = true;
		cv_to_log.notify_one();
	}


	~logger()
	{
		--indent_count_;
	}

private:
	static bool is_stopped_;
	static thread_local std::atomic<size_t> indent_count_;
	static std::condition_variable cv_to_log;
	constexpr static size_t max_queue_size = 5;
	static std::ostream& stream;
};
std::ostream& logger::stream = std::cout;
bool logger::is_stopped_ = false;
std::condition_variable logger::cv_to_log;
std::atomic<size_t> thread_local logger::indent_count_ = -1;


void myFunc()
{
	LOG("m1!", "1");
	{
		LOG("m2!", "2");
		{
			LOG("m3!", "3");
		}
	}
}
int main()
{
	using namespace std::chrono_literals;

	std::thread thr(myFunc);
	std::thread thr2(myFunc);
	std::thread thr3(myFunc);
	std::thread thr4(myFunc);
	std::thread thr5(myFunc);
	std::thread thr6(myFunc);
	std::thread thr7(myFunc);
	std::thread thr8(myFunc);
	std::thread thr9(myFunc);
	std::thread thr0(myFunc);

	LOG("m1!", "1");
	{
		LOG("m2!", "2");
		{
			LOG("m3!", "3");
		}
	}

	thr.join();
	thr2.join();
	thr3.join();
	thr4.join();
	thr5.join();
	thr6.join();
	thr7.join();
	thr8.join();
	thr9.join();
	thr0.join();
	
	STOP_LOGGER(); //on this point logger will finish current queue and after that will finish working (won't call wait anymore)
}