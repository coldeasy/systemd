/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

#pragma once

/***
  This file is part of systemd.

  Copyright (C) 2013 Intel Corporation. All rights reserved.

  systemd is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  systemd is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with systemd; If not, see <http://www.gnu.org/licenses/>.
***/

#include <stdint.h>

#include "dhcp-protocol.h"

int dhcp_network_send_raw_packet(int index, const void *packet, size_t len);

int dhcp_option_append(uint8_t **buf, size_t *buflen, uint8_t code,
                       size_t optlen, const void *optval);

typedef int (*dhcp_option_cb_t)(uint8_t code, uint8_t len,
                                const uint8_t *option, void *user_data);
int dhcp_option_parse(DHCPMessage *message, size_t len,
                      dhcp_option_cb_t cb, void *user_data);