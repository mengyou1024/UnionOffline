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

bool FileManagement::isFileExists(const QString& filepath) const {
    if (filepath.startsWith("file:///")) {
        return QFileInfo::exists(filepath.mid(8));
    }
    return QFileInfo::exists(filepath);
}

bool FileManagement::isDir(const QString& dirpath) const {
    QFileInfo file(dirpath);
    return file.isDir();
}

QStringList FileManagement::listDir(const QString& dirpath) const {
    QDir dir(dirpath);
    return dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
}

bool FileManagement::createDir(const QString& dirpath) const {
    QDir dir(dirpath.startsWith("file:///") ? dirpath.mid(8) : dirpath);

    if (!dir.exists()) {
        qDebug() << "make path:" << dir.absolutePath();
        return dir.mkpath(dir.absolutePath());
    }
    return false;
}

QString FileManagement::toAbsolutePath(const QString& filepath) const {
    QFileInfo file(filepath);
    return file.absoluteFilePath();
}

QString FileManagement::toQmlAbsolutePath(const QString& filepath) const {
    return "file:///" + toAbsolutePath(filepath);
}

QString FileManagement::resolveQmlFileDir(const QString& filepath) const {
    QFileInfo file_info(filepath.startsWith("file:///") ? filepath.mid(8) : filepath);
    return "file:///" + file_info.absolutePath();
}

QString FileManagement::getFilename(const QString& filepath) const {
    QFileInfo file_info(filepath);
    return file_info.fileName();
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

bool FileManagement::isDirExists(const QString& dirpath) const {
    return QDir().exists(dirpath);
}
