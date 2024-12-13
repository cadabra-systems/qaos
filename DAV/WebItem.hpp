#ifndef Qaos_DAV_WebItem_hpp
#define Qaos_DAV_WebItem_hpp

#include <QtCore/qglobal.h>
#include <QDateTime>
#include <QObject>
#include <QFlags>

namespace Qaos { namespace DAV {
	class WebItem
	{
	/** @name Qroperties */
	/** @{ */
	Q_GADGET
	/** @} */

	/** @name Classes */
	/** @{ */
	public:
		enum Option : quint8
		{
			None = 0b00,
			Directory = 0b01,
			Executable = 0b10
		};
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		WebItem(const QString& path, const QString& name, const QString& display_name, QFlags<Option> option_flagset, const QString& content_language, const QString& entity_tag, const QString& mime_type, const QString& source, quint64 size, const QDateTime& creation_datetime, const QDateTime& modification_datetime);
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
		QString _path;
		QString _name;
		QString _display_name;
		QFlags<Option> _option_flagset;
		QString _content_language;
		QString _entity_tag;
		QString _mime_type;
		QString _source;
		quint64 _size;
		QDateTime _creation_datetime;
		QDateTime _modification_datetime;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		bool operator<(const WebItem& rhs) const;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		QString extractExtension() const;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		const QString& getPath() const;
		const QString& getName() const;
		const QString& getDisplayName() const;
		const QString& getContentLanguage() const;
		const QString& getEntityTag() const;
		const QString& getMIMEType() const;
		const QString& getSource() const;
		const quint64& getSize() const;
		const QDateTime& getModificationDateTime() const;
		const QDateTime& getCreationDateTime() const;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool isDirectory() const;
		bool isExecutable() const;
	/** @} */
	};
} }

#endif
