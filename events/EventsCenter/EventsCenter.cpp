#include <QApplication>
#include "EventsCenter.h"

EventsCenter *EventsCenter::Instance()
{
	static EventsCenter instance;
	return &instance;
}

void EventsCenter::addPort( QObject *receiver, QEvent::Type type )
{
	m_ports[ type ].insert( receiver );
}

void EventsCenter::removePort( QObject *reveiver, QEvent::Type type )
{
	if ( m_ports.find( type ) != m_ports.end() )
	{
		std::set< QObject* > &objects = m_ports[ type ];
		objects.erase( reveiver );
	}
}

void EventsCenter::customEvent( QEvent *event )
{
	QEvent::Type type = event->type();
	if ( m_ports.find( type ) != m_ports.end() )
	{
		std::set< QObject* > &objects = m_ports[ type ];
		for ( auto objIter = objects.begin(); objIter != objects.end();
			++objIter )
		{
			QApplication::sendEvent( *objIter, event );
		}
	}
}
