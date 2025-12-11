#include "DynamicRole.hpp"

#include <QDebug>
#include <QCoreApplication>

namespace Qaos {
	DynamicRole::DynamicRole(const DynamicRole& origin)
	:
		QObject(origin.parent()),

		_role_index(origin._role_index),
		_role_name(origin._role_name),
		_role_value(origin._role_value)
	{

	}

	DynamicRole::DynamicRole(QObject* parent)
	:
		QObject(parent),

		_role_index(-1)
	{

	}

	DynamicRole::DynamicRole(QObject* parent, const QString& role_name, const QVariant& role_value)
	:
		QObject(parent),

		_role_name(role_name),
		_role_value(role_value)
	{

	}

	const int& DynamicRole::getRoleIndex() const
	{
		return _role_index;
	}

	const QString& DynamicRole::getRoleName() const
	{
		return _role_name;
	}

	const QVariant& DynamicRole::getRoleValue() const
	{
		return _role_value;
	}

	const QVariant DynamicRole::getRoleValue(int index) const
	{		
		return _role_map.value(index, _role_value);
	}

	void DynamicRole::setRoleName(const QString& name)
	{
		if (name != _role_name) {
			_role_name = name;
			_role_index = -1;
			emit roleNameChanged();
		}
	}

	void DynamicRole::setRoleValue(const QVariant& value)
	{
		if (value != _role_value) {
			_role_value = value;
			emit roleValueChanged();
		}
	}

	void DynamicRole::classBegin()
	{

	}

	void DynamicRole::componentComplete()
	{
		QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest), Qt::HighEventPriority);
	}
}
