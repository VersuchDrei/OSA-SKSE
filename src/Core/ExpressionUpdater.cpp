#include "ExpressionUpdater.h"

namespace OStim {
    int ExpressionUpdater::step() {
        if (current < goal) {
            current += speed;
            if (current > goal) {
                current = goal;
            }
        } else {
            current -= speed;
            if (current < goal) {
                current = goal;
            }
        }
        return current;
    }

    bool ExpressionUpdater::isDone() {
        return current == goal;
    }
}