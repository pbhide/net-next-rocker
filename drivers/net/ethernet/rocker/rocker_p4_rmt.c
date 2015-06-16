/*
 * drivers/net/ethernet/rocker/rocker_p4.c -
 * Rocker switch device driver for P4
 *
 * Copyright (c) 2015 Parag Bhide <parag.bhide@barefootnetworks.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "rocker_p4_rmt.h"

/* Functions in this file are not P4 "program-specific", they can be
 * shared between multiple different P4 programs.
 */
struct rocker_p4_table_request {
	int			table_id;
	unsigned short		opcode;
	void			*entry;
	int			entry_size;
	struct rocker_world	*world;
};

int
static rocker_p4_table_request_send(struct rocker *rocker,
				    struct rocker_port *rocker_port,
				    struct rocker_desc_info *desc_info,
				    void *priv)
{
	struct rocker_tlv *cmd_info;
	struct rocker_p4_table_request *req =
			(struct rocker_p4_table_request *)priv;

	printk("rocker p4 table request send for table %d\n", req->table_id);

	if (rocker_tlv_put_u16(desc_info, ROCKER_TLV_CMD_TYPE, req->opcode))
		return -EMSGSIZE;
	if (rocker_tlv_put_u32(desc_info, ROCKER_TLV_CMD_WORLD,
			       req->world->world_id))
		return -EMSGSIZE;
	cmd_info = rocker_tlv_nest_start(desc_info, ROCKER_TLV_CMD_INFO);
	if (!cmd_info)
		return -EMSGSIZE;
	if (rocker_tlv_put_u32(desc_info, ROCKER_TLV_P4_RMT_INFO_TABLE_ID,
			       req->table_id))
		return -EMSGSIZE;
	if (rocker_tlv_put(desc_info, ROCKER_TLV_P4_RMT_INFO_TABLE_ENTRY,
			   req->entry_size, req->entry))
		return -EMSGSIZE;
	rocker_tlv_nest_end(desc_info, cmd_info);
	return 0;
}

int
rocker_p4_table_write_request(struct rocker_world *w,
			      struct rocker_port *rocker_port,
			      int table_id, int op,
			      void *entry, int entry_size, bool nowait)
{
	struct rocker_p4_table_request req;

	req.table_id = table_id;
	req.opcode = op;
	req.entry = entry;
	req.entry_size = entry_size;
	req.world = w;

	return rocker_cmd_exec(w->rocker, rocker_port,
			       rocker_p4_table_request_send, &req,
			       NULL, NULL,
			       nowait);
}
