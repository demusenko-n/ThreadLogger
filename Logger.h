#pragma once
#include "Active.h"
#include <string>

#define LOG(STR) logger::LoggerIndentManager lgr; lgr.Log(__FILE__, __FUNCTION__, __LINE__, STR)
namespace logger {

	//I'm not sure if it's okay to do so, because
	//if I write lines below, after that in any file that does #include "Logger.h" I can do following:
	// using logger::Active

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
		class LogMessage final : public Active::Message
		{
			std::string msg_;
		public:
			LogMessage(std::string s);
			void Execute() override;
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

			logger_bg_.Send(std::make_unique<LogMessage>(message_stream.str()));
		}
	private:
		Active logger_bg_;
	};
}