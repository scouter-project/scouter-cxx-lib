/*
 * util.h
 *
 *  Created on: 2015. 4. 8.
 *      Author: windfree
 */

#ifndef PLUGIN_SPOTTER_UTILITY_H_
#define PLUGIN_SPOTTER_UTILITY_H_
#include <arpa/inet.h>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <sys/time.h>
#include <string>
using namespace std;
namespace scouter {

class util {
	static uint32_t hash_table[];
	static int64_t next_key;
public:
	virtual ~util();
	static uint64_t htonll2(uint64_t val);
	static uint64_t ntohll2(uint64_t val);
	static int32_t hash(const char* str);
	static uint64_t strtouint64(const char* str);
	static uint64_t get_current_time();
	static std::string to_string(int64_t num);
	static uint64_t get_seconds(const char* time, const char* format);
	static bool sleep(int32_t sec);
	static int64_t get_next_key();
    static void to_int(const char* value, int* dest);
    static int str2int(const string& str);
    template<typename T>
    static string num_to_string(T value) {
        std::ostringstream ss;
        ss.clear();
        ss << value;
        return ss.str();
    }
    static uint64_t get_file_size(string& file_name);
    static bool str_to_bool(string& value);


};

} /* namespace scouter */

#endif /* PLUGIN_SPOTTER_UTILITY_H_ */
