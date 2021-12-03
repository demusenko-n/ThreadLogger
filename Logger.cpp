﻿#include "Logger.h"
#include <chrono>
#include <iostream>

logger::LoggerIndentManager::LoggerIndentManager()
{
	++indent_count_;
}

logger::LoggerIndentManager::~LoggerIndentManager()
{
	--indent_count_;
}

void logger::LoggerIndentManager::Log(const std::string& file, const std::string& function, int line, const std::string& message)
{
	logger::LoggerInternal::GetLogger().Log(file, function, line, message, indent_count_);
}

logger::LoggerInternal::LogMessage::LogMessage(std::string s): msg_(std::move(s))
{}

void logger::LoggerInternal::LogMessage::Execute()
{
	using namespace std::chrono_literals;
	
	std::cout << msg_ << std::endl;
	std::this_thread::sleep_for(100ms);
}

int thread_local logger::LoggerIndentManager::indent_count_ = -1;