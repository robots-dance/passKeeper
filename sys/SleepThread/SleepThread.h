#ifndef _SLEEP_THREAD_H_
#define _SLEEP_THREAD_H_

#include <QThread>

class SleepThread: public QThread
{
	Q_OBJECT

public:
	explicit SleepThread( QObject *parent = 0 );
	
	// разрешаем "засыпать" потоку
	static void sleep( unsigned long );
	static void msleep( unsigned long );
	static void usleep( unsigned long );

signals:

public slots:

};

#endif // _SLEEP_THREAD_H_

