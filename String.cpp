#include "String.hpp"

#include <algorithm>

namespace Qaos {
	quint32 String::MurMur3(const QString& source)
	{
		const QByteArray uri(source.toLocal8Bit());
		const quint8* key(reinterpret_cast<const quint8*>(uri.data()));
		const size_t length(static_cast<size_t>(uri.length()));
		quint32 retval(0);

		if (length > 3) {
			const quint32* key_x4 = reinterpret_cast<const quint32*>(key);
			size_t i = length >> 2;
			do {
				quint32 k = *key_x4++;
				k *= 0xcc9e2d51;
				k = (k << 15) | (k >> 17);
				k *= 0x1b873593;
				retval ^= k;
				retval = (retval << 13) | (retval >> 19);
				retval = (retval * 5) + 0xe6546b64;
			} while (--i);
			key = reinterpret_cast<const quint8*>(key_x4);
		}

		if (length & 3) {
			std::size_t i(length & 3);
			quint32 k(0);
			key = &key[i - 1];
			do {
			  k <<= 8;
			  k |= *key--;
			} while (i--);

			k *= 0xcc9e2d51;
			k = (k << 15) | (k >> 17);
			k *= 0x1b873593;
			retval ^= k;
		}

		retval ^= length;
		retval ^= retval >> 16;
		retval *= 0x85ebca6b;
		retval ^= retval >> 13;
		retval *= 0xc2b2ae35;
		retval ^= retval >> 16;

		return retval;
	}

	std::array<quint32, 256> String::CRC32LookupTable() noexcept
	{
		quint32 const reversed_polynomial(0xEDB88320uL);
		struct byte_checksum
		{
			quint32 operator()() noexcept
			{
				quint32 checksum = static_cast<quint32>(n++);
				for (quint8 i = 0; i < 8; ++i) {
					checksum = (checksum >> 1) ^ ((checksum & 0x1u) ? reversed_polynomial : 0);
				}
				return checksum;
			}
			unsigned n = 0;
		};
		std::array<quint32, 256> table{};
		std::generate(table.begin(), table.end(), byte_checksum{});
		return table;
	}

	QString String::CamelCase(const QString& input, const QSet<QString>& char_set)
	{
		if (input.isEmpty()) {
			return input;
		}
		QString retval(input.at(0).toLower());
		bool up(false);
		for (int i = 1; i < input.length(); ++i) {
			const QChar c(input.at(i));
			if (char_set.contains(c)) {
				up = true;
			} else if (up) {
				retval.append(c.toUpper());
				up = false;
			} else {
				retval.append(c);
			}
		}
		return retval;
	}

	bool String::IsInteger(const QString& input)
	{
		if (input.isEmpty()) {
			return false;
		} else if (input.front() == '-' || input.front() == '+') {
			return (
					input.size() > 1
					&&
					std::find_if
					(
						input.constBegin() + 1, input.constEnd(),
						[](const QChar& c) { return !c.isDigit(); }
					) == input.constEnd()
			);
		}
		return std::find_if
		(
			input.constBegin(), input.constEnd(),
			[](const QChar& c) { return !c.isDigit(); }
		) == input.constEnd();
	}
}
