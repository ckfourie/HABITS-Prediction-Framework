#include <habits/segmentation/detectors/state_based_detector.h>
#include <boost/functional/hash.hpp>
#include <cctype>
#include <gppe/string_manipulations.h>
#include <simple-logger/simple-logger.h>

representations::interfaces::segmentation habits::segmentation::detectors::state_based_detector::run(const representations::interfaces::ordered_collection &collection, const unsigned long & start_index) {
    representations::interfaces::segmentation output (collection);
    std::vector<int> binary_vector (collection.size(),0);
    // now run the detector
//    #pragma omp parallel for default(none) shared(collection,binary_vector,m_zero_order_tests,m_first_order_tests,m_second_order_tests)
    for (unsigned long i = start_index; i < collection.size(); i++) {
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
    // parse binary vector to find start and end indices
    for (unsigned long i = start_index; i < binary_vector.size(); i++){
        if (i==0) continue;
        // rising edge
        if (binary_vector[i] && !binary_vector[i-1]) output.add_segmentation_index(representations::interfaces::semantic_index(i,m_entry_event));
        // falling edge
        if (!binary_vector[i] && binary_vector[i-1]) output.add_segmentation_index(representations::interfaces::semantic_index(i,m_exit_event));
    }
    return std::move(output);
}

std::vector<representations::interfaces::semantic_index> habits::segmentation::detectors::state_based_detector::incremental_run(const representations::interfaces::ordered_collection &collection) {
    unsigned long current_size = 0;
    if (m_binary_vector_mapping.count(collection.uuid()) == 0) {
        m_binary_vector_mapping.emplace(collection.uuid(),std::vector<int>(collection.size(),false));
        m_binary_vector_mapping[collection.uuid()].reserve(1024); // make sure we have suitable space available and don't need to resize unnecessarily
    } else {
        current_size = m_binary_vector_mapping[collection.uuid()].size();
        m_binary_vector_mapping[collection.uuid()].resize(collection.size()-1,false);
    }
    if (m_indices_map.count(collection.uuid()) == 0) m_indices_map.emplace(collection.uuid(),std::vector<representations::interfaces::semantic_index>());
    for (unsigned long i = current_size; i < collection.size()-1; i++) {
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
        m_binary_vector_mapping[collection.uuid()][i] = valid;
    }
    unsigned long index_size = m_indices_map[collection.uuid()].size();
    for (unsigned long i = current_size; i < m_binary_vector_mapping[collection.uuid()].size(); i++){
        if (i==0) continue;
        // rising edge
        if (m_binary_vector_mapping[collection.uuid()][i] && !m_binary_vector_mapping[collection.uuid()][i-1]) m_indices_map[collection.uuid()].emplace_back(representations::interfaces::semantic_index(i,m_entry_event));
        // falling edge
        if (!m_binary_vector_mapping[collection.uuid()][i] && m_binary_vector_mapping[collection.uuid()][i-1]) m_indices_map[collection.uuid()].emplace_back(representations::interfaces::semantic_index(i,m_exit_event));
    }
    return std::vector<representations::interfaces::semantic_index>(m_indices_map[collection.uuid()].begin()+index_size,m_indices_map[collection.uuid()].end());
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
