#include <QApplication>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

#include "EventsCenter.h"
#include "KeyEntEvent.h"
#include "KeyEntity.h"
#include "messages.h"

#include "NewKeyWgt.h"

NewKeyWgt::NewKeyWgt( QWidget *parent ) : QDialog( parent ), m_lastEnt( 0 )
{
	EventsCenter::Instance()->addPort( this, KEAnswerEvent::myType() );
	
	// создаем текстовые поля для ввода
	m_loginEdt = new QLineEdit();
	m_passEdt = new QLineEdit();
	m_passEdt->setEchoMode( QLineEdit::Password );
	m_titleEdt = new QLineEdit();
	
	// создаем кнопки
	QPushButton *btCreate = new QPushButton( "Create" );
	connect( btCreate, SIGNAL( clicked() ), SLOT( create() ) );
	QPushButton *btCancel = new QPushButton( "Cancel" );
	connect( btCancel, SIGNAL( clicked() ), SLOT( cancel() ) );
	
	// создаем сетку для размещения контролов
	QGridLayout *grid = new QGridLayout();
	
	grid->addWidget( new QLabel( "Title *:" ), 0, 0 );
	grid->addWidget( m_titleEdt, 0, 1 );
	
	grid->addWidget( new QLabel( "Password *:" ), 1, 0 );
	grid->addWidget( m_passEdt, 1, 1 );
	
	grid->addWidget( new QLabel( "Login:" ), 2, 0 );
	grid->addWidget( m_loginEdt, 2, 1 );
	
	// создаем компоновку для кнопок 
	QBoxLayout *hrsl = new QBoxLayout( QBoxLayout::LeftToRight );
	hrsl->addWidget( btCreate );
	hrsl->addWidget( btCancel );
	
	// создаем и настраиваем основную компоновку 
	QBoxLayout *vrtl = new QBoxLayout( QBoxLayout::TopToBottom );
	vrtl->addLayout( grid );
	vrtl->addLayout( hrsl );
	setLayout( vrtl );
	
	setWindowTitle( "Create new key" );
}

void NewKeyWgt::customEvent( QEvent *event )
{
	if ( event->type() == KEAnswerEvent::myType() )
	{
        // получаем ответ, имеется ли такой ключ в базе
		KEAnswerEvent *keEvent = static_cast< KEAnswerEvent * >( event );
		m_status = keEvent->isSuccess() ? Added : Skipped;
	}
}

void NewKeyWgt::clear()
{
	m_loginEdt->setText( "" );
	m_passEdt->setText( "" );
	m_titleEdt->setText( "" );
}

QString NewKeyWgt::title() const
{
	return m_titleEdt->text();
}


// ======== slots ======== 
void NewKeyWgt::cancel()
{
	close();
}

void NewKeyWgt::create()
{
	DEF_TITLE( "Create Key" );
	
	// извлекаем текст из полей ввода 
	QString login = m_loginEdt->text();
	QString password = m_passEdt->text();
	QString title = m_titleEdt->text();
	
	// проверяем обязательные поля
	if ( password.length() == 0 ||
		title.length() == 0 )
	// эти поля должны быть заполнены!
	{
		QMessageBox::warning( 0, GET_TITLE(),
			"Some fields are empty!" );
		return;
	}
	
	// создаем новый ключ
	KeyEntity *ent = new KeyEntity();
	m_lastEnt = ent;
	ent->setf( "login", login );
	ent->setf( "password", password );
	
	// высылаем сообщение-вопрос о существовании ключа
	KEQuestEvent event( ent );
	m_status = Waiting;
	QApplication::sendEvent( EventsCenter::Instance(), &event );
	while ( Waiting == m_status )
	{
		QApplication::processEvents();
	}
	
	// проверяем асинхронный ответ ( приходит через customEvent )
	if ( m_status != Added )
	{
		delete ent;
		m_lastEnt = NULL;
		QMessageBox::warning( 0, GET_TITLE(), "Can't add this key" );
	}
	else {
		close();
	}
}
