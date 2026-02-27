#include "acknowledge.hpp"
//not implement
DOWNLINK_ACKNOWLEDGE Acknowledge::get_data(){
	msg.packet_id = PACKET_ID_ACKNOWLEDGE;
	msg.length_in_bytes = sizeof(DOWNLINK_ACKNOWLEDGE) - 2;
	msg.acknowledge_packet_id = 0;
	msg.sequence_number = 1;
	msg.sequence_number = 1;
	msg.response_code = 13;
	return msg;
}
