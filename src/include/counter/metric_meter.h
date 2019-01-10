#ifndef __METRIC_METER_H__
#define __METRIC_METER_H__
#include "util/util.h"
#include <pthread.h>
namespace scouter {
    class metric_meter {
        private:
            int32_t pos ; 
            int64_t _time;
            const static int32_t BUCKET_SIZE = 301;
            int64_t value[BUCKET_SIZE];
            int64_t count[BUCKET_SIZE];
            pthread_mutex_t _mutex;
        public :
            metric_meter();
            ~metric_meter();
            int32_t get_position();
            int32_t check(int32_t period);
            int32_t desc(int32_t pos);
            int64_t get_time();
            int32_t get_bucket_size();
            void clear(int32_t pos);
            int64_t get_avg(int32_t period);
            int64_t get_sum(int32_t period);
            int64_t get_avg_period(int32_t period);
            void add(int64_t value);

    };
}
#endif
