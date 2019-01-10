/*
 * obj_name_util.h
 *
 *  Created on: 2015. 5. 20.
 *      Author: windfree
 */

#ifndef SRC_UTIL_OBJ_NAME_UTIL_H_
#define SRC_UTIL_OBJ_NAME_UTIL_H_

#include <iostream>
namespace scouter {
class util;
class obj_name_util {
private:
	obj_name_util();
	static obj_name_util* instance;
	std::string obj_name;
	int obj_hash;
	std::string obj_type;
	void init_name();
	void init_hash();


public:
	~obj_name_util();
	static obj_name_util* get_instance();
	std::string object_name();
	std::string object_type();
	int object_hash();
};

} /* namespace scouter */

#endif /* SRC_UTIL_OBJ_NAME_UTIL_H_ */
