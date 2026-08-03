#include "Qaos.hpp"

#include "LogRegister.hpp"

#include "QML.hpp"
#include "Range.hpp"

namespace Qaos {
	Qaos* GlobalInstance()
	{
		return Qaos::GlobalInstance();
	}

	Qaos* Qaos::GlobalInstance()
	{
		static Qaos retval;
		return &retval;
	}

	Qaos* Qaos::create(QQmlEngine* engine, QJSEngine* script_engine)
	{
		Qaos* retval(Qaos::Qaos::GlobalInstance());
		QQmlEngine::setObjectOwnership(retval, QQmlEngine::CppOwnership);
		return retval;
	}

	Qaos::Qaos(QObject* parent)
	:
		QObject(parent)
	{
		QML::TypeId("Qaos", "Range", 0, 1, true);
		QML::TypeId("Qaos", "DynamicRole", 0, 1, true);
		QML::TypeId("Qaos", "ObjectList", 0, 1, true);
		QML::TypeId("Qaos", "VariantItemModel", 0, 1, true);
		QML::TypeId("Qaos", "SortFilterProxyModel", 0, 1, true);
		QML::TypeId("Qaos", "ObjectPathModel", 0, 1, true);
		QML::TypeId("Qaos", "ObjectTreeModel", 0, 1, true);
	}

	Qaos::~Qaos()
	{

	}

	bool Qaos::configure(int& argc, char** argv)
	{
		QStringList string_list;
		for (int a = 0; a < argc; ++a) {
			string_list.append(argv[a]);
		}
		return configure(string_list);
	}

	bool Qaos::configure(const QStringList& argument_list)
	{
		const QCommandLineOption log_level_option("log-level", "Logging <debug|info|warning|critical|fatal|>", "level", "");
		QCommandLineParser cli_parser;
		if (!cli_parser.addOption(log_level_option)) {
			return false;
		}
		cli_parser.parse(argument_list);

		if (cli_parser.isSet(log_level_option)) {
			const QString log_level(cli_parser.value(log_level_option));
			if (log_level.isEmpty()) {
				LogRegister::GlobalInstance()->filter(QtMsgType::QtInfoMsg);
			} else if (0 == log_level.compare("debug")) {
				LogRegister::GlobalInstance()->filter(QtMsgType::QtDebugMsg);
			} else if (0 == log_level.compare("info")) {
				LogRegister::GlobalInstance()->filter(QtMsgType::QtInfoMsg);
			} else if (0 == log_level.compare("warning")) {
				LogRegister::GlobalInstance()->filter(QtMsgType::QtWarningMsg);
			} else if (0 == log_level.compare("critical")) {
				LogRegister::GlobalInstance()->filter(QtMsgType::QtCriticalMsg);
			} else if (0 == log_level.compare("fatal")) {
				LogRegister::GlobalInstance()->filter(QtMsgType::QtFatalMsg);
			}
		}

		return true;
	}

	QMap<QString, QVariant> Qaos::makeDataRoleMap(QAbstractItemModel* model) const
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

	::Qaos::ObjectPathModel* Qaos::makeObjectPathModel(QObject* root) const
	{
		return new ::Qaos::ObjectPathModel(root);
	}

	::Qaos::ObjectTreeModel* Qaos::makeObjectTreeModel(QObject* root) const
	{
		return new ::Qaos::ObjectTreeModel(root);
	}
}
