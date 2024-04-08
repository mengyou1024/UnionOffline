#pragma once

#include <QObject>

namespace TOFD_PE {
    class MaskStatusEnum : public QObject {
        Q_OBJECT
    public:
        enum class _MaskStatusEnum {
            Normal          = 0,
            RmThroughWave   = 0b1,
            PullThroughWave = 0b10,
        };
        Q_ENUM(_MaskStatusEnum)

        static MaskStatusEnum* instance();

    private:
        MaskStatusEnum() = default;
        Q_DISABLE_COPY_MOVE(MaskStatusEnum)
    };

} // namespace TOFD_PE
