#include "FileExists.hpp"
#include "qdebug.h"
#include <QDir>
#include <QFileInfo>
#include <QUrl>

namespace Morose::Utils {
    FileExists::FileExists(QObject* parent) :
    QObject{parent} {}

    FileExists* FileExists::Instance() {
        static FileExists inst;
        return &inst;
    }

    bool FileExists::isFileExists(const QString& fileName) const {
        return QFileInfo::exists(fileName);
    }

    bool FileExists::isDir(const QString& fileName) const {
        QFileInfo file(fileName);
        return file.isDir();
    }
} // namespace Morose::Utils
