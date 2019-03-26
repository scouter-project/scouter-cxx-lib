/*
 * udp_data_sender.cc
 *
 *  Created on: 2015. 4. 28.
 *      Author: windfree
 */
#include "data_pack/pack.h"
#include "io/data_output.h"
#include <iostream>
#include <string.h>
#include "net/udp_socket_client.h"
#include "util/obj_name_util.h"
#include "util/util.h"
#include "net/net_constants.h"
#include "config/configure.h"
#include <stdio.h>
#include <unistd.h>
#include <cerrno>
#ifdef __WITH_TUNA__
#include "log/ap_log.h"
#include "config/tuna_conf.h"
#include "util/trace_util.h"
#endif

namespace scouter {

udp_socket_client::udp_socket_client() {
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
	//is_connect = false;
}


udp_socket_client::~udp_socket_client() {
	::close(sockfd);
}

void udp_socket_client::set_addr(std::string ip, int port) {
	sock_addr.sin_family = AF_INET;
    int result = inet_pton(AF_INET,ip.c_str(),&sock_addr.sin_addr);
    if(result == 1) {
        sock_addr.sin_port = htons(port);
        dest_addr = ((struct sockaddr*)&sock_addr);
#ifdef __WITH_TUNA__
        tuna::ap_log::get_instance()->info(4,"===== TunA udp socket setting  target ip:",ip.c_str()," port:",util::to_string(port).c_str());
#endif
    } else {
        dest_addr = NULL;
    } 
}

//below connect function is not used. 
//this funciton is used when implement connected udp socket.
/*
bool udp_socket_client::connect() {
	if(is_connect) {
		return true;
	}
	::connect(sockfd,dest_addr,sizeof(*dest_addr));
	char* msg = "HB";
	::send(sockfd,msg,strlen(msg),0);
	unsigned int len = sizeof(*dest_addr);
	int rc = recvfrom(sockfd,msg,sizeof(msg),0,dest_addr,&len);
	if(rc < 0) {
		if(errno == EWOULDBLOCK) { // connect ok, but no more  data to read.
			is_connect = true;
			return true;
		}else {
			is_connect = false;
			return false;
		}
	}
	return true;
}

void udp_socket_client::close() {
	::close(sockfd);
	is_connect = false;
}

bool udp_socket_client::is_connected() {
	return is_connect;
}
*/

int udp_socket_client::send(char* buffer, int32_t len) {
	int max_packet_size = 0; 
#ifdef __WITH_TUNA__
	max_packet_size = tuna::tuna_conf::get_instance()->get_udp_max_packet_size();
#else 
	max_packet_size = configure::MAX_PACKET_SIZE; 
#endif
	if(len > max_packet_size) {
		send_group_packet(buffer,len);
		return 0;
	}
	char* buff = new char[len + 4];
	memcpy(buff,net_constants::SINGLE_PACK,4);
	memcpy(buff + 4,buffer,len);
	::sendto(sockfd,buff,len + 4,0,dest_addr,sizeof(*dest_addr));
#ifdef __WITH_TUNA__
	if(tuna::tuna_conf::get_instance()->get_trace_udp_packet()) {
		tuna::trace_util::get_instance()->print_packet(buff,len+4);
	}
#endif
#ifdef __TUNA_DEBUG__
	tuna::trace_util::get_instance()->print_message("[debug] udp_scoket::send_single pack");	
#endif
	delete[] buff;
	return 0;

}
void udp_socket_client::send(std::vector<data_output*>& out_list) {
	int max_packet_size = 0; 
#ifdef __WITH_TUNA__
	max_packet_size = tuna::tuna_conf::get_instance()->get_udp_max_packet_size();
#else 
	max_packet_size = configure::MAX_PACKET_SIZE; 
#endif
	int size = out_list.size();
	if(size == 0) {
		return;
	} else if(size == 1) { 
		data_output* out  = out_list[0];
		char* buffer = out->to_byte_array();
		send(buffer,out->get_offset());
		delete out;
		out = 0;
		delete[] buffer;
	} else if(size > 1){
		data_output* send_out = new data_output();
		int16_t  count = 0; 
		for(int i = 0; i < size; i++) {
			data_output* out = out_list[i];
			char* buffer = out->to_byte_array();
			int len = out->get_offset();
			if(len > max_packet_size) {
				send_group_packet(buffer,len);
			} else if((len + send_out->get_offset()) > max_packet_size ){
				char* send_buffer = send_out->to_byte_array();
				send_pack_list(send_buffer,send_out->get_offset(),count);
				delete send_out;
				send_out = 0; 
				delete[] send_buffer;
				send_out = new data_output();
				send_out->write_bytes(buffer,len);
				count = 1;
			} else {
				char* buffer = out->to_byte_array();
				send_out->write_bytes(buffer,out->get_offset());
				count++;
				delete[] buffer;
			}
			delete out;
			out = 0; 
			delete[] buffer;
		}
		int send_size = send_out->get_offset();
		if(send_size > 0) {
			char* sbuffer = send_out->to_byte_array();
			send_pack_list(sbuffer,send_size,count);
			delete send_out;
			send_out = 0; 
			delete[] sbuffer;
		}
	}

}

void udp_socket_client::send_pack_list(char* buffer,int32_t len, int16_t count) {
#ifdef __TUNA_DEBUG__
		tuna::trace_util::get_instance()->print_message("[debug] udp_scoket::send_pack_list");	
#endif
	data_output* out = new data_output();
	out->write_bytes(net_constants::MULTI_PACK,4);
	out->write_int16(count);
	out->write_bytes(buffer,len);
	char* send_buff = out->to_byte_array();
	::sendto(sockfd,send_buff,out->get_offset() ,0,dest_addr,sizeof(*dest_addr));
#ifdef __WITH_TUNA__
	if(tuna::tuna_conf::get_instance()->get_trace_udp_packet()) {
		tuna::trace_util::get_instance()->print_packet(send_buff,out->get_offset());
	}
#endif
	delete out;
	out = 0;
	delete[] send_buff;
}

void udp_socket_client::send_group_packet(char* buffer, int32_t len) {
	int max_packet_size = 0; 
#ifdef __WITH_TUNA__
	max_packet_size = tuna::tuna_conf::get_instance()->get_udp_max_packet_size();
#else 
	max_packet_size = configure::MAX_PACKET_SIZE;
#endif
#ifdef __TUNA_DEBUG__
	tuna::trace_util::get_instance()->print_message("[debug] udp_scoket::send_group_packet");	
#endif
	int64_t key = util::get_next_key();
	int32_t total = len / max_packet_size;
	int32_t remain = len % max_packet_size;
	if(remain > 0) {
		total++;
	}

	int num = 0;
	for(num = 0 ; num < len / max_packet_size ; num++	) {
		char* buff = new char[max_packet_size];
		memcpy(buff,buffer + (num * max_packet_size),max_packet_size);
		send_group_packet(key,total,num,max_packet_size,buff);
		delete[] buff;
	}

	if(remain > 0) {
		char* buff = new char[remain];
		memcpy(buff,buffer + (len - remain), remain);
		send_group_packet(key,total,num,remain,buff);
		delete[] buff;
	}


}


void udp_socket_client::send_group_packet(int64_t key, int32_t total,	int32_t num, int packet_size, char* buffer) {
	data_output* out = new data_output();
	int32_t obj_hash =  obj_name_util::get_instance()->object_hash();
	out->write_bytes(net_constants::MTU_PACK,4);
	out->write_int32(obj_hash);
	out->write_int64(key);
	out->write_int16((int16_t)total);
	out->write_int16((int16_t)num);
	out->write_blob(buffer,packet_size);
	char* send_buff = out->to_byte_array();
	int32_t len = out->get_offset();

	/*
     * std::cout<<"hash: "<<obj_hash<<"  key:" <<key<< " num:"<<num<<" body len:"<<packet_size<<std::endl;
	for(int i = 0; i < packet_size; i++) {
		printf("%d ",buffer[i]);
		if(i != 0 && i % 10 == 0) {
			cout<<std::endl;
		}
	}
	std::cout<<std::endl;
    */
	::sendto(sockfd,send_buff,len,0,dest_addr,sizeof(*dest_addr));
	delete[] send_buff ;
	send_buff = 0;
	delete out;
	out = 0;
}

} /* namespace scouter */

