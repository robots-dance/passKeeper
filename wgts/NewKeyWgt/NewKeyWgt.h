#ifndef _NEWKEYWGT_H_
#define _NEWKEYWGT_H_

#include <QDialog>
#include "types.h"

class KeyEntity;
class QLineEdit;

class NewKeyWgt: public QDialog
{
	Q_OBJECT

public:
	explicit NewKeyWgt( QWidget *parent = 0 );
	
	void clear();
	
	KeyEntity *lastEntity() const { return m_lastEnt; }
	
	QString title() const;

private:
	enum AddStatus { Waiting, Added, Skipped };
	
	// текстовые поля для ввода данных
	QLineEdit *m_titleEdt;
	QLineEdit *m_loginEdt, *m_passEdt;
	
	KeyEntity *m_lastEnt;
	// сущность, успешно созданная в предыдущий раз
	
	AddStatus m_status;
	// статус процесса добавления сущности

protected:
	void customEvent( QEvent * );

public slots:
	void cancel();
	void create();
};

#endif // _NEWKEYWGT_H_

