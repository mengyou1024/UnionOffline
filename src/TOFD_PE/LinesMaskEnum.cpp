#include "LinesMaskEnum.hpp"
#include <QLoggingCategory>
#include <QQmlProperty>
#include <QValueAxis>
#include <QtCore>
#include <Yo/File>

namespace TOFD_PE {

    LinesMakeEnum* LinesMakeEnum::Instance() {
        static LinesMakeEnum inst;
        return &inst;
    }

} // namespace TOFD_PE
