#pragma once
#include <habits/segmentation/interfaces/segmentation.h>
#include <habits/segmentation/detectors/state_based_detector.h>
namespace habits {
    namespace segmentation {
        class state_based_segmentation : public interfaces::segmentation {
        public:
            state_based_segmentation(const representations::interfaces::unordered_collection & collection);
        protected:
            bool run_strategy(const std::string & name, const representations::interfaces::ordered_collection & trajectory);
            bool try_subject_detector_map(const std::string & name, const representations::interfaces::ordered_collection & trajectory);
            void load_detectors_from_goal_information();
            std::map<std::string,std::map<std::string,boost::shared_ptr<detectors::state_based_detector>>> m_detector_map;
        };
    }
}
