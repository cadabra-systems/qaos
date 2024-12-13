/**
 @file MakeUnique.hpp
 @date 2014-01-01
 @copyright Cadabra Systems
 @author Daniil A Megrabyan <daniil@megrabyan.pro>
*/

#ifndef Qaos_MakeUnique_hpp
#define Qaos_MakeUnique_hpp

#include <memory>

namespace Qaos {
	template <typename T>
	using UniquePointer = std::unique_ptr<T>;
	/**
	 @brief Недостающий аналог std::make_shared<>
	 @details Exception-safe метод создания умных указателей
	 
	 @param ...args Аргументы для конструктора
	 
	 @return Умный уникальный указатель
	 */
	template<typename T, typename ...TArgs>
	UniquePointer<T> MakeUnique(TArgs&& ...args)
	{
		return UniquePointer<T>(new T(std::forward<TArgs>(args)...));
	}
}

#endif
