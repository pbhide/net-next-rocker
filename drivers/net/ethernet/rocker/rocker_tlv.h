/*
 * drivers/net/ethernet/rocker/rocker.h - Rocker switch device driver
 * Copyright (c) 2014 Jiri Pirko <jiri@resnulli.us>
 * Copyright (c) 2014 Scott Feldman <sfeldma@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef _ROCKER_TLV_H__
#define _ROCKER_TLV_H__
#include "rocker.h"

#define ROCKER_TLV_ALIGNTO 8U
#define ROCKER_TLV_ALIGN(len) \
	(((len) + ROCKER_TLV_ALIGNTO - 1) & ~(ROCKER_TLV_ALIGNTO - 1))
#define ROCKER_TLV_HDRLEN ROCKER_TLV_ALIGN(sizeof(struct rocker_tlv))

/*  <------- ROCKER_TLV_HDRLEN -------> <--- ROCKER_TLV_ALIGN(payload) --->
 * +-----------------------------+- - -+- - - - - - - - - - - - - - -+- - -+
 * |             Header          | Pad |           Payload           | Pad |
 * |      (struct rocker_tlv)    | ing |                             | ing |
 * +-----------------------------+- - -+- - - - - - - - - - - - - - -+- - -+
 *  <--------------------------- tlv->len -------------------------->
 */
struct rocker_tlv *rocker_tlv_next(const struct rocker_tlv *tlv,
				   int *remaining);
int rocker_tlv_ok(const struct rocker_tlv *tlv, int remaining);
int rocker_tlv_put(struct rocker_desc_info *desc_info,
		   int attrtype, int attrlen, const void *data);
int rocker_tlv_put_u8(struct rocker_desc_info *desc_info,
		      int attrtype, u8 value);
int rocker_tlv_put_u16(struct rocker_desc_info *desc_info,
		       int attrtype, u16 value);
int rocker_tlv_put_be16(struct rocker_desc_info *desc_info,
			int attrtype, __be16 value);
int rocker_tlv_put_u32(struct rocker_desc_info *desc_info,
		       int attrtype, u32 value);
int rocker_tlv_put_be32(struct rocker_desc_info *desc_info,
			int attrtype, __be32 value);
int rocker_tlv_put_u64(struct rocker_desc_info *desc_info,
		       int attrtype, u64 value);
struct rocker_tlv *rocker_tlv_nest_start(struct rocker_desc_info *desc_info,
					 int attrtype);
void rocker_tlv_nest_end(struct rocker_desc_info *desc_info,
			 struct rocker_tlv *start);
void rocker_tlv_nest_cancel(struct rocker_desc_info *desc_info,
			    struct rocker_tlv *start);
struct rocker_tlv *rocker_tlv_start(struct rocker_desc_info *desc_info);

#endif
