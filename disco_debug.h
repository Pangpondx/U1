#pragma once

#include <px4_platform_common/module.h>
#include <uORB/Subscription.hpp>
#include <uORB/topics/disco_data.h>
#include <poll.h>

class DiscoDebug : public ModuleBase<DiscoDebug>
{
public:
	DiscoDebug();
	~DiscoDebug();

	static int task_spawn(int argc, char *argv[]);
	static DiscoDebug *instantiate(int argc, char *argv[]);
	static int custom_command(int argc, char *argv[]);
	static int print_usage(const char *reason = nullptr);

	void run() override;

private:
	bool task_should_exit{false};
};
