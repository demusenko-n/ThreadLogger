#include "Active.h"
active_object::Active::Message::~Message() = default;

void active_object::Active::Message::Execute()
{}

active_object::Active::Active() : thd_(std::make_unique<thread>([this]() {Run(); }))
{}

active_object::Active::~Active()
{
	try
	{
		Interrupt();
		thd_->join();
	}
	catch (...)
	{}
}

void active_object::Active::Send(unique_ptr<Message> m)
{
	if (m != nullptr)
	{
		mq_.push(std::move(m));
	}
}

void active_object::Active::Run()
{
	unique_ptr<Message> msg;
	while ((msg = mq_.dequeue()) != nullptr)
	{
		msg->Execute();
	}
}

void active_object::Active::Interrupt()
{
	mq_.push(nullptr);
}
