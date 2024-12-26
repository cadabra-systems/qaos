#ifndef Qaos_Qaos_hpp
#define Qaos_Qaos_hpp

#include <QObject>

#include <QAbstractItemModel>

#include <QGuiApplication> /// < @note hack Qaos::Qaos
#include <QQmlApplicationEngine>
#include <QtQml>

namespace Qaos {
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
		virtual ~Qaos() = default;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		Q_INVOKABLE QMap<QString, QVariant> getDataRoleMap(QAbstractItemModel* model) const;
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
