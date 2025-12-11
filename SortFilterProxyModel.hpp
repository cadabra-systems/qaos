#ifndef Qaos_SortFilterProxyModel_hpp
#define Qaos_SortFilterProxyModel_hpp

#include <QSortFilterProxyModel>

#include "DynamicRole.hpp"
#include "PropertyList.hpp"

namespace Qaos {
	class DynamicRole;
}

namespace Qaos {
	class SortFilterProxyModel : public QSortFilterProxyModel
	{
	/** @name Qroperties */
	/** @{ */
	Q_OBJECT
		Q_PROPERTY(QAbstractItemModel* sourceModel READ getSource WRITE setSource NOTIFY sourceChanged)
		Q_PROPERTY(QString filterPattern READ getPattern WRITE setPattern NOTIFY patternChanged)
		Q_PROPERTY(QQmlListProperty<Qaos::DynamicRole> roleList READ getRoleList CONSTANT)
	/** @} */

	/** @name Statics */
	/** @{ */
	public:
		static constexpr int MaxDataRole = static_cast<int>(std::numeric_limits<int>::max());
	/** @} */

	/** @name Constructors */
	/** @{ */
	public:
		SortFilterProxyModel(QObject* parent = nullptr);
		virtual ~SortFilterProxyModel() = default;
	/** @} */

	/** @name Properties */
	/** @{ */
	private:
		ObjectPropertyList<DynamicRole> _role_list;
		QHash<int, QByteArray> _role_hash;
	/** @} */

	/** @name Factories*/
	/** @{ */
	public:
	/** @} */

	/** @name Procedures */
	/** @{ */
	protected:
		void validate();

	private:
		void validate(DynamicRole* role);
	/** @} */

	/** @name Getters */
	/** @{ */
	public:
		QAbstractItemModel* getSource();
		QString getPattern() const;

		QQmlListProperty<DynamicRole> getRoleList();
		Q_INVOKABLE int getRoleHashKey(const QString& name);

		virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
		virtual QHash<int, QByteArray> roleNames() const override;
	/** @} */

	/** @name Setters */
	/** @{ */
	public:
		Q_INVOKABLE void setSource(QAbstractItemModel* model);
		Q_INVOKABLE void setPattern(const QString& pattern);

		void addRole(DynamicRole* role);

		virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	/** @} */

	/** @{ */
	signals:
		void validated();
		void sourceChanged();
		void patternChanged();
	/** @} */

	/** @name Slots */
	/** @{ */
	protected slots:
		void onRoleListResized(bool increase);
		void onRoleListUpdated();
	/** @} */
	};
}

#endif
