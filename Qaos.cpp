#include "Qaos.hpp"

/*
#include <QtQuick/QQuickItem>
#include "VLCPlayer/Config.hpp"
#include "VLCPlayer/QmlPlayer.hpp"
#include "VLCPlayer/QmlSource.hpp"
#include "VLCPlayer/QmlVideoOutput.hpp"
#include "VLCPlayer/QmlVideoPlayer.hpp"
#include "VLCPlayer/Core/Enums.hpp"
#include "VLCPlayer/Core/TrackModel.hpp"
*/

namespace Qaos {
	Qaos& Instance()
	{
		return *Qaos::Instance();
	}

#if !defined(QT_QAOS)
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
		QByteArray module(LIBVLCQT_QML_MODULE);
		const char *m = module.data();

		qmlRegisterUncreatableType<Vlc>(m, 1, 1, "Qaos.VLC", QStringLiteral("Vlc cannot be instantiated directly"));
		qmlRegisterUncreatableType<VlcQmlSource>(m, 1, 1, "Qaos.VLCSource", QStringLiteral("VlcQmlSource cannot be instantiated directly"));
		qmlRegisterUncreatableType<VlcTrackModel>(m, 1, 1, "Qaos.VLCTrackModel", QStringLiteral("VlcTrackModel cannot be instantiated directly"));

		qmlRegisterType<VlcQmlPlayer>(m, 1, 1, "Qaos.VLCPlayer");
		qmlRegisterType<VlcQmlVideoOutput>(m, 1, 1, "Qaos.VLCVideoOutput");
		qmlRegisterType<VlcQmlVideoPlayer>(m, 1, 0, "Qaos.VLCVideoPlayer");
*/
	}
#endif
}
