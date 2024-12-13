#ifndef Qaos_Zip_hpp
#define Qaos_Zip_hpp

#include <QtCore>
#include <QtCore/qglobal.h>
//#include <QtGui/private/qtguiglobal_p.h>
#include <QFile>
#include <QString>
#include <QDateTime>
#include <QScopedPointer>

namespace Qaos {
	/**
	 * @brief The ZipArchiver class
	 */
	class ZipArchiver
	{
	/** @name Classes */
	/** @{ */
	public:
		enum Status
		{
			NoError,
			FileWriteError,
			FileReadError,
			FileOpenError,
			FilePermissionsError,
			FileError
		};

		enum CompressionPolicy
		{
			AlwaysCompress,
			NeverCompress,
			AutoCompress
		};

		/// @details http://www.pkware.com/documents/casestudies/APPNOTE.TXT
		enum HostOS
		{
			HostFAT = 0,
			HostAMIGA = 1,
			HostVMS = 2,  /// < VAX/VMS
			HostUnix = 3,
			HostVM_CMS = 4,
			HostAtari = 5,  /// < what if it's a minix filesystem? [cjh]
			HostHPFS = 6,  /// < filesystem used by OS/2 (and NT 3.x)
			HostMac = 7,
			HostZ_System = 8,
			HostCPM = 9,
			HostTOPS20 = 10, /// < pkzip 2.50 NTFS
			HostNTFS = 11, /// < filesystem used by Windows NT
			HostQDOS = 12, /// < SMS/QDOS
			HostAcorn = 13, /// < Archimedes Acorn RISC OS
			HostVFAT = 14, /// < filesystem used by Windows 95, NT
			HostMVS = 15,
			HostBeOS = 16, /// < hybrid POSIX/database filesystem
			HostTandem = 17,
			HostOS400 = 18,
			HostOSX = 19
		};

		enum GeneralPurposeFlag
		{
			Encrypted = 0x01,
			AlgTune1 = 0x02,
			AlgTune2 = 0x04,
			HasDataDescriptor = 0x08,
			PatchedData = 0x20,
			StrongEncrypted = 0x40,
			Utf8Names = 0x0800,
			CentralDirectoryEncrypted = 0x2000
		};

		enum CompressionMethod
		{
			CompressionMethodStored = 0,
			CompressionMethodShrunk = 1,
			CompressionMethodReduced1 = 2,
			CompressionMethodReduced2 = 3,
			CompressionMethodReduced3 = 4,
			CompressionMethodReduced4 = 5,
			CompressionMethodImploded = 6,
			CompressionMethodReservedTokenizing = 7, // reserved for tokenizing
			CompressionMethodDeflated = 8,
			CompressionMethodDeflated64 = 9,
			CompressionMethodPKImploding = 10,

			CompressionMethodBZip2 = 12,

			CompressionMethodLZMA = 14,

			CompressionMethodTerse = 18,
			CompressionMethodLz77 = 19,

			CompressionMethodJpeg = 96,
			CompressionMethodWavPack = 97,
			CompressionMethodPPMd = 98,
			CompressionMethodWzAES = 99
		};

		enum class WindowsFileAttribute : quint32
		{
			Dir = 0x10, // FILE_ATTRIBUTE_DIRECTORY
			File = 0x80, // FILE_ATTRIBUTE_NORMAL
			TypeMask = 0x90,

			ReadOnly = 0x01, // FILE_ATTRIBUTE_READONLY
			PermMask = 0x01
		};

		enum class UnixFileAttribute : quint32
		{
			Dir = 0040000, // __S_IFDIR
			File = 0100000, // __S_IFREG
			SymLink = 0120000, // __S_IFLNK
			TypeMask = 0170000, // __S_IFMT

			ReadUser = 0400, // __S_IRUSR
			WriteUser = 0200, // __S_IWUSR
			ExeUser = 0100, // __S_IXUSR
			ReadGroup = 0040, // __S_IRGRP
			WriteGroup = 0020, // __S_IWGRP
			ExeGroup = 0010, // __S_IXGRP
			ReadOther = 0004, // __S_IROTH
			WriteOther = 0002, // __S_IWOTH
			ExeOther = 0001, // __S_IXOTH
			PermMask = 0777
		};

		struct FileInfo
		{
			FileInfo() noexcept;
			operator bool() const noexcept;

			QString file_path;

			uint is_dir : 1;
			uint is_file : 1;
			uint is_symlink : 1;
			QFile::Permissions permission_mode;
			uint crc;
			qint64 size;
			QDateTime modification_date;
		};

		struct LocalFileHeader
		{
			uchar signature[4]; //  0x04034b50
			uchar version_needed[2];
			uchar general_purpose_bits[2];
			uchar compression_method[2];
			uchar last_mod_file[4];
			uchar crc_32[4];
			uchar compressed_size[4];
			uchar uncompressed_size[4];
			uchar file_name_length[2];
			uchar extra_field_length[2];
		};

		struct CentralFileHeader
		{
			uchar signature[4]; // 0x02014b50
			uchar version_made[2];
			uchar version_needed[2];
			uchar general_purpose_bits[2];
			uchar compression_method[2];
			uchar last_mod_file[4];
			uchar crc_32[4];
			uchar compressed_size[4];
			uchar uncompressed_size[4];
			uchar file_name_length[2];
			uchar extra_field_length[2];
			uchar file_comment_length[2];
			uchar disk_start[2];
			uchar internal_file_attributes[2];
			uchar external_file_attributes[4];
			uchar offset_local_header[4];
		};

		struct FileHeader
		{
			CentralFileHeader h;
			QByteArray file_name;
			QByteArray extra_field;
			QByteArray file_comment;
		};

		struct EndOfDirectory
		{
			uchar signature[4]; // 0x06054b50
			uchar this_disk[2];
			uchar start_of_directory_disk[2];
			uchar num_dir_entries_this_disk[2];
			uchar num_dir_entries[2];
			uchar directory_size[4];
			uchar dir_start_offset[4];
			uchar comment_length[2];
		};

		struct DataDescriptor
		{
			uchar crc_32[4];
			uchar compressed_size[4];
			uchar uncompressed_size[4];
		};
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		/**
		 * @brief Zip standard version for archives handled by this API (actually, the only basic support of this version is implemented but it is enough for now)
		 */
		static constexpr ushort Version = 20;

		static inline uint UnsignedInteger(const uchar* input);
		static inline void UnsignedInteger(uchar* output, uint input);
		static inline void UnsignedInteger(uchar* output, const uchar* input);

		static inline ushort UnsignedShort(const uchar* input);
		static inline void UnsignedShort(uchar* output, ushort input);
		static inline void UnsignedShort(uchar* output, const uchar* input);

		static QDateTime MSDOSDate(const uchar* src);
		static void MSDOSDate(uchar* output, const QDateTime& input);

		static QFile::Permissions ModePermission(quint32 mode);
		static quint32 PermissionMode(QFile::Permissions mode);

		static LocalFileHeader LocalHeader(const CentralFileHeader& input);
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		ZipArchiver(QSharedPointer<QIODevice> io_device);
		virtual ~ZipArchiver();
	/** @} */

	/** @name Properties */
	/** @{ */
	protected:
		QSharedPointer<QIODevice> _io_device;
		ZipArchiver::Status _status;
		QVector<FileHeader> _file_header_vector;
		bool _dirty_file_tree;
		uint _offset_directory;
		QByteArray _comment;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void close();
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
		ZipArchiver::FileInfo makeFileInfo(int index) const;
		ulong makeCRC32() const;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		QIODevice* getDevice() const;
		Status getStatus() const;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool isExist() const;
	/** @} */
	};

	/**
	 * @brief The ZipReader class
	 */
	class ZipReader : public ZipArchiver
	{
	/** @name Aliases */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		ZipReader();
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
	/** @} */

	/** @name Procedures */
	/** @{ */
	private:
		bool scan();

	public:
		bool open(const QString& file_path);
		bool extract(const QString& base_path) const;
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		int getCount() const;
		QVector<FileInfo> getFileInfoList() const;
		QByteArray getFileData(const QString& file_name) const;
		FileInfo getEntryInfoAt(int index) const;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool isReadable() const;
	/** @} */
	};

	/**
	 * @brief The ZipWriter class
	 */
	class ZipWriter : public ZipArchiver
	{
	/** @name Classes */
	/** @{ */
	public:
		enum class EntryType : quint32
		{
			Directory = static_cast<quint32>(ZipArchiver::UnixFileAttribute::Dir),
			File  = static_cast<quint32>(ZipArchiver::UnixFileAttribute::File),
			SymLink = static_cast<quint32>(ZipArchiver::UnixFileAttribute::SymLink)
		};
	/** } */

	/** @name Constructors */
	/** @{ */
	public:
		ZipWriter();
		ZipWriter(QSharedPointer<QIODevice> io_device);
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		QFile::Permissions _permission_mode;
		ZipArchiver::CompressionPolicy _compression_policy;
	/** } */

	/** @name Procedures */
	/** @{ */
	public:
		bool open(const QString& file_path);
		bool save();
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		CompressionPolicy getCompressionPolicy() const;
	/** @} */

	/** @name Mutators */
	/** @{ */
	private:
		bool add(EntryType type, const QString& name, const QByteArray& data);

	public:
		bool addDirectory(QString name);
		bool addSymLink(const QString &name, const QString& path);
		bool addFile(const QString& name, const QByteArray& data);
		bool addFile(const QString& name, QIODevice* io_device);

		void setCompressionPolicy(CompressionPolicy policy);
		void setPermissionMode(QFile::Permissions mode);
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool isWritable() const;
	/** @} */
	};
}

#endif
