#include "disco_debug.h"

DiscoDebug::DiscoDebug(): ModuleBase<DiscoDebug>()
{
}
DiscoDebug::~DiscoDebug()
{
}

int DiscoDebug::task_spawn(int argc, char *argv[])
{
	DiscoDebug *instance = instantiate(argc, argv);
	if(instance){
		_object.store(instance);
		_task_id = task_id_is_work_queue;

		_task_id = px4_task_spawn_cmd("disco_debug",
					      SCHED_DEFAULT,
					      SCHED_PRIORITY_DEFAULT -10,
					      2048,
					      (px4_main_t)&run_trampoline,
					      (char *const *)argv);
		if(_task_id < 0){
			_object.store(nullptr);
			delete instance;
			return PX4_ERROR;
		}
		return PX4_OK;
	}
	return PX4_ERROR;
}

void DiscoDebug::run()
{
	PX4_INFO("Disco Debugger: Waiting for data...");

	int sub_fd = orb_subscribe(ORB_ID(disco_data));

	px4_pollfd_struct_t fds[1];
	fds[0].fd = sub_fd;
	fds[0].events = POLLIN;

	while (!should_exit()) {

		int poll_ret = px4_poll(fds, 1, 1000);

		if (poll_ret == 0) {
			continue;
		}
		else if (poll_ret < 0) {
			PX4_ERR("Poll error");
			px4_usleep(50000);
			continue;
		}

		if (fds[0].revents & POLLIN) {
			disco_data_s data;
			orb_copy(ORB_ID(disco_data), sub_fd, &data);
			PX4_INFO("%llu %u %lu %lu",data.timestamp, data.sequence, data.success, data.error);
		}
	}

	orb_unsubscribe(sub_fd);
	PX4_INFO("Disco Debugger Stopped");
}

DiscoDebug *DiscoDebug::instantiate(int argc, char *argv[]) {
	return new DiscoDebug();
}

int DiscoDebug::custom_command(int argc, char *argv[]) {
	return print_usage("unknown command");
}

int DiscoDebug::print_usage(const char *reason) {
	if (reason) PX4_WARN("%s\n", reason);
	PRINT_MODULE_DESCRIPTION("Disco Debugger (Polling Mode)");
	PRINT_MODULE_USAGE_NAME("disco_debug", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	return 0;
}

extern "C" __EXPORT int debug_protocol_main(int argc, char *argv[]) {
	return DiscoDebug::main(argc, argv);
}
