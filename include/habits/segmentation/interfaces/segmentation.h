#pragma once
#include <representations/segmentation.h>
#include <representations/trajectory.h>
#include <unordered_map>
namespace habits {
    namespace segmentation {
        namespace interfaces {
        template<typename T> class segmentation : public representations::interfaces::cluster<representations::const_segmentation_semantic_ul<representations::interfaces::time_group<T>>> {
            public:
                explicit segmentation(const representations::interfaces::cluster<representations::interfaces::time_series<T>> &collection);
            };
        }
    }
}
#include <habits/segmentation/interfaces/impl/segmentation_impl.h>