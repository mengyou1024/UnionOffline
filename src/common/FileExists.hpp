#pragma once

#include <QObject>

namespace Morose::Utils {

    class FileExists : public QObject {
        Q_OBJECT
    public:
        Q_DISABLE_COPY_MOVE(FileExists)
        explicit FileExists(QObject* parent = nullptr);
        static FileExists* Instance();
        Q_INVOKABLE bool   isFileExists(const QString& fileName) const;
        Q_INVOKABLE bool   isDir(const QString& fileName) const;
    };
} // namespace Morose::Utils
