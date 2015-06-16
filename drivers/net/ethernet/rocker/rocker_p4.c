/*
 * Rocker switch device driver for rocker_p4 program
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
#include "rocker_p4.h"
#include "rocker_p4_enums.h"
#include "rocker_p4_tables.h"
#include "rocker_p4_pd.h"

int
rocker_p4_init(struct rocker_world *w)
{
	/* set default actions for all tables in this world */
	struct rocker_p4_port_vlan_mapping_entry	vlan_entry;
	struct rocker_p4_dmac_entry			dmac_entry;
	struct rocker_p4_smac_entry			smac_entry;
	struct rocker_p4_rmac_entry			rmac_entry;

	memset(&vlan_entry, 0, sizeof(vlan_entry));
	vlan_entry.action_id =
		rocker_p4_port_vlan_mapping_set_default_action_port_vlan_miss();

	rocker_p4_table_write_request(w, NULL,
				      RMT_TABLE_PORT_VLAN_MAPPING,
				      ROCKER_TLV_CMD_P4_TABLE_DEFAULT_ACTION,
				      &vlan_entry, sizeof(vlan_entry), false);

	memset(&dmac_entry, 0, sizeof(dmac_entry));
	dmac_entry.action_id = 
			rocker_p4_dmac_set_default_action_dmac_miss();
	rocker_p4_table_write_request(w, NULL,
				      RMT_TABLE_DMAC,
				      ROCKER_TLV_CMD_P4_TABLE_DEFAULT_ACTION,
				      &dmac_entry, sizeof(dmac_entry), false);

	printk("Add default action for dmac table %d\n", dmac_entry.action_id);

	memset(&smac_entry, 0, sizeof(smac_entry));
	smac_entry.action_id = rocker_p4_smac_set_default_action_smac_miss();
	rocker_p4_table_write_request(w, NULL,
				      RMT_TABLE_SMAC,
				      ROCKER_TLV_CMD_P4_TABLE_DEFAULT_ACTION,
				      &smac_entry, sizeof(smac_entry), false);

	memset(&rmac_entry, 0, sizeof(rmac_entry));
	rmac_entry.action_id = 
			rocker_p4_rmac_set_default_action_rmac_miss();

	rocker_p4_table_write_request(w, NULL,
				      RMT_TABLE_RMAC,
				      ROCKER_TLV_CMD_P4_TABLE_DEFAULT_ACTION,
				      &rmac_entry, sizeof(rmac_entry), false);
	return 0;
}

void
rocker_p4_uninit(struct rocker_world *w)
{
}

int rocker_p4_port_vlan(struct rocker_world *w,
			struct rocker_port *rocker_port, int flags,
			u16 vid)
{
	bool nowait = flags & ROCKER_OP_FLAG_NOWAIT;
	bool untagged = false;
	struct rocker_p4_port_vlan_mapping_match_spec	match_spec;
	struct rocker_p4_port_vlan_hit_action_spec	action_spec;
	struct rocker_p4_port_vlan_mapping_entry	entry;
	unsigned int entry_hdl;

	__be16 vlan = rocker_port_vid_to_vlan(rocker_port, vid, &untagged);
	dev_info(&rocker_port->rocker->pdev->dev,
		 "%s: vid %d, int_vlan %d, port %d, flags 0x%x\n",
		 __FUNCTION__,
		 (int)vid, (int)vlan, rocker_port->pport, flags);

	match_spec.vlan_tag_valid = !untagged;
	match_spec.vlan_tag_vid = untagged ? 0 : ntohs(vlan);
	match_spec.standard_metadata_ingress_port = rocker_port->pport;
	memset(&entry, 0, sizeof(entry));

	if (flags & ROCKER_OP_FLAG_REMOVE) {
		/* XXX - */
		rocker_p4_build_key_port_vlan_mapping(entry.key, &match_spec);
		dev_info(&rocker_port->rocker->pdev->dev,
			 "Delete : P4 port %d vlan %d\n",
			 rocker_port->pport,
			 (int)match_spec.vlan_tag_vid);

		rocker_p4_table_write_request(w, rocker_port,
					      RMT_TABLE_PORT_VLAN_MAPPING,
					      ROCKER_TLV_CMD_P4_TABLE_DEL,
					      &entry,
					      sizeof(entry), nowait);

	} else {
		action_spec.action_fid = ntohs(vlan);
		action_spec.action_vrf = 0; // XXX
		action_spec.action_rmac_id = 0; // XXX
		rocker_p4_port_vlan_mapping_entry_add_port_vlan_hit(
						&match_spec, &action_spec,
						&entry, &entry_hdl);

		dev_info(&rocker_port->rocker->pdev->dev,
			 "ADD : P4 port %d vlan %d fid %d\n",
			 rocker_port->pport,
			 (int)match_spec.vlan_tag_vid,
			 (int)action_spec.action_fid);

		rocker_p4_table_write_request(w, rocker_port,
					      RMT_TABLE_PORT_VLAN_MAPPING,
					      ROCKER_TLV_CMD_P4_TABLE_ADD,
					      &entry,
					      sizeof(entry), nowait);
	}
	return 0;
}

int rocker_p4_fdb_learn(struct rocker_world *w,
		        struct rocker_port *rocker_port,
			int flags, const u8 *addr, __be16 vlan_id)
{
	bool	nowait = flags & ROCKER_OP_FLAG_NOWAIT;
	int	ret = 0;
	unsigned int entry_hdl;
	/* dmac */
	struct rocker_p4_dmac_hit_action_spec	dmac_action_spec;
	struct rocker_p4_dmac_match_spec	dmac_match_spec;
	struct rocker_p4_dmac_entry		dmac_entry;
	/* smac */
	struct rocker_p4_smac_match_spec	smac_match_spec;
	struct rocker_p4_smac_entry		smac_entry;

	dmac_match_spec.ingress_metadata_fid = ntohs(vlan_id);
	memcpy(dmac_match_spec.ethernet_da, addr, 6);
	dmac_action_spec.action_port = rocker_port->pport;

	smac_match_spec.ingress_metadata_fid = ntohs(vlan_id);
	memcpy(smac_match_spec.ethernet_sa, addr, 6);

	if (flags & ROCKER_OP_FLAG_REFRESH) {
		/* TBD - requires to retrieve the entry_handle */
	} else if (flags & ROCKER_OP_FLAG_REMOVE) {
		/* TBD - requires to retrieve the entry_handle */
		dev_info(&rocker_port->rocker->pdev->dev,
			 "Del fdb entry on port %d, flags 0x%x\n", 
			 rocker_port->pport,
			 flags);

		memset(&dmac_entry, 0, sizeof(dmac_entry));
		rocker_p4_build_key_dmac(dmac_entry.key, &dmac_match_spec);
		rocker_p4_table_write_request(w, rocker_port,
					      RMT_TABLE_DMAC,
					      ROCKER_TLV_CMD_P4_TABLE_DEL,
					      &dmac_entry, sizeof(dmac_entry),
					      nowait);
		memset(&smac_entry, 0, sizeof(smac_entry));
		rocker_p4_build_key_smac(smac_entry.key, &smac_match_spec);
		rocker_p4_table_write_request(w, rocker_port,
					      RMT_TABLE_SMAC,
					      ROCKER_TLV_CMD_P4_TABLE_DEL,
					      &smac_entry,
					      sizeof(smac_entry), nowait);
	} else {
		/* ADD or LEARN */
		dev_info(&rocker_port->rocker->pdev->dev,
			 "Add fdb entry on port %d, flags 0x%x\n", 
			 rocker_port->pport,
			 flags);

		rocker_p4_dmac_entry_add_dmac_hit(&dmac_match_spec,
						  &dmac_action_spec,
						  &dmac_entry, &entry_hdl);

		/* XXX need to save the entry_handle for future ops */
		rocker_p4_table_write_request(w, rocker_port,
					      RMT_TABLE_DMAC,
					      ROCKER_TLV_CMD_P4_TABLE_ADD,
					      &dmac_entry, sizeof(dmac_entry),
					      nowait);

		rocker_p4_smac_entry_add_smac_hit(&smac_match_spec,
						  &smac_entry, &entry_hdl);

		/* XXX need to save the entry_handle for future ops */
		rocker_p4_table_write_request(w, rocker_port,
					      RMT_TABLE_SMAC,
					      ROCKER_TLV_CMD_P4_TABLE_ADD,
					      &smac_entry,
					      sizeof(smac_entry), nowait);
		rocker_send_mac_learn_notification_to_bridge(w, rocker_port,
					flags, addr, vlan_id);
	}
	return ret;
}
