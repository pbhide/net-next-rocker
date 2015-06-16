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

#include "rocker_p4_rmt.h"
#include "rocker_p4_enums.h"
#include "rocker_p4_tables.h"
#include "rocker_p4_pd.h"

#define BYTE_ROUND_UP(x) ((x + 7) >> 3)


void rocker_p4_build_key_port_vlan_mapping
(
	unsigned char *key,
	struct rocker_p4_port_vlan_mapping_match_spec *match_spec
)
{
	unsigned int tmp32;
	(void) tmp32;

	*(key++) = (match_spec->vlan_tag_valid > 0);

	tmp32 = match_spec->vlan_tag_vid;
	tmp32 = htonl(tmp32);
	*(unsigned int *) key = tmp32;
	key += sizeof(unsigned int);
	tmp32 = match_spec->standard_metadata_ingress_port;
	tmp32 = htonl(tmp32);
	*(unsigned int *) key = tmp32;
	key += sizeof(unsigned int);
}

void rocker_p4_build_key_rmac
(
	unsigned char *key,
	struct rocker_p4_rmac_match_spec *match_spec
)
{
	unsigned int tmp32;
	(void) tmp32;

	tmp32 = match_spec->ingress_metadata_rmac_id;
	tmp32 = htonl(tmp32);
	*(unsigned int *) key = tmp32;
	key += sizeof(unsigned int);
	memcpy(key, match_spec->ethernet_da, 6);
	key += 6;
}

void rocker_p4_build_key_ipv4_hrt
(
	unsigned char *key,
	struct rocker_p4_ipv4_hrt_match_spec *match_spec
)
{
	unsigned int tmp32;
	(void) tmp32;

	tmp32 = match_spec->ingress_metadata_vrf;
	tmp32 = htonl(tmp32);
	*(unsigned int *) key = tmp32;
	key += sizeof(unsigned int);
	tmp32 = match_spec->ipv4_dip;
	tmp32 = htonl(tmp32);
	*(unsigned int *) key = tmp32;
	key += sizeof(unsigned int);
}

void rocker_p4_build_key_dmac
(
	unsigned char *key,
	struct rocker_p4_dmac_match_spec *match_spec
)
{
	unsigned int tmp32;
	(void) tmp32;

	tmp32 = match_spec->ingress_metadata_fid;
	tmp32 = htonl(tmp32);
	*(unsigned int *) key = tmp32;
	key += sizeof(unsigned int);
	memcpy(key, match_spec->ethernet_da, 6);
	key += 6;
}

void rocker_p4_build_key_smac
(
	unsigned char *key,
	struct rocker_p4_smac_match_spec *match_spec
)
{
	unsigned int tmp32;
	(void) tmp32;

	tmp32 = match_spec->ingress_metadata_fid;
	tmp32 = htonl(tmp32);
	*(unsigned int *) key = tmp32;
	key += sizeof(unsigned int);
	memcpy(key, match_spec->ethernet_sa, 6);
	key += 6;
}


static inline void rocker_p4_build_action_spec_port_vlan_hit
(
	unsigned char *data,
	struct rocker_p4_port_vlan_hit_action_spec *action_spec
)
{
	unsigned short tmp16;
	unsigned int tmp32;
	(void) tmp16;
	(void) tmp32;

	tmp32 = action_spec->action_fid;
	tmp32 = htonl(tmp32);
	*(unsigned int *) data = tmp32;
	data += sizeof(unsigned int);


	tmp32 = action_spec->action_vrf;
	tmp32 = htonl(tmp32);
	*(unsigned int *) data = tmp32;
	data += sizeof(unsigned int);


	tmp32 = action_spec->action_rmac_id;
	tmp32 = htonl(tmp32);
	*(unsigned int *) data = tmp32;
	data += sizeof(unsigned int);


}

static inline void rocker_p4_build_action_spec_ipv4_hit_nh
(
	unsigned char *data,
	struct rocker_p4_ipv4_hit_nh_action_spec *action_spec
)
{
	unsigned short tmp16;
	unsigned int tmp32;
	(void) tmp16;
	(void) tmp32;

	memcpy(data, action_spec->action_rmac, 6);
	data += 6;


	memcpy(data, action_spec->action_nh_da, 6);
	data += 6;


	tmp32 = action_spec->action_nh_port;
	tmp32 = htonl(tmp32);
	*(unsigned int *) data = tmp32;
	data += sizeof(unsigned int);


}

static inline void rocker_p4_build_action_spec_dmac_hit
(
	unsigned char *data,
	struct rocker_p4_dmac_hit_action_spec *action_spec
)
{
	unsigned short tmp16;
	unsigned int tmp32;
	(void) tmp16;
	(void) tmp32;

	tmp32 = action_spec->action_port;
	tmp32 = htonl(tmp32);
	*(unsigned int *) data = tmp32;
	data += sizeof(unsigned int);


}


/* ADD ENTRIES */

unsigned int
rocker_p4_port_vlan_mapping_entry_add_port_vlan_hit(
	struct rocker_p4_port_vlan_mapping_match_spec *match_spec,
	struct rocker_p4_port_vlan_hit_action_spec *action_spec,
	struct rocker_p4_port_vlan_mapping_entry *entry,
	unsigned int *entry_hdl
)
{


	entry->action_id = 0;

	rocker_p4_build_key_port_vlan_mapping(
			entry->key,
			match_spec);

	rocker_p4_build_action_spec_port_vlan_hit(entry->action_data,
				action_spec);
	*entry_hdl = 0;
	return 0;
}

unsigned int
rocker_p4_port_vlan_mapping_entry_add_port_vlan_miss(
	struct rocker_p4_port_vlan_mapping_match_spec *match_spec,
	struct rocker_p4_port_vlan_mapping_entry *entry,
	unsigned int *entry_hdl
)
{


	entry->action_id = 1;

	rocker_p4_build_key_port_vlan_mapping(
			entry->key,
			match_spec);

	*entry_hdl = 0;
	return 0;
}

unsigned int
rocker_p4_rmac_entry_add_rmac_miss(
	struct rocker_p4_rmac_match_spec *match_spec,
	struct rocker_p4_rmac_entry *entry,
	unsigned int *entry_hdl
)
{


	entry->action_id = 0;

	rocker_p4_build_key_rmac(
			entry->key,
			match_spec);

	*entry_hdl = 0;
	return 0;
}

unsigned int
rocker_p4_rmac_entry_add_rmac_hit(
	struct rocker_p4_rmac_match_spec *match_spec,
	struct rocker_p4_rmac_entry *entry,
	unsigned int *entry_hdl
)
{


	entry->action_id = 1;

	rocker_p4_build_key_rmac(
			entry->key,
			match_spec);

	*entry_hdl = 0;
	return 0;
}

unsigned int
rocker_p4_ipv4_hrt_entry_add_ipv4_hit_nh(
	struct rocker_p4_ipv4_hrt_match_spec *match_spec,
	struct rocker_p4_ipv4_hit_nh_action_spec *action_spec,
	struct rocker_p4_ipv4_hrt_entry *entry,
	unsigned int *entry_hdl
)
{


	entry->action_id = 0;

	rocker_p4_build_key_ipv4_hrt(
			entry->key,
			match_spec);

	rocker_p4_build_action_spec_ipv4_hit_nh(entry->action_data,
				action_spec);
	*entry_hdl = 0;
	return 0;
}

unsigned int
rocker_p4_ipv4_hrt_entry_add_ipv4_hit_ecmp(
	struct rocker_p4_ipv4_hrt_match_spec *match_spec,
	struct rocker_p4_ipv4_hrt_entry *entry,
	unsigned int *entry_hdl
)
{


	entry->action_id = 1;

	rocker_p4_build_key_ipv4_hrt(
			entry->key,
			match_spec);

	*entry_hdl = 0;
	return 0;
}

unsigned int
rocker_p4_ipv4_hrt_entry_add_ipv4_glean(
	struct rocker_p4_ipv4_hrt_match_spec *match_spec,
	struct rocker_p4_ipv4_hrt_entry *entry,
	unsigned int *entry_hdl
)
{


	entry->action_id = 2;

	rocker_p4_build_key_ipv4_hrt(
			entry->key,
			match_spec);

	*entry_hdl = 0;
	return 0;
}

unsigned int
rocker_p4_ipv4_hrt_entry_add_ipv4_miss(
	struct rocker_p4_ipv4_hrt_match_spec *match_spec,
	struct rocker_p4_ipv4_hrt_entry *entry,
	unsigned int *entry_hdl
)
{


	entry->action_id = 3;

	rocker_p4_build_key_ipv4_hrt(
			entry->key,
			match_spec);

	*entry_hdl = 0;
	return 0;
}

unsigned int
rocker_p4_dmac_entry_add_dmac_hit(
	struct rocker_p4_dmac_match_spec *match_spec,
	struct rocker_p4_dmac_hit_action_spec *action_spec,
	struct rocker_p4_dmac_entry *entry,
	unsigned int *entry_hdl
)
{


	entry->action_id = 0;

	rocker_p4_build_key_dmac(
			entry->key,
			match_spec);

	rocker_p4_build_action_spec_dmac_hit(entry->action_data,
				action_spec);
	*entry_hdl = 0;
	return 0;
}

unsigned int
rocker_p4_dmac_entry_add_dmac_miss(
	struct rocker_p4_dmac_match_spec *match_spec,
	struct rocker_p4_dmac_entry *entry,
	unsigned int *entry_hdl
)
{


	entry->action_id = 1;

	rocker_p4_build_key_dmac(
			entry->key,
			match_spec);

	*entry_hdl = 0;
	return 0;
}

unsigned int
rocker_p4_smac_entry_add_smac_hit(
	struct rocker_p4_smac_match_spec *match_spec,
	struct rocker_p4_smac_entry *entry,
	unsigned int *entry_hdl
)
{


	entry->action_id = 0;

	rocker_p4_build_key_smac(
			entry->key,
			match_spec);

	*entry_hdl = 0;
	return 0;
}

unsigned int
rocker_p4_smac_entry_add_smac_miss(
	struct rocker_p4_smac_match_spec *match_spec,
	struct rocker_p4_smac_entry *entry,
	unsigned int *entry_hdl
)
{


	entry->action_id = 1;

	rocker_p4_build_key_smac(
			entry->key,
			match_spec);

	*entry_hdl = 0;
	return 0;
}


/* SET DEFAULT_ACTION */

unsigned int
rocker_p4_port_vlan_mapping_set_default_action_port_vlan_hit
(
	struct rocker_p4_port_vlan_hit_action_spec *action_spec
)
{
	unsigned char _data[12];
	rocker_p4_build_action_spec_port_vlan_hit(_data, action_spec);

	return 0;
}
unsigned int
rocker_p4_port_vlan_mapping_set_default_action_port_vlan_miss
(
	void
)
{

	return 1;
}
unsigned int
rocker_p4_rmac_set_default_action_rmac_miss
(
	void
)
{

	return 0;
}
unsigned int
rocker_p4_rmac_set_default_action_rmac_hit
(
	void
)
{

	return 1;
}
unsigned int
rocker_p4_ipv4_hrt_set_default_action_ipv4_hit_nh
(
	struct rocker_p4_ipv4_hit_nh_action_spec *action_spec
)
{
	unsigned char _data[16];
	rocker_p4_build_action_spec_ipv4_hit_nh(_data, action_spec);

	return 0;
}
unsigned int
rocker_p4_ipv4_hrt_set_default_action_ipv4_hit_ecmp
(
	void
)
{

	return 1;
}
unsigned int
rocker_p4_ipv4_hrt_set_default_action_ipv4_glean
(
	void
)
{

	return 2;
}
unsigned int
rocker_p4_ipv4_hrt_set_default_action_ipv4_miss
(
	void
)
{

	return 3;
}
unsigned int
rocker_p4_dmac_set_default_action_dmac_hit
(
	struct rocker_p4_dmac_hit_action_spec *action_spec
)
{
	unsigned char _data[4];
	rocker_p4_build_action_spec_dmac_hit(_data, action_spec);

	return 0;
}
unsigned int
rocker_p4_dmac_set_default_action_dmac_miss
(
	void
)
{

	return 1;
}
unsigned int
rocker_p4_smac_set_default_action_smac_hit
(
	void
)
{

	return 0;
}
unsigned int
rocker_p4_smac_set_default_action_smac_miss
(
	void
)
{

	return 1;
}




