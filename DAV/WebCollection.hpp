#ifndef Qaos_DAV_WebCollection_hpp
#define Qaos_DAV_WebCollection_hpp

#include "WebConnection.hpp"
#include "WebItem.hpp"

#include <QtCore/qglobal.h>
#include <QPointer>
#include <QSharedPointer>
#include <QScopedPointer>
#include <QDomDocument>
#include <QPointer>
#include <QDomElement>
#include <QDomNodeList>

namespace Qaos { namespace DAV {
	class WebCollection : public QObject
	{
	/** @name Qroperties */
	/** @{ */
	Q_OBJECT
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		explicit WebCollection(WebConnection* connection = nullptr, bool include_base_uri = false);
		virtual ~WebCollection();
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		bool _abort;

		WebConnection* const _connection;
		const bool _include_base_uri;
		const QScopedPointer<QRecursiveMutex> _mutex;

		QPointer<QNetworkReply> _reply;

		QList<WebItem> _item_list;
		QString _path;
		bool _busy;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		bool fetch(const QString& path);

	protected:
		void parseResponse(const QByteArray& data);
		bool parseResponse(const QDomElement& element);
		bool parseStatusPropertyList(QString path, const QDomNodeList& list);
		int parseResponseCode(const QString& response);
		QDateTime parseDateTime(const QString& input, const QString& type);
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
		QString makeFullPath() const;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const QString& getPath() const;
		const QList<WebItem>& getItemList() const;
	/** @} */

	/** @name Signals */
	/** @{ */
	signals:
		void finished();
		void failed(QString error);
	/** @} */

	/** @name Slots */
	/** @{ */
	public slots:
		void onAborted();

	protected slots:
//		void onFailed(QNetworkReply::NetworkError code);
		void onFinished();
		void onCleanedUp(QNetworkReply* reply);
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool isEmpty() const;
		bool isBusy() const;
		bool isFinished() const;
	/** @} */
	};
} }

#endif
