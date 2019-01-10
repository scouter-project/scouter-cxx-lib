/*
 * configure.cc
 *
 *  Created on: 2015. 6. 23.
 *      Author: windfree
 */

#include "config/configure.h"
#include <cstdlib>
#include <iostream>
namespace scouter {
configure* configure::instance;
pthread_mutex_t configure::mutex;
int32_t configure::MAX_PACKET_SIZE = 60000;
configure::configure() {
	this->debug_text = false;
	this->temp = 1;
	this->is_lendian = check_lendian();
}
configure::~configure() {
	if(instance != NULL) {
		pthread_mutex_destroy(&mutex);
	}
}

configure* configure::get_instance() {
	if(instance == NULL) {
		pthread_mutex_init(&mutex,NULL);
		pthread_mutex_lock(&mutex);
		instance = new configure();
		pthread_mutex_unlock(&mutex);
	}
	return instance;
}

bool configure::check_lendian() {
	if(((int)((char*)(&this->temp))[0]) == 0) {
		return false;
	}
	else {
		return true;
	}
}

} /* namespace scouter */
