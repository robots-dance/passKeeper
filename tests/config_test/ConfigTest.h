#ifndef _CONFIG_TEST_H_
#define _CONFIG_TEST_H_

#include <QTest>
#include <stdexcept>
#include <vector>
#include "Config.h"
#include "types.h"

class ConfigTest: public QObject
{
Q_OBJECT

public:
	ConfigTest( int maxSaveItemsCount ): m_forSaveSize( maxSaveItemsCount )
	{
		if ( m_forSaveSize <= 0 ) {
			throw std::invalid_argument( "m_forSaveSize" );
		}
	}

private slots:
	void initTestCase();
	void runCycle();

private:
	void compareConfigs( const Config &, const Config & );
	
	std::vector< KeyVal > m_forSave;
	int m_forSaveSize;
	KeyValMap m_saved;
};

#endif // _CONFIG_TEST_H_

