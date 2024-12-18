#ifndef _KEY_ENT_EVENT_H_
#define _KEY_ENT_EVENT_H_

#include <QEvent>

class KeyEntity;

class KEQuestEvent: public QEvent
{
public:
	KEQuestEvent( KeyEntity *entity ):
		QEvent( KEQuestEvent::myType() ), m_ent( entity )
	{}
	
	static Type myType() {
		return Type( 1024 );
	}
	
	KeyEntity *entity() const {
		return m_ent;
	}

private:
	KeyEntity *m_ent;
};


class KEAnswerEvent: public QEvent
{
public:
	KEAnswerEvent( bool status ):
		QEvent( KEAnswerEvent::myType() ), m_status( status )
	{}
	
	static Type myType() {
		return Type( 1025 );
	}
	
	bool isSuccess() const {
		return m_status;
	}
	
	void setStatus( bool status ) {
		m_status = status;
	}

private:
	bool m_status;
};

#endif // _KEY_ENT_EVENT_H_
