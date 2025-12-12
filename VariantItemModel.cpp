/**
 @file VariantItemModel.cpp
 @date 12.02.15
 @copyright Cadabra Systems
 @author Daniil A Megrabyan
 */

#include "VariantItemModel.hpp"

namespace Qaos {
	VariantItemModel::VariantItemModel(QObject* parent)
	:
		VariantItemModel(0, 0, parent)
	{

	}

	VariantItemModel::VariantItemModel(int rows, int columns, QObject* parent)
	:
		QStandardItemModel(rows, columns, parent)
	{
		QHash<int, QByteArray> role_hash_map(QStandardItemModel::roleNames());
		role_hash_map[Qt::UserRole] = "user";
		QStandardItemModel::setItemRoleNames(role_hash_map);
	}

	void VariantItemModel::append(const QString& display_value, const QVariant& user_value)
	{
		QScopedPointer<QStandardItem> item(new QStandardItem(display_value));
		item->setData(user_value, Qt::UserRole);
		QStandardItemModel::appendRow(item.take());
	}

	void VariantItemModel::prepend(const QString& display_value, const QVariant& user_value)
	{
		QScopedPointer<QStandardItem> item(new QStandardItem(display_value));
		item->setData(user_value, Qt::UserRole);
		QStandardItemModel::insertRow(0, item.take());
	}

	QVariant VariantItemModel::getFirst(int column) const
	{
		return (rowCount() < 1) ? QVariant{} : data(index(0, column), Qt::UserRole);
	}

	QVariant VariantItemModel::getLast(int column) const
	{
		const int row_count(rowCount());
		return (row_count < 1) ? QVariant{} : data(index(row_count -1 , column), Qt::UserRole);
	}

	bool VariantItemModel::isEmpty() const
	{
		return rowCount() < 1;
	}
}
