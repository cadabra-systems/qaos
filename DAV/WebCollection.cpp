#include "WebCollection.hpp"

namespace Qaos { namespace DAV {
	WebCollection::WebCollection(WebConnection* connection, bool include_base_uri)
	:
		QObject(connection),

		_abort(false),

		_connection(connection),
		_include_base_uri(include_base_uri),
		_mutex(new QRecursiveMutex()),

		_reply(nullptr),
		_path(),
		_busy(false)
	{

	}

	WebCollection::~WebCollection()
	{
		if (!_reply.isNull()) {
			_reply->deleteLater();
		}
	}

	bool WebCollection::fetch(const QString& path)
	{
		if (_busy || _reply || !_connection || path.isEmpty()) {
			return false;
		}
		_busy = true;
		_abort = false;
		_item_list.clear();
		_path = path;
		_reply =  _connection->list(_path, path.back() == '/');
		QObject::connect(_reply, &QNetworkReply::finished, this, &WebCollection::onFinished);

		return true;
	}

	void WebCollection::parseResponse(const QByteArray& data)
	{
		if (_abort) {
			return;
		}
		QDomDocument response_document;
		response_document.setContent(data, true);
		for (QDomNode n = response_document.documentElement().firstChild(); !n.isNull(); n = n.nextSibling()) {
			if (_abort) {
				return;
			}
			QDomElement response_element(n.toElement());
			if (response_element.isNull()) {
				continue;
			}
			const QString response_name(QUrl::fromPercentEncoding(response_element.namedItem("href").toElement().text().toUtf8()));
			if (response_name.isEmpty()) {
				continue;
			}

			/// @note ingore the path itself within the listing of the path
			if (!_include_base_uri && (_connection->makeRootPath() + _path) == response_name) { /// < Apache returns only path without scheme and authority
				continue;
			} else if (!_include_base_uri && response_name.startsWith("http")) { /// < MS IIS returns URL
				const QUrl response_url(response_name);
				if ((_connection->makeRootPath() + _path) == response_url.path()) {
					continue;
				}
			}

			if (parseResponse(response_element)) {
				/// @??? Сомнительная логика
				/// @brief container without slash at the end is a wrong answer, remove this item from the list
/*
				if (!_include_base_uri && _item_list.last().isDirectory() && !response_name.endsWith("/")) {
					if (response_name.startsWith("http")) {
						QUrl checkUrl(response_name);
						if ((_connection->makeRootPath() + _path) == checkUrl.path() + "/") {
							_item_list.removeLast();
						}
					} else if ((_connection->makeRootPath() + _path) == (response_name + "/")) {
						_item_list.removeLast();
					}
				}
*/
			}
		}
		std::sort(_item_list.begin(), _item_list.end());
	}

	bool WebCollection::parseResponse(const QDomElement& element)
	{
		if (_abort) {
			return false;
		}
		const QDomElement href(element.namedItem("href").toElement());
		return !href.isNull() ? parseStatusPropertyList(QUrl::fromPercentEncoding(href.text().toUtf8()), element.elementsByTagName("propstat")) : false;
	}

	bool WebCollection::parseStatusPropertyList(QString path, const QDomNodeList& list)
	{
		if (_abort) {
			return false;
		} else if (path.startsWith("http")) {
			path = QUrl(path).path();
		}

		const QStringList path_list(path.split('/', Qt::SkipEmptyParts));
		const QString name(path_list.isEmpty() ? "/" : path_list.back());
		if (name.isEmpty() || name.front() == '.') {
			return false;
		}

		QFlags<WebItem::Option> option_flagset;
		QString display_name;
		QString content_language;
		QString entity_tag;
		QString mime_type;
		QString source;
		quint64 size(0);
		QDateTime creation_datetime;
		QDateTime modification_datetime;
		for (int i = 0; i < list.count(); i++) {
			const QDomElement list_element(list.item(i).toElement());
			const QDomElement status(list_element.namedItem("status").toElement());

			if (status.isNull()) {
				qDebug() << "Error, no status code in this propstat";
				return false;
			} else if (_abort) {
				return false;
			} else if (parseResponseCode(status.text()) == 404) { /// < property not available
				continue;
			}

			const QDomElement property_list(list_element.namedItem("prop").toElement());
			if (property_list.isNull()) {
				qDebug() << "No prop segment in this propstat";
				return false;
			}

			for (QDomNode p = property_list.firstChild(); !p.isNull(); p = p.nextSibling()) {
				QDomElement property_element(p.toElement());
				if (_abort) {
					return false;
				} else if (property_element.isNull()) {
					continue;
				} else if (property_element.namespaceURI() != "DAV:") { /// < parse only DAV namespace properties
					continue;
				} else if (property_element.tagName() == "getcontentlength") {
					size = property_element.text().toULongLong();
				} else if (property_element.tagName() == "getlastmodified") {
					modification_datetime = parseDateTime(property_element.text(), property_element.attribute("dt"));
				} else if (property_element.tagName() == "resourcetype") {
					if (!property_element.namedItem("collection").toElement().isNull()) {
						option_flagset.setFlag(WebItem::Option::Directory);
					}
				} else {
					if (property_element.tagName() == "creationdate") {
						creation_datetime = parseDateTime(property_element.text(), property_element.attribute("dt"));
					} else if (property_element.tagName() == "displayname") {
						display_name = property_element.text();
					} else if (property_element.tagName() == "source") {
						const QDomElement source_element(property_element.namedItem("link").toElement().namedItem("dst").toElement());
						if (!source_element.isNull()) {
							source = source_element.text();
						}
					} else if (property_element.tagName() == "getcontentlanguage") {
						content_language = property_element.text();
					} else if (property_element.tagName() == "getcontenttype") {
						if (property_element.text() == "httpd/unix-directory") {
							option_flagset.setFlag(WebItem::Option::Directory);
						}
						mime_type = property_element.text();
					} else if (property_element.tagName() == "executable") {
						if (property_element.text() == "T") {
							option_flagset.setFlag(WebItem::Option::Executable);
						}
					} else if (property_element.tagName() == "getetag") {
						entity_tag = property_element.text();
					} else {
						qDebug() << "Unknown property:" << property_element.tagName() << property_element.text();
					}
				}
			}
		}

		if (option_flagset.testFlag(WebItem::Option::Directory) && !path.endsWith("/")) {
			path.append("/");
		}
		path.remove(0, _connection->makeRootPath().size());
		_item_list.append(WebItem{path, name, display_name, option_flagset, content_language, entity_tag, mime_type, source, size, creation_datetime, modification_datetime});

		return true;
	}

	int WebCollection::parseResponseCode(const QString& response)
	{
		const int first_space(response.indexOf(' '));
		const int second_space(response.indexOf(' ', first_space + 1));
		return response.midRef(first_space + 1, second_space - first_space - 1).toInt();
	}

	QDateTime WebCollection::parseDateTime(const QString& input, const QString& type)
	{
		QDateTime retval;
		/// @??? WebDAV всегда отдает дату в этой локали?
		static const QLocale locale(QLocale::English, QLocale::UnitedStates);
		if (type == "dateTime.tz") {
			retval =  QDateTime::fromString(input, Qt::ISODate);
		} else if ( type == "dateTime.rfc1123") {
			retval = locale.toDateTime(input);
		}

		if (retval.isValid()) {
			return retval;
		}

		retval = locale.toDateTime(input.left(25), "ddd, dd MMM yyyy hh:mm:ss");
		if (retval.isValid()) {
			return retval;
		}
		retval = locale.toDateTime(input.left(19), "yyyy-MM-ddThh:mm:ss");
		if (retval.isValid()) {
			return retval;
		}
		retval = locale.toDateTime(input.mid(5, 20) , "d MMM yyyy hh:mm:ss");
		if (retval.isValid()) {
			return retval;
		}
		retval = QDateTime(locale.toDate(input.mid(5, 11) , "d MMM yyyy"), locale.toTime(input.mid(17, 8) , "hh:mm:ss"));
		if(!retval.isValid()) {
			qDebug() << "Unknown date time format:" << input;
		}

		return retval;
	}

	QString WebCollection::makeFullPath() const
	{
		return !_connection ? "" : _connection->makeAbsolutePath(_path);
	}

	const QString& WebCollection::getPath() const
	{
		return _path;
	}

	const QList<WebItem>& WebCollection::getItemList() const
	{
		return _item_list;
	}

	void WebCollection::onAborted()
	{
		_abort = true;
		if (_reply) {
			_reply->abort();
			_reply->deleteLater();
		}
		_reply.clear();
		_busy = false;
	}

	void WebCollection::onFinished()
	{
		QNetworkReply* reply(qobject_cast<QNetworkReply*>(QObject::sender()));
		if (_reply != reply) {
			QMetaObject::invokeMethod(this, "onCleanedUp", Qt::QueuedConnection, Q_ARG(QNetworkReply*, reply));
			return;
		} else if (_abort) {
			return;
		}
		do {
			QMutexLocker locker(_mutex.data());
			const QString content_type(_reply->header(QNetworkRequest::ContentTypeHeader).toString());
			if ((_reply->error() != QNetworkReply::NoError) && (_reply->error() != QNetworkReply::OperationCanceledError)) {
				QString error_string(_reply->errorString());
				error_string = error_string.right(error_string.size() - error_string.indexOf("server replied:") + 1);
				emit failed(error_string);
				qDebug() << "Reply has error:" << _reply->errorString() << "Code:" << _reply->error();
			} else {
				QByteArray data = _reply->readAll();
				if (content_type.contains("xml")) {
					parseResponse(data);
				}
			}
			_reply.clear();
		} while (false);

		QMetaObject::invokeMethod(this, "onCleanedUp", Qt::QueuedConnection, Q_ARG(QNetworkReply*, reply));
	}

	void WebCollection::onCleanedUp(QNetworkReply* reply)
	{
		if (!reply) {
			return;
		} else if (!reply->isFinished() || reply->bytesToWrite() || reply->bytesAvailable()) {
			reply->readAll();
			reply->close();
			QMetaObject::invokeMethod(this, "onCleanedUp", Qt::QueuedConnection, Q_ARG(QNetworkReply*, reply));
			return;
		}
		do {
//			QMutexLocker locker(_mutex.data());
			QObject::disconnect(reply, nullptr, nullptr, nullptr);
			reply->deleteLater();
			_busy = false;
		} while (false);
		emit finished();
	}

	bool WebCollection::isEmpty() const
	{
		return _item_list.isEmpty();
	}

	bool WebCollection::isBusy() const
	{
		return _busy;
	}

	bool WebCollection::isFinished() const
	{
		return _reply ? _reply->isFinished() : true;
	}
} }
