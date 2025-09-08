/**
 @file Range.cpp
 @date 12.02.15
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
 */

#include "Range.hpp"

namespace Qaos {
	Range::Range(QObject* parent)
	:
		QObject(parent),

		_begin_position(0),
		_end_position(0)
	{

	}

	Range::Range(const Range& origin)
	:
		_begin_position(origin._begin_position),
		_end_position(origin._end_position)
	{

	}

	Range::Range(Range&& origin)
	:
		_begin_position(std::move(origin._begin_position)),
		_end_position(std::move(origin._end_position))
	{

	}

	void Range::zero()
	{
		_end_position = 0;
		emit endPositionChanged();
		_begin_position = 0;
		emit beginPositionChanged();
	}

	void Range::setBeginPosition(qreal value)
	{
		if (_begin_position != value) {
			_begin_position = value;
			emit beginPositionChanged();
		}
	}

	void Range::setEndPosition(qreal value)
	{
		if (_end_position != value) {
			_end_position = value;
			emit endPositionChanged();
		}
	}

	qreal Range::getBeginPosition() const
	{
		return _begin_position;
	}

	qreal Range::getEndPosition() const
	{
		return _end_position;
	}

	bool Range::operator<(const Range& rhs) const
	{
		/// @todo
		return false;
	}

	bool Range::operator>(const Range& rhs) const
	{
		/// @todo
		return false;
	}

	bool Range::operator==(const Range& rhs) const
	{
		return _begin_position == rhs._begin_position && _end_position == rhs._end_position;
	}

	Range& Range::operator=(const Range& rhs)
	{
		_begin_position = rhs._begin_position;
		_end_position = rhs._end_position;
		return *this;
	}

	bool Range::isZero() const
	{
		return 0 == _begin_position && 0 == _end_position;
	}

	QDebug operator<<(QDebug debug, const Range& self)
	{
		debug << self._begin_position << "—" << self._end_position;
		return debug;
	}
}
