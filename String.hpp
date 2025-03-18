#ifndef Qaos_String_hpp
#define Qaos_String_hpp

#include <array>
#include <QtCore>
#include <QtCore/qglobal.h>

namespace Qaos {
	class String
	{
	/** @name Statics */
	/** @{ */
	public:
		static quint32 MurMur3(const QString& source);

		static std::array<quint32, 256> CRC32LookupTable() noexcept;
		template <typename I>
		static quint32 CRC32(I first, I last)
		{
			/// @note Generate lookup table only on first use (thread-safe)
			static const std::array<quint32, 256> table(String::CRC32LookupTable());
			return
					quint32{0xFFFFFFFFuL}
					&
					~std::accumulate(
										first, last,
										~quint32{0} & quint32{0xFFFFFFFFuL},
										[](quint32 checksum, quint8 value)
										{
											return table[(checksum ^ value) & 0xFFu] ^ (checksum >> 8);
										}
					);
		}

		static QString CamelCase(const QString& input, const QSet<QString>& char_set = {"_"});

		static inline bool IsInteger(const QString& input)
		{
			if (input.isEmpty()) {
				return false;
			}
			return std::find_if
			(
				(input.front() == '-' || input.front() == '+')
				? input.constBegin() + 1
				: input.constBegin(),
				input.constEnd(),
				[](const QChar& c)
				{
					return !c.isDigit();
				}
			) == input.end();
		}
	/** @} */

	/** @name Constructors */
	/** @{ */
	private:
	/** @} */
	};
}

#endif
