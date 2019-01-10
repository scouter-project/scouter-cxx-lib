#ifndef __METER_MAP_H__
#define __METER_MAP_H__
#include <pthread.h>
#include "counter/metric_meter.h"
#include <map>
#include <string>
using namespace std;
namespace scouter {
    class meter_map{
        private:
            meter_map();
            ~meter_map();
            static meter_map* inst; 
            std::map<string, metric_meter*> m_map;
            //static pthread_mutex_t lock_mutex[1];


        public:
            static meter_map* get_instance();
            void add(string key, metric_meter* meter);
            metric_meter* get_meter(string key);

    };


}



#endif
