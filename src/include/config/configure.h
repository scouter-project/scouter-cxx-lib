/*
 * configure.h
 *
 *  Created on: 2015. 6. 23.
 *      Author: windfree
 */

#ifndef SRC_SCOUTER_ENV_H_
#define SRC_SCOUTER_ENV_H_

#include <pthread.h>
#include <stdint.h>
namespace scouter {

class configure {
private:
	static configure* instance;
	static pthread_mutex_t mutex;
	configure();
	bool check_lendian();
	int temp;

public:
	static configure* get_instance();
	virtual ~configure();
	static int32_t MAX_PACKET_SIZE;
	bool debug_text ;
	bool is_lendian;

};

} /* namespace scouter */

#endif /* SRC_CONFIG_CONFIGURE_H_ */
