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
#ifndef _ROCKER_P4_PD_H_
#define _ROCKER_P4_PD_H_


/* MATCH STRUCTS */

struct rocker_p4_port_vlan_mapping_match_spec {
	unsigned char vlan_tag_valid;
	unsigned short vlan_tag_vid;
	unsigned short standard_metadata_ingress_port;
};

struct rocker_p4_rmac_match_spec {
	unsigned char ingress_metadata_rmac_id;
	unsigned char ethernet_da[6];
};

struct rocker_p4_ipv4_hrt_match_spec {
	unsigned short ingress_metadata_vrf;
	unsigned int ipv4_dip;
};

struct rocker_p4_dmac_match_spec {
	unsigned short ingress_metadata_fid;
	unsigned char ethernet_da[6];
};

struct rocker_p4_smac_match_spec {
	unsigned short ingress_metadata_fid;
	unsigned char ethernet_sa[6];
};



/* ACTION STRUCTS */

/* ipv4_hit_ecmp has no parameters */

/* dmac_miss has no parameters */

/* smac_hit has no parameters */

struct rocker_p4_port_vlan_hit_action_spec {
	unsigned int action_fid;
	unsigned int action_vrf;
	unsigned int action_rmac_id;
};

/* rmac_hit has no parameters */

struct rocker_p4_ipv4_hit_nh_action_spec {
	unsigned char action_rmac[6];
	unsigned char action_nh_da[6];
	unsigned int action_nh_port;
};

/* smac_miss has no parameters */

/* ipv4_miss has no parameters */

/* rmac_miss has no parameters */

/* port_vlan_miss has no parameters */

/* ipv4_glean has no parameters */

struct rocker_p4_dmac_hit_action_spec {
	unsigned int action_port;
};



/* ADD ENTRIES */

unsigned int
rocker_p4_port_vlan_mapping_entry_add_port_vlan_hit
(
	struct rocker_p4_port_vlan_mapping_match_spec *match_spec,
	struct rocker_p4_port_vlan_hit_action_spec *action_spec,
	struct rocker_p4_port_vlan_mapping_entry *entry,
	unsigned int *entry_hdl
);

unsigned int
rocker_p4_port_vlan_mapping_entry_add_port_vlan_miss
(
	struct rocker_p4_port_vlan_mapping_match_spec *match_spec,
	struct rocker_p4_port_vlan_mapping_entry *entry,
	unsigned int *entry_hdl
);

unsigned int
rocker_p4_rmac_entry_add_rmac_miss
(
	struct rocker_p4_rmac_match_spec *match_spec,
	struct rocker_p4_rmac_entry *entry,
	unsigned int *entry_hdl
);

unsigned int
rocker_p4_rmac_entry_add_rmac_hit
(
	struct rocker_p4_rmac_match_spec *match_spec,
	struct rocker_p4_rmac_entry *entry,
	unsigned int *entry_hdl
);

unsigned int
rocker_p4_ipv4_hrt_entry_add_ipv4_hit_nh
(
	struct rocker_p4_ipv4_hrt_match_spec *match_spec,
	struct rocker_p4_ipv4_hit_nh_action_spec *action_spec,
	struct rocker_p4_ipv4_hrt_entry *entry,
	unsigned int *entry_hdl
);

unsigned int
rocker_p4_ipv4_hrt_entry_add_ipv4_hit_ecmp
(
	struct rocker_p4_ipv4_hrt_match_spec *match_spec,
	struct rocker_p4_ipv4_hrt_entry *entry,
	unsigned int *entry_hdl
);

unsigned int
rocker_p4_ipv4_hrt_entry_add_ipv4_glean
(
	struct rocker_p4_ipv4_hrt_match_spec *match_spec,
	struct rocker_p4_ipv4_hrt_entry *entry,
	unsigned int *entry_hdl
);

unsigned int
rocker_p4_ipv4_hrt_entry_add_ipv4_miss
(
	struct rocker_p4_ipv4_hrt_match_spec *match_spec,
	struct rocker_p4_ipv4_hrt_entry *entry,
	unsigned int *entry_hdl
);

unsigned int
rocker_p4_dmac_entry_add_dmac_hit
(
	struct rocker_p4_dmac_match_spec *match_spec,
	struct rocker_p4_dmac_hit_action_spec *action_spec,
	struct rocker_p4_dmac_entry *entry,
	unsigned int *entry_hdl
);

unsigned int
rocker_p4_dmac_entry_add_dmac_miss
(
	struct rocker_p4_dmac_match_spec *match_spec,
	struct rocker_p4_dmac_entry *entry,
	unsigned int *entry_hdl
);

unsigned int
rocker_p4_smac_entry_add_smac_hit
(
	struct rocker_p4_smac_match_spec *match_spec,
	struct rocker_p4_smac_entry *entry,
	unsigned int *entry_hdl
);

unsigned int
rocker_p4_smac_entry_add_smac_miss
(
	struct rocker_p4_smac_match_spec *match_spec,
	struct rocker_p4_smac_entry *entry,
	unsigned int *entry_hdl
);


/* SET DEFAULT_ACTION */

unsigned int
rocker_p4_port_vlan_mapping_set_default_action_port_vlan_hit
(
	struct rocker_p4_port_vlan_hit_action_spec *action_spec
);

unsigned int
rocker_p4_port_vlan_mapping_set_default_action_port_vlan_miss
(
	void
);

unsigned int
rocker_p4_rmac_set_default_action_rmac_miss
(
	void
);

unsigned int
rocker_p4_rmac_set_default_action_rmac_hit
(
	void
);

unsigned int
rocker_p4_ipv4_hrt_set_default_action_ipv4_hit_nh
(
	struct rocker_p4_ipv4_hit_nh_action_spec *action_spec
);

unsigned int
rocker_p4_ipv4_hrt_set_default_action_ipv4_hit_ecmp
(
	void
);

unsigned int
rocker_p4_ipv4_hrt_set_default_action_ipv4_glean
(
	void
);

unsigned int
rocker_p4_ipv4_hrt_set_default_action_ipv4_miss
(
	void
);

unsigned int
rocker_p4_dmac_set_default_action_dmac_hit
(
	struct rocker_p4_dmac_hit_action_spec *action_spec
);

unsigned int
rocker_p4_dmac_set_default_action_dmac_miss
(
	void
);

unsigned int
rocker_p4_smac_set_default_action_smac_hit
(
	void
);

unsigned int
rocker_p4_smac_set_default_action_smac_miss
(
	void
);

void rocker_p4_build_key_port_vlan_mapping
(
	unsigned char *key,
	struct rocker_p4_port_vlan_mapping_match_spec *match_spec
);
void rocker_p4_build_key_rmac
(
	unsigned char *key,
	struct rocker_p4_rmac_match_spec *match_spec
);
void rocker_p4_build_key_ipv4_hrt
(
	unsigned char *key,
	struct rocker_p4_ipv4_hrt_match_spec *match_spec
);
void rocker_p4_build_key_dmac
(
	unsigned char *key,
	struct rocker_p4_dmac_match_spec *match_spec
);
void rocker_p4_build_key_smac
(
	unsigned char *key,
	struct rocker_p4_smac_match_spec *match_spec
);
#endif
