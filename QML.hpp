#ifndef Qaos_QML_hpp
#define Qaos_QML_hpp

#include <QObject>

#include <QtQml>

namespace Qaos {
	class QML : public QObject
	{
	/** @name Statics */
	/** @{ */
	public:
		static int TypeId(const char* uri, const char* name, int major = 0, int minor = 1, bool critical_mode = false);
	/** @} */

	/** @name Qroperties */
	/** @{ */
	Q_OBJECT
		QML_NAMED_ELEMENT(Qaos)
		QML_SINGLETON
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static QML* create(QQmlEngine* engine, QJSEngine* script_engine);
	/** @} */

	/** @name Constructors */
	/** @{ */
	private:
		QML(QObject* parent = nullptr);
		virtual ~QML() override;
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
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
}

#endif
