#include <habits/segmentation/detectors/state_based_detector.h>
#include <boost/functional/hash.hpp>
representations::interfaces::segmentation habits::segmentation::detectors::state_based_detector::run(const representations::interfaces::ordered_collection &collection) {
    representations::interfaces::segmentation output (collection);
    std::vector<int> binary_vector (collection.size(),0);
    // now run the detector
    #pragma omp parallel for default(none) shared(collection,binary_vector,m_zero_order_tests,m_first_order_tests,m_second_order_tests)
    for (int i = 0; i < collection.size(); i++) {
        bool valid = true;
        for (const auto & f : m_zero_order_tests) {
            valid &= f(collection.at(i));
            if (!valid) continue;
        }
        for (const auto & f : m_first_order_tests) {
            valid &= f(collection.at(i));
            if (!valid) continue;
        }
        for (const auto & f : m_second_order_tests) {
            valid &= f(collection.at(i));
            if (!valid) continue;
        }
        binary_vector[i] = valid;
    }
    // parse binary vector to find start and end indices
    for (int i = 0; i < binary_vector.size(); i++){
        // rising edge
        if (i ==0) if (binary_vector[i]) output.add_segmentation_index(representations::interfaces::semantic_index(i,m_entry_event));
        else if (binary_vector[i] && !binary_vector[i-1]) output.add_segmentation_index(representations::interfaces::semantic_index(i,m_entry_event));
        // falling edge
        if (i ==0) continue;
        else if (!binary_vector[i] && binary_vector[i-1]) output.add_segmentation_index(representations::interfaces::semantic_index(i,m_exit_event));
    }
}
habits::segmentation::detectors::state_based_detector::state_based_detector(const long &base_id) : m_base_id(base_id) {
    boost::hash<std::string> hash_function;
    m_entry_event.id(hash_function("entry_event")+m_base_id);
    m_exit_event.id(hash_function("exit_event")+m_base_id);
    m_entry_event.label("state rising edge: " + std::to_string(m_base_id));
    m_entry_event.label("state falling edge: " + std::to_string(m_base_id));
}
