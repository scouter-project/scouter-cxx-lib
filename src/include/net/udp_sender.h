/*
 * udpsender.h
 *
 *  Created on: 2015. 6. 23.
 *      Author: windfree
 */

#ifndef SRC_UDPSENDER_H_
#define SRC_UDPSENDER_H_

#include <pthread.h>
#include <vector>
#include <queue>
#include <string>
#include "data_pack/pack.h"
#include "config/configure.h"
namespace scouter {
class data_output;
//class pack;
class udp_socket_client;
class socket_client;
class udp_sender {
private :
	static udp_sender* instance;
	static pthread_mutex_t mutex;
	udp_sender();
	udp_socket_client* udp;
	pthread_t thread_id;
	std::vector<data_output*> buffer_vector ;
	std::queue<pack*> data_queue;
	static void* start_thread(void* p);
    uint32_t net_queue_size;
	void run();
	bool running;

	//void send(std::vector<data_output*> outs);
	void send(int pack_count);
	configure* conf;

public:
	static udp_sender* get_instance();
	virtual ~udp_sender();
    void prepare(std::string ip, int port);
	bool add(pack* pack);
	void stop();
	bool is_running();
	void start();
	socket_client* get_socket();
    void set_net_queue_size(int n);
	void send(pack* pk);
};

} /* namespace scouter */

#endif /* SRC_UDPSENDER_H_ */
