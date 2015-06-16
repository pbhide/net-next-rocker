/*
 * drivers/net/ethernet/rocker/rocker_p4_l2l3.c
 * Rocker switch device driver for P4 l2l3 device
 *
 * Copyright (c) 2015 Parag Bhide <parag.bhide@barefootnetworks.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/hashtable.h>
#include <linux/crc32.h>
#include <linux/sort.h>
#include <linux/random.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/skbuff.h>
#include <linux/socket.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <linux/if_bridge.h>
#include <linux/bitops.h>
#include <linux/ctype.h>
#include <net/switchdev.h>
#include <net/rtnetlink.h>
#include <net/ip_fib.h>
#include <net/netevent.h>
#include <net/arp.h>
#include <asm-generic/io-64-nonatomic-lo-hi.h>
#include <generated/utsrelease.h>
#include "rocker.h"
#include "rocker_tlv.h"
#include "rocker_p4_rmt.h"
#include "rocker_p4_l2l3_enums.h"
#include "rocker_p4_l2l3_tables.h"
#include "rocker_p4_l2l3_pd.h"

int
rocker_p4_l2l3_init(struct rocker_world *w)
{
	/* set default actions for all tables in this world */
	struct rocker_p4_l2l3_port_vlan_mapping_entry_s	entry;
	struct rocker_p4_l2l3_dmac_entry_s		entry1;
	struct rocker_p4_l2l3_smac_entry_s		entry2;

	memset(&entry, 0, sizeof(entry));
	entry.action_id =
	  rocker_p4_l2l3_port_vlan_mapping_set_default_action_port_vlan_miss();

	rocker_p4_table_write_request(w, NULL,
				      RMT_TABLE_port_vlan_mapping,
				      ROCKER_TLV_CMD_P4_TABLE_DEFAULT_ACTION,
				      &entry, sizeof(entry), false);

	memset(&entry1, 0, sizeof(entry1));
	entry1.action_id = rocker_p4_l2l3_dmac_set_default_action_dmac_miss();
	rocker_p4_table_write_request(w, NULL,
				      RMT_TABLE_dmac,
				      ROCKER_TLV_CMD_P4_TABLE_DEFAULT_ACTION,
				      &entry1, sizeof(entry1), false);
	printk("Add default action for dmac table %d\n", entry1.action_id);

	memset(&entry2, 0, sizeof(entry2));
	entry2.action_id = rocker_p4_l2l3_smac_set_default_action_smac_miss();
	rocker_p4_table_write_request(w, NULL,
				      RMT_TABLE_smac,
				      ROCKER_TLV_CMD_P4_TABLE_DEFAULT_ACTION,
				      &entry2, sizeof(entry2), false);
	return 0;
}

void
rocker_p4_l2l3_uninit(struct rocker_world *w)
{
}

int rocker_p4_l2l3_port_vlan(struct rocker_world *w,
			     struct rocker_port *rocker_port, int flags,
			     u16 vid)
{
	bool nowait = flags & ROCKER_OP_FLAG_NOWAIT;
	bool untagged = false;
	struct rocker_p4_l2l3_port_vlan_mapping_match_spec	match_spec;
	struct rocker_p4_l2l3_port_vlan_hit_action_spec	 action_spec;
	struct rocker_p4_l2l3_port_vlan_mapping_entry_s	 entry;
	rocker_p4_entry_hdl_t entry_hdl;

	__be16 vlan = rocker_port_vid_to_vlan(rocker_port, vid, &untagged);
	dev_info(&rocker_port->rocker->pdev->dev,
		 "l2l3_port_vlan: vid %d, int_vlan %d, port %d, flags 0x%x\n",
		 (int)vid, (int)vlan, rocker_port->pport, flags);

	if (flags & ROCKER_OP_FLAG_REMOVE) {
		/* XXX - */
	} else {
		match_spec.vlan_tag__valid = !untagged;
		match_spec.vlan_tag__vid = untagged ? 0 : ntohs(vlan);
		match_spec.standard_metadata_ingress_port = rocker_port->pport;
		action_spec.action_fid = ntohs(vlan);
		rocker_p4_l2l3_port_vlan_mapping_table_entry_create_with_port_vlan_hit(&match_spec, &action_spec, &entry, &entry_hdl);
		dev_info(&rocker_port->rocker->pdev->dev,
			 "P4 port %d vlan %d fid %d\n", rocker_port->pport,
			 (int)match_spec.vlan_tag__vid,
			 (int)action_spec.action_fid);

		rocker_p4_table_write_request(w, rocker_port,
					      RMT_TABLE_port_vlan_mapping,
					      ROCKER_TLV_CMD_P4_TABLE_ADD,
					      &entry, sizeof(entry), nowait);
	}
	return 0;
}

int rocker_p4_l2l3_fdb_learn(struct rocker_world *w,
			     struct rocker_port *rocker_port,
			     int flags, const u8 *addr, __be16 vlan_id)
{
	bool	nowait = flags & ROCKER_OP_FLAG_NOWAIT;
	int	ret = 0;
	rocker_p4_entry_hdl_t entry_hdl;

	printk("Add fdb entry on port %d, flags 0x%x\n", rocker_port->pport,
	       flags);

	if (flags & ROCKER_OP_FLAG_REFRESH) {
		/* TBD - requires to retrieve the entry_handle */
	} else if (flags & ROCKER_OP_FLAG_REMOVE) {
		/* TBD - requires to retrieve the entry_handle */
	} else {
		/* ADD or LEARN */
		{
		/* dmac */
		struct rocker_p4_l2l3_dmac_hit_action_spec	action_spec;
		struct rocker_p4_l2l3_dmac_match_spec		match_spec;
		struct rocker_p4_l2l3_dmac_entry_s		entry;

		match_spec.ingress_metadata_fid = ntohs(vlan_id);
		memcpy(match_spec.ethernet_dstAddr, addr, 6);
		action_spec.action_port = rocker_port->pport;

		rocker_p4_l2l3_dmac_table_entry_create_with_dmac_hit(
						&match_spec, &action_spec,
						&entry, &entry_hdl);
		/* XXX need to save the entry_handle for future ops */
		rocker_p4_table_write_request(w, rocker_port,
					      RMT_TABLE_dmac,
					      ROCKER_TLV_CMD_P4_TABLE_ADD,
					      &entry, sizeof(entry), nowait);
		}
		{
		/* smac */
		struct rocker_p4_l2l3_smac_match_spec	match_spec;
		struct rocker_p4_l2l3_smac_entry_s	entry;

		match_spec.ingress_metadata_fid = ntohs(vlan_id);
		memcpy(match_spec.ethernet_srcAddr, addr, 6);

		rocker_p4_l2l3_smac_table_entry_create_with_smac_hit(
						&match_spec,
						&entry, &entry_hdl);
		/* XXX need to save the entry_handle for future ops */
		rocker_p4_table_write_request(w, rocker_port,
					      RMT_TABLE_smac,
					      ROCKER_TLV_CMD_P4_TABLE_ADD,
					      &entry, sizeof(entry), nowait);
		}
		rocker_send_mac_learn_notification_to_bridge(w, rocker_port,
					flags, addr, vlan_id);
	}
	return ret;
}
