#include "math/random.hpp"

#include <time.h>

using namespace math;

mrgen math::global_mrgen(static_cast<uint32_t>(time(0)));
