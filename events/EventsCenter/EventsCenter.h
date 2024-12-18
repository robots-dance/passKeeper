#ifndef _EVENTS_CENTER_H_
#define _EVENTS_CENTER_H_

#include <QEvent>
#include <QObject>
#include <map>
#include <set>

class EventsCenter: public QObject
{
	Q_OBJECT

public:
	static EventsCenter *Instance();
	// получает singleton
	
	void addPort( QObject *receiver, QEvent::Type type );
	// добавляет порт для прослушки
	
	void removePort( QObject *, QEvent::Type );
	// выключает порт

protected:
	void customEvent( QEvent * );

private:
	EventsCenter() {}
	EventsCenter( const QObject & ){}
	// protect from creating and copying
	// TODO: copy constructor without warning
	
	std::map< QEvent::Type, std::set< QObject* > > m_ports;
};

#endif // _EVENTS_CENTER_H_

