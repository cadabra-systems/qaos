#ifndef Qaos_Default_hpp
#define Qaos_Default_hpp

namespace Qaos {
	template <typename T>
	class Default
	{
	/** @name Statics */
	/** @{ */
	public:
		static const T& Instance()
		{
			static const T retval{};
			return retval;
		}
	/** @} */
	};
}

#endif
