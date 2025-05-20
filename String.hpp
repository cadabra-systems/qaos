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
		static quint32 MurMur32(const QString& source, quint32 seed = 0);

		static std::array<quint32, 256> CRC32LookupTable() noexcept;
		template <typename I>
		static quint32 CRC32(I first, I last)
		{
			/// @note Generate lookup table only on first use (thread-safe)
			static const std::array<quint32, 256> table(String::CRC32LookupTable());
			return
					quint32{0xFFFFFFFFuL}
					&
					~std::accumulate
					(
						first, last,
						~quint32{0} & quint32{0xFFFFFFFFuL},
						[](quint32 checksum, quint8 value)
						{
							return table[(checksum ^ value) & 0xFFu] ^ (checksum >> 8);
						}
					);
		}

		static QString CamelCase(const QString& input, const QSet<QString>& char_set = {"_"});

		static bool IsInteger(const QString& input);
	/** @} */

	/** @name Constructors */
	/** @{ */
	private:
	/** @} */
	};
}

#endif
