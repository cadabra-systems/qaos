#include "FilesystemTask.hpp"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

namespace Qaos {
	SearchFilesystemTask::SearchFilesystemTask(const QDir& root_directory, const QStringList& name_list, const QStringList& term_list)
	:
		ThreadTask(),

		_root_directory(root_directory),
		_name_list(name_list),
		_term_list(term_list)
	{

	}

	SearchFilesystemTask::~SearchFilesystemTask()
	{

	}

	void SearchFilesystemTask::run()
	{
		QList<QRegularExpression> name_regex_list;
		for (const QString& name_pattern : _name_list) {
			if (!name_pattern.isEmpty()) {
				name_regex_list.append(QRegularExpression{QRegularExpression::wildcardToRegularExpression(name_pattern)});
			}
		}

		QList<QRegularExpression> term_regex_list;
		for (const QString& term_pattern : _term_list) {
			if (!term_pattern.isEmpty()) {
				term_regex_list.append(QRegularExpression{term_pattern});
			}
		}

		QList<QFileInfo> file_info_list;
		QStringList directory_path_list(_root_directory.path());
		while (!directory_path_list.isEmpty()/* && !abort */) {
			QDir directory(directory_path_list.takeFirst());
			if (!directory.exists()) {
				continue;
			}

			file_info_list.append(directory.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable));
			const QFileInfoList subdirectory_info_list(directory.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot));
			for (const QFileInfo& subdirectory_info : subdirectory_info_list) {
				directory_path_list.append(subdirectory_info.absoluteFilePath());
			}
		}

		const qsizetype total_files_count(file_info_list.size());
		for (qsizetype file_index = 0; file_index < total_files_count; ++file_index) {
			const QFileInfo& file_info(file_info_list.at(file_index));

			/// @??? Not each iteration
			emit progressed(file_info.filePath(), file_index + 1, total_files_count);

			if (!name_regex_list.isEmpty()) {
				bool name_matches(false);
				for (const QRegularExpression& name_regex : name_regex_list) {
					if (name_regex.match(file_info.fileName()).hasMatch()) {
						name_matches = true;
						break;
					}
				}
				if (!name_matches) {
					continue;
				}
			}

			if (!term_regex_list.isEmpty()) {
				QFile file(file_info.absoluteFilePath());
				if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
					QTextStream stream(&file);
					qsizetype current_offset(0);
					while (!stream.atEnd()) {
						const QString line(stream.readLine());
						for (const QRegularExpression& term_regex : term_regex_list) {
							QRegularExpressionMatchIterator match_iterator(term_regex.globalMatch(line));
							while (match_iterator.hasNext()) {
								QRegularExpressionMatch match(match_iterator.next());
								const qsizetype file_offset(current_offset + match.capturedStart());
								emit found(file_info.filePath(), file_offset);
							}
						}
						current_offset += line.length() + 1;
					}
					file.close();
				}
			} else {
				emit found(file_info.filePath(), -1);
			}
		}
	}
}
