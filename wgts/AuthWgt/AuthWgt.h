#ifndef _AUTH_WIDGET_H_
#define _AUTH_WIDGET_H_

#include <QDialog>

class Crypter;
class QComboBox;
class QLineEdit;

class AuthWgt: public QDialog
{
	Q_OBJECT

public:
	explicit AuthWgt( QWidget *parent = 0 );
	void resetCrypter();
	void resetInputs();

private:
	void close();
	// очищает поля (не трогая m_crypter) и закрывает окно
	
	Crypter *m_crypter;
	
	QComboBox *m_cryptAlgsBox; // доступные типы шифрования
	QComboBox *m_keySizeBox; // размер ключа
	QComboBox *m_algModesBox; // режим шифрования
	
	QLineEdit *m_passEdt;

signals:
	void newCrypter( Crypter * );

public slots:
	void cancel();
	void ok();

private slots:
	void indexChanged( int );
};

#endif // _AUTH_WIDGET_H_

