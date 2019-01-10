#include "counter/meter_map.h"
#include <unistd.h>
namespace scouter {
    meter_map* meter_map::inst = NULL;
    static pthread_mutex_t lock_mutex[1] = { PTHREAD_MUTEX_INITIALIZER};

    meter_map* meter_map::get_instance() {
        pthread_mutex_lock(&lock_mutex[0]);
        if(meter_map::inst == NULL)  {
            meter_map::inst = new meter_map();
        }
        pthread_mutex_unlock(&lock_mutex[0]);
        return meter_map::inst;
    }
    meter_map::meter_map() {
        
    }

    meter_map::~meter_map() {
        map<string,metric_meter*>::iterator  it;
        for(it = m_map.begin(); it != m_map.end(); ++it) {
            if(it->second != NULL) {
                delete it->second;
            }
        } 
        
    }

    void meter_map::add(string key, metric_meter* meter) {
        try{
            map<string,metric_meter*>::iterator it;
            it =  m_map.find(key);
            if(it != m_map.end()) {
                delete it->second;
                it->second = meter;
            }else {
                m_map[key] = meter;
            }
        }catch(exception& e) {

        }        

    }

    metric_meter* meter_map::get_meter(string key) {
        metric_meter* value = NULL;
        try{
            map<string,metric_meter*>::iterator it;
            it =  m_map.find(key);
            if(it != m_map.end()) {
                value = it->second;
            } else {
                value = new metric_meter();
                add(key,value);
            }
        }catch(exception& e) {
        }
        return value; 

    }

}
