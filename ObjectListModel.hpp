/**
 @file ObjectListModel.hpp
 @date 12.02.15
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
*/

#ifndef Qaos_ObjectListModel_hpp
#define Qaos_ObjectListModel_hpp

#include <QAbstractListModel>

#include "Qaos.hpp"

#include "Default.hpp"

#include <QByteArray>
#include <QChar>
#include <QDebug>
#include <QHash>
#include <QList>
#include <QMetaMethod>
#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>
#include <QString>
#include <QStringBuilder>
#include <QVariant>
#include <QVector>

namespace Qaos {
	class AbstractObjectListModel : public QAbstractListModel
	{
	/** @name Constructors */
	/** @{ */
	public:
		explicit AbstractObjectListModel(QObject* parent = nullptr)
		:
			QAbstractListModel(parent)
		{

		}
	/** @} */

	/** @name Qroperties */
	/** @{ */
	Q_OBJECT
		Q_PROPERTY(int count READ getCount NOTIFY countChanged)
	/** @} */

	/** @name Procedures */
	/** @{ */
	public slots:
		virtual void insert(int index, QObject* value) = 0;
		virtual void append(QObject* value) = 0;
		virtual void prepend(QObject* value) = 0;
		virtual void move(int index, int position) = 0;
		virtual void remove(QObject* value) = 0;
		virtual void remove(int index) = 0;
		virtual void clear() = 0;
	/** @} */

	/** @name Getters */
	/** @{ */
	public slots:
		virtual QObject* getObject(int idx) const = 0;
		virtual QObject* getObject(const QString& uid) const = 0;
		virtual QObject* getFirstObject() const = 0;
		virtual QObject* getLastObject() const = 0;

		virtual int getSize() const = 0;
		virtual int getCount() const = 0;

		virtual int getIndex(QObject* item) const = 0;
		virtual int getRole(const QByteArray& name) const = 0;
	/** @} */

	/** @name Signals */
	/** @{ */
	signals:
		void countChanged();
	/** @} */

	/** @name Hooks */
	/** @{ */
	protected slots:
		virtual void onItemPropertyChanged() = 0;
	/** @} */

	/** @name States */
	/** @{ */
	public slots:
		virtual bool isEmpty() const = 0;
		virtual bool isExist(QObject* item) const = 0;
	/** @} */
	};

	/**
	 * @todo Memory(Reference) management
	 */
	template <typename T>
	class ObjectListModel : public AbstractObjectListModel
	{
	/** @name Aliases */
	/** @{ */
		using ConstIterator = typename QList<T*>::ConstIterator;
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		QList<T> List(const QVariantList& list)
		{
			QList<T> retval;
			retval.reserve(list.size());
			for (QVariantList::const_iterator i = list.constBegin(); i != list.constEnd(); ++i) {
				retval.append(static_cast<QVariant>(*i).value<T>());
			}
			return retval;
		}

		QVariantList VariantList(const QList<T>& list)
		{
			QVariantList retval;
			retval.reserve(list.size());
			for (typename QList<T>::const_iterator i = list.constBegin(); i != list.constEnd(); ++i) {
				retval.append(QVariant::fromValue(static_cast<T>(*i)));
			}
			return retval;
		}
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		explicit ObjectListModel(QObject* parent = nullptr, const QByteArray& display_role = QByteArray(), const QByteArray& uid_role = QByteArray())
		:
			AbstractObjectListModel(parent),

			_count(0),
			_uid_role_name(uid_role),
			_display_role_name(display_role),
			_meta_obj(T::staticMetaObject)
		{
			_handler = metaObject()->method(metaObject()->indexOfMethod("onItemPropertyChanged()"));
			if (!display_role.isEmpty()) {
				_role_hash.insert(Qt::DisplayRole, QByteArrayLiteral("display"));
			}
			_role_hash.insert(Qt::UserRole, QByteArrayLiteral("object"));
			const int length(_meta_obj.propertyCount());
			for (int p = 0, role = (Qt::UserRole + 1); p < length; p++, role++) {
				const QMetaProperty meta_property(_meta_obj.property(p));
				/// @xxx id, index, class, model, modelData
				_role_hash.insert(role, meta_property.name());
				if (meta_property.hasNotifySignal()) {
					_signal_idx_to_role.insert(meta_property.notifySignalIndex(), role);
				}
			}
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		int _count;
		QByteArray _uid_role_name;
		QByteArray _display_role_name;
		QMetaObject _meta_obj;
		QMetaMethod _handler;
		QHash<int, QByteArray> _role_hash;
		QHash<int, int> _signal_idx_to_role;
		QList<T*> _item_list;
		QHash<QString, T*> _index_by_uid;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void append(T* item)
		{
			if (item != nullptr) {
				const int index(_item_list.count());
				beginInsertRows(Default<QModelIndex>::Instance(), index, index);
				_item_list.append(item);
				reference(item);
				updateCounter();
				endInsertRows();
			}
		}

		void prepend(T* item)
		{
			if (item != nullptr) {
				beginInsertRows(Default<QModelIndex>::Instance(), 0, 0);
				_item_list.prepend(item);
				reference(item);
				updateCounter();
				endInsertRows();
			}
		}

		void insert(int index, T* item)
		{
			if (item != nullptr) {
				beginInsertRows(Default<QModelIndex>::Instance(), index, index);
				_item_list.insert(index, item);
				reference(item);
				updateCounter();
				endInsertRows();
			}
		}

		void append(const QList<T*>& item_list)
		{
			if (!item_list.isEmpty()) {
				const int index(_item_list.count());
				beginInsertRows(Default<QModelIndex>::Instance(), index, index + item_list.count() - 1);
				_item_list.reserve(_item_list.count() + item_list.count());
				_item_list.append(item_list);
				for (typename QList<T*>::ConstIterator i = item_list.constBegin(); i != item_list.constEnd(); ++i) {
					if (T* item = (*i)) {
						reference(item);
					}
				}
				updateCounter();
				endInsertRows();
			}
		}

		void prepend(const QList<T*>& item_list)
		{
			if (!item_list.isEmpty()) {
				beginInsertRows(Default<QModelIndex>::Instance(), 0, item_list.count() - 1);
				_item_list.reserve(_item_list.count() + item_list.count());
				int offset(0);
				for (typename QList<T*>::ConstIterator i = item_list.constBegin(); i != item_list.constEnd(); ++i) {
					if (T* item = (*i)) {
						_item_list.insert(offset, item);
						reference(item);
						offset++;
					}
				}
				updateCounter();
				endInsertRows();
			}
		}

		void insert(int index, const QList<T*>& item_list)
		{
			if (!item_list.isEmpty()) {
				beginInsertRows(Default<QModelIndex>::Instance(), index, index + item_list.count() - 1);
				_item_list.reserve(_item_list.count() + item_list.count());
				int offset(0);
				for (typename QList<T*>::ConstIterator i = item_list.constBegin(); i != item_list.constEnd(); ++i) {
					if (T* item = (*i)) {
						_item_list.insert(index + offset, item);
						reference(item);
						offset++;
					}
				}
				updateCounter();
				endInsertRows();
			}
		}

		void move(int index1, int index2) Q_DECL_FINAL
		{
			if (index1 != index2) {
				/// @todo use begin/end MoveRows when supported by Repeater, since then use remove/insert pair
//				beginMoveRows(Default<QModelIndex>::Instance(), idx, idx, Default<QModelIndex>::Instance(), (idx < pos ? pos + 1 : pos));
				beginRemoveRows(Default<QModelIndex>::Instance(), index1, index1);
				beginInsertRows(Default<QModelIndex>::Instance(), index2, index2);
				_item_list.move(index1, index2);
				endRemoveRows();
				endInsertRows();
//				endMoveRows();
			}
		}

		void remove(T* item)
		{
			if (item != nullptr) {
				remove(_item_list.indexOf(item));
			}
		}

		void remove(int index) Q_DECL_FINAL
		{
			if (index >= 0 && index < _item_list.size()) {
				beginRemoveRows(Default<QModelIndex>::Instance(), index, index);
				T* item(_item_list.takeAt(index));
				dereference(item);
				updateCounter();
				endRemoveRows();
			}
		}

		void append(QObject* item) Q_DECL_FINAL
		{
			append(qobject_cast<T*>(item));
		}

		void prepend(QObject* item) Q_DECL_FINAL
		{
			prepend(qobject_cast<T*>(item));
		}

		void insert(int idx, QObject* item) Q_DECL_FINAL
		{
			insert(idx, qobject_cast<T*>(item));
		}

		void remove(QObject* item) Q_DECL_FINAL
		{
			remove(qobject_cast<T*>(item));
		}

		void clear() Q_DECL_FINAL
		{
			if (!_item_list.isEmpty()) {
				beginRemoveRows(Default<QModelIndex>::Instance(), 0, _item_list.count() - 1);
				for (typename QList<T*>::ConstIterator i = _item_list.constBegin(); i != _item_list.constEnd(); ++i) {
					if (T* item = (*i)) {
						dereference(item);
					}
				}
				_item_list.clear();
				updateCounter();
				endRemoveRows();
			}
		}

	protected:
		inline void updateCounter()
		{
			if (_count != _item_list.count()) {
				_count = _item_list.count();
				emit countChanged();
			}
		}

		void reference(T* item)
		{
			if (item != nullptr) {
				if (!item->parent()) {
					item->setParent(this);
				}
				for (QHash<int, int>::const_iterator i = _signal_idx_to_role.constBegin(); i != _signal_idx_to_role.constEnd(); ++i) {
					QObject::connect(item, item->metaObject()->method(i.key()), this, _handler, Qt::UniqueConnection);
				}
				if (!_uid_role_name.isEmpty()) {
					const QString key(_index_by_uid.key(item, Default<QString>::Instance()));
					if (!key.isEmpty()) {
						_index_by_uid.remove(key);
					}
					const QString value(item->property(_uid_role_name).toString());
					if (!value.isEmpty()) {
						_index_by_uid.insert(value, item);
					}
				}
			}
		}

		void dereference(T* item)
		{
			if (item != nullptr) {
				QObject::disconnect(this, nullptr, item, nullptr);
				QObject::disconnect(item, nullptr, this, nullptr);
				if (!_uid_role_name.isEmpty()) {
					const QString key = _index_by_uid.key(item, Default<QString>::Instance());
					if (!key.isEmpty()) {
						_index_by_uid.remove(key);
					}
				}
				/// @??? maybe that's not the best way to test ownership
				if (item->parent() == this) {
					item->deleteLater();
				}
			}
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		T* get(int index) const // at
		{
			return (index >= 0 && index < _item_list.size()) ? _item_list.value(index) : nullptr;
		}

		T* get(const QString& uid) const
		{
			return !_index_by_uid.isEmpty() ? _index_by_uid.value(uid, nullptr) : nullptr;
		}

		T* getFirst() const
		{
			return _item_list.first();
		}

		T* getLast() const
		{
			return _item_list.last();
		}

		QObject* getObject(int index) const Q_DECL_FINAL
		{
			return static_cast<QObject*>(get(index));
		}

		QObject* getObject(const QString& uid) const Q_DECL_FINAL
		{
			return static_cast<QObject*>(get(uid));
		}

		QObject* getFirstObject() const Q_DECL_FINAL
		{
			return static_cast<QObject*>(first());
		}

		QObject* getLastObject() const Q_DECL_FINAL
		{
			return static_cast<QObject*>(last());
		}

		int getIndex(QObject* item) const Q_DECL_FINAL
		{
			return _item_list.indexOf(qobject_cast<T*>(item));
		}

		int getIndex(const QString& uid) const
		{
			return _item_list.indexOf(get(uid));
		}

		int getIndex(T* item) const
		{
			return _item_list.indexOf(item);
		}

		int getRole(const QByteArray& name) const Q_DECL_FINAL
		{
			return _role_hash.key(name, -1);
		}

		const QList<T*>& getList() const
		{
			return _item_list;
		}

		int getCount() const Q_DECL_FINAL
		{
			return _count;
		}

		int getSize() const Q_DECL_FINAL
		{
			return _count;
		}

		int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_FINAL
		{
			return !parent.isValid() ? _item_list.count() : 0;
		}

		QVariant data(const QModelIndex& index, int role) const Q_DECL_FINAL
		{
			QVariant retval;
			T* item(get(index.row()));
			const QByteArray role_name((role != Qt::DisplayRole ? _role_hash.value(role, Default<QByteArray>::Instance()) : _display_role_name));
			if (item != nullptr && !role_name.isEmpty()) {
				retval.setValue(role != Qt::UserRole ? item->property(role_name) : QVariant::fromValue(static_cast<QObject*>(item)));
			}
			return retval;
		}

		QHash<int, QByteArray> roleNames() const Q_DECL_FINAL
		{
			return _role_hash;
		}

		typename QList<T*>::ConstIterator begin() const
		{
			return _item_list.begin();
		}

		typename QList<T*>::ConstIterator end() const
		{
			return _item_list.end();
		}

		typename QList<T*>::ConstIterator constBegin() const
		{
			return _item_list.constBegin();
		}

		typename QList<T*>::ConstIterator constEnd() const
		{
			return _item_list.constEnd();
		}

		T* first() const
		{
			return _item_list.first();
		}

		T* last() const
		{
			return _item_list.last();
		}
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		bool setData(const QModelIndex& index, const QVariant& value, int role) Q_DECL_FINAL
		{
			bool retval(false);
			T* item(get(index.row()));
			const QByteArray rolename((role != Qt::DisplayRole ? _role_hash.value(role, Default<QByteArray>::Instance()) : _display_role_name));
			if (item != nullptr && role != Qt::UserRole && !rolename.isEmpty()) {
				retval = item->setProperty(rolename, value);
			}
			return retval;
		}
	/** @} */

	/** @name Hooks */
	/** @{ */
	protected:
		void onItemPropertyChanged() Q_DECL_FINAL
		{
			T* item = qobject_cast<T*>(sender());
			const int row = _item_list.indexOf(item);
			const int sig = senderSignalIndex();
			const int role = _signal_idx_to_role.value(sig, -1);
			if (row >= 0 && role >= 0) {
				const QModelIndex index = QAbstractListModel::index(row, 0, Default<QModelIndex>::Instance());
				QVector<int> roles_list;
				roles_list.append(role);
				if (_role_hash.value(role) == _display_role_name) {
					roles_list.append(Qt::DisplayRole);
				}
				emit dataChanged(index, index, roles_list);
			}
			if (!_uid_role_name.isEmpty()) {
				const QByteArray role_name = _role_hash.value(role, Default<QByteArray>::Instance());
				if (!role_name.isEmpty() && role_name == _uid_role_name) {
					const QString key = _index_by_uid.key(item, Default<QString>::Instance());
					if (!key.isEmpty()) {
						_index_by_uid.remove(key);
					}
					const QString value = item->property(_uid_role_name).toString();
					if (!value.isEmpty()) {
						_index_by_uid.insert(value, item);
					}
				}
			}
		}
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool isEmpty() const Q_DECL_FINAL
		{
			return _item_list.isEmpty();
		}

		bool isExist(T* item) const
		{
			return _item_list.contains(item);
		}

		bool isExist(QObject* item) const Q_DECL_FINAL
		{
			return _item_list.contains(qobject_cast<T*>(item));
		}
	/** @} */
	};
}

#endif
