#pragma once
#include <representations/interfaces/collection.h>
#include <representations/interfaces/segmentation.h>
#include <unordered_map>
namespace habits {
    namespace segmentation {
        namespace interfaces {
        class segmentation : public representations::interfaces::cluster<representations::interfaces::segmentation> {
            public:
                explicit segmentation(const representations::interfaces::unordered_collection &collection);
            protected:
                std::reference_wrapper<const representations::interfaces::unordered_collection> m_collection_reference;
            };
        }
    }
}