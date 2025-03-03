#include "SerialRunner.hpp"
#include <QLoggingCategory>
#include <QTemporaryFile>
#include <QThreadPool>
static Q_LOGGING_CATEGORY(TAG, "SerialRunner");

namespace Union::Extra {

    void SerialRuner::readSerialAndSaveFile(const QString& portName, int waitTime_ms) {
        m_portName = portName;
        m_waitTime = waitTime_ms;

        if (m_runner == nullptr) {
            static auto instance = std::make_shared<_Runner>(portName, waitTime_ms);
            m_runner             = instance;
            m_runner->setAutoDelete(false);
            connect(m_runner.get(), &_Runner::runDone, this, [&](QString fileName) {
                m_fileName = fileName;
                qCDebug(TAG) << "recv event runDone, fileName:" << m_fileName;
                emit fileNameChanged();
                setIsRunning(false);
            });
        } else {
            m_runner->setParam(portName, waitTime_ms);
        }

        if (!m_isRunning) {
            setIsRunning(true);
            QThreadPool::globalInstance()->start(m_runner.get());
        } else {
            qCWarning(TAG) << "serial thread already running";
        }
    }

    QVariantList SerialRuner::listSerialName() {
        QVariantList ret;
        auto         ports = QSerialPortInfo::availablePorts();
        for (const auto& port : qAsConst(ports)) {
            ret.push_back(port.description() + "(" + port.portName() + ")");
        }
        qCDebug(TAG) << ret;
        return ret;
    }

    QString SerialRuner::fileName() const {
        return m_fileName;
    }

    void SerialRuner::setFileName(const QString& newFileName) {
        if (m_fileName == newFileName)
            return;
        m_fileName = newFileName;
        emit fileNameChanged();
    }

    bool SerialRuner::isRunning() const {
        return m_isRunning;
    }

    void SerialRuner::setIsRunning(bool newIsRunning) {
        if (m_isRunning == newIsRunning)
            return;
        m_isRunning = newIsRunning;
        emit isRunningChanged();
    }

    void _Runner::setParam(const QString& portName, int waitTime_ms) {
        m_portName = portName;
        m_waitTime = waitTime_ms;
    }

    void _Runner::run() {
        _run();
        emit runDone(m_fileName);
    }

    void _Runner::_run() {
        qCDebug(TAG) << "thread run";
        QSerialPort port(m_portName);
        if (!port.open(QIODevice::ReadWrite)) {
            m_fileName = "";
            return;
        }

        QSettings cacheSetting("setting.ini", QSettings::IniFormat);
        cacheSetting.beginGroup("Cache");
        auto cachePath = cacheSetting.value("dir");
        if (cachePath.isNull()) {
            m_fileName = "";
            return;
        }

        QByteArray readed;
        auto       read_times = 0;
        bool       valid      = false;
        do {
            port.write("\x30", 1);
            if (port.waitForReadyRead(100)) {
                valid = true;
                readed += port.readAll();
                while (port.waitForReadyRead(100)) {
                    readed += port.readAll();
                }
                break;
            }
            read_times++;
        } while (read_times < m_waitTime / 100);
        if (valid == false) {
            m_fileName = "";
            return;
        }
        QTemporaryFile tempFile;
        tempFile.setFileTemplate(cachePath.toString() + "/XXXXXX.cod");
        tempFile.open();
        tempFile.setAutoRemove(false);
        tempFile.write(readed);
        m_fileName = tempFile.fileName();
        qInfo(TAG) << "save file:" << m_fileName;
    }

} // namespace Union::Extra
