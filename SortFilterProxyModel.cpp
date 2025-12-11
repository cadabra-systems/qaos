#include "SortFilterProxyModel.hpp"

namespace Qaos {
	SortFilterProxyModel::SortFilterProxyModel(QObject* parent)
	:
		QSortFilterProxyModel(parent)
	{
		QObject::connect(&_role_list, &AbstractPropertyList::resized, this, &SortFilterProxyModel::onRoleListResized);
		QObject::connect(&_role_list, &AbstractPropertyList::updated, this, &SortFilterProxyModel::onRoleListUpdated);
	}

	void SortFilterProxyModel::validate()
	{
		_role_hash = roleNames();
		emit validated();
	}

	void SortFilterProxyModel::validate(DynamicRole* role)
	{
		if (role) {
			_role_hash.insert(SortFilterProxyModel::roleNames());
			role->_role_index = getRoleHashKey(role->getRoleName());
		}
	}

	QAbstractItemModel* SortFilterProxyModel::getSource()
	{
		return QSortFilterProxyModel::sourceModel();
	}

	QString SortFilterProxyModel::getPattern() const
	{
		return QSortFilterProxyModel::filterRegularExpression().pattern();
	}

	QQmlListProperty<DynamicRole> SortFilterProxyModel::getRoleList()
	{
		return _role_list;
	}

	int SortFilterProxyModel::getRoleHashKey(const QString& name)
	{
		for (QHash<int, QByteArray>::ConstIterator r = _role_hash.constBegin(); r != _role_hash.constEnd(); ++r) {
			if (name == r.value()) {
				return r.key();
			}
		}
		return -1;
	}

	QHash<int, QByteArray> SortFilterProxyModel::roleNames() const
	{
		QHash<int, QByteArray> retval(QSortFilterProxyModel::roleNames());
		for (int i = 0; i < _role_list.length(); i++) {
			retval[MaxDataRole - i] = _role_list.at(i)->_role_name.toUtf8();
		}
		return retval;
	}

	QVariant SortFilterProxyModel::data(const QModelIndex& index, int role) const
	{
		if (role == std::clamp(role, MaxDataRole - _role_list.length(), MaxDataRole)) {
			return _role_list.at(MaxDataRole - role)->getRoleValue(index.row());
		} else {
			QHash<int,QByteArray> rolenames(QSortFilterProxyModel::roleNames());
			return QSortFilterProxyModel::data(index, role);
		}
	}

	void SortFilterProxyModel::setSource(QAbstractItemModel* model)
	{
		if (QSortFilterProxyModel::sourceModel() == model) {
			return ;
		}
		QSortFilterProxyModel::setSourceModel(model);
		if (!model) {
			_role_hash.clear();
		} else {
			_role_hash = QSortFilterProxyModel::roleNames();
		}
		emit sourceChanged();
	}

	void SortFilterProxyModel::setPattern(const QString& pattern)
	{
		if (pattern.size() > 2 && pattern.front() == '^' && pattern.back() == '$') {
			QSortFilterProxyModel::setFilterRegularExpression(pattern);
		} else if (pattern.contains('*')) {
			QSortFilterProxyModel::setFilterWildcard(pattern);
		} else {
			QSortFilterProxyModel::setFilterFixedString(pattern);
		}
		emit patternChanged();
	}

	void SortFilterProxyModel::addRole(DynamicRole* role)
	{
		onRoleListResized(true);
	}

	bool SortFilterProxyModel::setData(const QModelIndex& index, const QVariant& value, int role)
	{
		if (role == std::clamp(role, MaxDataRole - _role_list.length(), MaxDataRole)) {
			_role_list[MaxDataRole - role]->_role_map[index.row()] = value;
			emit dataChanged(index, index, {role});
			return true;
		} else {
			return QSortFilterProxyModel::setData(index, value, role);
		}
	}

	void SortFilterProxyModel::onRoleListResized(bool increase)
	{
		if (increase && !_role_list.isEmpty()) {
			validate(_role_list.last());
		}
		invalidate();
	}

	void SortFilterProxyModel::onRoleListUpdated()
	{
		invalidate();
	}
}
