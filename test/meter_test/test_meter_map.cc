#include "counter/meter_map.h"
#include "counter/metric_meter.h"
#include "gtest/gtest.h"
using namespace std;
namespace scouter {
    class test_meter_map : public ::testing::Test {
        protected:
            test_meter_map() {
            }
            virtual ~test_meter_map() {

            }

    };

    TEST_F(test_meter_map, t_meter_map) {
        meter_map* m_map = meter_map::get_instance();
        metric_meter* meter1 = new metric_meter();
        std::cout<<"meter1:"<<meter1<<std::endl;
        m_map->add("key1",meter1);
        metric_meter* meter2 = new metric_meter();
        std::cout<<"meter2:"<<meter2<<std::endl;
        m_map->add("key1",meter2);
        metric_meter* rmeter = m_map->get_meter("key1");
        std::cout<<"rmeter:"<<rmeter<<std::endl;
        EXPECT_EQ(1,1);

    }


}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
