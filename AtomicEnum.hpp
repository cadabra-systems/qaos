/**
 @file AtomicEnum.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Qaos_AtomicEnum_hpp
#define Qaos_AtomicEnum_hpp

#include <QAtomicInteger>

#include <type_traits>

namespace Qaos {
	/**
	 * @brief The AtomicEnum class
	 *
	 * @todo Oveerride base atomic methods
	 */
	template <typename E>
	class AtomicEnum : protected QAtomicInteger<std::underlying_type_t<E>>
	{
	/** @name Aliases */
	/** @{ */
	public:
		using UnderlyingType = std::underlying_type_t<E>;
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		constexpr AtomicEnum(E value) noexcept
		:
			QAtomicInteger<UnderlyingType>(static_cast<UnderlyingType>(value))
		{
		}
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		E load() const
		{
			return static_cast<E>(this->loadRelaxed());
		}

		E store(E destination)
		{
			return static_cast<E>(this->fetchAndStoreRelaxed(static_cast<UnderlyingType>(destination)));
		}

		bool transit(E& expectation, E destination)
		{
			UnderlyingType actual(0);
			if (this->testAndSetRelaxed(static_cast<UnderlyingType>(expectation), static_cast<UnderlyingType>(destination), actual)) {
				return true;
			}
			expectation = static_cast<E>(actual);
			return false;
		}
	/** @} */
	};
}

#endif
