#pragma once

#include <QObject>

namespace TofdPe {
    class MaskStatusEnum : public QObject {
        Q_OBJECT
    public:
        enum class _MaskStatusEnum {
            Normal          = 0,
            RmThroughWave   = 0b1,
            PullThroughWave = 0b10,
        };
        Q_ENUM(_MaskStatusEnum)

        static MaskStatusEnum* Instance();

    private:
        MaskStatusEnum() = default;
        Q_DISABLE_COPY_MOVE(MaskStatusEnum)
    };

} // namespace TofdPe
