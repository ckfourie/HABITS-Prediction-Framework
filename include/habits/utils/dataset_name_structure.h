#pragma once
#include <string>
namespace habits {
    namespace utils {
        class dataset_name_structure {
        public:
            dataset_name_structure(const std::string & name);
            std::string dataset_name;
            std::string subject_name;
            std::string data_element_name;
        };
    }
}