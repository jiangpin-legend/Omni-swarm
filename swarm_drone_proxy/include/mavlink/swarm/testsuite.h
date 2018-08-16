/** @file
 *    @brief MAVLink comm protocol testsuite generated from swarm.xml
 *    @see http://qgroundcontrol.org/mavlink/
 */
#pragma once
#ifndef SWARM_TESTSUITE_H
#define SWARM_TESTSUITE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAVLINK_TEST_ALL
#define MAVLINK_TEST_ALL
static void mavlink_test_common(uint8_t, uint8_t, mavlink_message_t *last_msg);
static void mavlink_test_swarm(uint8_t, uint8_t, mavlink_message_t *last_msg);

static void mavlink_test_all(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
    mavlink_test_common(system_id, component_id, last_msg);
    mavlink_test_swarm(system_id, component_id, last_msg);
}
#endif

#include "../common/testsuite.h"


static void mavlink_test_swarm_info(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_SWARM_INFO >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_swarm_info_t packet_in = {
        17.0,45.0,73.0,101.0,129.0,157.0,{ 185.0, 186.0, 187.0, 188.0, 189.0, 190.0, 191.0, 192.0, 193.0, 194.0 },197
    };
    mavlink_swarm_info_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.x = packet_in.x;
        packet1.y = packet_in.y;
        packet1.z = packet_in.z;
        packet1.vx = packet_in.vx;
        packet1.vy = packet_in.vy;
        packet1.vz = packet_in.vz;
        packet1.odom_vaild = packet_in.odom_vaild;
        
        mav_array_memcpy(packet1.remote_distance, packet_in.remote_distance, sizeof(float)*10);
        
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_SWARM_INFO_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_SWARM_INFO_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_swarm_info_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_swarm_info_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_swarm_info_pack(system_id, component_id, &msg , packet1.odom_vaild , packet1.x , packet1.y , packet1.z , packet1.vx , packet1.vy , packet1.vz , packet1.remote_distance );
    mavlink_msg_swarm_info_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_swarm_info_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.odom_vaild , packet1.x , packet1.y , packet1.z , packet1.vx , packet1.vy , packet1.vz , packet1.remote_distance );
    mavlink_msg_swarm_info_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_swarm_info_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_swarm_info_send(MAVLINK_COMM_1 , packet1.odom_vaild , packet1.x , packet1.y , packet1.z , packet1.vx , packet1.vy , packet1.vz , packet1.remote_distance );
    mavlink_msg_swarm_info_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}

static void mavlink_test_swarm(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
    mavlink_test_swarm_info(system_id, component_id, last_msg);
}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // SWARM_TESTSUITE_H
