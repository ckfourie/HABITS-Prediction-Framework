#pragma once
#include <habits/segmentation/interfaces/segmentation.h>
#include <representations/interfaces/cluster.h>
namespace habits {
    namespace data {
    template<typename T> class activity_segmentation : public representations::semantic_key_cluster<representations::interfaces::cluster<representations::const_segment_semantic_ul<representations::interfaces::time_group<T>>>> {
        public:
            explicit activity_segmentation(const habits::segmentation::interfaces::segmentation<T> & segmentation);
        };
    }
}

#include <habits/data/impl/activity_segmentation_impl.h>