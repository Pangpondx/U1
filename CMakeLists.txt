px4_add_module(
	MODULE modules_downlink_protocol
	MAIN downlink_protocol
	SRCS
		downlink_protocol.cpp
		telemetry.cpp
		engine_status.cpp
		temperature_report.cpp
		acknowledge.cpp
		control_surface.cpp

)
