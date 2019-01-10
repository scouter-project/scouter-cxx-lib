/*
 * data_output.h
 *
 *  Created on: 2015. 4. 8.
 *      Author: windfree
 */

#ifndef PLUGIN_MONM_IO_DATA_OUTPUT_H_
#define PLUGIN_MONM_IO_DATA_OUTPUT_H_
#include <stdint.h>
#include "config/configure.h"
namespace scouter {
class pack;
class value;
class util;
class data_output {
private:
	char* buffer;
	int buffer_length;
	int offset;
	void check(int size);
	configure* conf;

public:
	data_output();
	data_output(int size);
	virtual ~data_output();
	void write_decimal(int64_t value);
	void write_uint32(uint32_t value);
	void write_uint64 (uint64_t value);
	void write_int32(int32_t value);
	void write_int64(int64_t value);
	void write_bytes(const char* value, int size);
	void write_uint16(uint16_t value);
	void write_int16(int16_t value);
	void write_string(const char* value);
	void write_uint8(uint8_t bytes);
	void write_int8(int8_t  bytes);
	void write_value( value* value);
	void write_blob(const char* bytes, int size);

	char* to_byte_array();
	data_output* write_pack(pack* pack);
	int get_offset();
};

} /* namespace scouter */

#endif /* PLUGIN_MONM_IO_DATA_OUTPUT_H_ */
