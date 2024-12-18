#ifndef _FIELDS_WIDGET_H_
#define _FIELDS_WIDGET_H_

#include <QWidget>
#include <map>

class QTableWidget;
class KeyEntity;

class FieldsWgt: public QWidget
{
	Q_OBJECT

public:
	explicit FieldsWgt( QWidget *parent = 0 );
	void reset();

private:
	// == methods ==
	void emitChanged();
	/*
		перед выбросом сигнала changed проверяет, не происходит
		ли операция обновления посредством update
	*/
	
	QString getValue( int row ) const;
	// возвращает текст из столбца value в строке row
	
	void processKeyChange( int row );
	
	void processValueChange( int row );
	
	void update();
	// обновляет графическое представление в соотв. с curEnt
	
	
	// == fields ==
	bool m_callRepeated;
	
	KeyEntity *m_curEnt;
	// текущая ( открытая ) сущность
	
	std::map< int, QString > m_tableKeys;
	/*
		связи вида 'номер строки' - 'значение ключа'
		необходимо при изменении ключей ( "вспомнить" старый ключ )
	*/
	
	QTableWidget *m_tableWgt;
	// содержит два столбца с парами key - value
	
	bool m_updateComplete;
	// обновление таблицы завершилось

signals:
	void changed();
	// высылается при любых изменениях полей curEnt

public slots:
	void addRow();
	void openEntity( KeyEntity * );
	void removeRow();

private slots:
	void cellChanged( int, int );
};

#endif // _FIELDS_WIDGET_H_

