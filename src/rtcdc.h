// rtcdc.h
// Copyright (c) 2015 Xiaohan Song <chef@dark.kitchen>
// This file is licensed under a GNU GPLv3 license.

#ifndef _RTCDC_H_
#define _RTCDC_H_

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef RTCDC_MAX_CHANNEL_NUM
#define RTCDC_MAX_CHANNEL_NUM 32
#endif

#define RTCDC_ROLE_UNKNOWN 0
#define RTCDC_ROLE_CLIENT  1
#define RTCDC_ROLE_SERVER  2

struct dtls_transport;
struct sctp_transport;
struct rtcdc_data_channel;

typedef void (*rtcdc_on_message_cb)(struct rtcdc_data_channel *channel,
                                    int datatype, void *data, size_t len, void *user_data);

typedef void (*rtcdc_on_channel_cb)(struct rtcdc_data_channel *channel);

struct rtcdc_data_channel {
  uint8_t type;
  uint16_t priority;
  uint32_t rtx;
  uint32_t lifetime;
  char *label;
  char *protocol;
  int state;
  uint16_t sid;
  rtcdc_on_message_cb on_message;
  void *user_data;
};

struct rtcdc_transport {
  int role;
  struct dtls_context *ctx;
  struct ice_transport *ice;
  struct dtls_transport *dtls;
  struct sctp_transport *sctp;
};

struct rtcdc_peer_connection {
  struct rtcdc_transport *transport;
  struct rtcdc_data_channel *channels[RTCDC_MAX_CHANNEL_NUM];
  rtcdc_on_channel_cb on_channel;
};

struct rtcdc_peer_connection *
rtcdc_create_peer_connection(rtcdc_on_channel_cb);

void
rtcdc_destroy_peer_connection(struct rtcdc_peer_connection *peer);

char *
rtcdc_generate_offer_sdp(struct rtcdc_peer_connection *peer);

int
rtcdc_parse_offer_sdp(struct rtcdc_peer_connection *peer, const char *offer);

char *
rtcdc_generate_candidate_sdp(struct rtcdc_peer_connection *peer);

int
rtcdc_parse_candidate_sdp(struct rtcdc_peer_connection *peer, const char *candidates);

struct rtcdc_data_channel *
rtcdc_create_reliable_ordered_channel(struct rtcdc_peer_connection *peer,
                                      const char *label, const char *protocol,
                                      rtcdc_on_message_cb, void *user_data);

void
rtcdc_destroy_data_channel(struct rtcdc_data_channel *channel);

int
rtcdc_send_message(struct rtcdc_data_channel *channel, int datatype, void *data, size_t len);

void
rtcdc_loop(struct rtcdc_peer_connection *peer);

#ifdef  __cplusplus
}
#endif

#endif // _RTCDC_H_
