#include "String.hpp"

#include <algorithm>

namespace Qaos {
	quint32 String::MurMur32(const QString& source, quint32 seed)
	{
		constexpr quint32 c1(0xcc9e2d51);
		constexpr quint32 c2(0x1b873593);
		constexpr quint32 r1(15);
		constexpr quint32 r2(13);
		constexpr quint32 m(5);
		constexpr quint32 n(0xe6546b64);

		quint32 retval(seed);

		const QByteArray uri(source.toLocal8Bit());
		const quint8* data(reinterpret_cast<const quint8*>(uri.data()));
		const size_t length(static_cast<size_t>(uri.length()));
		const int nblocks(length / 4);
		const quint32* blocks(reinterpret_cast<const quint32*>(data));
		for (int i = 0; i < nblocks; i++) {
			quint32 k = blocks[i];

			k *= c1;
			k = (k << r1) | (k >> (32 - r1)); /// < rotleft
			k *= c2;

			retval ^= k;
			retval = (retval << r2) | (retval >> (32 - r2)); /// < rotleft
			retval = retval * m + n;
		}

		quint32 k1(0);
		const std::uint8_t* tail(data + nblocks * 4);
		switch (length & 3) {
			case 3:
				k1 ^= tail[2] << 16;
				[[fallthrough]];

			case 2:
				k1 ^= tail[1] << 8;
				[[fallthrough]];

			case 1:
				k1 ^= tail[0];
				k1 *= c1;
				k1 = (k1 << r1) | (k1 >> (32 - r1));
				k1 *= c2;
				retval ^= k1;
				break;
		}

		retval ^= length;
		/// @note fmix32
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
