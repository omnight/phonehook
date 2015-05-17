#ifndef DB_MODEL_H
#define DB_MODEL_H

#include <QSqlQueryModel>
#include <QSqlDatabase>
#include <QObject>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>

class PhSqlModel : public QSqlQueryModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY count_changed)

public:


    PhSqlModel(QObject* parent = 0) : QSqlQueryModel(parent)
    {
    }

    QString SQL_QUERY;

    void generateRoleNames()
    {
        roleNamesHash.clear();
        for( int i = 0; i < record().count(); i++) {
            roleNamesHash[Qt::UserRole + i + 1] = record().fieldName(i).toLocal8Bit();
        }
    }

    void setQuery(const QString & query, const QSqlDatabase &db = QSqlDatabase() )
    {
        SQL_QUERY = query;
        QSqlQueryModel::setQuery(query, db);
        generateRoleNames();

        emit count_changed(rowCount());
    }


    QVariant data(const QModelIndex &index, int role) const
    {
        QVariant value = QSqlQueryModel::data(index, role);
        if(role < Qt::UserRole)
        {
            value = QSqlQueryModel::data(index, role);
        }
        else
        {
            int columnIdx = role - Qt::UserRole - 1;
            QModelIndex modelIndex = this->index(index.row(), columnIdx);
            value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
        }

        return value;
    }



    void refresh() {
        QSqlQueryModel::clear();
        QSqlQueryModel::setQuery(SQL_QUERY);

        qDebug() << "setting query" << SQL_QUERY;

    }

    Q_INVOKABLE QString getValue(int row, QString column) {

        if(row >= rowCount())
            return "";

        for(QHash<int, QByteArray>::iterator r = roleNamesHash.begin(); r != roleNamesHash.end(); r++) {
            if(r.value().toLower() == column.toLower()) {
                return record(row).value(r.key() - Qt::UserRole - 1).toString();
            }
        }

        return "";
    }

    QHash<int, QByteArray> roleNames() const { return roleNamesHash; }

public:
    int count() {
        return rowCount();
    }

signals:
    void count_changed(int count);


private:
    QHash<int, QByteArray> roleNamesHash;
};

#endif // DB_MODEL_H
