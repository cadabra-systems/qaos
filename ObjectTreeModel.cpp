/**
 @file ObjectTreeModel.cpp
 @date 12.02.15
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
 */

#include "ObjectTreeModel.hpp"

#include <QChildEvent>

namespace Qaos {
	ObjectTreeModel::ObjectTreeModel(QObject* parent)
	:
		QAbstractItemModel(parent),

		_root(nullptr),
		_direction(Direction::Descendants)
	{
		QObject::connect(this, &QAbstractItemModel::modelReset, this, &ObjectTreeModel::rootChanged);
	}

	void ObjectTreeModel::removeFilter(QObject* object, Direction direction)
	{
		if (!object) {
			return;
		}
		object->removeEventFilter(this);
		if (Direction::Descendants == direction) {
			for (QObject* child : object->children()) {
				removeFilter(child, Direction::Descendants);
			}
		} else if (object->parent()) {
			removeFilter(object->parent(), Direction::Ancestors);
		}
	}

	void ObjectTreeModel::installFilter(QObject* object, Direction direction)
	{
		if (!object) {
			return;
		}
		object->installEventFilter(this);
		if (Direction::Descendants == direction) {
			for (QObject* child : object->children()) {
				installFilter(child, Direction::Descendants);
			}
		} else if (object->parent()) {
			installFilter(object->parent(), Direction::Ancestors);
		}
	}

	QModelIndex ObjectTreeModel::makeIndex(QObject* object) const
	{
		if (!object || object == _root || !_root) {
			return QModelIndex();
		}
		QObject* const parent(object->parent());
		if (!parent) {
			return QModelIndex();
		}
		const int row(parent->children().indexOf(object));
		if (row < 0) {
			return QModelIndex();
		} else if (parent == _root) {
			return createIndex(row, 0, object);
		}
		return index(row, 0, makeIndex(parent));
	}

	void ObjectTreeModel::setRoot(QObject* value)
	{
		if (value != _root) {
			beginResetModel();
			if (_root) {
				if (_direction == Direction::Descendants) {
					removeFilter(_root, Direction::Descendants);
				} else {
					removeFilter(_root, Direction::Ancestors);
				}
			}
			_root = value;
			if (_root) {
				if (_direction == Direction::Descendants) {
					installFilter(_root, Direction::Descendants);
				} else {
					installFilter(_root, Direction::Ancestors);
				}
			}
			endResetModel();
		}
	}

	void ObjectTreeModel::setDirection(ObjectTreeModel::Direction value)
	{
		if (value != _direction) {
			beginResetModel();
			if (_root) {
				if (_direction == Direction::Descendants) {
					removeFilter(_root, Direction::Descendants);
				} else {
					removeFilter(_root, Direction::Ancestors);
				}
			}
			_direction = value;
			if (_root) {
				if (_direction == Direction::Descendants) {
					installFilter(_root, Direction::Descendants);
				} else {
					installFilter(_root, Direction::Ancestors);
				}
			}
			endResetModel();
		}
	}

	QObject* ObjectTreeModel::getRoot() const
	{
		return _root;
	}

	QObject* ObjectTreeModel::getAncestor() const
	{
		return _root ? _root->parent() : nullptr;
	}

	ObjectTreeModel::Direction ObjectTreeModel::getDirection() const
	{
		return _direction;
	}

	QHash<int, QByteArray> ObjectTreeModel::roleNames() const
	{
		return
		{
			{DataRole::Name, "name"},
			{DataRole::Object, "object"},
			{DataRole::Ancestor, "ancestor"},
			{DataRole::DescendantList, "descendantList"}
		};
	}

	int ObjectTreeModel::columnCount(const QModelIndex&) const
	{
		return 1;
	}

	int ObjectTreeModel::rowCount(const QModelIndex& parent_index) const
	{
		QObject* const root_object(parent_index.isValid() ? static_cast<QObject*>(parent_index.internalPointer()) : _root.data());
		if (!root_object) {
			return 0;
		} else if (Direction::Descendants == _direction) {
			return root_object->children().size();
		} else if (Direction::Ancestors == _direction) {
			return root_object->parent() ? 1 : 0;
		} else {
			return 0;
		}
	}

	QModelIndex ObjectTreeModel::parent(const QModelIndex& child_index) const
	{
		if (!child_index.isValid()) {
			return QModelIndex();
		}
		QObject* const child_object(static_cast<QObject*>(child_index.internalPointer()));
		if (child_object == nullptr) {
			return QModelIndex();
		}
		QObject* const root_object(_root.data());
		switch (_direction) {
			case Direction::Descendants: {
				QObject* const parent_object(child_object->parent());
				if (!parent_object || parent_object == root_object) {
					return QModelIndex();
				}
				QObject* const grandparent_object(parent_object->parent());
				if (!grandparent_object) {
					return QModelIndex();
				}
				const int parental_row_index(grandparent_object->children().indexOf(parent_object));
				return parental_row_index < 0 ? QModelIndex() : createIndex(parental_row_index, 0, parent_object);
			}
			case Direction::Ancestors: {
				QObject* const parent_object(child_object->parent());
				if (!parent_object || child_object == root_object) {
					return QModelIndex();
				}
				QObject* const grandparent_object(parent_object->parent());
				if (!grandparent_object || parent_object == root_object) {
					return QModelIndex();
				}
				return createIndex(0, 0, grandparent_object);
			}
		}
		return QModelIndex();
	}

	QModelIndex ObjectTreeModel::index(int row, int column, const QModelIndex& parent_index) const
	{
		if (row < 0 || column < 0 || column >= 1) {
			return QModelIndex();
		}
		QObject* const root_object(parent_index.isValid() ? static_cast<QObject*>(parent_index.internalPointer()) : _root.data());
		if (!root_object) {
			return QModelIndex();
		} else if (Direction::Descendants == _direction) {
			return row >= root_object->children().size() ? QModelIndex() : createIndex(row, column, root_object->children().at(row));
		} else if (row != 0) {
			return QModelIndex();
		}
		return root_object->parent() ? createIndex(0, column, root_object->parent()) : QModelIndex();
	}

	QVariant ObjectTreeModel::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid()) {
			return QVariant();
		} else if (QObject* const object = static_cast<QObject*>(index.internalPointer())) {
			if (DataRole::Name == role) {
				return object->objectName();
			} else if (DataRole::Object == role) {
				return QVariant::fromValue(object);
			} else if (DataRole::Ancestor == role) {
				return QVariant::fromValue(object->parent());
			} else if (DataRole::DescendantList == role) {
				return QVariant::fromValue(object->children());
			}
		}
		return QVariant();
	}

	bool ObjectTreeModel::eventFilter(QObject* target, QEvent* event)
	{
		if (!target || !_root) {
			return QAbstractItemModel::eventFilter(target, event);
		} else if (Direction::Descendants == _direction) {
			if (event->type() == QEvent::ChildAdded) {
				QChildEvent* child_event = static_cast<QChildEvent*>(event);
				QObject* child = child_event->child();
				if (child) {
					QObject* const parent_object(target);
					const int row = parent_object->children().indexOf(child);
					if (row >= 0) {
						const QModelIndex parent_index = (parent_object == _root) ? QModelIndex() : makeIndex(parent_object);
						beginInsertRows(parent_index, row, row);
						installFilter(child, Direction::Descendants);
						endInsertRows();
					}
				}
			} else if (event->type() == QEvent::ChildRemoved) {
				QChildEvent* child_event = static_cast<QChildEvent*>(event);
				QObject* child = child_event->child();
				if (child) {
					QObject* parent_object = target;
					const int row = parent_object->children().indexOf(child);
					if (row >= 0) {
						const QModelIndex parent_index = (parent_object == _root) ? QModelIndex() : makeIndex(parent_object);
						beginRemoveRows(parent_index, row, row);
						removeFilter(child, Direction::Descendants);
						endRemoveRows();
					}
				}
			}
		} else if (Direction::Ancestors == _direction) {
			if (event->type() == QEvent::ParentChange) {
				bool is_in_chain(false);
				QObject* current(_root);
				while (current) {
					if (current == target) {
						is_in_chain = true;
						break;
					}
					current = current->parent();
				}
				if (is_in_chain) {
					beginResetModel();
					removeFilter(_root, Direction::Ancestors);
					installFilter(_root, Direction::Ancestors);
					endResetModel();
				}
			}
		}
		return QAbstractItemModel::eventFilter(target, event);
	}

	QDebug operator<<(QDebug debug, const ObjectTreeModel& self)
	{
		struct _
		{
			static QStringList descend(const QObject* node, int depth)
			{
				QStringList retval;
				retval <<
							QStringLiteral("%1%2 (%3)")
							.arg(QString(depth * 2, QChar(' ')))
							.arg(node->objectName().isEmpty() ? QStringLiteral("<unnamed>") : node->objectName())
							.arg(QString::fromLatin1(node->metaObject()->className()))
				;
				const QObjectList& children = node->children();
				for (const QObject* child : children) {
					retval += _::descend(child, depth + 1);
				}
				return retval;
			}

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

		const QObject* const root_object(self._root.data());
		if (!root_object) {
			debug.nospace() << "<null root>";
		} else switch (self._direction) {
			case ObjectTreeModel::Direction::Descendants:
				debug.nospace() << _::descend(root_object, 0);
				break;
			case ObjectTreeModel::Direction::Ancestors:
				debug.nospace() << _::ascend(root_object, 0);
				break;
		}
		return debug;
	}
}
