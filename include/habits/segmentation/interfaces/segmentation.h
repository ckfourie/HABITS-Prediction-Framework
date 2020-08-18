#pragma once
#include <representations/interfaces/collection.h>
#include <representations/interfaces/segmentation.h>
#include <representations/interfaces/homogeneous_cluster.h>
#include <unordered_map>
namespace habits {
    namespace segmentation {
        namespace interfaces {
        class segmentation : public representations::interfaces::homogeneous_cluster<representations::interfaces::segmentation> {
            public:
                typedef std::unordered_map<std::string, representations::interfaces::segmentation> map_type;
                explicit segmentation(const representations::interfaces::unordered_collection &collection) : m_collection_reference(collection){};

            protected:
                std::reference_wrapper<const representations::interfaces::unordered_collection> m_collection_reference;
            };
        }
    }
}