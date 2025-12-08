/**
 @file ObjectPathModel.hpp
 @date 12.02.15
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
*/

#ifndef Qaos_ObjectPathModel_hpp
#define Qaos_ObjectPathModel_hpp

#include <QAbstractListModel>

#include <QPointer>
#include <QEvent>

#include <QDebug>

namespace Qaos {
	/**
	 */
	class ObjectPathModel : public QAbstractListModel
	{
	/** @name Classes */
	/** @{ */
	public:
		enum DataRole
		{
			Name = Qt::UserRole,
			Object
		};

		enum Filter
		{
			Root,
			Base,
			Complete
		};
	/** @} */

	/** @name Qroperties */
	/** @{ */
	Q_OBJECT
		Q_ENUM(Filter)
		Q_PROPERTY(QObject* object READ getObject WRITE setObject NOTIFY objectChanged)
		Q_PROPERTY(Filter filter READ getFilter WRITE setFilter NOTIFY objectChanged)
	/** @} */

	/** @name Classes */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		ObjectPathModel(QObject* parent = nullptr);
		virtual ~ObjectPathModel() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		QPointer<QObject> _object;
		Filter _filter;
	/** @} */

	/** @name Procedures */
	/** @{ */
	private:
		void removeFilter(QObject* object);
		void installFilter(QObject* object);
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		void setObject(QObject* value);
		void setFilter(Filter valuel);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		QObject* getObject() const;
		const ObjectPathModel::Filter& getFilter() const;

		virtual QHash<int, QByteArray> roleNames() const override;
		Q_INVOKABLE virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
		Q_INVOKABLE virtual QModelIndex parent(const QModelIndex& child) const override;
		Q_INVOKABLE virtual QVariant data(const QModelIndex& index, int role) const override;
	/** @} */

	/** @name Signals */
	/** @{ */
	signals:
		void objectChanged();
	/** @} */

	/** @name Hooks */
	/** @{ */
	public:
		virtual bool eventFilter(QObject* watched, QEvent* event) override;
	/** @} */

	/** @name Friends */
	/** @{ */
	friend QDebug operator<<(QDebug debug, const ObjectPathModel& self);
	/** @} */
	};
}

#endif
