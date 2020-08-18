#include <habits/utils/dataset_name_structure.h>
#include <gppe/string_manipulations.h>
#include <simple-logger/simple-logger.h>
habits::utils::dataset_name_structure::dataset_name_structure(const std::string &name) {
    // strip name by colon
    std::vector<std::string> items = gppe::to_vector<std::string>(name,":");

    data_element_name = items.at(1);
    std::vector<std::string> split = gppe::to_vector<std::string>(items.at(0),"/");
    dataset_name = split[0];
    subject_name = split[1];
    DEBUG_VALUE(dataset_name);
    DEBUG_VALUE(subject_name);
    DEBUG_VALUE(data_element_name);
}
