#pragma once

#include <QObject>

namespace TOFD_PE {
    class LinesMakeEnum : public QObject {
        Q_OBJECT
    public:
        enum class CursorType {
            Normal    = 0,
            ModeX1    = 0b1,
            ModeX2    = 0b10,
            ModeY1    = 0b100,
            ModeY2    = 0b1000,
            ModeX1_Y1 = ModeX1 | ModeY1,
            ModeX1_Y2 = ModeX1 | ModeY2,
            ModeX2_Y1 = ModeX2 | ModeX1,
            ModeX2_Y2 = ModeX2 | ModeY2,
        };
        Q_ENUM(CursorType)

        static LinesMakeEnum* Instance();

    private:
        LinesMakeEnum() = default;
        Q_DISABLE_COPY_MOVE(LinesMakeEnum)
    };

} // namespace TOFD_PE
