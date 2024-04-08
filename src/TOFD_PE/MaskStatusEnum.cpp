#include "MaskStatusEnum.hpp"
#include <QLoggingCategory>
#include <QQmlProperty>
#include <QValueAxis>
#include <QtCore>
#include <Yo/File>

namespace TOFD_PE {

    MaskStatusEnum* MaskStatusEnum::instance() {
        static MaskStatusEnum inst;
        return &inst;
    }

} // namespace TOFD_PE
