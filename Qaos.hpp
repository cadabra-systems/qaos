#ifndef Qaos_Qaos_hpp
#define Qaos_Qaos_hpp

#include <QObject>

#include "DynamicRole.hpp"
#include "ObjectList.hpp"

#include "VariantItemModel.hpp"
#include "SortFilterProxyModel.hpp"
#include "ObjectPathModel.hpp"
#include "ObjectTreeModel.hpp"

#include <QAbstractItemModel>

#include <QGuiApplication> /// < @note hack Qaos::Qaos
#include <QQmlApplicationEngine>
#include <QtQml>

#include <QDebug>

#define QaosDebug() qDebug() << Q_FUNC_INFO
#define QaosWarning() qWarning() << Q_FUNC_INFO
#define QaosCritical() qCritical() << Q_FUNC_INFO

namespace Qaos {
	/**
	 */
	class Qaos : public QObject
	{
	/** @name Statics */
	/** @{ */
	public:
		static Qaos* Instance();
		static QObject* QInstance(QQmlEngine* engine, QJSEngine* script_engine);
	/** @} */

	/** @name Qroperties */
	/** @{ */
	Q_OBJECT
	/** @} */

	/** @name Constructors */
	/** @{ */
	private:
		Qaos(QObject* parent = nullptr);
		virtual ~Qaos() override;
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
		Q_INVOKABLE QMap<QString, QVariant> makeDataRoleMap(QAbstractItemModel* model) const;
		Q_INVOKABLE ::Qaos::ObjectPathModel* makeObjectPathModel(QObject* root) const;
		Q_INVOKABLE ::Qaos::ObjectTreeModel* makeObjectTreeModel(QObject* root) const;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
	/** @} */

	/** @name States */
	/** @{ */
	public:
	/** @} */
	};

	Qaos& Instance();
}
#endif
