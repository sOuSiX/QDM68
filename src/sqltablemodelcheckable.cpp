#include "sqltablemodelcheckable.h"

SqlTableModelCheckable::SqlTableModelCheckable(QObject * parent, QSqlDatabase db)
                        : QSqlTableModel(parent, db), checklist(), fontBold(), fontNormal()
{
    fontBold.setBold(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

Qt::ItemFlags SqlTableModelCheckable::flags(const QModelIndex &index) const
{
    return ( (QSqlTableModel::flags(index) | Qt::ItemIsUserCheckable ) & ~Qt::ItemIsEditable );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

QVariant SqlTableModelCheckable::data(const QModelIndex& index, int role) const
{
    if( role == Qt::CheckStateRole && index.column() == 1 )
    {
        if( checklist.contains(index) )
            return Qt::Checked;
        else
            return Qt::Unchecked;
    }

    if( role == Qt::FontRole && index.column() == 1 )
    {
        if( checklist.contains(index) )
            return fontBold;
        else
            return fontNormal;
    }

    return QSqlTableModel::data(index, role);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool SqlTableModelCheckable::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if( role == Qt::CheckStateRole && index.column() == 1 )
    {
        if ( value == Qt::Checked )
            checklist.insert( index );
        else
            checklist.remove( index );

        emit dataChanged( index, index );
        return true;
    }

    return QSqlTableModel::setData( index, value, role );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool SqlTableModelCheckable::removeRows ( int position, int rows, const QModelIndex & parent )
{
    QSet<QPersistentModelIndex>::iterator it;
    int i;

    if( checklist.count() > 0 )
    {
        // Delete checked box for deleted rows
        beginRemoveRows( QModelIndex(), position, position+rows-1 );
        for ( i = 0; i < rows && checklist.count() > 0; ++i )
        {
            it = checklist.begin();
            while ( it != checklist.end() )
            {
                if( (*it).row() == i )
                {
                    checklist.erase( it );
                    emit dataChanged( *it, *it );
                    break;
                }
                ++it;
            }
        }
        endRemoveRows();
    }

    QSqlTableModel::removeRows( position, rows, parent );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool SqlTableModelCheckable::hasBoxChecked()
{
    return ( checklist.count() > 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int SqlTableModelCheckable::countBoxChecked()
{
    return checklist.count();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void SqlTableModelCheckable::clearBox()
{
    checklist.clear();
    emit dataChanged( QModelIndex(), QModelIndex() );
}
