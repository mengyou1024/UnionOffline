#include "MaskStatusEnum.hpp"

namespace TOFD_PE {

    MaskStatusEnum* MaskStatusEnum::Instance() {
        static MaskStatusEnum inst;
        return &inst;
    }

} // namespace TOFD_PE
