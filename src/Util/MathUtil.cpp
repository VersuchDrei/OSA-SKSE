#include "MathUtil.h"

#define _USE_MATH_DEFINES

#include <math.h>

namespace MathUtil {
    float toDegrees(float angle) {
        return angle * 180 / M_PI;
    }
}