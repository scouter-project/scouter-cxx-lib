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
         send_multi_pack(queue_size);
        }
        pthread_mutex_unlock(&mutex);
        sleep(1);
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
	data_output* out = new data_output();
	out->write_bytes(net_constants::SINGLE_PACK,4);
	char* buffer = out->write_pack(pk)->to_byte_array();
	int len = out->get_offset();
	udp->send(buffer,len);
	delete out;
	delete[] buffer;
}


void udp_sender::send(std::vector<data_output*>& outs) {
	int size = outs.size();
	if(size == 0) {
		return;
	} else if(size == 1) { //multi 로 보내려고 그랬으나 하나의 pack 이  udp max size 를 초과했을 때
		data_output* out = new data_output();
		out->write_bytes(net_constants::SINGLE_PACK,4);
		data_output* temp = outs[0];
		char* buffer = temp->to_byte_array();
		out->write_bytes(buffer,temp->get_offset());
		char* send_buffer = out->to_byte_array();
		udp->send(send_buffer,out->get_offset());
		delete out;
		out = 0;
		delete[] send_buffer;
		delete[] buffer;
		delete temp;
		temp = 0;
	} else if(size > 1){
		data_output* out = new data_output();
		out->write_bytes(net_constants::MULTI_PACK,4);
		out->write_int16(size);
		for(int i = 0; i < size; i++) {
			data_output* temp_out = outs[i];
			char* buffer = temp_out->to_byte_array();
			out->write_bytes(buffer,temp_out->get_offset()	);
			delete temp_out;
			temp_out = 0;
			delete[] buffer;
		}
		char* send_buffer = out->to_byte_array();
		udp->send(send_buffer,out->get_offset());
		delete[] send_buffer;
		delete out;
		out = 0;
	}

}


void udp_sender::send_multi_pack(int pack_count) {
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
			send(buffer_vector);
			size = 0;
			buffer_vector.clear();
		}
		size += buff_size;
		buffer_vector.push_back(out);
		data_queue.pop();
		delete pk;

	}
	send(buffer_vector);
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
