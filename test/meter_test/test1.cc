#include "counter/meter_map.h"
#include<iostream>
#include<unistd.h>
#include<pthread.h>
void* func1(void* p);
void* func2(void* p);
int main(int argc, char* argv[]) {
	pthread_t threads[2];
	pthread_create(&threads[0],NULL,func1,NULL);
	std::cout<<"create thread1"<<std::endl;
	pthread_create(&threads[1],NULL,func2,NULL);
	std::cout<<"create thread2"<<std::endl;
	sleep(20);

}

void* func1(void* p) {
	scouter::meter_map* map1 = scouter::meter_map::get_instance();
	std::cout<<std::endl<<"map:"<<map1<<std::endl;
	return NULL;
}


void* func2(void* p) {
	scouter::meter_map* map2 = scouter::meter_map::get_instance();
	std::cout<<std::endl<<"map:"<<map2<<std::endl;
	return NULL;
}

