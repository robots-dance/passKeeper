#include "utils.h"

#include "ConfigTest.h"

const QString g_fileName = "./pkconf.ini";
const QString g_newFileName = "./pkconf-new.ini";

void ConfigTest::compareConfigs( const Config &masterConf,
	const Config &slaveConf )
{
	Config::KeysList masterKeys = masterConf.getKeys();
	Config::KeysList slaveKeys = slaveConf.getKeys();
	QString errMess;
	for ( auto keyIter = masterKeys.begin(); keyIter != masterKeys.end();
		++keyIter )
	{
		QString key = *keyIter;
		
		errMess = QString( "the '%1' key not exists" ).arg( key );
		if ( !slaveConf.exists( key ) )
		{
			if ( m_saved.find( key ) == m_saved.end() )
			{
				QFAIL( errMess.toStdString().c_str() );
			}
			else {
				continue;
			}
		}
		
		errMess = QString( "values aren't equal for '%1' key" ).arg( key );
		QVERIFY2( masterConf.getValue( key ) == slaveConf.getValue( key ),
			errMess.toStdString().c_str() );
	}
}

void ConfigTest::initTestCase()
{
	const QString keyAlpha = "abcdefghijklmnopqrstuvwxyz";
	const QString valueAlpha = keyAlpha + "0123456789" + "_-";
	const int keyLimits[ 2 ] = { 4, 6 };
	const int valueLimits[ 2 ] = { 10, 20 };
	m_forSave.resize( m_forSaveSize );
	Utils::initGenerator();
	for ( int saveItemIndex = 0; saveItemIndex < m_forSaveSize;
		saveItemIndex++ )
	{
		QString key = Utils::generateString( keyAlpha,
			keyLimits[ 0 ], keyLimits[ 1 ] );
		QString value = Utils::generateString( valueAlpha,
			valueLimits[ 0 ], valueLimits[ 1 ] );
		m_forSave[ saveItemIndex ] = KeyVal( key, value );
	}
}

void ConfigTest::runCycle()
{
	// try to load a test config
	QVERIFY2( QFile::exists( g_fileName ), "config file not exists" );
	Config conf;
	QVERIFY2( conf.load( g_fileName ), "cannot load config file" );
	
	// try to save an unchanged config
	QFile::remove( g_newFileName );
	conf.setNewPath( g_newFileName );
	conf.save();
	QVERIFY2( !QFile::exists( g_newFileName ), "changing flag doesn't work" );
	
	// try to perform some change and save operations0 
	int remainsCount = m_forSaveSize;
	int curItemIndex = 0;
	while ( remainsCount > 0 )
	{
		int maxVal = remainsCount >= 2 ? remainsCount / 2 : 1;
		int curSaveCount = Utils::generateNumber( 1, maxVal );
		for ( int saveIndex = 0; saveIndex < curSaveCount; saveIndex++ )
		{
			const KeyVal &pair = m_forSave[ curItemIndex ];
			QString key = pair.first;
			QString value = pair.second;
			conf.setValue( key, value );
			m_saved[ key ] = value;
			curItemIndex++;
		}
		QVERIFY2( conf.save(), "cannot save changed config file" );
		remainsCount -= curSaveCount;
	}
	
	conf.load( g_fileName );
	
	// compare two config files
	Config newConf;
	QVERIFY2( newConf.load( g_newFileName ), "cannot load new config file" );
	
	compareConfigs( conf, newConf );
	compareConfigs( newConf, conf );
}
