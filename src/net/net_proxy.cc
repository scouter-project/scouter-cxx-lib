/*
 * Netproxy.cpp
 *
 *  Created on: 2015. 4. 23.
 *      Author: windfree
 */

#include "net/net_proxy.h"
#include <arpa/inet.h>
#include "util/hash_set.h"
#include "data_pack/pack.h"
#include "data_pack/text_pack.h"
#include <cstring>
namespace scouter {

net_proxy* net_proxy::instance = NULL;
static pthread_mutex_t lock_mutex = PTHREAD_MUTEX_INITIALIZER ;

net_proxy* net_proxy::get_instance() {
	if(instance == NULL) {
		pthread_mutex_lock(&lock_mutex);
		instance = new net_proxy();
		pthread_mutex_unlock(&lock_mutex);

	}
	return instance;
}

net_proxy::net_proxy() {
	sender =  udp_sender::get_instance();
	string_hash = new hash_set();
}


net_proxy::~net_proxy() {
	//pthread_mutex_destroy(&mutex);
	//sender->stop();
    if(sender != NULL) { 
        delete sender;
        sender = 0;
    }

    if(string_hash != NULL) {
        delete string_hash;
    }
}

void net_proxy::send_text(const char* text, int32_t text_hash) {
    if(string_hash == NULL) {
        return;
    }
	if(string_hash->count() > text_hash_set_size) {
			return;
	}

	if(text == NULL || (strcmp(text,"")==0)) {
		return;
	}

	if(string_hash->contains(text_hash)) {
		return;
	}
	text_pack* pack = new text_pack("enduser",text_hash,text	);
	if(sender->add(pack)) {
		string_hash->add(text_hash);
	}

}

void net_proxy::send_pack(pack* p) {
    if(sender == NULL) {
        delete p;
        return;
    }
	sender->add(p);
}

char* net_proxy::get_local_addr() {
	return local_addr;

}

bool net_proxy::is_available() {
	return sender->is_running();
}


void net_proxy::clear_hash_set() {
	string_hash->clear();
}

udp_sender* net_proxy::get_sender() {
    return sender;
}

} /* namespace scouter */

