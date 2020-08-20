#pragma once
#include <boost/shared_ptr.hpp>
#include <representations/interfaces/collection.h>
namespace habits {
    // load data, create shared object representation
    const representations::interfaces::unordered_collection & active_dataset();
    const representations::interfaces::unordered_collection & goal_information();
    const representations::interfaces::unordered_collection & load_dataset(const std::string & dataset_name, const std::string & subject_regex);
}