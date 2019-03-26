/*
 * udp_data_sender.h
 *
 *  Created on: 2015. 4. 28.
 *      Author: windfree
 */

#ifndef SRC_NET_UDP_DATA_SENDER_H_
#define SRC_NET_UDP_DATA_SENDER_H_

#include "net/socket_client.h"
#include <netinet/in.h>
#include <queue>
#include <vector>
using namespace std;
namespace scouter {
class pack;
class data_output;
class udp_socket_client: public socket_client {
private:
    struct sockaddr_in sock_addr;
public:
	udp_socket_client(struct sockaddr* serv_addr);
	udp_socket_client();
	~udp_socket_client();
//	void close();
	//bool connect();
	//bool is_connected();
	int send(char* buffer,int32_t len);
	void send_group_packet(char* buffer, int32_t len);
	void send_group_packet(int64_t key, int32_t total,int32_t num, int packet_size, char* buffer);
	void set_addr(std::string ip, int port);
	void send(std::vector<data_output*>& out_list);
	void send_pack_list(char* buffer,int32_t len, int16_t count); 
};

} /* namespace scouter */

#endif /* SRC_NET_UDP_DATA_SENDER_H_ */
