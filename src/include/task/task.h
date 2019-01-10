/*
 * task.h
 *
 *  Created on: 2015. 6. 15.
 *      Author: windfree
 */

#ifndef SRC_TASK_TASK_H_
#define SRC_TASK_TASK_H_
#ifndef MYSQL_SERVER
#define MYSQL_SERVER
#endif

//#include "table.h"
#include "util/util.h"

namespace scouter {
class net_proxy;
class task {
private:
	virtual bool going_down();
	pthread_t spotter_thread_id;

protected:
	void* plugin_data;
	net_proxy* netproxy;
	//mutex sleep_mutex;
	//mysql_cond_t sleep_condition;
	volatile bool shutdown_plugin;
	int interval;
	virtual int execute(void* p) = 0 ;
	int slept_ok(time_t sec);
	bool is_running;
	static void* start_thread(void* p) {
										((task*)p)->execute(p);
										return NULL;
	}
public:
	task();

	virtual ~task();
	void run(void* p);
	void stop() ;
	void set_interval(int val);
	bool is_run();
};

} /* namespace scouter */

#endif /* SRC_TASK_TASK_H_ */
