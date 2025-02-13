#pragma once

#include <QObject>

class FileManagement : public QObject {
    Q_OBJECT
public:
    explicit FileManagement(QObject* parent = nullptr);
    static FileManagement*  Instance();
    Q_INVOKABLE bool        isFileExists(const QString& fileName) const;
    Q_INVOKABLE bool        isDir(const QString& fileName) const;
    Q_INVOKABLE QStringList listDir(const QString& dirName) const;
    Q_INVOKABLE bool        createDir(const QString& dirName) const;
    Q_INVOKABLE QString     toAbsolutePath(const QString& filename) const;
    Q_INVOKABLE QString     toQmlAbsolutePath(const QString& filename) const;
    Q_INVOKABLE bool        deleteFile(const QString& filepath) const;
    Q_INVOKABLE bool        deletePath(const QString& path) const;
    Q_INVOKABLE bool        isDirExists(const QString& dirname) const;
signals:
};
