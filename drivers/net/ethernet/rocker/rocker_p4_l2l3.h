/*
 * drivers/net/ethernet/rocker/rocker_p4_l2l3.h -
 * Rocker switch device driver for P4 l2l3 device
 *
 * Copyright (c) 2015 Parag Bhide <parag.bhide@barefootnetworks.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef _ROCKER_P4_L2L3_H_
#define _ROCKER_P4_L2L3_H_
/* P4 l2l3 world functions */
int   rocker_p4_l2l3_init(struct rocker_world *w);
void  rocker_p4_l2l3_uninit(struct rocker_world *w);
int   rocker_p4_l2l3_fdb_learn(struct rocker_world *w,
			       struct rocker_port *rocker_port,
			       int flags, const u8 *addr, __be16 vlan_id);
int   rocker_p4_l2l3_port_vlan(struct rocker_world *w,
			       struct rocker_port *rocker_port, int flags,
			       u16 vid);

#endif /*_ROCKER_P4_L2L3_H_ */
