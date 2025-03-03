#include "LinesMaskEnum.hpp"

namespace TOFD_PE {

    LinesMakeEnum* LinesMakeEnum::Instance() {
        static LinesMakeEnum inst;
        return &inst;
    }

} // namespace TOFD_PE
