/**
 @file PropertyList.hpp
 @date 12.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Qaos_PropertyList_hpp
#define Qaos_PropertyList_hpp

#include <QObject>

#include <QMap>
#include <QSet>
#include <QList>
#include <QMultiMap>
#include <QQmlListProperty>

namespace Qaos {
	/**
	 * @brief
	 */
	class AbstractPropertyList : public QObject
	{
	/** @name Qroperties */
	/** @{ */
	Q_OBJECT
		Q_PROPERTY(int defaultIndex READ getDefaultIndex NOTIFY defaulted)
	/** @} */

	/** @name Constructors*/
	/** @{ */
	public:
		AbstractPropertyList(QObject* parent = nullptr);
		virtual ~AbstractPropertyList();
	/** @} */

	/** @name Properties */
	/** @{ */
	public:
		int _default_index;
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void setDefaultIndex(int value);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		virtual int length() const = 0;
		int getLength() const;
		int getDefaultIndex() const;
	/** @} */

	/** @name Hooks */
	/** @{ */
	public:
		virtual bool eventFilter(QObject* object, QEvent* event) override;
	/** @} */

	/** @name Signals */
	/** @{ */
	signals:
		void resized(bool increase);
		void updated();
		void defaulted();
	/** @} */

	/** @name Slots */
	/** @{ */
	private slots:
		void onResized(bool increase);
	/** @} */
	};

	/**
	 * @brief
	 */
	template <typename G>
	class GadgetPropertyList : public AbstractPropertyList
	{
	/** @name Aliases */
	/** @{ */
	private:
		using Container = QList<G*>;
	/** @} */

	/** @name Statics */
	/** @{ */
	private:
		static G* Item(QQmlListProperty<G>* list, int index)
		{
			return reinterpret_cast<Container*>(list->data)->value(index);
		}

		static int Size(QQmlListProperty<G>* list)
		{
			return reinterpret_cast<Container*>(list->data)->count();
		}

		static void Clear(QQmlListProperty<G>* list)
		{
			Container* container(reinterpret_cast<Container*>(list->data));
			if (!container) {
				return ;
			}
			qDeleteAll(container->begin(), container->end());
			container->clear();
			AbstractPropertyList* delegate(qobject_cast<AbstractPropertyList*>(list->object));
			if (delegate) {
				emit delegate->resized(false);
			}
		}

		static void Append(QQmlListProperty<G>* list, G* item)
		{
			Container* container(reinterpret_cast<Container*>(list->data));
			if (!container) {
				return ;
			}
			container->append(item);
			AbstractPropertyList* delegate(qobject_cast<AbstractPropertyList*>(list->object));
			if (delegate) {
				emit delegate->resized(true);
			}
		}

		static void Replace(QQmlListProperty<G>* list, int index, G* item)
		{
			Container* container(reinterpret_cast<Container*>(list->data));
			const int count(!container ? 0 : container->count());
			if (index >= count) {
				return ;
			}
			container->append(item);
			container->swapItemsAt(index, count);
			QScopedPointer<G> item2(container->takeLast());
			AbstractPropertyList* delegate(qobject_cast<AbstractPropertyList*>(list->object));
			if (delegate) {
				emit delegate->updated();
			}
		}

		static void Pop(QQmlListProperty<G>* list)
		{
			Container* container(reinterpret_cast<Container*>(list->data));
			if (!container || container->isEmpty()) {
				return ;
			}
			QScopedPointer<G> item(container->takeLast());
			AbstractPropertyList* delegate(qobject_cast<AbstractPropertyList*>(list->object));
			if (delegate) {
				emit delegate->resized(false);
			}
		}
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		GadgetPropertyList(QObject* parent = nullptr)
		:
			AbstractPropertyList(parent)
		{

		}

		virtual ~GadgetPropertyList() override
		{
			qDeleteAll(_container.begin(), _container.end());
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		Container _container;
	/** @} */

	/** @name Operators  */
	/** @{ */
	public:
		/// @todo different access levels
		operator QQmlListProperty<G>()
		{
			return QQmlListProperty<G>
			(
				this,
				&_container,
				&GadgetPropertyList<G>::Append,
				&GadgetPropertyList<G>::Size,
				&GadgetPropertyList<G>::Item,
				&GadgetPropertyList<G>::Clear,
				&GadgetPropertyList<G>::Replace,
				&GadgetPropertyList<G>::Pop
			);
		}

		operator QList<G*>&()
		{
			return _container;
		}

		operator const QList<G*>&() const
		{
			return qAsConst(_container);
		}
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
		QQmlListProperty<G> makeQMLAdapter(bool read_only = false)
		{
			if (read_only) {
				return QQmlListProperty<G>
				(
					this,
					&_container,
					&GadgetPropertyList<G>::Size,
					&GadgetPropertyList<G>::Item
				);
			} else {
				return QQmlListProperty<G>
				(
					this,
					&_container,
					&GadgetPropertyList<G>::Append,
					&GadgetPropertyList<G>::Size,
					&GadgetPropertyList<G>::Item,
					&GadgetPropertyList<G>::Clear,
					&GadgetPropertyList<G>::Replace,
					&GadgetPropertyList<G>::Pop
				);
			}
		}
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		void push_back(typename QList<G*>::const_reference reference, bool defalt_index = false)
		{
			_container.push_back(reference);
			if (defalt_index) {
				setDefaultIndex(_container.length());
			}
			emit resized(true);
		}

		void merge(GadgetPropertyList<G>& source)
		{
			for (typename QList<G*>::const_reference reference : source._container) {
				_container.push_back(reference);
			}
			source._container.clear();
			emit resized(true);
		}

		void swap(GadgetPropertyList<G>& origin)
		{
			origin._container.swap(_container);
			const qsizetype count1(_container.size());
			const qsizetype count2(origin._container.size());
			if (count1 == count2) {
				emit updated();
				emit origin.updated();
			} else if (count1 < count2) {
				emit resized(false);
				emit origin.resized(true);
			} else {
				emit resized(true);
				emit origin.resized(false);
			}
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		virtual int length() const override
		{
			return _container.size();
		}

		typename QList<G*>::reference last()
		{
			return _container.last();
		}

		typename QList<G*>::const_reference last() const
		{
			return qAsConst(_container).last();
		}

		typename QList<G*>::reference first()
		{
			return _container.first();
		}

		typename QList<G*>::const_reference first() const
		{
			return qAsConst(_container).first();
		}

		typename QList<G*>::iterator begin()
		{
			return _container.begin();
		}

		typename QList<G*>::const_iterator begin() const
		{
			return qAsConst(_container).begin();
		}

		typename QList<G*>::iterator end()
		{
			return _container.end();
		}

		typename QList<G*>::const_iterator end() const
		{
			return qAsConst(_container).end();
		}

		typename QList<G*>::reference back()
		{
			return _container.back();
		}

		typename QList<G*>::const_reference back() const
		{
			return qAsConst(_container).back();
		}

		typename QList<G*>::const_reference at(int index) const
		{
			return qAsConst(_container).at(index);
		}

		typename QList<G*>::reference operator[](int index)
		{
			return _container[index];
		}
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool isEmpty() const
		{
			return _container.isEmpty();
		}
	/** @} */
	};

	/**
	 * @brief
	 */
	template <typename O>
	class ObjectPropertyList : public AbstractPropertyList
	{
	/** @name Aliases */
	/** @{ */
	private:
		using Container = QPair<QList<O*>, QMultiMap<QString, O*>>;
	/** @} */

	/** @name Statics */
	/** @{ */
	private:
		static O* Item(QQmlListProperty<O>* list, int index)
		{
			return reinterpret_cast<Container*>(list->data)->first.value(index);
		}

		static int Size(QQmlListProperty<O>* list)
		{
			return reinterpret_cast<Container*>(list->data)->first.count();
		}

		static void Clear(QQmlListProperty<O>* list)
		{
			Container* container(reinterpret_cast<Container*>(list->data));
			if (!container) {
				return ;
			}
			qDeleteAll(container->first.begin(), container->first.end());
			container->first.clear();
			container->second.clear();
			AbstractPropertyList* delegate(qobject_cast<AbstractPropertyList*>(list->object));
			if (delegate) {
				emit delegate->resized(false);
			}
		}

		static void Append(QQmlListProperty<O>* list, O* item)
		{
			Container* container(reinterpret_cast<Container*>(list->data));
			if (!container) {
				return ;
			}
			container->first.append(item);
			container->second.insert(!item ? "" : item->objectName(), item);
			AbstractPropertyList* delegate(qobject_cast<AbstractPropertyList*>(list->object));
			if (delegate) {
				if (item) {
					item->installEventFilter(delegate);
				}
				emit delegate->resized(true);
			}
		}

		static void Replace(QQmlListProperty<O>* list, int index, O* item)
		{
			Container* container(reinterpret_cast<Container*>(list->data));
			const int count(!container ? 0 : container->first.count());
			if (index >= count) {
				return ;
			}
			container->first.append(item);
			container->first.swapItemsAt(index, count);
			QScopedPointer<O> item2(container->first.takeLast());
			AbstractPropertyList* delegate(qobject_cast<AbstractPropertyList*>(list->object));
			if (delegate) {
				if (item) {
					item->installEventFilter(delegate);
				}
				if (item2) {
					item2->removeEventFilter(delegate);
				}
				emit delegate->updated();
			}
		}

		static void Pop(QQmlListProperty<O>* list)
		{
			Container* container(reinterpret_cast<Container*>(list->data));
			if (!container || container->first.isEmpty()) {
				return ;
			}
			QScopedPointer<O> item(container->first.takeLast());
			container->second.remove(item->objectName(), item.data());
			AbstractPropertyList* delegate(qobject_cast<AbstractPropertyList*>(list->object));
			if (delegate) {
				if (item) {
					item->removeEventFilter(delegate);
				}
				emit delegate->resized(false);
			}
		}
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		ObjectPropertyList(QObject* parent = nullptr)
		:
			AbstractPropertyList(parent)
		{

		}

		virtual ~ObjectPropertyList() override
		{
			qDeleteAll(_container.first.begin(), _container.first.end());
		}
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		Container _container;
	/** @} */

	/** @name Operators  */
	/** @{ */
	public:
		/// @todo different access levels
		operator QQmlListProperty<O>()
		{
			return QQmlListProperty<O>
			(
				this,
				&_container,
				&ObjectPropertyList<O>::Append,
				&ObjectPropertyList<O>::Size,
				&ObjectPropertyList<O>::Item,
				&ObjectPropertyList<O>::Clear,
				&ObjectPropertyList<O>::Replace,
				&ObjectPropertyList<O>::Pop
			);
		}

		operator QList<O*>&()
		{
			return _container.first;
		}

		operator const QList<O*>&() const
		{
			return qAsConst(_container.first);
		}
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
		QQmlListProperty<O> makeQMLAdapter(bool read_only = false)
		{
			if (read_only) {
				return QQmlListProperty<O>
				(
					this,
					&_container,
					&ObjectPropertyList<O>::Size,
					&ObjectPropertyList<O>::Item
				);
			} else {
				return QQmlListProperty<O>
				(
					this,
					&_container,
					&ObjectPropertyList<O>::Append,
					&ObjectPropertyList<O>::Size,
					&ObjectPropertyList<O>::Item,
					&ObjectPropertyList<O>::Clear,
					&ObjectPropertyList<O>::Replace,
					&ObjectPropertyList<O>::Pop
				);
			}
		}
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		void push_back(typename QList<O*>::const_reference reference, bool defalt_index = false)
		{
			_container.first.push_back(reference);
			_container.second.insert(reference->objectName(), reference);
			if (reference) {
				reference->installEventFilter(this);
			}
			if (defalt_index) {
				setDefaultIndex(_container.first.length());
			}
			emit resized(true);
		}

		void merge(ObjectPropertyList<O>& source)
		{
			for (typename QList<O*>::const_reference reference : source._container.first) {
				_container.first.push_back(reference);
				_container.second.insert(reference->objectName(), reference);
				if (reference) {
					reference->installEventFilter(this);
				}
			}
			source._container.first.clear();
			emit resized(true);
		}

		void swap(ObjectPropertyList<O>& origin)
		{
			origin._container.swap(_container);
			const qsizetype count1(_container.second.size());
			const qsizetype count2(origin._container.second.size());
			if (count1 == count2) {
				emit updated();
				emit origin.updated();
			} else if (count1 < count2) {
				emit resized(false);
				emit origin.resized(true);
			} else {
				emit resized(true);
				emit origin.resized(false);
			}
		}
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		virtual int length() const override
		{
			return _container.second.size();
		}

		QList<typename Container::second_type::mapped_type> index(const QString& key) const
		{
			QList<typename Container::second_type::mapped_type> retval;
			typename Container::second_type::ConstIterator i(_container.second.find(key));
			while (i != _container.second.cend() && i.key() == key) {
				retval.push_back((i++).value());
			}
			return retval;
		}

		typename Container::second_type::mapped_type map(const QString& key) const
		{
			typename Container::second_type::ConstIterator i(_container.second.find(key));
			return _container.second.constEnd() != i ? i.value() : nullptr;
		}

		typename QList<O*>::reference last()
		{
			return _container.first.last();
		}

		typename QList<O*>::const_reference last() const
		{
			return qAsConst(_container.first).last();
		}

		typename QList<O*>::reference first()
		{
			return _container.first.first();
		}

		typename QList<O*>::const_reference first() const
		{
			return qAsConst(_container.first).first();
		}

		typename QList<O*>::iterator begin()
		{
			return _container.first.begin();
		}

		typename QList<O*>::const_iterator begin() const
		{
			return qAsConst(_container.first).begin();
		}

		typename QList<O*>::iterator end()
		{
			return _container.first.end();
		}

		typename QList<O*>::const_iterator end() const
		{
			return qAsConst(_container.first).end();
		}

		typename QList<O*>::reference back()
		{
			return _container.first.back();
		}

		typename QList<O*>::const_reference back() const
		{
			return qAsConst(_container.first).back();
		}

		typename QList<O*>::const_reference at(int index) const
		{
			return qAsConst(_container.first).at(index);
		}

		typename QList<O*>::reference operator[](int index)
		{
			return _container.first[index];
		}
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool isEmpty() const
		{
			return _container.first.isEmpty();
		}
	/** @} */
	};
}

#endif
