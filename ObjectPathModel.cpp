/**
 @file ObjectPathModel.cpp
 @date 12.02.15
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
 */

#include "ObjectPathModel.hpp"

#include <QChildEvent>

namespace Qaos {
	ObjectPathModel::ObjectPathModel(QObject* parent)
	:
		QAbstractListModel(parent),

		_object(nullptr)
	{
		QObject::connect(this, &QAbstractListModel::modelReset, this, &ObjectPathModel::objectChanged);
	}

	void ObjectPathModel::removeFilter(QObject* object)
	{
		if (!object) {
			return;
		}
		object->removeEventFilter(this);
		if (object->parent()) {
			removeFilter(object->parent());
		}
	}

	void ObjectPathModel::installFilter(QObject* object)
	{
		if (!object) {
			return;
		}
		object->installEventFilter(this);
		if (object->parent()) {
			installFilter(object->parent());
		}
	}

	void ObjectPathModel::setObject(QObject* value)
	{
		if (value != _object) {
			beginResetModel();
			if (_object) {
				removeFilter(_object);
			}
			_object = value;
			if (_object) {
				installFilter(_object);
			}
			endResetModel();
		}
	}

	void ObjectPathModel::setFilter(ObjectPathModel::Filter value)
	{
		if (value != _filter) {
			_filter = value;
			emit objectChanged();
		}
	}

	QObject* ObjectPathModel::getObject() const
	{
		return _object;
	}

	const ObjectPathModel::Filter& ObjectPathModel::getFilter() const
	{
		return _filter;
	}

	QHash<int, QByteArray> ObjectPathModel::roleNames() const
	{
		return
		{
			{DataRole::Name, "name"},
			{DataRole::Object, "object"}
		};
	}

	int ObjectPathModel::rowCount(const QModelIndex& parent_index) const
	{
		const QObject* object(_object);
		if (!object) {
			return 0;
		} else if (ObjectPathModel::Filter::Root == _filter) {
			return 1;
		}
		int retval(ObjectPathModel::Filter::Base == _filter ? -1 : 0);
		do {
			retval += 1;
		} while ((object = object->parent()));
		return retval;
	}

	QModelIndex ObjectPathModel::parent(const QModelIndex& child_index) const
	{
		return QModelIndex{};
	}

	QVariant ObjectPathModel::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid()) {
			return QVariant{};
		}
		QObject* object(_object);
		if (!object) {
			return QVariant{};
		} else if (ObjectPathModel::Filter::Root == _filter) {
			while (object->parent()) {
				object = object->parent();
			}
		} else {
			if (ObjectPathModel::Filter::Base == _filter) {
				if (!(object = object->parent())) {
					return QVariant{};
				}
			}
			for (int r = 0; r != index.row(); ++r) {
				if (!(object = object->parent())) {
					return QVariant{};
				}
			}
		}
		if (DataRole::Name == role) {
			return object->objectName();
		} else if (DataRole::Object == role) {
			return QVariant::fromValue(object);
		}
		return QVariant{};
	}

	bool ObjectPathModel::eventFilter(QObject* target, QEvent* event)
	{
		if (!target || !_object) {
			return QAbstractItemModel::eventFilter(target, event);
		} else if (event->type() == QEvent::ParentChange) {
			bool is_in_chain(false);
			QObject* current(_object);
			while (current) {
				if (current == target) {
					is_in_chain = true;
					break;
				}
				current = current->parent();
			}
			if (is_in_chain) {
				beginResetModel();
				removeFilter(_object);
				installFilter(_object);
				endResetModel();
			}
		}
		return QAbstractListModel::eventFilter(target, event);
	}

	QDebug operator<<(QDebug debug, const ObjectPathModel& self)
	{
		struct _
		{
			static QStringList ascend(const QObject* node, int depth)
			{
				QStringList retval;
				retval <<
							QStringLiteral("%1%2 (%3)")
							.arg(QString(depth * 2, QChar(' ')))
							.arg(node->objectName().isEmpty() ? QStringLiteral("<unnamed>") : node->objectName())
							.arg(QString::fromLatin1(node->metaObject()->className()))
				;
				const QObject* parent = node->parent();
				if (parent) {
					retval += _::ascend(parent, depth + 1);
				}
				return retval;
			}
		};

		const QObject* const root_object(self._object.data());
		if (!root_object) {
			debug.nospace() << "<null root>";
		} else {
			debug.nospace() << _::ascend(root_object, 0);
		}
		return debug;
	}
}
