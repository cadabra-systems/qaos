/**
 @file VariantItemModel.hpp
 @date 12.02.15
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
*/

#ifndef Qaos_VariantItemModel_hpp
#define Qaos_VariantItemModel_hpp

#include <QStandardItemModel>

#include "Qaos.hpp"

namespace Qaos {
	/**
	 * @brief
	 */
	class VariantItemModel : public QStandardItemModel
	{
	/** @name Constructors */
	/** @{ */
	public:
		explicit VariantItemModel(QObject* parent = nullptr);
		VariantItemModel(int rows, int columns, QObject* parent = nullptr);
		virtual ~VariantItemModel() override = default;
	/** @} */

	/** @name Qroperties */
	/** @{ */
	Q_OBJECT
	/** @} */

	/** @name Procedures */
	/** @{ */
	public:
		void append(const QString& display_value, const QVariant& user_value);
		void prepend(const QString& display_value, const QVariant& user_value);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		QVariant getFirst(int column = 0) const;
		QVariant getLast(int column = 0) const;
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
	/** @} */

	/** @name Signals */
	/** @{ */
	signals:
	/** @} */

	/** @name Hooks */
	/** @{ */
	protected:
	/** @} */

	/** @name States */
	/** @{ */
	public:
		bool isEmpty() const;
	/** @} */
	};
}

#endif
