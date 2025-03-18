#include "MaskStatusEnum.hpp"

namespace TofdPe {

    MaskStatusEnum* MaskStatusEnum::Instance() {
        static MaskStatusEnum inst;
        return &inst;
    }

} // namespace TofdPe
