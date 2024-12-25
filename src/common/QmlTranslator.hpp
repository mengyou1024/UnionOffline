#pragma once

#include "morose_config.h"
#include <QtCore>

class QmlTranslator : public QObject {
    Q_OBJECT

public:
    static QmlTranslator* Instance();

    Q_INVOKABLE QVariantList languageNameList() const;
    int                      languageSelected() const;
    void                     setLanguageSelected(int newLanguageSelected);

signals:

    void languageSelectedChanged();

private:
    int          m_languageSelected = 0;
    QVariantList m_languageNames    = {
        "简体中文",
        "English",
    };

    QTranslator m_systemTranslator;

    QVector<QString> m_systemTranslateFileList = {
        "",
        "qt_en",
    };

    QTranslator m_projectTranslaotr;

    QVector<QString> m_projectTranslateFileList = {
        "",
        PROJECT_NAME "_en",
    };

    QmlTranslator(QObject* parent = nullptr);
    Q_DISABLE_COPY_MOVE(QmlTranslator)
    Q_PROPERTY(int languageSelected READ languageSelected WRITE setLanguageSelected NOTIFY languageSelectedChanged FINAL)
};
