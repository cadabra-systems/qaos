#include "Qaos.hpp"

namespace Qaos {
	Qaos& Instance()
	{
		return *Qaos::Instance();
	}

	Qaos* Qaos::Instance()
	{
		static Qaos retval;
		return &retval;
	}

	QObject* Qaos::QInstance(QQmlEngine* engine, QJSEngine* script_engine)
	{
		Q_UNUSED(engine)
		Q_UNUSED(script_engine)

		Qaos* retval(Qaos::Qaos::Instance());

		QQmlEngine::setObjectOwnership(retval, QQmlEngine::CppOwnership);
		return retval;
	}

	Qaos::Qaos(QObject* parent)
	:
		QObject(parent)
	{
		qmlRegisterSingletonType<Qaos>("Qaos", 0, 1, "Qaos", Qaos::QInstance);

/*
		qmlRegisterUncreatableType<>(?, 1, 1, "Qaos.xyz", QStringLiteral(""));
		qmlRegisterType<>(?, 1, 0, "Qaos.xyz");
*/
	}

	QMap<QString, QVariant> Qaos::getDataRoleMap(QAbstractItemModel* model) const
	{
		QMap<QString, QVariant> retval;
		if (!model) {
			return retval;
		}
		QHash<int, QByteArray> hash(model->roleNames());
		for (QHash<int, QByteArray>::ConstIterator r = hash.constBegin(); r != hash.constEnd(); ++r) {
			retval.insert(r.value(), r.key());
		}
		return retval;
	}
}
