#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include "KeyEntity.h"
#include "consts.h"
#include "messages.h"

#include "FieldsWgt.h"

#define PREDEF_FIELDS_COUNT 1

FieldsWgt::FieldsWgt( QWidget *parent ): QWidget( parent ),
	m_callRepeated( false ), m_curEnt( 0 ), m_updateComplete( true )
{
	// создаем и настраиваем виджет таблицы
	m_tableWgt = new QTableWidget();
	m_tableWgt->setColumnCount( 2 );
	
	m_tableWgt->setSortingEnabled( false );
	// если разрешить сортировку, пострадает m_tableKeys
	// его придется пересчитывать после каждой сортировки
	
	connect( m_tableWgt,
		SIGNAL( cellChanged( int, int ) ),
		SLOT( cellChanged( int, int ) ),
		Qt::DirectConnection );
	
	// создаем кнопки
	QPushButton *addRowBtn = new QPushButton( QIcon( ADD_ROW_ICON ), "" );
	addRowBtn->setToolTip( "Add row" );
	connect( addRowBtn, SIGNAL( clicked() ), SLOT( addRow() ) );
	
	QPushButton *removeRowBtn = new QPushButton( QIcon( DEL_ROW_ICON ), "" );
	removeRowBtn->setToolTip( "Remove row" );
	connect( removeRowBtn, SIGNAL( clicked() ), SLOT( removeRow() ) );
	
	// размещаем кнопки
	QHBoxLayout *hrsl = new QHBoxLayout();
	hrsl->addWidget( addRowBtn );
	hrsl->addSpacing( DEF_BTN_ALIGN );
	hrsl->addWidget( removeRowBtn );
	hrsl->addStretch();
	
	// создаем центральную компоновку
	QVBoxLayout *vrtl = new QVBoxLayout();
	vrtl->addWidget( m_tableWgt );
	vrtl->addLayout( hrsl );
	setLayout( vrtl );
	
	update(); // для корректной настройки m_tableWgt
}

QString FieldsWgt::getValue( int row ) const
{
	QTableWidgetItem *item = m_tableWgt->item( row, 1 );
	return item != NULL ? item->text() : QString();
}

void FieldsWgt::processKeyChange( int row )
{
	DEF_TITLE( "Change Cell" );
	
	QTableWidgetItem *changedItem = m_tableWgt->item( row, 0 );
	QString changedKey = changedItem->text();
	if ( m_tableKeys.find( row ) != m_tableKeys.end() )
	// переименовываем ключ
	{
		const QString oldKey = m_tableKeys[ row ];
		if ( oldKey == changedKey )
		{
			return;
		}
		
		try
		{
			m_curEnt->getf( changedKey ); // can throw exception
			m_callRepeated = true;
			changedItem->setText( oldKey );
			QMessageBox::warning( 0, GET_TITLE(), "This field exists!" );
		}
		catch ( FieldNoFound )
		{
			m_curEnt->delf( oldKey );
			if ( changedKey != "" )
			{
				m_curEnt->setf( changedKey, getValue( row ) );
				m_tableKeys[ row ] = changedKey;
			}
			else {
				m_tableKeys.erase( row );
			}
			emitChanged();
		}
	}
	else
	// задаем новый
	{
		if ( "" == changedKey )
		{
			// новое "пустое" значение нам не нужно
			return;
		}
		
		try
		{
			m_curEnt->getf( changedKey ); // can throw exception
			m_callRepeated = true;
			changedItem->setText( "" );
			QMessageBox::warning( 0, GET_TITLE(), "This field exists!" );
		}
		catch ( FieldNoFound )
		{
			m_curEnt->setf( changedKey, getValue( row ) );
			m_tableKeys[ row ] = changedKey;
			emitChanged();
		}
	}
}

void FieldsWgt::processValueChange( int row )
{
	QTableWidgetItem *changedItem = m_tableWgt->item( row, 1 );
	QString newValue = changedItem->text();
	QString key = m_tableWgt->item( row, 0 )->text();
	const QString oldValue = m_curEnt->getf( key );
	if ( oldValue != newValue )
	{
		m_curEnt->setf( key, newValue );
		emitChanged();
	}
}

void FieldsWgt::reset()
{
	m_curEnt = NULL;
	update();
	m_tableWgt->setRowCount( 0 );
}

void FieldsWgt::update()
{
	// производим очистку
	m_tableWgt->clear();
	m_tableWgt->setRowCount( 0 );
	m_tableKeys.clear();
	m_tableWgt->setHorizontalHeaderLabels( QStringList() <<
		"Key" << "Value" );
	
	if ( NULL == m_curEnt )
	{
		return;
	}
	
	// обновление начато
	m_updateComplete = false;
	
	m_tableWgt->setRowCount( m_curEnt->fieldsCount() +
		PREDEF_FIELDS_COUNT );
	
	/*
		добавляем строки для базовых полей
		уже не настолько нужно, т.к. поле всего одно, но может
		пригодиться на будущее
	*/
	unsigned fieldIndex = 0;
	static const QString basicFields[] = { "login" };
	for ( ; fieldIndex < sizeof( basicFields ) / sizeof( QString );
		fieldIndex++ )
	{
		// устанавливаем ключ
		const QString &keyText = basicFields[ fieldIndex ];
		m_tableKeys[ fieldIndex ] = keyText;
		QTableWidgetItem *keyItem = new QTableWidgetItem( keyText );
		keyItem->setFlags( Qt::ItemIsSelectable );
		m_tableWgt->setItem( fieldIndex, 0, keyItem );
		
		// устанавливаем значение ключа
		QString valText = m_curEnt->getf( keyText );
		QTableWidgetItem *valItem = new QTableWidgetItem( valText );
		m_tableWgt->setItem( fieldIndex, 1, valItem );
	}
	
	// добавляем остальные поля
	auto fieldIter = m_curEnt->cbegin();
	for ( ; fieldIter != m_curEnt->cend(); ++fieldIter )
	{
		QString name = fieldIter->first;
		m_tableKeys[ fieldIndex ] = name;
		m_tableWgt->setItem( fieldIndex, 0, new QTableWidgetItem( name ) );
		QString value = m_curEnt->getf( name );
		m_tableWgt->setItem( fieldIndex, 1, new QTableWidgetItem( value ) );
		fieldIndex++;
	}
	
	m_tableWgt->resizeColumnsToContents();
	
	// обновление окончено
	m_updateComplete = true;
}


// ======== slots ========
void FieldsWgt::addRow()
{
	if ( m_curEnt != NULL )
	{
		m_tableWgt->insertRow( m_tableWgt->rowCount() );
	}
	else {
		QMessageBox::warning( 0, "Add Row", TABLE_ISNT_LOADED );
	}
}

void FieldsWgt::cellChanged( int row, int column )
{
	if ( m_callRepeated )
	{
		m_callRepeated = false;
		return;
	}
	if ( !m_updateComplete )
	{
		return;
	}
	
	if ( 0 == column ) // изменился ключ 
	{
		processKeyChange( row );
	}
	else // изменилось значение
	{
		processValueChange( row );
	}
}

void FieldsWgt::emitChanged()
{
	if ( m_updateComplete )
	{
		m_tableWgt->resizeColumnsToContents();
		emit changed();
	}
	else
	{
		QMessageBox::warning( 0, "Change record",
			"Can't process changes during table updating" );
	}
}

void FieldsWgt::openEntity( KeyEntity *ent )
{
	if ( m_curEnt != ent )
	{
		m_curEnt = ent;
		update();
	}
}

void FieldsWgt::removeRow()
{
	DEF_TITLE( "Remove Row" );
	
	if ( NULL == m_curEnt )
	{
		QMessageBox::warning( 0, "Remove Row", TABLE_ISNT_LOADED );
		return;
	}
	
	QList< QTableWidgetSelectionRange > selections =
		m_tableWgt->selectedRanges();
	if ( selections.size() == 0 )
	{
		QMessageBox::warning( 0, GET_TITLE(),
			"You should to select least one row!" );
		return;
	}
	
	int row = selections[ 0 ].topRow();
	QString key = m_tableKeys[ row ];
	if ( "login" == key )
	{
		QMessageBox::warning( 0, GET_TITLE(),
			"You cannot remove this record!" );
	}
	else
	{
		m_curEnt->delf( key );
		m_tableWgt->removeRow( row );
		m_tableKeys.clear();
		for ( int rowIndex = 0; rowIndex < m_tableWgt->rowCount();
			rowIndex++ )
		{
			m_tableKeys[ rowIndex ] = m_tableWgt->item(
				rowIndex, 0 )->text();
		}
		emitChanged();
	}
}
