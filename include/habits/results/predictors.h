#pragma once
#include <habits/predictors/interfaces/predictor.h>
#include <habits/predictors/bestpta_predictor.h>
#include <habits/datasets.h>
namespace habits {
    namespace results {
        // output structure
        class predictor_results {
        public:
            predictor_results() = default;
        protected:
            double wl_peformance;
        };

        std::vector<predictor_results> calculate_predictor_results(const representations::interfaces::unordered_collection & data = habits::active_dataset(), const habits::predictors::interfaces::predictor & predictor_template = habits::predictors::bestpta_predictor());
    }
}