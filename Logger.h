#pragma once
#include "Active.h"
#include <string>

#define LOG(STR) logger::LoggerIndentManager lgr; lgr.Log(__FILE__, __FUNCTION__, __LINE__, STR)
namespace logger {
	using active_object::Active;


	class LoggerIndentManager
	{
	public:
		LoggerIndentManager();

		~LoggerIndentManager();

		void Log(const std::string& file, const std::string& function, int line, const std::string& message);
	private:
		static int thread_local indent_count_;
	};

	class LoggerInternal
	{
		class LogMessage final
		{
			std::string msg_;
			bool is_final_msg_;
			LogMessage() : is_final_msg_(true)
			{}
		public:
			LogMessage(std::string&& s);
			LogMessage(const std::string& s);
			static const LogMessage& GetStopMessage();
			bool IsStopMessage();
			void Execute();
		};

	public:
		static LoggerInternal& GetLogger()
		{
			static LoggerInternal logger;
			return logger;
		}

		void Log(const std::string& file, const std::string& function, int line, const std::string& message, int level)
		{
			std::stringstream message_stream;
			message_stream << std::string(level, '\t') << "tid [" << std::hex << std::this_thread::get_id() << "] [" << " " << "] ["
				<< file << '|' << function << ':' << std::dec << line << "] " << message;

			logger_bg_.Send(LogMessage(message_stream.str()));
		}
	private:
		Active<LogMessage> logger_bg_;
	};
}