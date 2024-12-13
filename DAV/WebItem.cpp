#include "WebItem.hpp"

#include "../NaturalSort.hpp"

namespace Qaos { namespace DAV {
	WebItem::WebItem(const QString& path, const QString& name, const QString& display_name, QFlags<Option> option_flagset, const QString& content_language, const QString& entity_tag, const QString& mime_type, const QString& source, quint64 size, const QDateTime& creation_datetime, const QDateTime& modification_datetime)
	:
		_path(path),
		_name(name),
		_display_name(display_name),
		_option_flagset(option_flagset),
		_content_language(content_language),
		_entity_tag(entity_tag),
		_mime_type(mime_type),
		_source(source),
		_size(size),
		_creation_datetime(creation_datetime),
		_modification_datetime(modification_datetime)
	{

	}

	QString WebItem::extractExtension() const
	{
		const int offset(_name.lastIndexOf("."));
		return (offset == -1) ? "" : _name.right(_name.size() - offset - 1).toUpper();
	}

	const QString& WebItem::getPath() const
	{
		return _path;
	}

	const QString& WebItem::getName() const
	{
		return _name;
	}

	const QString& WebItem::getDisplayName() const
	{
		return _display_name;
	}

	const QString& WebItem::getContentLanguage() const
	{
		return _content_language;
	}

	const QString& WebItem::getEntityTag() const
	{
		return _entity_tag;
	}

	const QString& WebItem::getMIMEType() const
	{
		return _mime_type;
	}

	const QString& WebItem::getSource() const
	{
		return _source;
	}

	const quint64& WebItem::getSize() const
	{
		return _size;
	}

	const QDateTime& WebItem::getCreationDateTime() const
	{
		return _creation_datetime;
	}

	const QDateTime& WebItem::getModificationDateTime() const
	{
		return _modification_datetime;
	}

	bool WebItem::isDirectory() const
	{
		return _option_flagset.testFlag(Option::Directory);
	}

	bool WebItem::isExecutable() const
	{
		return _option_flagset.testFlag(Option::Executable);
	}

	bool WebItem::operator<(const WebItem& rhs) const
	{
		return isDirectory() != rhs.isDirectory() ? isDirectory() : NaturalSort::NaturalCompare(_name.toLower(), rhs.getName().toLower()) < 0;
	}
} }
