/**
 @file NetworkAccessManager.cpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#include "NetworkAccessManager.hpp"

#include <QNetworkDiskCache>

namespace Qaos {
	NetworkAccessManager::Factory::Factory(const QDir& cache_directory)
	:
		_cache_directory(cache_directory)
	{

	}

	QNetworkAccessManager* NetworkAccessManager::Factory::create(QObject* parent)
	{
		NetworkAccessManager* retval(new NetworkAccessManager(parent));
		QNetworkDiskCache* disk_cache(new QNetworkDiskCache(retval));
		if (_cache_directory.exists(".")) {
			disk_cache->setCacheDirectory(_cache_directory.absolutePath());
		}
		retval->setCache(disk_cache);
		return retval;
	}

	NetworkAccessManager* NetworkAccessManager::Instance()
	{
		static NetworkAccessManager retval(nullptr);
		return &retval;
	}

	NetworkAccessManager::NetworkAccessManager(QObject* parent)
	:
		QNetworkAccessManager(parent)
	{
	}

	NetworkAccessManager::~NetworkAccessManager()
	{

	}
}
