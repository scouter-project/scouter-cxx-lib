#include "counter/metric_meter.h"
#include "gtest/gtest.h"
using namespace std;
namespace scouter {
	class test_metric_meter : public ::testing::Test {
		protected:
			test_metric_meter() {
			}
			virtual ~test_metric_meter() {

			}
	};

    TEST_F(test_metric_meter,t_metric_meter) {
        metric_meter *meter = new metric_meter();
        meter->add(10);
        sleep(1);
        meter->add(11);
        sleep(1);
        meter->add(9);
        sleep(1);
        meter->add(8);
        sleep(1);
        meter->add(7);
        sleep(1);
        EXPECT_EQ(45,meter->get_sum(10));

        delete meter;
    }
}

int main(int argc, char* argv[]) {
	::testing::InitGoogleTest(&argc,argv);
	return RUN_ALL_TESTS();

}
