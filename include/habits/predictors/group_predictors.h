#pragma once
#include <habits/predictors/interfaces/predictor.h>
namespace habits {
    namespace predictors {
        class group_predictors {
        public:
            group_predictors(const std::vector<interfaces::predictor::predictor_ptr> & predictors);
            
        protected:
            std::vector<interfaces::predictor::predictor_ptr> m_predictors;
        };
    }
}