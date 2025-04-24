#pragma once

#include <QObject>

class FileManagement : public QObject {
    Q_OBJECT
public:
    explicit FileManagement(QObject* parent = nullptr);
    static FileManagement*  Instance();
    Q_INVOKABLE bool        isFileExists(const QString& filepath) const;
    Q_INVOKABLE bool        isDir(const QString& dirpath) const;
    Q_INVOKABLE QStringList listDir(const QString& dirpath) const;
    Q_INVOKABLE bool        createDir(const QString& dirpath) const;
    Q_INVOKABLE QString     toAbsolutePath(const QString& filepath) const;
    Q_INVOKABLE QString     toQmlAbsolutePath(const QString& filepath) const;
    Q_INVOKABLE QString     resolveQmlFileDir(const QString& filepath) const;
    Q_INVOKABLE QString     getFilename(const QString& filepath) const;
    Q_INVOKABLE bool        deleteFile(const QString& filepath) const;
    Q_INVOKABLE bool        deletePath(const QString& path) const;
    Q_INVOKABLE bool        isDirExists(const QString& dirname) const;
signals:
};
