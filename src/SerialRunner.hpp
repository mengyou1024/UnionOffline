#pragma once
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QRunnable>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSettings>

namespace Union::Extra {
    class _Runner : public QObject, public QRunnable {
        Q_OBJECT
        QString m_fileName = {};

    public:
        explicit _Runner(const QString& portName, int waitTime_ms, QObject* parent = nullptr) :
        QObject(parent),
        m_portName(portName),
        m_waitTime(waitTime_ms) {}
        void setParam(const QString& portName, int waitTime_ms);
    signals:
        void runDone(QString fileName);

    protected:
        void run() override;

    private:
        void    _run();
        QString m_portName = {};
        int     m_waitTime = 0;
    };

    class SerialRuner : public QObject {
        Q_OBJECT

    public:
        Q_INVOKABLE void         readSerialAndSaveFile(const QString& portName, int waitTime_ms);
        Q_INVOKABLE QVariantList listSerialName();
        QString                  fileName() const;
        void                     setFileName(const QString& newFileName);

        bool isRunning() const;
        void setIsRunning(bool newIsRunning);

    signals:
        void fileNameChanged();

        void isRunningChanged();

    private:
        QString                  m_portName  = {};
        int                      m_waitTime  = 0;
        QString                  m_fileName  = {};
        bool                     m_isRunning = {};
        std::shared_ptr<_Runner> m_runner    = nullptr;
        Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged FINAL)
        Q_PROPERTY(bool isRunning READ isRunning WRITE setIsRunning NOTIFY isRunningChanged FINAL)
    };

} // namespace Union::Extra
