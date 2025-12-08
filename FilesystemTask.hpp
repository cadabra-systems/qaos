#ifndef Qaos_FilesystemTask_hpp
#define Qaos_FilesystemTask_hpp

#include "ThreadTaskManager.hpp"

#include <QDir>
#include <QString>
#include <QFileInfo>

namespace Qaos {
	class SearchFilesystemTask : public ThreadTask
	{
	/** @name Qroperties */
	/** @{ */
	Q_OBJECT
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		SearchFilesystemTask(const QDir& path, const QStringList& name_list, const QStringList& term_list);
		virtual ~SearchFilesystemTask() override;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		const QDir _root_directory;
		const QStringList _name_list;
		const QStringList _term_list;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		virtual void run() override;
	/** @} */

	/** @name Signals */
	/** @{ */
	signals:
		void progressed(QString file_path, int file_index, int file_count);
		void found(QString file_path, int file_offset);
	/** @} */
	};

/*
	class CopyFilesystemTask : public ThreadTask
	{

	};

	class MoveFilesystemTask : public ThreadTask
	{

	};

	class RemoveFilesystemTask : public ThreadTask
	{

	};
*/
}

#endif
