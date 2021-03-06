#pragma once
#include <representations/interfaces/segmentation.h>
// detector runs on an ordered collection and produces a segmentation
namespace habits {
    namespace segmentation {
        namespace interfaces {
            class detector {
            public:
                detector() = default;
                virtual std::vector<representations::semantic_index_ul> run(const representations::interfaces::const_collection & collection, const unsigned long & start_index) =0;
//                virtual std::vector<representations::interfaces::semantic_index> incremental_run(const representations::interfaces::ordered_collection & collection) =0;
            };
        }
    }
}