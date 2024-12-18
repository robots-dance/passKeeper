#ifndef _SEC_CLIPBOARD_H_
#define _SEC_CLIPBOARD_H_

#include <QObject>

#include "types.h"

class QTimer;

class SecClipboard: public QObject
{
	Q_OBJECT

public:
	static SecClipboard &Instance()
	{
		static SecClipboard obj;
		return obj;
	}
	
	void pasteText( cst_str );

private:
	QTimer *m_clipbTimer;
	int m_lifeTime;
	
	SecClipboard();
	
	SecClipboard( const SecClipboard & ){}
	
	const SecClipboard &operator=( SecClipboard & ) {
		return *this;
	}

private slots:
	void timeout();
};

#endif // _SEC_CLIPBOARD_H_

