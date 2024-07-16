#include "FileReader.hpp"
#include <QFile>
namespace Morose::Utils {

    FileReader::FileReader(QObject* parent) :
    QObject(parent) {
        connect(this, &FileReader::fileNameChanged, this, [this]() {
            QFile file(m_fileName);
            if (file.open(QIODevice::ReadOnly)) {
                setString(file.readAll());
            } else {
                setString("");
            }
        });
    }

    const QString& FileReader::string() const {
        return m_string;
    }

    void FileReader::setString(const QString& newString) {
        if (m_string == newString)
            return;
        m_string = newString;
        emit stringChanged();
    }

    QString FileReader::fileName() const {
        return m_fileName;
    }

    void FileReader::setFileName(const QString& newFileName) {
        if (m_fileName == newFileName)
            return;
        m_fileName = newFileName;
        emit fileNameChanged();
    }

} // namespace Morose::Utils
