#include <habits/segmentation/detectors/state_based_detector.h>
#include <boost/functional/hash.hpp>
#include <cctype>
#include <gppe/string_manipulations.h>
#include <simple-logger/simple-logger.h>

representations::interfaces::segmentation habits::segmentation::detectors::state_based_detector::run(const representations::interfaces::ordered_collection &collection) {
    representations::interfaces::segmentation output (collection);
    std::vector<int> binary_vector (collection.size(),0);
    // now run the detector
//    #pragma omp parallel for default(none) shared(collection,binary_vector,m_zero_order_tests,m_first_order_tests,m_second_order_tests)
    for (int i = 0; i < collection.size(); i++) {
        bool valid = true;
        for (const auto & f : m_zero_order_tests) {
            valid &= f(collection.at(i));
            if (!valid) break;
        }
        for (const auto & f : m_first_order_tests) {
            valid &= f(collection.ddt().at(i));
            if (!valid) break;
        }
        for (const auto & f : m_second_order_tests) {
            valid &= f(collection.ddt().ddt().at(i));
            if (!valid) break;
        }
        binary_vector[i] = valid;
    }
    // remove short segments
//    bool state = false; // false is low, true is high
//    int counter = 0;
//    for (int i = 0; i < binary_vector.size(); i++) {
//        if (!state && binary_vector[i]){
//            state = true;
//            counter = 0;
//        }
//        if (state && binary_vector[i]){
//            counter++;
//        }
//        if (state && !binary_vector[i]){
//            state = false;
//            if (counter < 25) {
//                int j = i-1;
//                while (j > 0) {
//                    if (binary_vector[j]) binary_vector[j] = 0;
//                    else break;
//                }
//            }
//        }
//    }
    // parse binary vector to find start and end indices
    for (int i = 1; i < binary_vector.size(); i++){
        // rising edge
        if (binary_vector[i] && !binary_vector[i-1]) output.add_segmentation_index(representations::interfaces::semantic_index(i,m_entry_event));
        // falling edge
        if (!binary_vector[i] && binary_vector[i-1]) output.add_segmentation_index(representations::interfaces::semantic_index(i,m_exit_event));
    }
    return std::move(output);
}
habits::segmentation::detectors::state_based_detector::state_based_detector(const long &base_id) : m_base_id(base_id) {
    boost::hash<std::string> hash_function;
    m_entry_event.id(hash_function("entry_event")+m_base_id);
    m_exit_event.id(hash_function("exit_event")+m_base_id);
    m_entry_event.label("state rising edge: " + std::to_string(m_base_id));
    m_exit_event.label("state falling edge: " + std::to_string(m_base_id));
}
habits::segmentation::detectors::state_based_detector::state_based_detector(const std::string &base_name) {
    // try get base_id from name
    std::vector<long> ids = gppe::extract_numbers(base_name);
    if (ids.size() == 1) {
        // yay!
        m_base_id = ids[0];
        boost::hash<std::string> hash_function;
        m_entry_event.id(hash_function("entry_event")+m_base_id);
        m_exit_event.id(hash_function("exit_event")+m_base_id);
        m_entry_event.label("state rising edge: " + std::to_string(m_base_id));
        m_exit_event.label("state falling edge: " + std::to_string(m_base_id));
    } else throw std::runtime_error("invalid name");
}
