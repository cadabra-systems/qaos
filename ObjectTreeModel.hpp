/**
 @file ObjectTreeModel.hpp
 @date 12.02.15
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
*/

#ifndef Qaos_ObjectTreeModel_hpp
#define Qaos_ObjectTreeModel_hpp

#include <QAbstractItemModel>

#include <QDebug>
#include <QEvent>
#include <QPointer>

namespace Qaos {
	/**
	 */
	class ObjectTreeModel : public QAbstractItemModel
	{
	/** @name Classes */
	/** @{ */
	public:
		enum DataRole
		{
			Name = Qt::UserRole,
			Object,
			Ancestor,
			DescendantList
		};

		enum Direction
		{
			Ancestors, /// < Parents
			Descendants /// < Children
		};
	/** @} */

	/** @name Qroperties */
	/** @{ */
	Q_OBJECT
		Q_ENUM(Direction)
		Q_PROPERTY(QObject* root READ getRoot WRITE setRoot NOTIFY rootChanged)
		Q_PROPERTY(QObject* ancestor READ getAncestor NOTIFY rootChanged)
		Q_PROPERTY(Direction direction READ getDirection WRITE setDirection NOTIFY rootChanged)
	/** @} */

	/** @name Classes */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		ObjectTreeModel(QObject* parent = nullptr);
		virtual ~ObjectTreeModel() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		QPointer<QObject> _root;
		Direction _direction;
	/** @} */

	/** @name Procedures */
	/** @{ */
	private:
		void removeFilter(QObject* object, Direction direction);
		void installFilter(QObject* object, Direction direction);
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
		QModelIndex makeIndex(QObject* object) const;
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		void setRoot(QObject* value);
		void setDirection(Direction value);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		QObject* getRoot() const;
		QObject* getAncestor() const;
		Direction getDirection() const;

		virtual QHash<int, QByteArray> roleNames() const override;
		Q_INVOKABLE virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
		Q_INVOKABLE virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
		Q_INVOKABLE virtual QModelIndex parent(const QModelIndex& child) const override;
		Q_INVOKABLE virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
		Q_INVOKABLE virtual QVariant data(const QModelIndex& index, int role) const override;
	/** @} */

	/** @name Signals */
	/** @{ */
	signals:
		void rootChanged();
	/** @} */

	/** @name Hooks */
	/** @{ */
	public:
		virtual bool eventFilter(QObject* watched, QEvent* event) override;
	/** @} */

	/** @name Friends */
	/** @{ */
	friend QDebug operator<<(QDebug debug, const ObjectTreeModel& self);
	/** @} */
	};
}

#endif
