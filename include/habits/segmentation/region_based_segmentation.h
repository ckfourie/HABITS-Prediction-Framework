#pragma once
#include <habits/segmentation/interfaces/segmentation.h>

namespace habits {
    namespace segmentation {
        class region_based_segmentation : public interfaces::segmentation {
        public:
            region_based_segmentation(const representations::interfaces::unordered_collection & collection);
        protected:
            representations::interfaces::segmentation run_strategy(const std::string & name, const representations::interfaces::ordered_collection & trajectory);
        };
    }
}
