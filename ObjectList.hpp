/**
 @file ObjectList.hpp
 @date 12.02.15
 @copyright Cadabra Systems / Abra
 @author Daniil A Megrabyan
*/

#ifndef Qaos_ObjectList_hpp
#define Qaos_ObjectList_hpp

#include "PropertyList.hpp"

namespace Qaos {
	class ObjectList : public QObject
	{
	/** @name Qroperties */
	/** @{ */
	Q_OBJECT
		Q_PROPERTY(QQmlListProperty<QObject> list READ makeList CONSTANT)
		Q_CLASSINFO("DefaultProperty", "list")
	/** @} */

	/** @name Constructors*/
	/** @{ */
	public:
		ObjectList(QObject* parent = nullptr);
		virtual ~ObjectList() override;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		TemplatedPropertyList<QObject> _list;
	/** @} */

	/** @name Factories */
	/** @{ */
	public:
		QQmlListProperty<QObject> makeList();
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		Q_INVOKABLE QList<QObject*> getAll(const QString& key);
		Q_INVOKABLE QObject* getFirst(const QString& key);
	/** @} */
	};
}

#endif
