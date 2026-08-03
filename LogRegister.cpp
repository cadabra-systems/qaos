/**
 @file LogRegister.cpp
 @date 12.02.15
 @author Daniil A Megrabyan
 */

#include "LogRegister.hpp"

#include <QtCore/qdebug.h>
#include <QLoggingCategory>

namespace Qaos {
	void CustomMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
	{
		LogRegister::GlobalInstance()->put(type, context, message);
	}

	QtMessageHandler SystemMessageHandler = qInstallMessageHandler(&CustomMessageHandler);

	LogRegister* LogRegister::GlobalInstance()
	{
		static LogRegister retval;
		return &retval;
	}

	LogRegister::LogRegister()
	:
		QObject(nullptr),

		_stream(&_file)
	{
	}

	LogRegister::~LogRegister()
	{
		_file.close();
	}

	bool LogRegister::open(const QString& filename)
	{
		if (_file.isOpen()) {
			return false;
		}
		_file.setFileName(filename);
		return _file.open(QFile::WriteOnly | QFile::Append) && _file.isWritable();
	}

	bool LogRegister::close()
	{
		if (!_file.isOpen()) {
			return false;
		}
		_file.close();
		return !_file.isOpen();
	}

	void LogRegister::filter(QtMsgType level)
	{
		QLoggingCategory::defaultCategory()->setEnabled(QtMsgType::QtDebugMsg, (level <= QtMsgType::QtDebugMsg));
		QLoggingCategory::defaultCategory()->setEnabled(QtMsgType::QtInfoMsg, (level <= QtMsgType::QtInfoMsg));
		QLoggingCategory::defaultCategory()->setEnabled(QtMsgType::QtWarningMsg, (level <= QtMsgType::QtWarningMsg));
		QLoggingCategory::defaultCategory()->setEnabled(QtMsgType::QtCriticalMsg, (level <= QtMsgType::QtCriticalMsg));
		QLoggingCategory::defaultCategory()->setEnabled(QtMsgType::QtFatalMsg, (level <= QtMsgType::QtFatalMsg));
	}

	bool LogRegister::dump(QFile& file)
	{
		if (!_file.isOpen()) {
			return false;
		} else if (file.fileName().isEmpty()) {
			_file.close();
			_file.open(QFile::WriteOnly | QFile::Truncate);
			return false;
		} else if (!file.isOpen()) {
			return false;
		}
		file.close();
		const QString filename(file.fileName());
		file.remove();
		_file.flush();
		_file.close();
		if (_file.rename(filename)) {
			file.open(QFile::ReadOnly);
		}
		_file.open(QFile::WriteOnly | QFile::Append);
		return true;
	}

	void LogRegister::put(QtMsgType type, const QMessageLogContext& context, const QString& message)
	{
		if (SystemMessageHandler) {
			SystemMessageHandler(type, context, message);
		}
		/// @todo Optional
		_stream <<  message;
		if (context.function) {
			_stream << " in " << context.function;
		}
		if (context.file) {
			_stream << " at " << context.file << ":" << context.line;
		}
		_stream << Qt::endl;

		/// @??? Qt::DirectConnection
		switch (type)
		{
			case QtDebugMsg:
//				fprintf(stdout, "[D] %s> %s (%s:%u)\n", function, message.toLocal8Bit().constData(), file, context.line);
//				fflush(stdout);
				emit debugMessaged(context.file, context.function, context.line, message);
				break;

			case QtInfoMsg:
//				fprintf(stdout, "[I] %s> %s (%s:%u)\n", function, message.toLocal8Bit().constData(), file, context.line);
//				fflush(stdout);
				emit infoMessaged(context.file, context.function, context.line, message);
				break;

			case QtWarningMsg:
//				fprintf(stdout, "[W] %s> %s (%s:%u)\n", function, message.toLocal8Bit().constData(), file, context.line);
//				fflush(stdout);
				emit warningMessaged(context.file, context.function, context.line, message);
				break;

			case QtCriticalMsg:
//				fprintf(stderr, "[C] %s> %s (%s:%u)\n", function, message.toLocal8Bit().constData(), file, context.line);
//				fflush(stderr);
				emit criticalMessaged(context.file, context.function, context.line, message);
				break;

			case QtFatalMsg:
//				fprintf(stderr, "[F] %s> %s (%s:%u)\n", function, message.toLocal8Bit().constData(), file, context.line);
//				fflush(stderr);
				emit fatalMessaged(context.file, context.function, context.line, message);
				break;
		}
	}
}
