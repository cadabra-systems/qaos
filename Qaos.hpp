#ifndef Qaos_Qaos_hpp
#define Qaos_Qaos_hpp

#include <QObject>

#include <QGuiApplication> /// < @note hack Qaos::Qaos
#include <QQmlApplicationEngine>
#include <QtQml>

namespace Qaos {
#if !defined(QT_QAOS)
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

	/** @name States */
	/** @{ */
	public:
	/** @} */

	/** @name States */
	/** @{ */
	public:
	/** @} */
	};
#endif

	Qaos& Instance();
}
#endif
