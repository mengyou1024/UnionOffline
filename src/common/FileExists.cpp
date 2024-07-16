#include "FileExists.hpp"
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

    QStringList FileExists::listDir(const QString& dirName) const {
        QDir dir(dirName);
        return dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    }
} // namespace Morose::Utils
