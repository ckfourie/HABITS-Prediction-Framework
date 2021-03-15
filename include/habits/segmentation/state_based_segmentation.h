#pragma once
#include <habits/segmentation/interfaces/segmentation.h>
#include <habits/segmentation/detectors/state_based_detector.h>
#include <service/parameters.h>
#include <gppe/file_operations.h>
namespace habits {
    namespace segmentation {
        template<typename T> class state_based_segmentation : public interfaces::segmentation<T> {
        public:
            explicit state_based_segmentation(const representations::interfaces::cluster<representations::interfaces::time_series<T>> & collection);
        protected:
            bool run_strategy(const std::string & name, const representations::interfaces::time_series<T> & trajectory);
//            bool try_subject_detector_map(const std::string & name, const representations::interfaces::ordered_collection & trajectory);
            void load_detectors_from_goal_information();
//            void update_segmentation(const representations::interfaces::representation & data, const std::string & name, const std::map<std::string,boost::shared_ptr<detectors::state_based_detector>> & local_detectors);
            std::map<std::string,std::map<std::string,boost::shared_ptr<detectors::state_based_detector>>> m_detector_map;
        };
    }
}
#include <habits/segmentation/impl/state_based_segmentation_impl.h>