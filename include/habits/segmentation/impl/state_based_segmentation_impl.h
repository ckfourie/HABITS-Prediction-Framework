#include <habits/utils/dataset_name_structure.h>

namespace habits {
    namespace segmentation {
        template<typename T>
        state_based_segmentation<T>::state_based_segmentation( const representations::interfaces::cluster<representations::interfaces::time_series<T>> &collection) : interfaces::segmentation<T>(collection) {
            load_detectors_from_goal_information(); // technically we shouldn't have this stuff in the constructor.
            // step through keys
            std::vector<std::string> keys = collection.keys();
            #pragma omp parallel for
            for (size_t i = 0; i < keys.size(); i++) {
                if (!run_strategy(keys[i],collection.at(keys[i]))) {
                    SLOG(error) << "failed to segment trajectory with key: " << keys[i];
                }
            }
        }
        template<typename T>
        void state_based_segmentation<T>::load_detectors_from_goal_information() {
            // cast goal information:
            gppe::timer t;
            auto goal_information = dynamic_cast<const representations::interfaces::cluster<T>&>(habits::goal_information());
            // step through and load detectors:
            std::vector<std::string> keys = goal_information.keys();
            std::mutex lock; // incase we want to thread this (no need at the moment)
            for (size_t i = 0; i < keys.size(); i++) {
                // construct dataset name structure:
                auto names = utils::dataset_name_structure(keys[i]);
                if (m_detector_map.count(names.subject_name) == 0) m_detector_map.emplace(names.subject_name,std::map<std::string,boost::shared_ptr<detectors::state_based_detector>>());
                if (m_detector_map[names.subject_name].count(names.data_element_name) == 0) {
                    // create detector
                    boost::shared_ptr<detectors::state_based_detector> detector (new detectors::state_based_detector(names.data_element_name));
                    // get the value
                    T value = goal_information.at(keys[i]);
                    // add a position check
                    try {
                        double radius = service::parameters::read({gppe::path_join(names.dataset_name,names.subject_name)},gppe::path_join(names.data_element_name,"event_region_radius")).primitive<double>();
                        detector->add_zero_order_detection([radius,value](const representations::interfaces::has_distance & d)->bool {
                            double distance = d.distance(value);
                            return distance < radius;
                        });
                    } catch (const std::exception & e) {
                        SLOG(error) << e.what();
                        throw std::runtime_error("state_based_segmentation<T>:: couldn't create position check");
                    }
                    try {
                        double velocity_limit = service::parameters::read({gppe::path_join(names.dataset_name,names.subject_name)},gppe::path_join(names.data_element_name,"velocity_limit")).primitive<double>();
                        T p; // defaults to zero (if you get funny things here make sure your class initializes correctly)
                        detector->add_first_order_detection([velocity_limit,p](const representations::interfaces::has_distance & d)->bool {
                            // calculate velocity:
                            double distance = d.distance(p);
                            return distance < velocity_limit*1400;
                        });
                    } catch (const std::exception & e) {
                        SLOG(error) << e.what();
                        throw std::runtime_error("state_based_segmentation<T>:: couldn't create velocity check");
                    }
                    std::lock_guard<std::mutex> lk (lock);
                    m_detector_map[names.subject_name].emplace(names.data_element_name,detector);
                } else {
                    SLOG(warning) << "state_based_segmentation:: multiple entries for " << keys[i];
                }
            }
            SLOG(trace) << "state_based_segmentation:: loaded detectors from goal information in " << t.elapsed() << " seconds";
        }

        template<typename T>
        bool state_based_segmentation<T>::run_strategy(const std::string & name, const representations::interfaces::time_series<T> & trajectory){
            auto names = utils::dataset_name_structure(name);
            // check for detectors
            if (m_detector_map.count(names.subject_name) == 0) return false;
            if (m_detector_map[names.subject_name].empty()) return false;
            // run each detector:
            std::vector<representations::semantic_index_ul> indices; indices.reserve(1024);
            std::mutex lk;
            for (auto it = m_detector_map[names.subject_name].cbegin(); it != m_detector_map[names.subject_name].cend(); ++it) {
                auto local_indices = it->second->run(trajectory,0);
                std::lock_guard<std::mutex> lkk (lk);
                indices.insert(indices.end(),local_indices.begin(),local_indices.end());
            }
            // sort and add:
            std::sort(indices.begin(),indices.end());
            auto & seg_ref = this->at(name);
            for (const auto & index : indices) {
                seg_ref.add_segmentation_index(index);
            }
            // filter for short segments
            seg_ref.filter_short_segments(12);
            // add callback to the collection here to continue segmentation if more data is added.
//            trajectory.on_change_signal().connect(boost::bind(&state_based_segmentation::update_segmentation,this,boost::placeholders::_1,name,m_detector_map[names.subject_name]));
            return true;
        }
    }
}