#pragma once

#include <QObject>
#include <QStringList>

namespace Morose::Utils {

    class FileReader : public QObject {
        Q_OBJECT
        QString m_fileName = {};
        QString m_string   = {};

    public:
        explicit FileReader(QObject* parent = nullptr);

        QString        fileName() const;
        void           setFileName(const QString& newFileName);
        const QString& string() const;
        void           setString(const QString& newString);

    signals:
        void fileNameChanged();

        void stringChanged();

    private:
        Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged FINAL)
        Q_PROPERTY(QString string READ string WRITE setString NOTIFY stringChanged FINAL)
    };
} // namespace Morose::Utils
