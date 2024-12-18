#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <typeinfo>

#include "AES_Crypter.h"
#include "Camellia_Crypter.h"
#include "IDEA_Crypter.h"
#include "messages.h"

#include "AuthWgt.h"

AuthWgt::AuthWgt( QWidget *parent ) : QDialog( parent ), m_crypter( 0 )
{
	// создаем список с крипто-алгоритмами
	m_cryptAlgsBox = new QComboBox();
	m_cryptAlgsBox->addItems( QStringList() <<
		"AES" << "IDEA" << "Camellia" );
	connect( m_cryptAlgsBox, SIGNAL( currentIndexChanged( int ) ),
		SLOT( indexChanged( int ) ) );
	
	m_keySizeBox = new QComboBox();
	m_algModesBox = new QComboBox();
	
	indexChanged( 0 ); // выбираем первый элемент списка
	
	// создаем поле ввода пароля
	m_passEdt = new QLineEdit();
	m_passEdt->setEchoMode( QLineEdit::Password );
	
	// размещаем поля на форме
	QGridLayout *grid = new QGridLayout();
	
	grid->addWidget( new QLabel( "Password:" ), 0, 0 );
	grid->addWidget( m_passEdt, 0, 1 );
	
	grid->addWidget( new QLabel( "Algorithm:" ), 1, 0 );
	grid->addWidget( m_cryptAlgsBox, 1, 1 );
	
	grid->addWidget( new QLabel( "Key size:" ), 2, 0 );
	grid->addWidget( m_keySizeBox, 2, 1 );
	
	grid->addWidget( new QLabel( "Mode:" ), 3, 0 );
	grid->addWidget( m_algModesBox, 3, 1 );
	// --
	
	// создаем кнопку 'ok'
	QPushButton *btOk = new QPushButton( "Ok" );
	connect( btOk, SIGNAL( clicked() ), SLOT( ok() ) );
	
	// создаем кнопку 'cancel'
	QPushButton *btCancel = new QPushButton( "Cancel" );
	connect( btCancel, SIGNAL( clicked() ), SLOT( cancel() ) );
	
	// создаем горизонтальную компановку для кнопок 
	QHBoxLayout *hrsl = new QHBoxLayout();
	hrsl->addWidget( btOk );
	hrsl->addWidget( btCancel );
	
	// создаем вертикальную компановку 
	QVBoxLayout *vrtl = new QVBoxLayout();
	vrtl->addLayout( grid );
	vrtl->addLayout( hrsl );
	setLayout( vrtl );
}

void AuthWgt::close()
{
	resetInputs();
	m_passEdt->setFocus();
	QWidget::close();
}

void AuthWgt::resetCrypter()
{
	if ( m_crypter != NULL )
	{
		m_crypter->setPass( "" );
		delete m_crypter;
		m_crypter = 0;
	}
}

void AuthWgt::resetInputs()
{
	m_cryptAlgsBox->setCurrentIndex( 0 );
	indexChanged( 0 );
	m_passEdt->setText( "" );
}


// slots
void AuthWgt::cancel()
{
	close();
}

void AuthWgt::indexChanged( int index )
{
	m_keySizeBox->clear();
	m_algModesBox->clear();
	if ( AES == index )
	{
		m_keySizeBox->addItems( QStringList() <<
			"128" << "192" << "256" );
	}
	else if ( IDEA == index )
	{
		m_keySizeBox->addItem( "128" );
	}
	else if ( Camellia == index )
	{
		m_keySizeBox->addItem( "128" );
		m_keySizeBox->addItem( "256" );
	}
	else // in the future
	{
		// do nothing
	}
	m_algModesBox->addItems( QStringList() <<
		"CBC" << "CFB" << "ECB" << "OFB" );
}

void AuthWgt::ok()
{
	DEF_TITLE( "Authorization" );
	
	QString pass = m_passEdt->text();
	if ( pass.length() == 0 )
	{
		QMessageBox::warning( 0, GET_TITLE(), "Empty password!" );
		return;
	}
	
	// extract input values
	CryptAlg curAlg = static_cast< CryptAlg >(
		m_cryptAlgsBox->currentIndex() );
	
	int keySize = m_keySizeBox->currentText().toInt();
	
	Crypter::CryptMode mode = Crypter::CBC;
	QString modeStr = m_algModesBox->currentText();
	if ( "CBC" == modeStr )
	{
		mode = Crypter::CBC;
	}
	else if ( "CFB" == modeStr )
	{
		mode = Crypter::CFB;
	}
	else if ( "ECB" == modeStr )
	{
		mode = Crypter::ECB;
	}
	else if ( "OFB" == modeStr )
	{
		mode = Crypter::OFB;
	}
	
	if ( m_crypter != NULL )
	// in case of a 'Change security' activation
	{
		CryptAlg prevAlg;
		if ( typeid( *m_crypter ) == typeid( AES_Crypter ) )
		{
			prevAlg = AES;
		}
		else if ( typeid( *m_crypter ) == typeid( IDEA_Crypter ) )
		{
			prevAlg = IDEA;
		}
		else
		{
			prevAlg = Camellia;
		}
		if ( prevAlg == curAlg &&
			m_crypter->keySize() == keySize &&
			m_crypter->mode() == mode &&
			m_crypter->password() == pass )
		{
			QMessageBox::information( 0, GET_TITLE(),
				"You haven't changed params." );
			return;
		}
	}
	
	// create a crypter
	if ( AES == curAlg )
	{
		m_crypter = new AES_Crypter();
	}
	else if ( IDEA == curAlg )
	{
		m_crypter = new IDEA_Crypter();
	}
	else if ( Camellia == curAlg )
	{
		m_crypter = new Camellia_Crypter();
	}
	else
	{
		QMessageBox::critical( 0, GET_TITLE(), "Unknown algorithm!" );
		return;
	}
	
	m_crypter->setKeySize( keySize );
	m_crypter->setMode( mode );
	m_crypter->setPass( pass );
	
	emit newCrypter( m_crypter );
	close();
}
