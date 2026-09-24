/**
 @file ScopedObjectConnection.cpp
 @date 12.02.15
 @author Daniil A Megrabyan
 */

#include "ScopedObjectConnection.hpp"

namespace Qaos {
	ScopedObjectConnection::ScopedObjectConnection(QMetaObject::Connection&& connection, bool auto_mode)
	:
		_auto_mode(auto_mode),
		_connection(std::move(connection))
	{

	}

	ScopedObjectConnection::ScopedObjectConnection(ScopedObjectConnection&& origin)
	:
		_auto_mode(origin._auto_mode),
		_connection(std::move(origin._connection))
	{

	}

	ScopedObjectConnection::ScopedObjectConnection(const ScopedObjectConnection& origin)
	:
		_auto_mode(origin._auto_mode),
		_connection(origin._connection)
	{
		origin._auto_mode = false;
	}

	ScopedObjectConnection::~ScopedObjectConnection()
	{
		if (_auto_mode && _connection) {
			QObject::disconnect(_connection);
		}
	}

	bool ScopedObjectConnection::disconnect()
	{
		return QObject::disconnect(_connection);
	}

	void ScopedObjectConnection::setAutoMode(bool onoff)
	{
		_auto_mode = onoff;
	}
}
