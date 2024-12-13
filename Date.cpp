#include "Date.hpp"

#include <QDebug>

namespace Qaos {
	Date::Date()
	:
		_day(-1),
		_month(-1),
		_year(-1)
	{

	}

	Date::Date(int day, int month, int year)
	{
		setDay(day);
		setMonth(month);
		setYear(year);
	}

	Date::Date(const QString& date_string)
	:
		Date()
	{
		QStringList string_values(date_string.split("-"));
		if (string_values.size() != 3) {
			return;
		}

		setDay(string_values.at(0));
		setMonth(string_values.at(1));
		setYear(string_values.at(2));
	}

	Date::operator QString() const
	{
		QString result;

		if (isDayValid()) {
			QString day(QString::number(_day));
			if (1 <= _day && _day <= 9) {
				day.prepend("0");
			}
			result += day + "-";
		} else if (isDayAny()) {
			result += "**-";
		} else {
			result += "##-";
		}

		if (isMonthValid()) {
			QString month(QString::number(_month));
			if (1 <= _month && _month <= 9) {
				month.prepend("0");
			}
			result += month + "-";
		} else if (isMonthAny()) {
			result += "**-";
		} else {
			result += "##-";
		}

		if (isYearValid()) {
			result += QString::number(_year);
		} else if (isYearAny()) {
			result += "****";
		} else {
			result += "####";
		}

		return result;
	}

	bool Date::operator==(const Date& rhs) const
	{
		if ((isAny() && rhs.isUndefined()) || (isUndefined() && rhs.isAny())) {
			return false;
		}

		bool result(true);

		if (isDayValid() && rhs.isDayValid()) {
			result = _day == rhs.getDay();
		}

		if ((isDayValid() && rhs.isDayUndefined()) || (isDayUndefined() && rhs.isDayValid())) {
			result = false;
		}

		if (!result) {
			return false;
		}

		if (isMonthValid() && rhs.isMonthValid()) {
			result = _month == rhs.getMonth();
		}

		if ((isMonthValid() && rhs.isMonthUndefined()) || (isMonthUndefined() && rhs.isMonthValid())) {
			result = false;
		}

		if (!result) {
			return false;
		}

		if (isYearValid() && rhs.isYearValid()) {
			result = _year == rhs.getYear();
		}

		if ((isYearValid() && rhs.isYearUndefined()) || (isYearUndefined() && rhs.isYearValid())) {
			result = false;
		}

		return result;
	}

	bool Date::operator!=(const Date& rhs) const
	{
		if ((isAny() && rhs.isUndefined()) || (isUndefined() && rhs.isAny())) {
			return false;
		}

		bool result(false);

		if (isDayValid() && rhs.isDayValid()) {
			result = _day != rhs.getDay();
		}

		if ((isDayValid() && rhs.isDayUndefined()) || (isDayUndefined() && rhs.isDayValid())) {
			result = true;
		}

		if (result) {
			return true;
		}

		if (isMonthValid() && rhs.isMonthValid()) {
			result = _month != rhs.getMonth();
		}

		if ((isMonthValid() && rhs.isMonthUndefined()) || (isMonthUndefined() && rhs.isMonthValid())) {
			result = true;
		}

		if (result) {
			return true;
		}

		if (isYearValid() && rhs.isYearValid()) {
			result = _year != rhs.getYear();
		}

		if ((isYearValid() && rhs.isYearUndefined()) || (isYearUndefined() && rhs.isYearValid())) {
			result = true;
		}

		return result;
	}

	bool Date::operator>(const Date& rhs) const
	{
		if ((isAny() && rhs.isUndefined()) || (isUndefined() && rhs.isAny())) {
			return false;
		}

		bool result(false);

		if (isYearValid() && rhs.isYearValid()) {
			result = _year > rhs.getYear();
		}

		if (_year != rhs.getYear()) {
			return result;
		}

		if (isMonthValid() && rhs.isMonthValid()) {
			result = _month > rhs.getMonth();
		}

		if (_month != rhs.getMonth()) {
			return result;
		}

		if (isDayValid() && rhs.isDayValid()) {
			result = _day > rhs.getDay();
		}

		return result;
	}

	bool Date::operator>=(const Date& rhs) const
	{
		if ((isAny() && rhs.isUndefined()) || (isUndefined() && rhs.isAny())) {
			return false;
		}

		bool result(true);

		if (isYearValid() && rhs.isYearValid()) {
			result = _year >= rhs.getYear();
		}

		if ((isYearValid() && rhs.isYearUndefined()) || (isYearUndefined() && rhs.isYearValid())) {
			result = false;
		}

		if (_year != rhs.getYear()) {
			return result;
		}

		if (isMonthValid() && rhs.isMonthValid()) {
			result = _month >= rhs.getMonth();
		}

		if ((isMonthValid() && rhs.isMonthUndefined()) || (isMonthUndefined() && rhs.isMonthValid())) {
			result = false;
		}

		if (_month != rhs.getMonth()) {
			return result;
		}

		if (isDayValid() && rhs.isDayValid()) {
			result = _day >= rhs.getDay();
		}

		if ((isDayValid() && rhs.isDayUndefined()) || (isDayUndefined() && rhs.isDayValid())) {
			result = false;
		}

		return result;
	}

	bool Date::operator<(const Date& rhs) const
	{
		if ((isAny() && rhs.isUndefined()) || (isUndefined() && rhs.isAny())) {
			return false;
		}

		bool result(false);

		if (isYearValid() && rhs.isYearValid()) {
			result = _year < rhs.getYear();
		}

		if (_year != rhs.getYear()) {
			return result;
		}

		if (isMonthValid() && rhs.isMonthValid()) {
			result = _month < rhs.getMonth();
		}

		if (_month != rhs.getMonth()) {
			return result;
		}

		if (isDayValid() && rhs.isDayValid()) {
			result = _day < rhs.getDay();
		}

		return result;
	}

	bool Date::operator<=(const Date& rhs) const
	{
		if ((isAny() && rhs.isUndefined()) || (isUndefined() && rhs.isAny())) {
			return false;
		}

		bool result(true);

		if (isYearValid() && rhs.isYearValid()) {
			result = _year <= rhs.getYear();
		}

		if ((isYearValid() && rhs.isYearUndefined()) || (isYearUndefined() && rhs.isYearValid())) {
			result = false;
		}

		if (_year != rhs.getYear()) {
			return result;
		}

		if (isMonthValid() && rhs.isMonthValid()) {
			result = _month <= rhs.getMonth();
		}

		if ((isMonthValid() && rhs.isMonthUndefined()) || (isMonthUndefined() && rhs.isMonthValid())) {
			result = false;
		}

		if (_month != rhs.getMonth()) {
			return result;
		}

		if (isDayValid() && rhs.isDayValid()) {
			result = _day <= rhs.getDay();
		}

		if ((isDayValid() && rhs.isDayUndefined()) || (isDayUndefined() && rhs.isDayValid())) {
			result = false;
		}

		return result;
	}

	QDebug operator<<(QDebug stream, const Date& rhs)
	{
		stream << static_cast<QString>(rhs);
		return stream;
	}

	void Date::setDay(int day)
	{
		if (0 <= day && day <= 31) {
			_day = day;
		} else {
			_day = -1;
		}
	}

	void Date::setMonth(int month)
	{
		if (0 <= month && month <= 12) {
			_month = month;
		} else {
			_month = -1;
		}
	}

	void Date::setYear(int year)
	{
		if (0 <= year && year <= 9999) {
			_year = year;
		} else {
			_year = -1;
		}
	}

	void Date::setDay(const QString& day)
	{
		bool ok;
		int integer_day(day.toInt(&ok));

		if (ok) {
			setDay(integer_day);
		} else {
			if (day == "**") {
				_day = 0;
			} else if (day == "##") {
				_day = -1;
			} else {
				qWarning() << day << " is not a valid integer value, an asterisk sign or a number sign";
				_day = -1;
			}
		}
	}

	void Date::setMonth(const QString& month)
	{
		bool ok;
		int integer_month(month.toInt(&ok));

		if (ok) {
			setMonth(integer_month);
		} else {
			if (month == "**") {
				_month = 0;
			} else if (month == "##") {
				_month = -1;
			} else {
				qWarning() << month << " is not a valid integer value, an asterisk sign or a number sign";
				_month = -1;
			}
		}
	}

	void Date::setYear(const QString& year)
	{
		bool ok;
		int integer_year(year.toInt(&ok));

		if (ok) {
			setYear(integer_year);
		} else {
			if (year == "****") {
				_year = 0;
			} else if (year == "####") {
				_year = -1;
			} else {
				qWarning() << year << " is not a valid integer value, an asterisk sign or a number sign";
				_year = -1;
			}
		}
	}

	int Date::getDay() const
	{
		return _day;
	}

	int Date::getMonth() const
	{
		return _month;
	}

	int Date::getYear() const
	{
		return _year;
	}

	bool Date::isDayValid() const
	{
		return _day > 0;
	}

	bool Date::isMonthValid() const
	{
		return _month > 0;
	}

	bool Date::isYearValid() const
	{
		return _year > 0;
	}

	bool Date::isDayAny() const
	{
		return _day == 0;
	}

	bool Date::isMonthAny() const
	{
		return _month == 0;
	}

	bool Date::isYearAny() const
	{
		return _year == 0;
	}

		bool Date::isDayUndefined() const
	{
		return _day < 0;
	}

		bool Date::isMonthUndefined() const
	{
		return _month < 0;
	}

		bool Date::isYearUndefined() const
	{
		return _year < 0;
	}

	bool Date::isValid() const
	{
		return isDayValid() && isMonthValid() && isYearValid();
	}

	bool Date::isAny() const
	{
		return isDayAny() || isMonthAny() || isYearAny();
	}

	bool Date::isUndefined() const
	{
		return isDayUndefined() || isMonthUndefined() || isYearUndefined();
	}
}
