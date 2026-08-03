/**
 @file LogRegister.hpp
 @date 12.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Qaos_LogRegister_hpp
#define Qaos_LogRegister_hpp

#include <QObject>

#include <QFile>
#include <QTextStream>
#include <QMessageLogContext>
#include <QtGlobal>

namespace Qaos {
	class LogRegister : public QObject
	{
	/** @name Qroperties */
	/** @{ */
	Q_OBJECT
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static LogRegister* GlobalInstance();
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors*/
	/** @{ */
	public:
		LogRegister();
		~LogRegister();
	/** @} */

	/** @name Properties */
	/** @{ */
	public:
		QFile _file;
		QTextStream _stream;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		bool open(const QString& filename);
		bool close();
		void filter(QtMsgType level);
		bool dump(QFile& file);
		void put(QtMsgType type, const QMessageLogContext& context, const QString& message);
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
	/** @} */

	/** @name Hooks */
	/** @{ */
	public:
	/** @} */

	/** @name Signals */
	/** @{ */
	signals:
		void debugMessaged(QString file_name, QString function_name, int code_line, QString message);
		void infoMessaged(QString file_name, QString function_name, int code_line, QString message);
		void warningMessaged(QString file_name, QString function_name, int code_line, QString message);
		void criticalMessaged(QString file_name, QString function_name, int code_line, QString message);
		void fatalMessaged(QString file_name, QString function_name, int code_line, QString message);
	/** @} */

	/** @name Slots */
	/** @{ */
	private slots:
	/** @} */
	};
}

#endif
