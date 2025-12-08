#ifndef Qaos_NaturalSort_hpp
#define Qaos_NaturalSort_hpp

#include <QtCore>
#include <QtCore/qglobal.h>

namespace Qaos {
	/**
	 */
	class NaturalSort
	{
	/** @name Statics */
	/** @{ */
	public:
		static int NaturalCompare(const QString& s1, const QString& s2, Qt::CaseSensitivity cs = Qt::CaseInsensitive);
	/** @} */

	/** @name Constructors */
	/** @{ */
	private:
		NaturalSort();
		~NaturalSort();
	/** @} */
	};
}

#endif
