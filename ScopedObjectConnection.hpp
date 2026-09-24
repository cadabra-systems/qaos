/**
 @file ScopedObjectConnection.hpp
 @date 12.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Qaos_ScopedObjectConnection_hpp
#define Qaos_ScopedObjectConnection_hpp

#include <QObject>

namespace Qaos {
	class ScopedObjectConnection
	{
	/** @name Constructors*/
	/** @{ */
	public:
		ScopedObjectConnection(QMetaObject::Connection&& connection, bool auto_mode = true);
		ScopedObjectConnection(ScopedObjectConnection&& origin);
		ScopedObjectConnection(const ScopedObjectConnection& origin);
		~ScopedObjectConnection();
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		mutable bool _auto_mode;
		QMetaObject::Connection _connection;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		bool disconnect();
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void setAutoMode(bool onoff = true);
	/** @} */
	};
}

#endif
