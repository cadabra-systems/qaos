#ifndef Qaos_DAV_WebConnection_hpp
#define Qaos_DAV_WebConnection_hpp

#include <QtCore>
#include <QtCore/qglobal.h>
#include <QtNetwork>

namespace Qaos { namespace DAV {
	/**
	 * @brief WebConnection class
	 * @todo lock, unlock
	 */
	class WebConnection : public QNetworkAccessManager
	{
	Q_OBJECT
	/** @name Statics */
	/** @{ */
	public:
		/**
		 * @brief converts a digest from QByteArray to hexadecimal format (XX:XX:XX:... with X in [0-9,A-F])
		 * @param input
		 * @return
		 */
		static QString DigestToHex(const QByteArray &input);
		/**
		 * @brief converts a digest from hexadecimal format (XX:XX:XX:... with X in [0-9,A-F] ) to QByteArray
		 * @param input
		 * @return
		 */
		static QByteArray HexToDigest(const QString& input);
	/** @} */

	/** @name Aliases */
	/** @{ */
	public:
		using PropertyValueMap = QMap<QString, QMap<QString, QVariant>>;
		using PropertyNameMap = QMap<QString, QStringList>;
	/** @} */

	/** @name Classes */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		WebConnection(QObject* parent = nullptr);
		WebConnection(const QUrl& base_url, QObject* parent = nullptr);
		WebConnection(const WebConnection& origin) = delete;
		WebConnection(WebConnection&& origin) = delete;
		virtual ~WebConnection();
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		QUrl _base_url;
		QByteArray _md5_digest;
		QByteArray _sha1_digest;

		QMap<QNetworkReply*, QIODevice*> _out_data_map;
		QMap<QNetworkReply*, QIODevice*> _in_data_map;
		QNetworkReply* _authenticator_reply;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		QNetworkReply* list(const QString& path, bool depth);
		QNetworkReply* search(const QString& path, const QString& query);
		QNetworkReply* get(const QString& path);
		QNetworkReply* get(const QString& path, QIODevice* data);
		QNetworkReply* get(const QString& path, QIODevice* data, quint64 range);
		QNetworkReply* put(const QString& path, QIODevice* data);
		QNetworkReply* put(const QString& path, const QByteArray& data);
		QNetworkReply* mkdir(const QString& name);
		QNetworkReply* copy(const QString& path_from, const QString& path_to, bool overwrite = false);
		QNetworkReply* move(const QString& path_from, const QString& path_to, bool overwrite = false);
		QNetworkReply* remove(const QString& path);
		QNetworkReply* find(const QString& path, const QByteArray& query, bool depth = false);
		QNetworkReply* find(const QString& path, const WebConnection::PropertyNameMap& map, bool depth = false);
		QNetworkReply* patch(const QString& path, const WebConnection::PropertyValueMap& map);
		QNetworkReply* patch(const QString& path, const QByteArray& query);
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		WebConnection& operator()(const WebConnection& origin);
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
		QString makeAbsolutePath(const QString& path);
		QString makeRootPath() const;

	private:
		QNetworkRequest makeRequest();
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		QNetworkReply* sendRequest(const QString& method, QNetworkRequest& request, QIODevice* data = nullptr);
		QNetworkReply* sendRequest(const QString& method, QNetworkRequest& request, const QByteArray& data);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const QUrl& getBaseURL() const;
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		void setMD5Digest(const QString& digest);
		void setSHA1Digest(const QString& digest);
	/** @} */

	/** @name Signals */
	/** @{ */
	signals:
		void networkFailed(QString error);
		void sslFailed(const QList<QSslError>& error_list);
	/** @} */

	/** @name Slots */
	/** @{ */
	protected slots:
		void onFinished(QNetworkReply*);
		void onAuthenication(QNetworkReply* reply, QAuthenticator* authenticator);
		void onSSLError(QNetworkReply* reply, const QList<QSslError>& error_list);
		void onNetworkError(QNetworkReply::NetworkError);
		void onRead();
	/** @} */

	/** @name States */
	/** @{ */
	public:
	/** @} */
	};
} }

#endif
