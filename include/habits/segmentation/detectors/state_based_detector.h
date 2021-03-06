#pragma once
#include <habits/segmentation/interfaces/detector.h>

namespace habits {
    namespace segmentation {
        namespace detectors {
            class state_based_detector : public interfaces::detector {
            public:
                explicit state_based_detector(const std::string & label = "");
                std::vector<representations::semantic_index_ul> run (const representations::interfaces::const_collection & collection, const unsigned long & start_index) override;
//                std::vector<representations::interfaces::semantic_index> incremental_run(const representations::interfaces::ordered_collection & collection) override;
                void add_zero_order_detection(const std::function<bool(const representations::interfaces::has_distance &)> & test){m_zero_order_tests.emplace_back(test);}
                void add_first_order_detection(const std::function<bool(const representations::interfaces::has_distance &)> & test){m_first_order_tests.emplace_back(test);}
                void add_second_order_detection(const std::function<bool(const representations::interfaces::has_distance &)> & test){m_second_order_tests.emplace_back(test);}
            protected:
                representations::interfaces::semantic m_entry_event, m_exit_event;
                std::vector<std::function<bool(const representations::interfaces::has_distance &)>> m_zero_order_tests;
                std::vector<std::function<bool(const representations::interfaces::has_distance &)>> m_first_order_tests;
                std::vector<std::function<bool(const representations::interfaces::has_distance &)>> m_second_order_tests;
                std::map<unsigned long, std::vector<int>> m_binary_vector_mapping;
                std::map<unsigned long, std::vector<representations::semantic_index_ul>> m_indices_map;
            };
        }
    }
}