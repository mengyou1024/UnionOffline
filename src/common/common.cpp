#include "common.hpp"
#include "AScanInteractor.hpp"
#include "CImage.hpp"
#include "FileExists.hpp"
#include "FileManagement.hpp"
#include "FileReader.hpp"
#include "GlobalCppProgress.hpp"
#include "LinesMaskEnum.hpp"
#include "MaskStatusEnum.hpp"
#include "QmlConditionalVariable.hpp"
#include "QmlTranslator.hpp"
#include "RailWeldDigram.hpp"
#include "SerialRunner.hpp"
#include "TofdPeAScanView.hpp"
#include "TofdPeDScanView.hpp"
#include "morose_config.h"
#include <AScanView.hpp>
#include <AppUpdater.hpp>
#include <BScanView.hpp>
#include <BasicView.hpp>
#include <CScanView.hpp>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QQmlEngine>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <RailWeldSimulation.hpp>
#include <UnionType>

static Q_LOGGING_CATEGORY(TAG, "Morose.com");

void Morose::logMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    static QMutex mutex;
    static int    maxCatelogyLen = 12;
    int           categoryLen    = QString(context.category).length();
    static int    maxMsgLen      = 0;
    int           msgLen         = msg.length();
    if (categoryLen > maxCatelogyLen) {
        maxCatelogyLen = categoryLen;
    }
    if (msgLen > maxMsgLen) {
        maxMsgLen = msgLen;
    }
    QString logLevel;
    auto    temp        = QString("[%1]").arg(QString(context.category)).toStdString();
    QString logCategory = QString::asprintf("%-*s", maxCatelogyLen + 2, temp.c_str());

#ifdef QT_DEBUG
    switch (type) {
        case QtDebugMsg: {
            logLevel = QString("\033[49m[D]\033[0m");
            break;
        }
        case QtWarningMsg: {
            logLevel = QString("\033[43m[W]\033[0m");
            break;
        }
        case QtCriticalMsg: {
            logLevel = QString("\033[45m[E]\033[0m");
            break;
        }
        case QtFatalMsg: {
            logLevel = QString("\033[41m[F]\033[0m");
            break;
        }
        case QtInfoMsg: {
            logLevel = QString("\033[42m[I]\033[0m");
            break;
        }
    }
#else
    switch (type) {
        case QtDebugMsg: {
            logLevel = QString("[D]");
            break;
        }
        case QtWarningMsg: {
            logLevel = QString("[W]");
            break;
        }
        case QtCriticalMsg: {
            logLevel = QString("[E]");
            break;
        }
        case QtFatalMsg: {
            logLevel = QString("[F]");
            break;
        }
        case QtInfoMsg: {
            logLevel = QString("[I]");
            break;
        }
    }
#endif

    QString logTimeInfo = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz]");
    QString message     = {};
#if ENABLE_DEBUG_FUNCTION
    if (type == QtDebugMsg) {
        message = QString("%1 %2 %3 □□□%4■■■%5").arg(logTimeInfo, logLevel, logCategory, context.function, QString("%1%2").arg(msg, QString(maxMsgLen - msgLen, ' ')));
    } else {
        message = QString("%1 %2 %3 %4").arg(logTimeInfo, logLevel, logCategory, QString("%1%2").arg(msg, QString(maxMsgLen - msgLen, ' ')));
    }
#else
    message = QString("%1 %2 %3 %4").arg(logTimeInfo, logLevel, logCategory, QString("%1%2").arg(msg, QString(maxMsgLen - msgLen, ' ')));
#endif
    message.replace("\n", QString("\n") + QString(33 + maxCatelogyLen, ' '));

#if defined(QT_DEBUG)
    mutex.lock();
    printf("%s\n", message.toLocal8Bit().toStdString().c_str());
    fflush(stdout);
    mutex.unlock();
#else
    QFile file("./log/log.txt");
    mutex.lock();
    if (file.size() > 1024 * 1024 * 50) {
        QFile::remove("log/log.txt.bak");
        QFile::rename("log/log.txt", "log/log.txt.bak");
    }
    file.open(QFile::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\n";
    file.flush();
    file.close();
    mutex.unlock();
#endif
}

void Morose::registerVariable(QQmlContext* context) {
    context->setContextProperty("MOROSE_APP_VERSION", APP_VERSION);
    context->setContextProperty("MOROSE_APP_NAME_ZH_CN", APP_NAME_ZH_CN_TR);
    context->setContextProperty("MOROSE_APP_COMMIT_HASH", APP_COMMIT_HASH);
    context->setContextProperty("MOROSE_APP_ENABLE_UPGRADE", MOROSE_ENABLE_UPGRADE_FEATURE);
    context->setContextProperty("MOROSE_ENABLE_INSTRUMENT_COMMUNICATION_FEATURE", MOROSE_ENABLE_INSTRUMENT_COMMUNICATION_FEATURE);
    context->setContextProperty("MOROSE_ENABLE_MULTI_LANGUATE_FEATURE", MOROSE_ENABLE_MULTI_LANGUATE_FEATURE);
    context->setContextProperty("MOROSE_ENABLE_SETTING_FEATURE", MOROSE_ENABLE_SETTING_FEATURE);
    qmlRegisterType<AScanInteractor>("Union.Interactor", 1, 0, "AScanInteractor");
    qmlRegisterType<TofdPe::TofdPeDScanView>("Union.TofdPe", 1, 0, "TofdPeDScanView");
    qmlRegisterType<TofdPe::TofdPeAScanView>("Union.TofdPe", 1, 0, "TofdPeAScanView");
    qmlRegisterType<TofdPe::TofdPeInteractor>("Union.TofdPe", 1, 0, "TofdPeIntr");
    qmlRegisterType<Union::Extra::SerialRuner>("Union.Extra", 1, 0, "SerialRunner");
    qmlRegisterType<AScan::CImage>("Union.AScan", 1, 0, "CImage");
    qmlRegisterType<Union::AScan::RailWeld::RailWeldDigram>("Union.AScan", 1, 0, "RailWeldDigram");
    qmlRegisterType<Morose::Utils::QmlConditionalVariable>("Morose.Utils", 1, 0, "Conditional");
    qmlRegisterType<Morose::Utils::FileReader>("Morose.Utils", 1, 0, "FileReader");
    qmlRegisterType<Union::View::BasicView>("Union.View", 1, 0, "BasicView");
    qmlRegisterType<Union::View::AScanView>("Union.View", 1, 0, "AScanView");
    qmlRegisterType<Union::View::BScanView>("Union.View", 1, 0, "BScanView");
    qmlRegisterType<Union::View::CScanView>("Union.View", 1, 0, "CScanView");
    qmlRegisterType<Union::AScan::RailWeld::RailWeldSimulation>("Union.AScan", 1, 0, "RailWeldSimulation");
    qmlRegisterSingletonInstance("Union.TofdPe", 1, 0, "LinesMaskEnum", TofdPe::LinesMakeEnum::Instance());
    qmlRegisterSingletonInstance("Union.TofdPe", 1, 0, "MaskStatusEnum", TofdPe::MaskStatusEnum::Instance());
    qmlRegisterSingletonInstance("Morose.Utils", 1, 0, "GlobalCppProgress", Morose::Utils::GlobalCppProgress::Instance());
    qmlRegisterSingletonInstance("Morose.Utils", 1, 0, "FileExists", Morose::Utils::FileExists::Instance());
    qmlRegisterSingletonInstance("Morose.Utils", 1, 0, "AppUpdater", Morose::Utils::AppUpdater::Instance());
    qmlRegisterSingletonInstance("Union.Utils", 1, 0, "FileManagement", FileManagement::Instance());
    registeAllAScanFileSelector();
    registeAllTofdPeFileSelector();
    registNameFilter(context);
    // 翻译
    auto translatorInstance = QmlTranslator::Instance();
    qmlRegisterSingletonInstance("Union.Utils", 1, 0, "Translator", translatorInstance);
    QObject::connect(translatorInstance, &QmlTranslator::languageSelectedChanged, context->engine(), [=]() {
        qDebug() << "languageChanged";
        context->engine()->retranslate();
    });
}

void Morose::registeAllAScanFileSelector() {
    [[maybe_unused]]
    auto _ascan_register = ::Union::UniversalApparatus::AScan::AScanFileSelector::Instance();
    using namespace ::Union::UniversalApparatus::AScan::Instance;
#if MOROSE_ENABLE_DAS_DAT || defined(QT_DEBUG)
    _ascan_register->RegistReader("*.das", "N系列单幅图像", DASType::FromFile);
    _ascan_register->RegistReader("*.DAT", "N系列连续图像", DATType::FromFile);
    _ascan_register->RegistReader("*.DAT", "R系列连续图像", DATType_R::FromFile);
#endif

#if MOROSE_ENABLE_COD || defined(QT_DEBUG)
    _ascan_register->RegistReader("*.cod", "N系列串口数据", Serial_330::FromFile);
#endif

#if MOROSE_ENABLE_MDAT || defined(QT_DEBUG)
    _ascan_register->RegistReader("*.mdat", "390N、T8图像", Union::UniversalApparatus::AScan::Instance::UnType::FromFile);
#endif

#if MOROSE_ENABLE_DAA_HFD || defined(QT_DEBUG)
    _ascan_register->RegistReader("*.daa", "390钢轨焊缝单幅图像", DAAType::FromFile);
    _ascan_register->RegistReader("*.HFD", "390钢轨焊缝连续图像", HFDATType::FromFile);
#endif
}

void Morose::registeAllTofdPeFileSelector() {
    [[maybe_unused]]
    auto _TofdPe_register = ::Union::UniversalApparatus::TofdPe::TofdPeFileSelector::Instance();
    using namespace ::Union::UniversalApparatus::TofdPe::Instance;

#if MOROSE_ENABLE_TOF_TPE || defined(QT_DEBUG)
    _TofdPe_register->RegistReader("*.tpe", "TOFD/PE图像", TpeType::FromFile);
    _TofdPe_register->RegistReader("*.tof", "TOFD/PE图像", TofType::FromFile);
#endif
}

QJsonObject& Morose::getGlobalEnvironment() {
    static QJsonObject obj;
    return obj;
}

QJsonObject& Morose::loadGlobalEnvironment() {
    QFile file("Config.json");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QJsonParseError err;
        QJsonDocument   doc = QJsonDocument::fromJson(file.readAll(), &err);
        if (doc.isObject()) {
            getGlobalEnvironment() = doc.object();
            qCInfo(TAG) << ("Config:\n" + doc.toJson(QJsonDocument::JsonFormat::Indented).toStdString()).c_str();
        } else {
            qCWarning(TAG) << err.errorString();
        }
        file.close();

    } else {
        qCWarning(TAG) << "open file: Config.json failed!";
    }
    return getGlobalEnvironment();
}

void Morose::registNameFilter(QQmlContext* context) {
    using namespace ::Union::UniversalApparatus::AScan;
    using namespace ::Union::UniversalApparatus::TofdPe;

    QVariantList folderListModel_nameFilter;
    QVariantList filedialog_nameFilter;
    QVariantMap  mainUi_map;

    filedialog_nameFilter.push_back("所有文件 (*.*)");
    // AScan
    mainUi_map.insert(AScanFileSelector::Instance()->GetUINameMap().toVariantMap());
    folderListModel_nameFilter += AScanFileSelector::Instance()->GetFileListModelNameFilter().toVariantList();
    filedialog_nameFilter += AScanFileSelector::Instance()->GetFileNameFilter().toVariantList();

    // TofdPe
    mainUi_map.insert(TofdPeFileSelector::Instance()->GetUINameMap().toVariantMap());
    folderListModel_nameFilter += TofdPeFileSelector::Instance()->GetFileListModelNameFilter().toVariantList();
    filedialog_nameFilter += TofdPeFileSelector::Instance()->GetFileNameFilter().toVariantList();

    context->setContextProperty("FOLDERLISTMODEL_NAMEFILTER", folderListModel_nameFilter);
    context->setContextProperty("FILEDIALOG_NAMEFILTER", filedialog_nameFilter);
    context->setContextProperty("MAINUI_MAP", mainUi_map);
    qDebug() << "FOLDERLISTMODEL_NAMEFILTER:" << folderListModel_nameFilter;
    qDebug() << "FILEDIALOG_NAMEFILTER:" << filedialog_nameFilter;
    qDebug() << "MAINUI_MAP:" << mainUi_map;
}

std::string Morose::FormatCompilerDATEString(const char* str) {
    std::string dateString(str);

    // 分割日期字符串
    std::string month = dateString.substr(0, 3);
    std::string day   = dateString.substr(4, 2);
    std::string year  = dateString.substr(7, 4);

    // clang-format off
           // 将月份转换为数字
    std::map<std::string, int> monthMap = {
        {"Jan", 1 }, {"Feb", 2 }, {"Mar", 3 }, {"Apr", 4 },
        {"May", 5 }, {"Jun", 6 }, {"Jul", 7 }, {"Aug", 8 },
        {"Sep", 9 }, {"Oct", 10}, {"Nov", 11}, {"Dec", 12},
    };
    // clang-format on

    int monthNumber = monthMap[month];

    // 格式化日期
    return std::format("{}-{:02d}-{:02d}", year, monthNumber, std::stoi(day));
}
