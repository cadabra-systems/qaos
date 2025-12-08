/**
 @file Range.hpp
 @date 12.02.15
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
*/

#ifndef Qaos_Range_hpp
#define Qaos_Range_hpp

#include <QObject>

#include <QDebug>
#include <QVariant>
#include <QMetaType>
#include <QDataStream>

namespace Qaos {
	/**
	 * @todo Multiply ranges
	 */
	class Range : public QObject
	{
	/** @name Qroperties */
	/** @{ */
	Q_OBJECT
		Q_PROPERTY(qreal beginPosition READ getBeginPosition WRITE setBeginPosition NOTIFY beginPositionChanged)
		Q_PROPERTY(qreal endPosition READ getEndPosition WRITE setEndPosition NOTIFY endPositionChanged)
	/** @} */

	/** @name Classes */
	/** @{ */
	public:
	/** @} */

	/** @name Classes */
	/** @{ */
	public:
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		Range(QObject* parent = nullptr);
		Range(const Range& origin);
		Range(Range&& origin);
		virtual ~Range() = default;
	/** @} */

	/** @name Operators */
	/** @{ */
	public:
		bool operator<(const Range& rhs) const;
		bool operator>(const Range& rhs) const;
		bool operator==(const Range& rhs) const;
		Range& operator=(const Range& rhs);
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		qreal _begin_position;
		qreal _end_position;
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		Q_INVOKABLE void zero();
	/** @} */

	/** @name Mutators */
	/** @{ */
	public:
		void setBeginPosition(qreal value);
		void setEndPosition(qreal value);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		qreal getBeginPosition() const;
		qreal getEndPosition() const;
	/** @} */

	/** @name Signals */
	/** @{ */
	signals:
		void beginPositionChanged();
		void endPositionChanged();
	/** @} */

	/** @name States */
	/** @{ */
	public:
		Q_INVOKABLE bool isZero() const;
	/** @} */

	/** @name Friends */
	/** @{ */
	friend QDebug operator<<(QDebug debug, const Range& self);
	/** @} */
	};
}

#endif
