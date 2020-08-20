#pragma once
#include <habits/segmentation/interfaces/segmentation.h>

namespace habits {
    namespace segmentation {
        class state_based_segmentation : public interfaces::segmentation {
        public:
            state_based_segmentation(const representations::interfaces::unordered_collection & collection);
        protected:
            representations::interfaces::segmentation run_strategy(const std::string & name, const representations::interfaces::ordered_collection & trajectory);
            bool try_subject_detector_map(const std::string & name);
        };
    }
}
