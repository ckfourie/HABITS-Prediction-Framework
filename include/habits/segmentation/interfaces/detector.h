#pragma once
#include <representations/interfaces/segmentation.h>
// detector runs on an ordered collection and produces a segmentation
namespace habits {
    namespace segmentation {
        namespace interfaces {
            class detector {
            public:
                detector() = default;
                virtual representations::interfaces::segmentation run(const representations::interfaces::ordered_collection & collection) =0;
            };
        }
    }
}