#include "LinesMaskEnum.hpp"

namespace TofdPe {

    LinesMakeEnum* LinesMakeEnum::Instance() {
        static LinesMakeEnum inst;
        return &inst;
    }

} // namespace TofdPe
