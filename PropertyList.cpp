/**
 @file PropertyList.cpp
 @date 12.02.15
 @author Daniil A Megrabyan
 */

#include "PropertyList.hpp"

#include <QEvent>
#include <QObject>

namespace Qaos {
	AbstractPropertyList::AbstractPropertyList(QObject* parent)
	:
		QObject(parent),

		_default_index(0)
	{
		QObject::connect(this, &AbstractPropertyList::resized, this, &AbstractPropertyList::onResized);
	}

	AbstractPropertyList::~AbstractPropertyList()
	{

	}

	void AbstractPropertyList::setDefaultIndex(int value)
	{
		if (length() > value) {
			return ;
		}
		_default_index = value;
		emit defaulted();
	}

	int AbstractPropertyList::getLength() const
	{
		return length();
	}

	int AbstractPropertyList::getDefaultIndex() const
	{
		return _default_index;
	}

	bool AbstractPropertyList::eventFilter(QObject* object, QEvent* event)
	{
		if (event->type() == QEvent::UpdateRequest) {
			emit updated();
			return true;
		}
		return false;
	}

	void AbstractPropertyList::onResized(bool increase)
	{
		if (!increase) {
			if (length() < _default_index) {
				_default_index = 0;
				emit defaulted();
			}
		}
	}
}
