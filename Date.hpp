#ifndef Qaos_Date_hpp
#define Qaos_Date_hpp

#include <QMetaType>
#include <QString>

namespace Qaos {
	/**
	 */
	class Date
	{
	/** @name Qroperties */
	/** @{ */
	Q_GADGET
		Q_PROPERTY(int day READ getDay CONSTANT)
		Q_PROPERTY(int month READ getMonth CONSTANT)
		Q_PROPERTY(int year READ getYear CONSTANT)
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		Date();
		Date(int day, int month, int year);
		Date(const QString& date_string);
		~Date() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		int _day;
		int _month;
		int _year;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		Q_INVOKABLE void setDate(int day, int month, int year)
		{
			_day = day;
			_month = month;
			_year = year;
		}
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		operator QString() const;
		bool operator==(const Date& rhs) const;
		bool operator!=(const Date& rhs) const;
		bool operator>(const Date& rhs) const;
		bool operator>=(const Date& rhs) const;
		bool operator<(const Date& rhs) const;
		bool operator<=(const Date& rhs) const;

		friend QDebug operator<<(QDebug stream, const Date& rhs);
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		void setDay(int day);
		void setMonth(int month);
		void setYear(int year);

		void setDay(const QString& day);
		void setMonth(const QString& month);
		void setYear(const QString& year);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		int getDay() const;
		int getMonth() const;
		int getYear() const;
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool isDayValid() const;
		bool isMonthValid() const;
		bool isYearValid() const;

		bool isDayAny() const;
		bool isMonthAny() const;
		bool isYearAny() const;

		bool isDayUndefined() const;
		bool isMonthUndefined() const;
		bool isYearUndefined() const;

		bool isValid() const;
		bool isAny() const;
		bool isUndefined() const;
	/** @} */
	};
}

Q_DECLARE_METATYPE(Qaos::Date)

#endif
