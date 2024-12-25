#include "QmlTranslator.hpp"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(TAG, "translator");

namespace {
    constexpr auto CONFIG_FILE_NAME   = "setting.ini";
    constexpr auto CONFIG_GROUP_NAME  = "language";
    constexpr auto CONFIG_PARAM_FIELD = "selected";
} // namespace

QmlTranslator::QmlTranslator(QObject* parent) :
QObject(parent) {
    QSettings languageSetting(CONFIG_FILE_NAME, QSettings::IniFormat);
    languageSetting.beginGroup(CONFIG_GROUP_NAME);
    auto language_selected = languageSetting.value(CONFIG_PARAM_FIELD);
    if (language_selected.isNull()) {
        QLocale locale;
        if (locale.language() == QLocale::Chinese) {
            setLanguageSelected(0);
        } else {
            setLanguageSelected(1);
        }
    } else {
        setLanguageSelected(language_selected.toInt());
    }
}

QVariantList QmlTranslator::languageNameList() const {
    return m_languageNames;
}

QmlTranslator* QmlTranslator::Instance() {
    static QmlTranslator inst;
    return &inst;
}

int QmlTranslator::languageSelected() const {
    return m_languageSelected;
}

void QmlTranslator::setLanguageSelected(int newLanguageSelected) {
    if (m_languageSelected == newLanguageSelected)
        return;
    m_languageSelected = newLanguageSelected;

    // 卸载之前的翻译文件
    if (!m_systemTranslator.isEmpty()) {
        qApp->removeTranslator(&m_systemTranslator);
    }
    if (!m_projectTranslaotr.isEmpty()) {
        qApp->removeTranslator(&m_projectTranslaotr);
    }
    try {
        // 加载新的翻译文件
        auto is_ok = m_systemTranslator.load(m_systemTranslateFileList.at(newLanguageSelected), "translations");
        qCDebug(TAG) << "load system translate file:" << is_ok;
        is_ok = m_systemTranslator.load(m_projectTranslateFileList.at(newLanguageSelected), "translations");
        qCDebug(TAG) << "load project translate file:" << is_ok;
    } catch (std::exception& e) {
        qCWarning(TAG) << e.what();
    }
    // 安装新的翻译文件
    if (!m_systemTranslator.isEmpty()) {
        qApp->installTranslator(&m_systemTranslator);
    }
    if (!m_projectTranslaotr.isEmpty()) {
        qApp->installTranslator(&m_projectTranslaotr);
    }

    // 修改配置文件
    QSettings languageSetting(CONFIG_FILE_NAME, QSettings::IniFormat);
    languageSetting.beginGroup(CONFIG_GROUP_NAME);
    languageSetting.setValue(CONFIG_PARAM_FIELD, languageSelected());
    languageSetting.endGroup();
    languageSetting.sync();

    emit languageSelectedChanged();
}
