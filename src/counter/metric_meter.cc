#include "counter/metric_meter.h"
#include<iostream>
namespace scouter {
    metric_meter::metric_meter() {
        pthread_mutex_init(&_mutex,NULL);
        for(int i = 0; i < BUCKET_SIZE; i++) {
            value[i] = 0;
            count[i] = 0;
        }
        this->_time = get_time();
        this->pos = (int32_t) this->_time % BUCKET_SIZE;

    }    
    metric_meter::~metric_meter() {
        pthread_mutex_destroy(&_mutex);

    }    

    int64_t metric_meter::get_time() {
        return util::get_current_time();
    }

    int32_t metric_meter::get_position(){
        pthread_mutex_lock(&_mutex);
        int64_t ctime = get_time();
        if(ctime != this->_time) {
            for(int i = 0; i < (ctime - _time) && i < BUCKET_SIZE; i++) {
                if(pos + 1 > BUCKET_SIZE - 1) {
                    pos  = 0; 
                } else {
                    pos = pos + 1;
                }
                clear(pos);
            }
            this->_time = ctime;
            this->pos = (int32_t) this->_time % BUCKET_SIZE;
        }
        pthread_mutex_unlock(&_mutex);
        return this->pos;
            
    }

    int32_t  metric_meter::check(int32_t period) {
        pthread_mutex_lock(&_mutex);
        if(period >= BUCKET_SIZE) {
            period = BUCKET_SIZE -1;
        }
        pthread_mutex_unlock(&_mutex);

        return period;
    }

    int32_t metric_meter::desc(int pos) {
        pthread_mutex_lock(&_mutex);
        if(pos == 0) {
            pos = BUCKET_SIZE - 1; 
        } else {
            pos--;
        }
        pthread_mutex_unlock(&_mutex);
        return pos;
    }

    int32_t metric_meter::get_bucket_size() {
        return BUCKET_SIZE;
    }

    void metric_meter::clear(int32_t pos) {
        this->value[pos] = 0;
        this->count[pos] = 0;
    }

    void metric_meter::add(int64_t val) {
        int32_t current_pos = get_position();
        value[current_pos] = value[current_pos] + val;
        count[current_pos] = count[current_pos] + 1;
        pthread_mutex_unlock(&_mutex);
    }

    int64_t metric_meter::get_avg(int period) {
        period = check(period);
        int pos = get_position();
        int64_t sum_count  = 0; 
        int64_t sum_value = 0; 
        for(int32_t i = 0; i < period; i++) {
            sum_value += value[pos];
            sum_count += count[pos];
            pos = desc(pos);
        }

        if(sum_count == 0) {
            return 0;
        }

        return (int64_t) (sum_value / sum_count);
    }

    int64_t metric_meter::get_avg_period(int period) {
        period = check(period);
        int pos = get_position();
        int64_t sum_value = 0; 
        for(int32_t i = 0; i < period; i++) {
            sum_value += value[pos];
            pos = desc(pos);
        }

        return (int64_t) (sum_value / period);
    }


    int64_t metric_meter::get_sum(int period) {
        period = check(period);
        int64_t pos = get_position();
        int64_t sum = 0; 
        for(int32_t i = 0; i < period; i++) {\
            sum += count[pos];
            pos = desc(pos);
        }
        return sum;

    }
}
