#include "FileManagement.hpp"
#include <QDebug>
#include <QDir>
#include <QFileInfo>

FileManagement::FileManagement(QObject* parent) :
QObject{parent} {}

FileManagement* FileManagement::Instance() {
    static FileManagement inst;
    return &inst;
}

bool FileManagement::isFileExists(const QString& fileName) const {
    if (fileName.startsWith("file:///")) {
        return QFileInfo::exists(fileName.mid(8));
    }
    return QFileInfo::exists(fileName);
}

bool FileManagement::isDir(const QString& fileName) const {
    QFileInfo file(fileName);
    return file.isDir();
}

QStringList FileManagement::listDir(const QString& dirName) const {
    QDir dir(dirName);
    return dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
}

bool FileManagement::createDir(const QString& _dirName) const {
    QString dirName = _dirName;
    if (dirName.startsWith("file:///")) {
        dirName = dirName.mid(8);
    }
    QDir dir(dirName);

    if (!dir.exists()) {
        qDebug() << "make path:" << dir.absolutePath();
        return dir.mkpath(dir.absolutePath());
    }
    return false;
}

QString FileManagement::toAbsolutePath(const QString& filename) const {
    QFileInfo file(filename);
    return file.absoluteFilePath();
}

QString FileManagement::toQmlAbsolutePath(const QString& filename) const {
    return "file:///" + toAbsolutePath(filename);
}

bool FileManagement::deleteFile(const QString& filepath) const {
    QFile file(filepath);
    if (file.exists()) {
        file.remove();
    }
    return true;
}

bool FileManagement::deletePath(const QString& path) const {
    if (!isFileExists(path)) {
        qWarning() << "file:" << path << "not exists!";
    }
    if (isDir(path)) {
        return QDir(path).removeRecursively();
    } else {
        return QFile(path).remove();
    }
}

bool FileManagement::isDirExists(const QString& dirname) const {
    return QDir().exists(dirname);
}
