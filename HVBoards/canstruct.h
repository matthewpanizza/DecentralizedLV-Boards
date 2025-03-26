#pragma once

// CAN_STRUCT is created by the DBC code generation tool.
struct CAN_STRUCT {
	virtual int pack(uint8_t* dst_p, size_t size) = 0;
	virtual int unpack(const uint8_t* dst_p, size_t size) = 0;
};