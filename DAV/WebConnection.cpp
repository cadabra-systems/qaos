#include "WebConnection.hpp"

namespace Qaos { namespace DAV {
	QString WebConnection::DigestToHex(const QByteArray &input)
	{
		const QByteArray input_hex(input.toHex());

		QString retval;
		for (int i = 0; i < input_hex.size(); i += 2) {
			retval.append(input_hex.at(i));
			retval.append(input_hex.at(i + 1));
			retval.append(":");
		}
		retval.chop(1);

		return retval.toUpper();
	}

	QByteArray WebConnection::HexToDigest(const QString& input)
	{
		QByteArray retval;
		int i(2), l(static_cast<int>(input.size()));
		retval.append(input.left(2).toLatin1());
		while ((i < l) && (input.at(i) == ':')) {
			retval.append(input.mid(++i, 2).toLatin1());
			i += 2;
		}
		return QByteArray::fromHex(retval);
	}

	WebConnection::WebConnection(QObject* parent)
	:
		QNetworkAccessManager(parent),

		_authenticator_reply(nullptr)
	{

	}

	WebConnection::WebConnection(const QUrl& base_url, QObject* parent)
	:
		QNetworkAccessManager(parent),

		_base_url(base_url),
		_authenticator_reply(nullptr)
	{
		QString path(base_url.path());
		if (!path.isEmpty() && path.back() != '/') {
			_base_url.setPath(path.append('/'));
		}
		QObject::connect(this, &WebConnection::finished, this, &WebConnection::onFinished);
		QObject::connect(this, &WebConnection::sslErrors, this, &WebConnection::onSSLError);
		QObject::connect(this, &WebConnection::authenticationRequired, this, &WebConnection::onAuthenication);
	}

	WebConnection::~WebConnection()
	{
	}

	QNetworkReply* WebConnection::list(const QString& path, bool depth)
	{
		/// @link http://www.webdav.org/specs/rfc4918.html#PROPERTY_getlastmodified
		WebConnection::PropertyNameMap query;
		QStringList property_list;
		property_list << "getlastmodified";
		property_list << "getcontentlength";
		property_list << "resourcetype";
		property_list << "creationdate";
		property_list << "displayname";
		property_list << "getcontentlanguage";
		property_list << "getcontenttype";
		property_list << "getetag";
		query["DAV:"] = property_list;

		return find(path, query, depth);
	}

	QNetworkReply* WebConnection::search(const QString& path, const QString& q)
	{
		QByteArray query("<?xml version=\"1.0\"?>\r\n");
		query.append("<d:searchrequest xmlns:d=\"DAV:\">\r\n");
		query.append(q.toUtf8());
		query.append("</d:searchrequest>\r\n");
		QUrl url(_base_url);
		url.setPath(makeAbsolutePath(path));
		QNetworkRequest request(url);

		return sendRequest("SEARCH", request, query);
	}

	QNetworkReply* WebConnection::get(const QString& path)
	{
		QUrl url(_base_url);
		url.setPath(makeAbsolutePath(path));
		QNetworkRequest request(makeRequest());
		request.setUrl(url);

		return QNetworkAccessManager::get(request);
	}

	QNetworkReply* WebConnection::get(const QString& path, QIODevice* data)
	{
		return get(path, data, 0);
	}

	QNetworkReply* WebConnection::get(const QString& path, QIODevice* data, quint64 range)
	{
		QUrl url(_base_url);
		url.setPath(makeAbsolutePath(path));

		QNetworkRequest request(makeRequest());
		request.setUrl(url);

		if (range > 0) {
			/// @note RFC2616 http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
			QByteArray fromRange = "bytes=" + QByteArray::number(range) + "-"; /// < byte-ranges-specifier
			request.setRawHeader("Range", fromRange);
		}

		QNetworkReply* reply(QNetworkAccessManager::get(request));
		_in_data_map.insert(reply, data);
		connect(reply, &QNetworkReply::readyRead, this, &WebConnection::onRead);
		connect(reply, &QNetworkReply::errorOccurred, this, &WebConnection::onNetworkError);

		return reply;
	}

	QNetworkReply* WebConnection::put(const QString& path, QIODevice* data)
	{
		QUrl url(_base_url);
		url.setPath(makeAbsolutePath(path));
		QNetworkRequest request(makeRequest());
		request.setUrl(url);

		return QNetworkAccessManager::put(request, data);
	}

	QNetworkReply* WebConnection::put(const QString& path, const QByteArray& data)
	{
		QUrl url(_base_url);
		url.setPath(makeAbsolutePath(path));
		QNetworkRequest request(makeRequest());
		request.setUrl(url);

		return QNetworkAccessManager::put(request, data);
	}

	QNetworkReply* WebConnection::find(const QString& path, const WebConnection::PropertyNameMap& map, bool depth)
	{
		QString query;
		query = R"(<?xml version="1.0" encoding="utf-8"?>)";
		query += R"(<d:propfind xmlns:d="DAV:">)";
		for (PropertyNameMap::ConstIterator ns = map.constBegin(); ns != map.constEnd(); ++ns) {
			for (const QString& key : ns.value()) {
				query += (ns.key() == "DAV:") ? "<d:" + key + "/>" : "<" + key + " xmlns=\"" + ns.key() + "\"/>";
			}
		}
		query += "</d:propfind>";

		return find(path, query.toUtf8(), depth);
	}

	QNetworkReply* WebConnection::find(const QString& path, const QByteArray& query, bool depth)
	{
		QUrl url(_base_url);
		url.setPath(makeAbsolutePath(path));
		QNetworkRequest request(makeRequest());
		request.setUrl(url);
		request.setRawHeader("Depth", QString::number(depth).toUtf8());

		return sendRequest("PROPFIND", request, query);
	}

	QNetworkReply* WebConnection::patch(const QString& path, const WebConnection::PropertyValueMap& map)
	{
		QString query;
		query = R"(<?xml version="1.0" encoding="utf-8" ?>)";
		query += R"(<d:patch xmlns:d="DAV:">)";
		query += "<d:prop>";
		for (PropertyValueMap::ConstIterator ns = map.constBegin(); ns != map.constEnd(); ++ns) {
			for (QMap<QString, QVariant>::ConstIterator p = ns->constBegin(); p != ns->constEnd(); ++p) {
					query += (ns.key() == "DAV:")
												? "<d:" + p.key() + ">" + p.value().toString() + "</d:" + p.key() + ">"
												: "<" + p.key() + " xmlns=\"" + ns.key() + "\">" + p.value().toString() + "</" + p.key() + " xmlns=\"" + ns.key() + "\"/>";

			}
		}
		query += "</d:prop>";
		query += "</d:patch>";

		return patch(path, query.toUtf8());
	}

	QNetworkReply* WebConnection::patch(const QString& path, const QByteArray& query)
	{
		QUrl url(_base_url);
		url.setPath(makeAbsolutePath(path));
		QNetworkRequest request(makeRequest());
		request.setUrl(url);

		return sendRequest("PROPPATCH", request, query);
	}

	QNetworkReply* WebConnection::mkdir(const QString& path)
	{
		QUrl url(_base_url);
		url.setPath(makeAbsolutePath(path));
		QNetworkRequest request(makeRequest());
		request.setUrl(url);

		return sendRequest("MKCOL", request);
	}

	QNetworkReply* WebConnection::copy(const QString& path_from, const QString& path_to, bool overwrite)
	{
		QUrl url(_base_url);
		url.setPath(makeAbsolutePath(path_from));
		QNetworkRequest request(makeRequest());
		request.setUrl(url);

		/**
		 * @brief RFC4918 Section 10.3 requires an absolute URI for destination raw header http://tools.ietf.org/html/rfc4918#section-10.3
		 * @brief RFC3986 Section 4.3 specifies the term absolute URI http://tools.ietf.org/html/rfc3986#section-4.3
		*/
		QUrl destination_url(_base_url);
		destination_url.setPath(makeAbsolutePath(path_to));
//		destination_url.setUserInfo("");
		request.setRawHeader("Destination", destination_url.toString().toUtf8());
		request.setRawHeader("Depth", "infinity");
		request.setRawHeader("Overwrite", overwrite ? "T" : "F");

		return sendRequest("COPY", request);
	}

	QNetworkReply* WebConnection::move(const QString& path_from, const QString& path_to, bool overwrite)
	{
		QUrl url(_base_url);
		url.setPath(makeAbsolutePath(path_from));

		QNetworkRequest request(makeRequest());
		request.setUrl(url);
		/**
		 * @brief RFC4918 Section 10.3 requires an absolute URI for destination raw header http://tools.ietf.org/html/rfc4918#section-10.3
		 * @brief RFC3986 Section 4.3 specifies the term absolute URI http://tools.ietf.org/html/rfc3986#section-4.3
		 */
		QUrl destination_url(_base_url);
		destination_url.setPath(makeAbsolutePath(path_to));
//		destination_url.setUserInfo("");
		request.setRawHeader("Destination", destination_url.toString().toUtf8());
		request.setRawHeader("Depth", "infinity");
		request.setRawHeader("Overwrite", overwrite ? "T" : "F");

		return sendRequest("MOVE", request);
	}

	QNetworkReply* WebConnection::remove(const QString& path)
	{
		QUrl url(_base_url);
		url.setPath(makeAbsolutePath(path));
		QNetworkRequest request(makeRequest());
		request.setUrl(url);
		return sendRequest("DELETE", request);
	}

	QString WebConnection::makeAbsolutePath(const QString& path)
	{
		return _base_url.path().append(path);
	}

	QString WebConnection::makeRootPath() const
	{
		/// @??? last slash
		return _base_url.path();
	}

	QNetworkRequest WebConnection::makeRequest()
	{
		QNetworkRequest retval;
		if (!_base_url.userName().isEmpty()) {
			retval.setRawHeader("Authorization", QString("Basic ").append(QByteArray((_base_url.userName() + ":" + _base_url.password()).toLocal8Bit().toBase64())).toLocal8Bit());
		}
		return retval;
	}

	QNetworkReply* WebConnection::sendRequest(const QString& method, QNetworkRequest& request, QIODevice* data)
	{
		if (data != nullptr && data->size() !=0) {
			request.setHeader(QNetworkRequest::ContentLengthHeader, data->size());
			request.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml; charset=utf-8");
		}
		return sendCustomRequest(request, method.toLatin1(), data);
	}

	QNetworkReply* WebConnection::sendRequest(const QString& method, QNetworkRequest& request, const QByteArray& data)
	{
		QScopedPointer<QBuffer> buffer(new QBuffer());
		buffer->setData(data);
		buffer->open(QIODevice::ReadOnly);
		QNetworkReply* reply(sendRequest(method, request, buffer.get()));
		_out_data_map.insert(reply, buffer.take());
		return reply;
	}

	const QUrl& WebConnection::getBaseURL() const
	{
		return _base_url;
	}

	void WebConnection::setMD5Digest(const QString& digest)
	{
		_md5_digest = HexToDigest(digest);
	}

	void WebConnection::setSHA1Digest(const QString& digest)
	{
		_sha1_digest = HexToDigest(digest);
	}

	void WebConnection::onFinished(QNetworkReply* reply)
	{
		disconnect(reply, &QNetworkReply::redirectAllowed, this, &WebConnection::onRead);
		disconnect(reply, &QNetworkReply::errorOccurred, this, &WebConnection::onNetworkError);

		QIODevice* input_device(_in_data_map.value(reply, 0));
		if (input_device) {
			input_device->write(reply->readAll());
			static_cast<QFile*>(input_device)->flush();
			input_device->close();
			delete input_device;
		}
		_in_data_map.remove(reply);

		QIODevice* output_device(_out_data_map.value(reply, 0));
		if (output_device) {
			output_device->deleteLater();
		}
		_out_data_map.remove(reply);
	}

	void WebConnection::onAuthenication(QNetworkReply* reply, QAuthenticator* authenticator)
	{
		if (reply == _authenticator_reply) {
			reply->abort();
			emit networkFailed("WebDAV server requires authentication");
			reply->deleteLater();
			reply = nullptr;
		}
		_authenticator_reply = reply;
		authenticator->setUser(_base_url.userName());
		authenticator->setPassword(_base_url.password());
	}

	void WebConnection::onSSLError(QNetworkReply* reply, const QList<QSslError>& error_list)
	{
		if (!reply) {
			return;
		}
		qDebug() << "SSL error:" << reply->url().toString(QUrl::RemoveUserInfo);
		const QSslCertificate ssl_certificate(error_list[0].certificate());
		if ((ssl_certificate.digest(QCryptographicHash::Md5) == _md5_digest) && (ssl_certificate.digest(QCryptographicHash::Sha1) == _sha1_digest)) {
			/// @note user accepted this SSL certifcate already ==> ignore SSL errors
			reply->ignoreSslErrors();
		} else {
			/// @note user has to check the SSL certificate and has to accept manually
			emit sslFailed(error_list);
			reply->abort();
		}
	}

	void WebConnection::onNetworkError(QNetworkReply::NetworkError)
	{
		QNetworkReply* reply(qobject_cast<QNetworkReply*>(QObject::sender()));
		if (!reply) {
			return;
		}
		qDebug() << "Network error:" << reply->url().toString(QUrl::RemoveUserInfo);
		if (reply->error() == QNetworkReply::OperationCanceledError) {
			QIODevice* io_device(_in_data_map.value(reply, 0));
			if (io_device) {
				delete io_device;
				_in_data_map.remove(reply);
			}
			return;
		}
		emit networkFailed(reply->errorString());
	}

	void WebConnection::onRead()
	{
		QNetworkReply* reply(qobject_cast<QNetworkReply*>(QObject::sender()));
		if (reply->bytesAvailable() < 256000) {
			return;
		}
		QIODevice* io_device(_in_data_map.value(reply, 0));
		if (io_device) {
			io_device->write(reply->readAll());
		}
	}
} }
