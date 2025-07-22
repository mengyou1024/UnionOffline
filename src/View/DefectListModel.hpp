#pragma once

#include "BScanView.hpp"
#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QObject>

namespace Union::Model {
    using View::DefectItem;

    class DefectListModel : public QAbstractListModel {
        Q_OBJECT
        QML_ELEMENT

        QPointer<QQuickItem> m_viewItem = nullptr;

        int     m_defectCount  = 0;
        int     m_currentIndex = 0;
        double  m_horLen       = 0;
        double  m_verLen       = 0;
        double  m_ampValue     = 0;
        double  m_ampX         = 0;
        double  m_ampY         = 0;
        double  m_s1           = 0;
        double  m_s2           = 0;
        QString m_h            = {};
        QString m_aMax         = {};
        QString m_area         = {};

    public:
        enum RoleEnum {
            HOR_LEN   = 0,
            VER_LEN   = 1,
            AMP_VALUE = 2,
            AMP_X     = 3,
            AMP_Y     = 4,
            S1        = 5,
            S2        = 6,
            H         = 7,
            A_MAX     = 8,
            AREA      = 9,
        };

        int                    rowCount(const QModelIndex& = QModelIndex()) const override;
        QVariant               data(const QModelIndex& index, int role) const override;
        QHash<int, QByteArray> roleNames() const override;
        QQuickItem*            viewItem() const;
        void                   setViewItem(QQuickItem* newViewItem);

        int     defectCount() const;
        void    setDefectCount(int newDefectCount);
        int     currentIndex() const;
        void    setCurrentIndex(int newCurrentIndex);
        double  horLen() const;
        void    setHorLen(double newHorLen);
        double  verLen() const;
        void    setVerLen(double newVerLen);
        double  ampValue() const;
        void    setAmpValue(double newAmpValue);
        double  ampX() const;
        void    setAmpX(double newAmpX);
        double  ampY() const;
        void    setAmpY(double newAmpY);
        void    updateParam();
        double  s1() const;
        void    setS1(double newS1);
        double  s2() const;
        void    setS2(double newS2);
        QString h() const;
        void    setH(const QString& newH);
        QString aMax() const;
        void    setAMax(const QString& newAMax);
        QString area() const;
        void    setArea(const QString& newArea);

    public slots:
        void updateModel();

    signals:
        void viewItemChanged();
        void defectCountChanged();
        void currentIndexChanged();
        void horLenChanged();
        void verLenChanged();
        void ampValueChanged();
        void ampXChanged();
        void ampYChanged();
        void s1Changed();
        void s2Changed();
        void hChanged();
        void aMaxChanged();
        void areaChanged();

    private:
        std::vector<View::DefectItem> _defect_list = {};
        QPointer<View::BScanView>     _view        = nullptr;

        Q_PROPERTY(QQuickItem* viewItem READ viewItem WRITE setViewItem NOTIFY viewItemChanged FINAL)
        Q_PROPERTY(int defectCount READ defectCount WRITE setDefectCount NOTIFY defectCountChanged FINAL)
        Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged FINAL)
        Q_PROPERTY(double horLen READ horLen WRITE setHorLen NOTIFY horLenChanged FINAL)
        Q_PROPERTY(double verLen READ verLen WRITE setVerLen NOTIFY verLenChanged FINAL)
        Q_PROPERTY(double ampValue READ ampValue WRITE setAmpValue NOTIFY ampValueChanged FINAL)
        Q_PROPERTY(double ampX READ ampX WRITE setAmpX NOTIFY ampXChanged FINAL)
        Q_PROPERTY(double ampY READ ampY WRITE setAmpY NOTIFY ampYChanged FINAL)
        Q_PROPERTY(double s1 READ s1 WRITE setS1 NOTIFY s1Changed FINAL)
        Q_PROPERTY(double s2 READ s2 WRITE setS2 NOTIFY s2Changed FINAL)
        Q_PROPERTY(QString h READ h WRITE setH NOTIFY hChanged FINAL)
        Q_PROPERTY(QString aMax READ aMax WRITE setAMax NOTIFY aMaxChanged FINAL)
        Q_PROPERTY(QString area READ area WRITE setArea NOTIFY areaChanged FINAL)
    };
} // namespace Union::Model
