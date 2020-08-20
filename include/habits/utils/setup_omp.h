#pragma once
#include <openmpi/mpi.h>

namespace habits {
    namespace utils {
        inline void setup_omp () {
            omp_set_dynamic(1);
            omp_set_nested(1);
        }
    }
}