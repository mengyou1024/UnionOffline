#include "DefectListModel.hpp"

#include <union_common.hpp>

using namespace ::Union::Common;

namespace Union::Model {

    QQuickItem* DefectListModel::viewItem() const {
        return m_viewItem;
    }

    void DefectListModel::setViewItem(QQuickItem* newViewItem) {
        if (m_viewItem == newViewItem)
            return;

        _view      = dynamic_cast<View::BScanView*>(newViewItem);
        m_viewItem = newViewItem;
        emit viewItemChanged();
        updateModel();
    }

    void DefectListModel::updateModel() {
        beginResetModel();
        if (_view == nullptr) {
            _defect_list.clear();
        } else {
            _defect_list = _view->defect_list_;
        }

        setDefectCount(std::ssize(_defect_list));
        setCurrentIndex(std::ssize(_defect_list) > 0 ? 0 : -1);

        endResetModel();
        updateParam();
    }

    int DefectListModel::defectCount() const {
        return m_defectCount;
    }

    void DefectListModel::setDefectCount(int newDefectCount) {
        if (m_defectCount == newDefectCount)
            return;
        m_defectCount = newDefectCount;
        emit defectCountChanged();
    }

    int DefectListModel::currentIndex() const {
        return m_currentIndex;
    }

    void DefectListModel::setCurrentIndex(int newCurrentIndex) {
        if (m_currentIndex == newCurrentIndex)
            return;
        m_currentIndex = newCurrentIndex;
        emit currentIndexChanged();
        updateParam();
    }

    double DefectListModel::horLen() const {
        return m_horLen;
    }

    void DefectListModel::setHorLen(double newHorLen) {
        if (qFuzzyCompare(m_horLen, newHorLen))
            return;
        m_horLen = newHorLen;
        emit horLenChanged();
    }

    double DefectListModel::verLen() const {
        return m_verLen;
    }

    void DefectListModel::setVerLen(double newVerLen) {
        if (qFuzzyCompare(m_verLen, newVerLen))
            return;
        m_verLen = newVerLen;
        emit verLenChanged();
    }

    double DefectListModel::ampValue() const {
        return m_ampValue;
    }

    void DefectListModel::setAmpValue(double newAmpValue) {
        if (qFuzzyCompare(m_ampValue, newAmpValue))
            return;
        m_ampValue = newAmpValue;
        emit ampValueChanged();
    }

    double DefectListModel::ampX() const {
        return m_ampX;
    }

    void DefectListModel::setAmpX(double newAmpX) {
        if (qFuzzyCompare(m_ampX, newAmpX))
            return;
        m_ampX = newAmpX;
        emit ampXChanged();
    }

    double DefectListModel::ampY() const {
        return m_ampY;
    }

    void DefectListModel::setAmpY(double newAmpY) {
        if (qFuzzyCompare(m_ampY, newAmpY))
            return;
        m_ampY = newAmpY;
        emit ampYChanged();
    }

    void DefectListModel::updateParam() {
        if (_view.isNull() || std::size(_defect_list) == 0 || currentIndex() < 0 || currentIndex() >= std::ssize(_defect_list)) {
            setHorLen(0);
            setVerLen(0);
            setAmpValue(0);
            setAmpX(0);
            setAmpY(0);
            return;
        }

        setHorLen(KeepDecimals<1>(_defect_list.at(currentIndex()).hor_len));
        setVerLen(KeepDecimals<1>(_defect_list.at(currentIndex()).ver_len));
        setAmpValue(KeepDecimals<1>(_defect_list.at(currentIndex()).max_amp));
        setAmpX(KeepDecimals<1>(_defect_list.at(currentIndex()).pos.x()));
        setAmpY(KeepDecimals<1>(_defect_list.at(currentIndex()).pos.y()));
    }

    int DefectListModel::rowCount(const QModelIndex&) const {
        return std::ssize(_defect_list);
    }

    QVariant DefectListModel::data(const QModelIndex& index, int role) const {
        const auto row = index.row();
        if (row >= std::ssize(_defect_list) || _view == nullptr) {
            return {};
        }
        switch (role) {
            case HOR_LEN: {
                return KeepDecimals<1>(_defect_list.at(row).hor_len);
            }
            case VER_LEN: {
                return KeepDecimals<1>(_defect_list.at(row).ver_len);
            }
            case AMP_VALUE: {
                return KeepDecimals<1>(_defect_list.at(row).max_amp);
            }
            case AMP_X: {
                return KeepDecimals<1>(_defect_list.at(row).pos.x());
            }
            case AMP_Y: {
                return KeepDecimals<1>(_defect_list.at(row).pos.y());
            }
            default: {
                break;
            }
        }
        return {};
    }

    QHash<int, QByteArray> DefectListModel::roleNames() const {
        return {
            {HOR_LEN,   "defect_hor_len"  },
            {VER_LEN,   "defect_ver_len"  },
            {AMP_VALUE, "defect_amp_value"},
            {AMP_X,     "defect_amp_x"    },
            {AMP_Y,     "defect_amp_y"    },
        };
    }

} // namespace Union::Model
