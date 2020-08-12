#pragma once
#include <representations/interfaces/collection.h>
#include <representations/interfaces/segmentation.h>
#include <unordered_map>
namespace habits {
    namespace segmentation {
        class segmenter {
        public:
            explicit segmenter(const representations::interfaces::unordered_collection & collection);
        protected:
            std::reference_wrapper<const representations::interfaces::unordered_collection> m_collection_reference;
            std::unordered_map<std::string,representations::interfaces::segmentation> m_segmentations;
        };
    }
}