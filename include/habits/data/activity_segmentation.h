#pragma once
#include <habits/segmentation/interfaces/segmentation.h>

namespace habits {
    namespace data {
        class activity_segmentation : public representations::interfaces::segment_cluster_group {
        public:
            explicit activity_segmentation(const habits::segmentation::interfaces::segmentation & segmentation);
        protected:
            void run (const habits::segmentation::interfaces::segmentation & segmentation);
        };
    }
}