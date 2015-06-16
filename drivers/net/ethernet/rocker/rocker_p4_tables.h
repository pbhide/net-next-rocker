/*
 * Rocker P4 switch device driver
 *
 * Copyright (c) 2015 Parag Bhide <parag.bhide@barefootnetworks.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _ROCKER_P4_TABLES_H_
#define _ROCKER_P4_TABLES_H_

#define TABLES_DEFAULT_INIT_SIZE 65536 // entries


struct rocker_p4_port_vlan_mapping_entry {
	unsigned char key[9];
	int action_id;
	unsigned char action_data[12];

};

struct rocker_p4_rmac_entry {
	unsigned char key[10];
	int action_id;
	unsigned char action_data[0];

};

struct rocker_p4_ipv4_hrt_entry {
	unsigned char key[8];
	int action_id;
	unsigned char action_data[16];

};

struct rocker_p4_dmac_entry {
	unsigned char key[10];
	int action_id;
	unsigned char action_data[4];

};

struct rocker_p4_smac_entry {
	unsigned char key[10];
	int action_id;
	unsigned char action_data[0];

};


#endif
