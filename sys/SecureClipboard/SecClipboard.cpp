#include <QApplication>
#include <QClipboard>
#include <QTimer>

#include "ConfigWrapper.h"
#include "consts.h"

#include "SecClipboard.h"

SecClipboard::SecClipboard()
{
	// получаем значение опции времени жизни буфера
	ConfigWrapper &config = ConfigWrapper::Instance();
	const QString optionName = "buffer_lifetime";
	m_lifeTime = DEF_CLR_TIME;
	if ( config.exists( optionName ) )
	{
		QString lifeTimeStr = config.getValue( optionName );
		bool convertSuccess;
		m_lifeTime = lifeTimeStr.toInt( &convertSuccess );
		if ( !convertSuccess ) {
			m_lifeTime = DEF_CLR_TIME;
		}
	}
	
	// создаем таймер для уничтожения буфера
	m_clipbTimer = new QTimer( this );
	m_clipbTimer->setSingleShot( true );
	connect( m_clipbTimer, SIGNAL( timeout() ), SLOT( timeout() ) );
}

void SecClipboard::pasteText( cst_str text )
{
	QApplication::clipboard()->setText( text );
	if ( m_lifeTime > 0 )
	{
		m_clipbTimer->setInterval( m_lifeTime * 1000 );
		m_clipbTimer->start();
	}
}


// ==== slots ====
void SecClipboard::timeout()
{
	 QApplication::clipboard()->clear();
}
