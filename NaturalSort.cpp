#include "NaturalSort.hpp"

namespace Qaos {
	NaturalSort::NaturalSort()
	{
	}

	static inline QChar NextChar(const QString& s, int location)
	{
		return (location < s.length()) ? s.at(location) : QChar();
	}

	/**
	 * @brief Natural number sort, skips spaces.
	 * 1, 2, 10, 55, 100
	 * 01.jpg, 2.jpg, 10.jpg
	 * Only as many characters as necessary are looked at and at most they all are looked at once.
	 * Slower then QString::compare() (of course)
	 */
	int NaturalSort::NaturalCompare(const QString& s1, const QString& s2, Qt::CaseSensitivity cs)
	{
		for (int l1 = 0, l2 = 0; l1 <= s1.count() && l2 <= s2.count(); ++l1, ++l2) {
			/// @brief skip spaces, tabs and 0's
			QChar c1(NextChar(s1, l1));
			while (c1.isSpace()) {
				c1 = NextChar(s1, ++l1);
			}
			QChar c2(NextChar(s2, l2));
			while (c2.isSpace()) {
				c2 = NextChar(s2, ++l2);
			}

			if (c1.isDigit() && c2.isDigit()) {
				while (c1.digitValue() == 0) {
					c1 = NextChar(s1, ++l1);
				}
				while (c2.digitValue() == 0) {
					c2 = NextChar(s2, ++l2);
				}
				int retval(0);
				int look_ahead_location1(l1);
				int look_ahead_location2(l2);
				/// @brief find the last digit, setting retval as we go if it isn't equal
				for (
						QChar look_ahead1(c1), look_ahead2(c2);
						(look_ahead_location1 <= s1.length() && look_ahead_location2 <= s2.length());
						look_ahead1 = NextChar(s1, ++look_ahead_location1),
						look_ahead2 = NextChar(s2, ++look_ahead_location2)
					) {
					bool is_1_adigit = !look_ahead1.isNull() && look_ahead1.isDigit();
					bool is_2_adigit = !look_ahead2.isNull() && look_ahead2.isDigit();
					if (!is_1_adigit && !is_2_adigit)
						break;
					if (!is_1_adigit) {
						return -1;
					} else if (!is_2_adigit) {
						return 1;
					} else if (retval == 0) {
						if (look_ahead1 < look_ahead2) {
							retval = -1;
						} else if (look_ahead1 > look_ahead2) {
							retval = 1;
						}
					}
				}
				if (retval != 0) {
					return retval;
				}
			}

			if (cs == Qt::CaseInsensitive) {
				if (!c1.isLower()) {
					c1 = c1.toLower();
				}
				if (!c2.isLower()) {
					c2 = c2.toLower();
				}
			}
			const int r(QString::localeAwareCompare(c1, c2));
			if (r < 0) {
				return -1;
			} else if (r > 0) {
				return 1;
			}
		}
		/// @note The two strings are the same (02 == 2) so fall back to the normal sort
		return QString::compare(s1, s2, cs);
	}
}
