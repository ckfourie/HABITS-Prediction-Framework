#pragma once
#include <string>
namespace habits {
    namespace utils {
        class dataset_name_structure {
        public:
            explicit dataset_name_structure(const std::string & name);
            std::string dataset_name;
            std::string subject_name;
            std::string data_element_name;
            friend std::ostream& operator<<(std::ostream & os, const dataset_name_structure & obj);
        };

    }
}