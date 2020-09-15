#pragma once
#include <habits/segmentation/interfaces/segmentation.h>
#include <habits/segmentation/detectors/state_based_detector.h>
namespace habits {
    namespace segmentation {
        class state_based_segmentation : public interfaces::segmentation {
        public:
            explicit state_based_segmentation(const representations::interfaces::unordered_collection & collection);
            void incremental_update_interval(const unsigned long & interval){m_update_interval = interval;}
            unsigned long incremental_update_interval() const {return m_update_interval;}
        protected:
            bool run_strategy(const std::string & name, const representations::interfaces::ordered_collection & trajectory);
            bool try_subject_detector_map(const std::string & name, const representations::interfaces::ordered_collection & trajectory);
            void load_detectors_from_goal_information();
            void update_segmentation(const representations::interfaces::representation & data, const std::string & name, const std::map<std::string,boost::shared_ptr<detectors::state_based_detector>> & local_detectors);
            std::map<std::string,std::map<std::string,boost::shared_ptr<detectors::state_based_detector>>> m_detector_map;
            unsigned long m_update_interval = 20;
        };
    }
}
