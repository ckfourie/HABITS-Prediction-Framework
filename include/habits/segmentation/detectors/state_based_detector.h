#pragma once
#include <habits/segmentation/interfaces/detector.h>

namespace habits {
    namespace segmentation {
        namespace detectors {
            class state_based_detector : public interfaces::detector {
            public:
                state_based_detector(const long & base_id);
                state_based_detector(const std::string & base_name);
                representations::interfaces::segmentation run (const representations::interfaces::ordered_collection & collection) override;
                void add_zero_order_detection(const std::function<bool(const representations::interfaces::representation &)> & test){m_zero_order_tests.emplace_back(test);}
                void add_first_order_detection(const std::function<bool(const representations::interfaces::representation &)> & test){m_first_order_tests.emplace_back(test);}
                void add_second_order_detection(const std::function<bool(const representations::interfaces::representation &)> & test){m_second_order_tests.emplace_back(test);}
            protected:
                long m_base_id;
                representations::interfaces::semantic m_entry_event, m_exit_event;
                std::vector<std::function<bool(const representations::interfaces::representation &)>> m_zero_order_tests;
                std::vector<std::function<bool(const representations::interfaces::representation &)>> m_first_order_tests;
                std::vector<std::function<bool(const representations::interfaces::representation &)>> m_second_order_tests;
            };
        }
    }
}