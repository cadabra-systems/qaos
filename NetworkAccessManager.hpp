/**
 @file NetworkAccessManager.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Qaos_NetworkAccessManager_hpp
#define Qaos_NetworkAccessManager_hpp

#include <QNetworkAccessManager>

#include <QDir>
#include <QQmlNetworkAccessManagerFactory>

namespace Qaos {
	class NetworkAccessManager : public QNetworkAccessManager
	{
	/** @name Classes */
	/** @{ */
	public:
		class Factory : public QQmlNetworkAccessManagerFactory
		{
		/** @name Constructors */
		/** @{ */
		public:
			Factory(const QDir& cache_directory);
			virtual ~Factory() override = default;
		/** @} */

		/** @name Constructors */
		/** @{ */
		public:
			const QDir _cache_directory;
		/** @} */

		/** @name Constructors */
		/** @{ */
		public:
			virtual QNetworkAccessManager* create(QObject* parent) override;
		/** @} */
		};
	/** @} */

	/** @name Qroperties */
	/** @{ */
	Q_OBJECT
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static NetworkAccessManager* Instance();
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		NetworkAccessManager(QObject* parent = nullptr);
		virtual ~NetworkAccessManager() override;
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
	/** @} */

	/** @name Procedures */
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

	/** @name Signals */
	/** @{ */
	signals:
	/** @} */

	/** @name Hooks */
	/** @{ */
	protected:
	/** @} */

	/** @name States */
	/** @{ */
	public:
	/** @} */

	/** @name Friends */
	/** @{ */
	/** @} */
	};
}

#endif
