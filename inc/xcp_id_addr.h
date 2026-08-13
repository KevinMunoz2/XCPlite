#pragma once
/* SPDX-License-Identifier: MIT */

//-----------------------------------------------------------------------------------------------
// Layout of the 32 bit address field under identifier (resolve-table) addressing.
//
// The identifier names the object; the low bits are a byte offset into it. Split for the same
// reason segment relative addressing splits it (XcpAddrEncodeSegIndex): a master does arithmetic
// on ECU_ADDRESS and the result has to keep meaning the same object. Element i of an array is
// ECU_ADDRESS + i*elemsize, and an object wider than one ODT entry is armed as chunks at
// ECU_ADDRESS + k*XCP_MAX_ODT_ENTRY_SIZE. With the whole word spent on a dense identifier
// counter, id+k was another object's identifier: such a request sampled an unrelated variable, or
// was refused as out of range, depending only on how many objects the application happened to
// have.
//
// 16/16: 65535 identifiers, 64 KiB per object -- the same ceiling xcplite already imposes on a
// calibration segment.
//
// ONE definition, in a header with no dependencies, because three parties have to agree on it and
// they do not all see the same headers: an application emitting these addresses sees only the
// public inc/xcplib.h, the server decoding them compiles against the private src/xcp_cfg.h, and
// the offline A2L generator (tools/xcpclient) mirrors it in Rust. It used to be written out twice
// behind a shared include guard, which made a double include safe and a divergence silent -- the
// guard meant whichever header was reached first won, so an edit to one copy was discarded rather
// than diagnosed. The Rust mirror still has to be kept by hand; mc-instrument's id_offset test
// asserts these values so the two cannot part company unnoticed.
//-----------------------------------------------------------------------------------------------

#include <stdint.h>

/// Bits of the address field spent on the byte offset into the object.
#define XCP_ID_OFFSET_BITS 16

/// Mask selecting the byte offset.
#define XCP_ID_OFFSET_MASK ((uint32_t)((1u << XCP_ID_OFFSET_BITS) - 1u))

/// The largest identifier the field can hold. Identifier 0 is reserved as invalid.
#define XCP_ID_MAX ((uint32_t)(0xFFFFFFFFu >> XCP_ID_OFFSET_BITS))

/// The most bytes one identifier can address, offset field inclusive.
#define XCP_ID_OBJECT_MAX_BYTES ((uint32_t)XCP_ID_OFFSET_MASK + 1u)

/// Pack an identifier and a byte offset into an ODT entry's address field.
#define XcpAddrEncodeId(id, offset) (uint32_t)((((uint32_t)(id)) << XCP_ID_OFFSET_BITS) | (((uint32_t)(offset)) & XCP_ID_OFFSET_MASK))

/// The identifier an address field names.
#define XcpAddrDecodeId(addr) (uint32_t)(((uint32_t)(addr)) >> XCP_ID_OFFSET_BITS)

/// The byte offset into the object an address field names.
#define XcpAddrDecodeIdOffset(addr) (uint32_t)(((uint32_t)(addr)) & XCP_ID_OFFSET_MASK)
