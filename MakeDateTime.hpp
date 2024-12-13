/**
 @file MakeDateTime.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Qaos_MakeDateTime_hpp
#define Qaos_MakeDateTime_hpp

#include <QDateTime>
#include <QVariant>

namespace Qaos {
	inline QDateTime MakeDateTime(const QVariant& variant)
	{
		if (variant.isNull()) {
			return QDateTime();
		}
		const QMetaType::Type type(static_cast<QMetaType::Type>(variant.type()));
		if (QMetaType::LongLong == type) {
			return QDateTime::fromSecsSinceEpoch(variant.toLongLong(), Qt::UTC);
		} else if (QMetaType::QString == type) {
			QString string(variant.toString());
			return QDateTime::fromString(string.back() != 'Z' ? string.append('Z') : string, Qt::DateFormat::ISODateWithMs);
		} else if (variant.canConvert(QMetaType::QDateTime)) {
			return variant.toDateTime();
		}
		return QDateTime();
	}
}

#endif
