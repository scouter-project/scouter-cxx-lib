/*
 * udpsender.cc
 *
 *  Created on: 2015. 6. 23.
 *      Author: Kim kyungnam
 */
#include "net/udp_sender.h"
#include "net/udp_socket_client.h"
#include "io/data_output.h"
#include <cstddef>
#include <arpa/inet.h>
#include "config/configure.h"
#include <unistd.h>
#include <sstream>
#include "net/net_constants.h"
#ifdef __WITH_TUNA__
#include "config/tuna_conf.h"
#include "util/trace_util.h"
#endif

namespace scouter {

udp_sender* udp_sender::instance = NULL;
pthread_mutex_t udp_sender::mutex ;
udp_sender::udp_sender() {
    net_queue_size = 100;
	thread_id = 0;
	conf = configure::get_instance(); 
	
}


udp_sender::~udp_sender() {
	pthread_mutex_lock(&mutex);
	while(!data_queue.empty()) {
		pack* pk = data_queue.front();
		data_queue.pop();
		delete pk;
	}
	pthread_mutex_unlock(&mutex);
	pthread_mutex_destroy(&mutex);
    /*
	if(udp->is_connected()) {
		udp->close();
	}
    */
	delete udp;
}

udp_sender* udp_sender::get_instance() {
	if(instance == NULL) {
		pthread_mutex_init(&mutex,NULL);
		pthread_mutex_lock(&mutex);
		if(instance == NULL) { //DCL
			instance = new udp_sender();
		}
		pthread_mutex_unlock(&mutex);
	}
	return instance;
}

void udp_sender::prepare(std::string ip, int port) {
	udp = new udp_socket_client();
    running = true;
    if(this->udp != NULL) {
        udp->set_addr(ip,port);
    }
}

void udp_sender::start() {
	/* create thread */
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,	PTHREAD_CREATE_JOINABLE);
	if (pthread_create(&thread_id, &attr, start_thread,(void*)this)!= 0) {
		running = false;
        return;
	}
    running = true;
}

bool udp_sender::add(pack* pack) {
#ifdef __WITH_TUNA__
    net_queue_size = tuna::tuna_conf::get_instance()->get_udp_queue_size();
#endif
	if(data_queue.size()> net_queue_size) {
		delete pack;  // if queue size exceeds net_queue_size, ignore pack.
		return false;
	}
	if(running) {
		pthread_mutex_lock(&mutex);
		data_queue.push(pack);
#ifdef __TUNA_DEBUG___
		tuna::trace_util::get_instance()->print_message("[debug] add pack to udp sernder sending queue");	
#endif
		pthread_mutex_unlock(&mutex);
		return true;
	} else {
		delete pack; // if fails to connect to scouter server, ignore pack.
		return false;
	}
}

void* udp_sender::start_thread(void* p) {
	((udp_sender*)p)->run();
	return NULL;
}

void udp_sender::run() {
    while(running) {
        pthread_mutex_lock(&mutex);
        int queue_size = data_queue.size();
        if(data_queue.size() == 1 ) {
            pack* pk = data_queue.front();
            send(pk);
            data_queue.pop();
            delete pk;
        }else if(data_queue.size() > 1) {
			send(queue_size);
        }
        pthread_mutex_unlock(&mutex);
#ifdef __LINUX__
        usleep(50000);
#else 
        sleep(50);
#endif
    }
}


void udp_sender::stop() {
	running = false;
	if(thread_id != 0) {
		pthread_cancel(thread_id);
		pthread_join(thread_id, NULL);
	}

}


void udp_sender::send(pack* pk) {
#ifdef __TUNA_DEBUG__
	tuna::trace_util::get_instance()->print_message("[debug] udp_sender::send(pack *pk) called");	
#endif
	data_output* out = new data_output();
	char* buffer = out->write_pack(pk)->to_byte_array();
	int len = out->get_offset();
	udp->send(buffer,len);
	delete out;
	delete[] buffer;
}



void udp_sender::send(int pack_count) {
	int32_t size = 0;
    int max_udp_packet_size = configure::MAX_PACKET_SIZE;
#ifdef __WITH_TUNA__
    max_udp_packet_size = tuna::tuna_conf::get_instance()->get_udp_max_packet_size();
#endif
	for(int i = 0; i < pack_count; i++ ) {
		data_output* out = new data_output();
		pack* pk = data_queue.front();
		out->write_pack(pk);
		int buff_size = out->get_offset();
		if(size + buff_size > max_udp_packet_size) {
			udp->send(buffer_vector);
			size = 0;
			buffer_vector.clear();
		}
		size += buff_size;
		buffer_vector.push_back(out);
		data_queue.pop();
		delete pk;

	}
	udp->send(buffer_vector);
	buffer_vector.clear();
}

bool udp_sender::is_running() {
	return running;
}



socket_client* udp_sender::get_socket() {
	return udp;
}


void udp_sender::set_net_queue_size(int n) {
    net_queue_size = n;
}

} /* namespace scouter */
