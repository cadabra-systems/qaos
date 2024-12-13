#include "Zip.hpp"

#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QtEndian>

#include <qglobal.h>
#include <zlib.h>

namespace Qaos {
	static int inflate(Bytef* output, ulong* output_length, const Bytef* input, ulong input_length)
	{
		z_stream stream;
		stream.next_in = const_cast<Bytef*>(input);
		stream.avail_in = static_cast<uInt>(input_length);
		if (static_cast<uLong>(stream.avail_in) != input_length) {
			return Z_BUF_ERROR;
		}

		stream.next_out = output;
		stream.avail_out = (uInt)*output_length;
		if (static_cast<uLong>(stream.avail_out) != *output_length) {
			return Z_BUF_ERROR;
		}

		stream.zalloc = (alloc_func)nullptr;
		stream.zfree = (free_func)nullptr;

		int error = inflateInit2(&stream, -MAX_WBITS);
		if (error != Z_OK) {
			return error;
		}
		error = inflate(&stream, Z_FINISH);
		if (error != Z_STREAM_END) {
			inflateEnd(&stream);
			if (error == Z_NEED_DICT || (error == Z_BUF_ERROR && stream.avail_in == 0)) {
				return Z_DATA_ERROR;
			}
			return error;
		}
		*output_length = stream.total_out;
		error = inflateEnd(&stream);
		return error;
	}

	static int deflate(Bytef* output, ulong* output_length, const Bytef* input, ulong input_length)
	{
		z_stream stream;
		stream.next_in = const_cast<Bytef*>(input);
		stream.avail_in = static_cast<uInt>(input_length);
		stream.next_out = output;
		stream.avail_out = static_cast<uInt>(*output_length);
		if (static_cast<uLong>(stream.avail_out) != *output_length) {
			return Z_BUF_ERROR;
		}
		stream.zalloc = (alloc_func)nullptr;
		stream.zfree = (free_func)nullptr;
		stream.opaque = (voidpf)nullptr;

		int error = deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
		if (error != Z_OK) {
			return error;
		}
		error = deflate(&stream, Z_FINISH);
		if (error != Z_STREAM_END) {
			deflateEnd(&stream);
			return error == Z_OK ? Z_BUF_ERROR : error;
		}
		*output_length = stream.total_out;
		error = deflateEnd(&stream);
		return error;
	}

	ZipArchiver::FileInfo::FileInfo() noexcept
	:
		is_dir(false),
		is_file(false),
		is_symlink(false),
		crc(0),
		size(0)
	{
	}

	ZipArchiver::FileInfo::operator bool() const noexcept
	{
		return is_dir || is_file || is_symlink;
	}

	inline uint ZipArchiver::UnsignedInteger(const uchar* input)
	{
		return (input[0]) + (input[1]<<8) + (input[2]<<16) + (input[3]<<24);
	}

	inline void ZipArchiver::UnsignedInteger(uchar* output, uint input)
	{
		output[0] = input & 0xff;
		output[1] = (input >> 8) & 0xff;
		output[2] = (input >> 16) & 0xff;
		output[3] = (input >> 24) & 0xff;
	}

	inline void ZipArchiver::UnsignedInteger(uchar* output, const uchar* input)
	{
		output[0] = input[0];
		output[1] = input[1];
		output[2] = input[2];
		output[3] = input[3];
	}

	inline ushort ZipArchiver::UnsignedShort(const uchar* input)
	{
		return (input[0]) + (input[1] << 8);
	}

	inline void ZipArchiver::UnsignedShort(uchar* output, ushort input)
	{
		output[0] = input & 0xff;
		output[1] = (input >> 8) & 0xff;
	}

	inline void ZipArchiver::UnsignedShort(uchar* output, const uchar* input)
	{
		output[0] = input[0];
		output[1] = input[1];
	}

	QDateTime ZipArchiver::MSDOSDate(const uchar* input)
	{
		uint msdos_date(ZipArchiver::UnsignedInteger(input));
		quint64 u_date(static_cast<quint64>(msdos_date >> 16));
		const uint mday(u_date & 0x1f);
		const uint mon((u_date & 0x1E0) >> 5);
		const uint year(((u_date & 0x0FE00) >> 9) + 1980);
		const uint hour((msdos_date & 0xF800) >> 11);
		const uint min((msdos_date & 0x7E0) >> 5);
		const uint sec((msdos_date & 0x1f) << 1);
		return QDateTime(QDate(year, mon, mday), QTime(hour, min, sec));
	}

	void ZipArchiver::MSDOSDate(uchar* output, const QDateTime& input)
	{
		if (input.isValid()) {
			quint16 time =
							(input.time().hour() << 11) /// < 5 bit hour
							| (input.time().minute() << 5) /// < 6 bit minute
							| (input.time().second() >> 1) /// < 5 bit double seconds
			;

			output[0] = time & 0xff;
			output[1] = time >> 8;

			quint16 date =
							((input.date().year() - 1980) << 9) /// < 7 bit year 1980-based
							| (input.date().month() << 5) /// < 4 bit month
							| input.date().day() /// < 5 bit day
			;

			output[2] = char(date);
			output[3] = char(date >> 8);
		} else {
			output[0] = 0;
			output[1] = 0;
			output[2] = 0;
			output[3] = 0;
		}
	}

	QFile::Permissions ZipArchiver::ModePermission(quint32 input)
	{
		QFile::Permissions retval;
		if (input & static_cast<quint32>(UnixFileAttribute::ReadUser)) {
			retval |= QFile::ReadOwner | QFile::ReadUser;
		}
		if (input & static_cast<quint32>(UnixFileAttribute::WriteUser)) {
			retval |= QFile::WriteOwner | QFile::WriteUser;
		}
		if (input & static_cast<quint32>(UnixFileAttribute::ExeUser)) {
			retval |= QFile::ExeOwner | QFile::ExeUser;
		}
		if (input & static_cast<quint32>(UnixFileAttribute::ReadGroup)) {
			retval |= QFile::ReadGroup;
		}
		if (input & static_cast<quint32>(UnixFileAttribute::WriteGroup)) {
			retval |= QFile::WriteGroup;
		}
		if (input & static_cast<quint32>(UnixFileAttribute::ExeGroup)) {
			retval |= QFile::ExeGroup;
		}
		if (input & static_cast<quint32>(UnixFileAttribute::ReadOther)) {
			retval |= QFile::ReadOther;
		}
		if (input & static_cast<quint32>(UnixFileAttribute::WriteOther)) {
			retval |= QFile::WriteOther;
		}
		if (input & static_cast<quint32>(UnixFileAttribute::ExeOther)) {
			retval |= QFile::ExeOther;
		}
		return retval;
	}

	quint32 ZipArchiver::PermissionMode(QFile::Permissions input)
	{
		quint32 retval(0);
		if (input & (QFile::ReadOwner | QFile::ReadUser)) {
			retval |= static_cast<quint32>(UnixFileAttribute::ReadUser);
		}
		if (input & (QFile::WriteOwner | QFile::WriteUser)) {
			retval |= static_cast<quint32>(UnixFileAttribute::WriteUser);
		}
		if (input & (QFile::ExeOwner | QFile::ExeUser)) {
			retval |= static_cast<quint32>(UnixFileAttribute::WriteUser);
		}
		if (input & QFile::ReadGroup) {
			retval |= static_cast<quint32>(UnixFileAttribute::ReadGroup);
		}
		if (input & QFile::WriteGroup) {
			retval |= static_cast<quint32>(UnixFileAttribute::WriteGroup);
		}
		if (input & QFile::ExeGroup) {
			retval |= static_cast<quint32>(UnixFileAttribute::ExeGroup);
		}
		if (input & QFile::ReadOther) {
			retval |= static_cast<quint32>(UnixFileAttribute::ReadOther);
		}
		if (input & QFile::WriteOther) {
			retval |= static_cast<quint32>(UnixFileAttribute::WriteOther);
		}
		if (input & QFile::ExeOther) {
			retval |= static_cast<quint32>(UnixFileAttribute::ExeOther);
		}
		return retval;
	}

	ZipArchiver::LocalFileHeader ZipArchiver::LocalHeader(const CentralFileHeader& input)
	{
		LocalFileHeader retval;
		ZipArchiver::UnsignedInteger(retval.signature, 0x04034b50);
		ZipArchiver::UnsignedShort(retval.version_needed, input.version_needed);
		ZipArchiver::UnsignedShort(retval.general_purpose_bits, input.general_purpose_bits);
		ZipArchiver::UnsignedShort(retval.compression_method, input.compression_method);
		ZipArchiver::UnsignedInteger(retval.last_mod_file, input.last_mod_file);
		ZipArchiver::UnsignedInteger(retval.crc_32, input.crc_32);
		ZipArchiver::UnsignedInteger(retval.compressed_size, input.compressed_size);
		ZipArchiver::UnsignedInteger(retval.uncompressed_size, input.uncompressed_size);
		ZipArchiver::UnsignedShort(retval.file_name_length, input.file_name_length);
		ZipArchiver::UnsignedShort(retval.extra_field_length, input.extra_field_length);
		return retval;
	}

	ZipArchiver::ZipArchiver(QSharedPointer<QIODevice> io_device)
	:
		_io_device(io_device),
		_status(ZipArchiver::NoError),
		_dirty_file_tree(true),
		_offset_directory(0)
	{

	}

	ZipArchiver::~ZipArchiver()
	{
		if (_io_device) {
			_io_device->close();
		}
	}

	void ZipArchiver::close()
	{
		if (_io_device) {
			_io_device->close();
			_io_device.reset();
		}
		_status = ZipArchiver::NoError;
		_file_header_vector.clear();
		_dirty_file_tree = true;
		_offset_directory = 0;
		_comment.clear();
	}

	ZipArchiver::FileInfo ZipArchiver::makeFileInfo(int index) const
	{
		ZipReader::FileInfo retval;
		FileHeader header(_file_header_vector.at(index));
		quint32 mode = ZipArchiver::UnsignedInteger(header.h.external_file_attributes);
		const HostOS hostOS = HostOS(ZipArchiver::UnsignedShort(header.h.version_made) >> 8);
		switch (hostOS) {
			case HostUnix:
				mode = (mode >> 16) & 0xffff;
				switch (static_cast<UnixFileAttribute>(mode & static_cast<quint32>(UnixFileAttribute::TypeMask))) {
					case UnixFileAttribute::SymLink:
						retval.is_symlink = true;
						break;

					case UnixFileAttribute::Dir:
						retval.is_dir = true;
						break;

					case UnixFileAttribute::File:
					default:
						/// @??? just for the case; should we warn
						retval.is_file = true;
						break;
				}
				retval.permission_mode = ZipArchiver::ModePermission(mode);
				break;

			case HostFAT:
			case HostNTFS:
			case HostHPFS:
			case HostVFAT:
				switch (static_cast<WindowsFileAttribute>(mode & static_cast<quint32>(WindowsFileAttribute::TypeMask))) {
					case WindowsFileAttribute::Dir:
						retval.is_dir = true;
						break;
					case WindowsFileAttribute::File:
					default:
						retval.is_file = true;
						break;
				}
				retval.permission_mode |= QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther;
				if ((mode & static_cast<quint32>(WindowsFileAttribute::ReadOnly)) == 0) {
					retval.permission_mode |= QFile::WriteOwner | QFile::WriteUser | QFile::WriteGroup | QFile::WriteOther;
				}
				if (retval.is_dir) {
					retval.permission_mode |= QFile::ExeOwner | QFile::ExeUser | QFile::ExeGroup | QFile::ExeOther;
				}
				break;

			default:
				qWarning("Zip entry format at %d is not supported.", index);
				return retval; // we don't support anything else
		}

		ushort general_purpose_bits = ZipArchiver::UnsignedShort(header.h.general_purpose_bits);

		/// @note if bit 11 is set, the filename and comment fields must be encoded using UTF-8
		const bool inUtf8 = (general_purpose_bits & Utf8Names) != 0;
		retval.file_path = inUtf8 ? QString::fromUtf8(header.file_name) : QString::fromLocal8Bit(header.file_name);
		retval.crc = ZipArchiver::UnsignedInteger(header.h.crc_32);
		retval.size = ZipArchiver::UnsignedInteger(header.h.uncompressed_size);
		retval.modification_date = ZipArchiver::MSDOSDate(header.h.last_mod_file);

		/// @brief fix the file path, if broken (convert separators, eat leading and trailing ones)
		retval.file_path = QDir::fromNativeSeparators(retval.file_path);
		QStringRef filePathRef(&retval.file_path);
		while (filePathRef.startsWith(QLatin1Char('.')) || filePathRef.startsWith(QLatin1Char('/'))) {
			filePathRef = filePathRef.mid(1);
		}
		while (filePathRef.endsWith(QLatin1Char('/'))) {
			filePathRef.chop(1);
		}
		retval.file_path = filePathRef.toString();
		return retval;
	}

	ulong ZipArchiver::makeCRC32() const
	{
		if (!_io_device) {
			return 0;
		}
		const QByteArray byte_array(_io_device->readAll());
		ulong retval(::crc32(0, nullptr, 0));
		retval = ::crc32(retval , reinterpret_cast<const uchar *>(byte_array.constData()), byte_array.length());
		return retval;
	}

	QIODevice* ZipArchiver::getDevice() const
	{
		return _io_device.data();
	}

	ZipArchiver::Status ZipArchiver::getStatus() const
	{
		return _status;
	}

	bool ZipArchiver::isExist() const
	{
		QFile* file(qobject_cast<QFile*> (_io_device.data()));
		return (file == nullptr) ? true : file->exists();
	}

	ZipReader::ZipReader()
	:
		ZipArchiver(nullptr)
	{

	}

	bool ZipReader::scan()
	{
		if (!_dirty_file_tree) {
			return false;
		} else if (!_io_device || (!_io_device->isOpen() && !_io_device->open(QIODevice::ReadOnly))) {
			_status = ZipReader::FileOpenError;
			return false;
		} else if ((_io_device->openMode() & QIODevice::ReadOnly) == 0) { /// < only read the index from readable files.
			_status = ZipReader::FileReadError;
			return false;
		}

		_dirty_file_tree = false;
		uchar tmp[4];
		_io_device->read((char *)tmp, 4);
		if (ZipArchiver::UnsignedInteger(tmp) != 0x04034b50) {
			qWarning("Not a zip file");
			return false;
		}

		/// @note find EndOfDirectory header
		int i = 0;
		int offset_directory = -1;
		int num_dir_entries = 0;
		EndOfDirectory eod;
		while (offset_directory == -1) {
			const int pos = _io_device->size() - int(sizeof(EndOfDirectory)) - i;
			if (pos < 0 || i > 65535) {
				qWarning("Scan error: `end of directory` not found");
				return false;
			}
			_io_device->seek(pos);
			_io_device->read((char *)&eod, sizeof(EndOfDirectory));
			if (ZipArchiver::UnsignedInteger(eod.signature) == 0x06054b50) {
				break;
			}
			++i;
		}

		/// @note have the eod
		offset_directory = ZipArchiver::UnsignedInteger(eod.dir_start_offset);
		num_dir_entries = ZipArchiver::UnsignedShort(eod.num_dir_entries);
		int comment_length = ZipArchiver::UnsignedShort(eod.comment_length);
		if (comment_length != i) {
			qWarning("Failed to parse zip file");
		}
		_comment = _io_device->read(qMin(comment_length, i));

		_io_device->seek(offset_directory);
		for (i = 0; i < num_dir_entries; ++i) {
			FileHeader header;
			int read = _io_device->read((char *) &header.h, sizeof(CentralFileHeader));
			if (read < (int) sizeof(CentralFileHeader)) {
				qWarning("Failed to read complete header, index may be incomplete");
				break;
			}
			if (ZipArchiver::UnsignedInteger(header.h.signature) != 0x02014b50) {
				qWarning("Invalid header signature, index may be incomplete");
				break;
			}
			int l = ZipArchiver::UnsignedShort(header.h.file_name_length);
			header.file_name = _io_device->read(l);
			if (header.file_name.length() != l) {
				qWarning("Failed to read filename from zip index, index may be incomplete");
				break;
			}
			l = ZipArchiver::UnsignedShort(header.h.extra_field_length);
			header.extra_field = _io_device->read(l);
			if (header.extra_field.length() != l) {
				qWarning("Failed to read extra field in zip file, skipping file, index may be incomplete");
				break;
			}
			l = ZipArchiver::UnsignedShort(header.h.file_comment_length);
			header.file_comment = _io_device->read(l);
			if (header.file_comment.length() != l) {
				qWarning("Failed to read read file comment, index may be incomplete");
				break;
			}
			qDebug("Found file '%s'", header.file_name.data());

			_file_header_vector.append(header);
		}

		return true;
	}

	bool ZipReader::open(const QString& file_path)
	{
		close();
		QScopedPointer<QFile> file(new QFile(file_path));
		if (!file->open(QIODevice::ReadOnly)) {
			switch (file->error()) {
				case QFile::ReadError:
					_status = FileReadError;
					break;

				case QFile::OpenError:
					_status = FileOpenError;
					break;

				case QFile::PermissionsError:
					_status = FilePermissionsError;
					break;

				default:
					_status = FileError;
					break;
			}
			return false;
		}
		_io_device.reset(file.take());
		return scan();
	}

	/**
	 * @brief ZipReader::extract
	 * @param base_path
	 * @todo Refactor and Optize
	 * @note Issues with special directories like  __MACOSX/ for .filename.ext
	 * @return
	 */
	bool ZipReader::extract(const QString& base_path) const
	{
		const QDir base_directory(base_path);
		const QVector<FileInfo> file_info_vector(getFileInfoList());

		/// @brief Create directories first
		for (const FileInfo& file_info : file_info_vector) {
			const QString absolute_path(base_path + QDir::separator() + file_info.file_path);
			if (file_info.is_dir) {
				if (!base_directory.mkpath(file_info.file_path)) {
					return false;
				} else if (!QFile::setPermissions(absolute_path, file_info.permission_mode)) {
					return false;
				}
			}
		}

		/// @brief Set up symlinks
		for (const FileInfo& file_info : file_info_vector) {
			const QString absolute_path(base_path + QDir::separator() + file_info.file_path);
			if (file_info.is_symlink) {
				const QString destination(QFile::decodeName(getFileData(file_info.file_path)));
				if (destination.isEmpty()) {
					return false;
				}
				QFileInfo symlink(absolute_path);
				if (!QFile::exists(symlink.absolutePath())) {
					QDir::root().mkpath(symlink.absolutePath());
				}
				if (!QFile::link(destination, absolute_path)) {
					return false;
				}
			}
		}

		for (const FileInfo& file_info : file_info_vector) {
			const QString absolute_path(base_path + QDir::separator() + file_info.file_path);
			if (file_info.is_file) {
				QFile f(absolute_path);
				if (!f.open(QIODevice::WriteOnly)) {
					return false;
				}
				f.write(getFileData(file_info.file_path));
				f.setPermissions(file_info.permission_mode);
				f.close();
			}
		}
		return true;
	}

	QVector<ZipArchiver::FileInfo> ZipReader::getFileInfoList() const
	{
		QVector<FileInfo> retval;
		const int numFileHeaders = _file_header_vector.size();
		retval.reserve(numFileHeaders);
		for (int i = 0; i < numFileHeaders; ++i) {
			retval.append(makeFileInfo(i));
		}
		return retval;
	}

	int ZipReader::getCount() const
	{
		return _file_header_vector.count();
	}

	ZipReader::FileInfo ZipReader::getEntryInfoAt(int index) const
	{
		if (index >= 0 && index < _file_header_vector.count()) {
			return makeFileInfo(index);
		}
		return {};
	}

	QByteArray ZipReader::getFileData(const QString& file_name) const
	{
		int i(0);
		for (; i < _file_header_vector.size(); ++i) {
			if (QString::fromLocal8Bit(_file_header_vector.at(i).file_name) == file_name) {
				break;
			}
		}
		if (i == _file_header_vector.size()) {
			return QByteArray();
		}

		FileHeader header = _file_header_vector.at(i);
		const ushort version_needed(ZipArchiver::UnsignedShort(header.h.version_needed));
		if (version_needed > ZipArchiver::Version) {
			qWarning("ZIP specification version %d implementationis needed to extract the data.", version_needed);
			return QByteArray();
		}

		ushort general_purpose_bits = ZipArchiver::UnsignedShort(header.h.general_purpose_bits);
		int compressed_size = ZipArchiver::UnsignedInteger(header.h.compressed_size);
		int uncompressed_size = ZipArchiver::UnsignedInteger(header.h.uncompressed_size);
		int start = ZipArchiver::UnsignedInteger(header.h.offset_local_header);
		//qDebug("uncompressing file %d: local header at %d", i, start);

		_io_device->seek(start);

		LocalFileHeader local_header;
		_io_device->read(reinterpret_cast<char *>(&local_header), sizeof(LocalFileHeader));
		uint skip = ZipArchiver::UnsignedShort(local_header.file_name_length) + ZipArchiver::UnsignedShort(local_header.extra_field_length);
		_io_device->seek(_io_device->pos() + skip);

		const int compression_method(ZipArchiver::UnsignedShort(local_header.compression_method));
//		qDebug("file=%s: compressed_size=%d, uncompressed_size=%d", fileName.toLocal8Bit().data(), compressed_size, uncompressed_size);

		if ((general_purpose_bits & Encrypted) != 0) {
			qWarning("Unsupported encryption method is needed to extract the data.");
			return QByteArray();
		}

//		qDebug("file at %lld", d->device->pos());
		QByteArray compressed_data(_io_device->read(compressed_size));
		if (compression_method == CompressionMethodStored) {
			/// @note No compression
			compressed_data.truncate(uncompressed_size);
			return compressed_data;
		} else if (compression_method == CompressionMethodDeflated) {
//			qDebug("compressed=%d", compressed.size());
			compressed_data.truncate(compressed_size);

			ulong length(qMax(uncompressed_size, 1));
			QByteArray uncompressed_data;
			int retcode(Z_OK);
			do {
				uncompressed_data.resize(length);
				retcode = inflate((uchar*)uncompressed_data.data(), &length, (const uchar*)compressed_data.constData(), compressed_size);
				switch (retcode) {
					case Z_OK:
						if ((int)length != uncompressed_data.size()) {
							uncompressed_data.resize(length);
						}
						break;

					case Z_MEM_ERROR:
						qWarning("Error: Z_MEM_ERROR(Not enough memory)");
						break;

					case Z_BUF_ERROR:
						length *= 2;
						break;

					case Z_DATA_ERROR:
						qWarning("Error: Z_DATA_ERROR(Input data is corrupted)");
						break;
				}
			} while (retcode == Z_BUF_ERROR);
			return uncompressed_data;
		}

		qWarning("Error: Unsupported compression method %d is needed to extract the data.", compression_method);
		return QByteArray();
	}

	bool ZipReader::isReadable() const
	{
		return _io_device && _io_device->isReadable();
	}

	ZipWriter::ZipWriter()
	:
		ZipArchiver(nullptr),

		_permission_mode(QFile::ReadOwner | QFile::WriteOwner),
		_compression_policy(ZipWriter::AlwaysCompress)
	{

	}

	ZipWriter::ZipWriter(QSharedPointer<QIODevice> device)
	:
		ZipArchiver(device),

		_permission_mode(QFile::ReadOwner | QFile::WriteOwner),
		_compression_policy(ZipWriter::AlwaysCompress)
	{
		if ((_io_device->isOpen() && !_io_device->isWritable()) || !_io_device->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			_io_device.reset();
		}
	}

	bool ZipWriter::open(const QString& file_path)
	{
		close();

		QScopedPointer<QFile> file(new QFile(file_path));
		if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			switch (file->error()) {
				case QFile::WriteError:
					_status = FileWriteError;
					break;

				case QFile::OpenError:
					_status = FileOpenError;
					break;

				case QFile::PermissionsError:
					_status = FilePermissionsError;
					break;

				default:
					_status = FileError;
					break;
			}
			return false;
		}

		_io_device.reset(file.take());
		return true;
	}

	bool ZipWriter::save()
	{
		if (!_io_device || !(_io_device->openMode() & QIODevice::WriteOnly)) {
			return false;
		}

		qDebug("Writing directory with %d entries", _file_header_vector.size());
		_io_device->seek(_offset_directory);

		/// @brief Write new directory
		for (int i = 0; i < _file_header_vector.size(); ++i) {
			const FileHeader& header(_file_header_vector.at(i));
			_io_device->write((const char *)&header.h, sizeof(CentralFileHeader));
			_io_device->write(header.file_name);
			_io_device->write(header.extra_field);
			_io_device->write(header.file_comment);
		}

		/// @brief Write end of directory
		EndOfDirectory eod; memset(&eod, 0, sizeof(EndOfDirectory));
		ZipArchiver::UnsignedInteger(eod.signature, 0x06054b50);
//		uchar this_disk[2];
//		uchar offset_directory_disk[2];
		ZipArchiver::UnsignedShort(eod.num_dir_entries_this_disk, _file_header_vector.size());
		ZipArchiver::UnsignedShort(eod.num_dir_entries, _file_header_vector.size());
		ZipArchiver::UnsignedInteger(eod.directory_size, (_io_device->pos() - _offset_directory));
		ZipArchiver::UnsignedInteger(eod.dir_start_offset, _offset_directory);
		ZipArchiver::UnsignedShort(eod.comment_length, _comment.length());

		_io_device->write(reinterpret_cast<const char *>(&eod), sizeof(EndOfDirectory));
		_io_device->write(_comment);

		close();

		return true;
	}

	ZipWriter::CompressionPolicy ZipWriter::getCompressionPolicy() const
	{
		return _compression_policy;
	}

	bool ZipWriter::add(EntryType type, const QString& name, const QByteArray& data/*, QFile::Permissions permissions, QZip::Method m*/)
	{
		if (!_io_device || !_io_device->isOpen() || !_io_device->open(QIODevice::WriteOnly)) {
			_status = ZipWriter::FileOpenError;
			return false;
		}
		_io_device->seek(_offset_directory);

		const QByteArray* ata(&data);
		ulong crc_32 = ::crc32(0, nullptr, 0);
		crc_32 = ::crc32(crc_32, reinterpret_cast<const uchar *>(ata->constData()), ata->length());

		/// @brief Don't compress small files
		const ZipWriter::CompressionPolicy compression_policy(_compression_policy != ZipWriter::AutoCompress ? _compression_policy : (data.length() < 64) ? ZipWriter::NeverCompress : ZipWriter::AlwaysCompress);

		FileHeader header; memset(&header.h, 0, sizeof(CentralFileHeader));
		ZipArchiver::UnsignedInteger(header.h.signature, 0x02014b50);
		ZipArchiver::UnsignedShort(header.h.version_needed, ZipArchiver::Version);
		ZipArchiver::UnsignedInteger(header.h.uncompressed_size, data.length());
		ZipArchiver::MSDOSDate(header.h.last_mod_file, QDateTime::currentDateTime());

		QByteArray zata;
		if (compression_policy == ZipWriter::AlwaysCompress) {
			ZipArchiver::UnsignedShort(header.h.compression_method, CompressionMethodDeflated);
			ulong zata_length(data.length());
			/// @note Shamelessly copied form zlib
			zata_length += (zata_length >> 12) + (zata_length >> 14) + 11;
			int deflate_retcode(Z_OK);
			do {
				zata.resize(zata_length);
				deflate_retcode = deflate(reinterpret_cast<uchar*>(zata.data()), &zata_length, reinterpret_cast<const uchar*>(data.constData()), data.length());
				switch (deflate_retcode) {
					case Z_OK:
						zata.resize(zata_length);
						break;

					case Z_MEM_ERROR:
						qWarning("Error: Z_MEM_ERROR(Not enough memory)");
						/// @note ...to compress file, skipping
						zata.resize(0);
						break;

					case Z_BUF_ERROR:
						zata_length *= 2;
						break;
				}
			} while (deflate_retcode == Z_BUF_ERROR);
			ata = &zata;
		}

		/// @todo If deflated > original and try to store the original and revert the compression method to be uncompressed
		ZipArchiver::UnsignedInteger(header.h.compressed_size, ata->length());
		ZipArchiver::UnsignedInteger(header.h.crc_32, crc_32);

		/// @note If bit 11 is set, the filename and comment fields must be encoded using UTF-8
		const ushort general_purpose_bits(Utf8Names); /// < Always use utf-8
		ZipArchiver::UnsignedShort(header.h.general_purpose_bits, general_purpose_bits);

		header.file_name = (general_purpose_bits & Utf8Names) != 0 ? name.toUtf8() : name.toLocal8Bit();
		if (header.file_name.size() > 0xffff) {
			qWarning("Filename is too long, chopping it to 65535 bytes");
			header.file_name = header.file_name.left(0xffff); /// < ### don't break the utf-8 sequence, if any
		}
		if (header.file_comment.size() + header.file_name.size() > 0xffff) {
			qWarning("File comment is too long, chopping it to 65535 bytes");
			header.file_comment.truncate(0xffff - header.file_name.size()); /// < ### don't break the utf-8 sequence, if any
		}

		ZipArchiver::UnsignedShort(header.h.file_name_length, header.file_name.length());
//		h.extra_field_length[2];
		ZipArchiver::UnsignedShort(header.h.version_made, HostUnix << 8);
//		uchar internal_file_attributes[2];
//		uchar external_file_attributes[4];
		ZipArchiver::UnsignedInteger(header.h.external_file_attributes, ((ZipArchiver::PermissionMode(_permission_mode) | static_cast<quint32>(type)) << 16));
		ZipArchiver::UnsignedInteger(header.h.offset_local_header, _offset_directory);

		_file_header_vector.append(header);

		LocalFileHeader local_header(ZipArchiver::LocalHeader(header.h));
		_io_device->write(reinterpret_cast<const char *>(&local_header), sizeof(LocalFileHeader));
		_io_device->write(header.file_name);
		_io_device->write(*ata);
		_offset_directory = _io_device->pos();
		_dirty_file_tree = true;

		return true;
	}

	bool ZipWriter::addDirectory(QString name)
	{
		name = QDir::fromNativeSeparators(name);
		/// @note separator is mandatory
		if (!name.endsWith(QLatin1Char('/'))) {
			name.append(QLatin1Char('/'));
		}
		return add(ZipWriter::EntryType::Directory, name, QByteArray());
	}

	bool ZipWriter::addSymLink(const QString& name, const QString& path)
	{
		return add(ZipWriter::EntryType::SymLink, QDir::fromNativeSeparators(name), QFile::encodeName(path));
	}

	bool ZipWriter::addFile(const QString& name, const QByteArray& data)
	{
		return add(ZipWriter::EntryType::File, QDir::fromNativeSeparators(name), data);
	}

	bool ZipWriter::addFile(const QString& name, QIODevice* io_device)
	{
		if (io_device && (io_device->openMode() & QIODevice::ReadOnly) == 0) {
			if (!io_device->open(QIODevice::ReadOnly)) {
				qWarning("Error: Cannot read target file");
				return false;
			}
			const bool retval(add(ZipWriter::EntryType::File, QDir::fromNativeSeparators(name), io_device->readAll()));
			io_device->close();
			return retval;
		}
		return add(ZipWriter::EntryType::File, QDir::fromNativeSeparators(name), io_device->readAll());
	}

	void ZipWriter::setCompressionPolicy(CompressionPolicy value)
	{
		_compression_policy = value;
	}

	void ZipWriter::setPermissionMode(QFile::Permissions mode)
	{
		_permission_mode = mode;
	}

	bool ZipWriter::isWritable() const
	{
		return _io_device && _io_device->isWritable();
	}
}
