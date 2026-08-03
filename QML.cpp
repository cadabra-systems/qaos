#include "QML.hpp"

#include "Qaos.hpp"

#include <QDebug>

namespace Qaos {
	QML* QML::create(QQmlEngine*/* engine*/, QJSEngine*/* script_engine*/)
	{
		static QML retval;
		return &retval;
	}

	QML::QML(QObject* parent)
	:
		QObject(parent)
	{
	}

	QML::~QML()
	{
	}

	int QML::TypeId(const char* uri, const char* name, int major, int minor, bool critical_mode)
	{
		if (!uri || !name) {
			if (critical_mode) {
				QaosCritical() << "Unspecified URI or name";
			}
			return -1;
		}
		const int retval(qmlTypeId(uri, major, minor, name));
		if (retval < 0) {
			(critical_mode ? QaosCritical() : QaosDebug()) << "Unknown type" << name << "in" << uri;
		} else {
			QaosDebug() << "QML Type" << name << "in" << uri << "registered with" << retval << "id";
		}
		return retval;
	}
}
