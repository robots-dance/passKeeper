#include "SleepThread.h"

SleepThread::SleepThread( QObject *parent ): QThread( parent )
{
	;
}

void SleepThread::sleep( unsigned long time )
{
	QThread::sleep( time );
}

void SleepThread::msleep( unsigned long time )
{
	QThread::msleep( time );
}

void SleepThread::usleep( unsigned long time )
{
	QThread::usleep( time );
}
