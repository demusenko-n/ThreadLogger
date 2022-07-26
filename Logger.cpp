#include "Logger.h"
#include <chrono>
#include <iostream>

namespace logger
{
	LoggerIndentManager::LoggerIndentManager()
	{
		++indent_count_;
	}

	LoggerIndentManager::~LoggerIndentManager()
	{
		--indent_count_;
	}

	void LoggerIndentManager::Log(const std::string& file, const std::string& function, int line, const std::string& message)
	{
		logger::LoggerInternal::GetLogger().Log(file, function, line, message, indent_count_);
	}

	LoggerInternal::LogMessage::LogMessage(std::string&& s) : msg_(std::move(s)), is_final_msg_(false)
	{}

	LoggerInternal::LogMessage::LogMessage(const std::string& s) : msg_(s), is_final_msg_(false)
	{}

	bool LoggerInternal::LogMessage::IsStopMessage() 
	{
		return is_final_msg_;
	}

	const LoggerInternal::LogMessage& LoggerInternal::LogMessage::GetStopMessage()
	{
		static LogMessage msg_final;
		return msg_final;
	}

	void LoggerInternal::LogMessage::Execute()
	{
		using namespace std::chrono_literals;

		std::cout << msg_ << std::endl;
		std::this_thread::sleep_for(100ms);
	}

	int thread_local LoggerIndentManager::indent_count_ = -1;
}
