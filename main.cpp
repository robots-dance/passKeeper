#include <QDesktopWidget>
#include <QMessageBox>

#include "ConfigWrapper.h"
#include "MainWgt.h"
#include "consts.h"
#include "tests.h"
#include "types.h"
#include "wsizes.h"

#define TEST_CONF_ITEMS_MAX 25

#ifdef _MSC_VER
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int ExecMain( int argc, char *argv[] );
int TestMain( int argc, char *argv[] );

int main( int argc, char *argv[] )
{
	int result = 0;
	if ( 1 == argc )
	{
		result = ExecMain( argc, argv );
	}
	else
	{
		char *firstParam = argv[ 1 ];
		if ( !strcmp( firstParam, "test" ) )
		{
			result = TestMain( argc, argv );
		}
		else {
			result = ExecMain( argc, argv );
		}
	}
	return result;
}

int ExecMain( int argc, char *argv[] )
{
	QApplication app( argc, argv );
	QTextCodec::setCodecForCStrings( QTextCodec::codecForName( "UTF-8" ) );
	
	// пытаемся загрузить файл конфигурации 
	if ( !ConfigWrapper::Instance().load( CONFIG_NAME ) )
	{
		QString errMess = QString( "Cannot load config file %1" );
		QMessageBox::critical( 0, "Config", errMess.arg( CONFIG_NAME ) );
		exit( 1 );
	}
 	
	// отображаем главный виджет по центру экрана 
	MainWgt wgt;
	int width = QApplication::desktop()->width();
	int height = QApplication::desktop()->height();
	const int ww = WIN_WIDTH, wh = WIN_HEIGHT;
	wgt.setGeometry( ( width - ww ) / 2, ( height - wh ) / 2, ww, wh );
	wgt.show();
	
	int retval = app.exec();
	return retval;
}

int TestMain( int argc, char *argv[] )
{
	if ( argc < 3 )
	{
		return 1;
	}
	
	/*
		cmd line's structure:
			1) executable path
			2) "test"
			... our custom params for test
			n) $test_name
			... ; method names from test to call
				(parsed by QTest, we cannot add our params to it)
	*/
	QObject *test = NULL;
	int argIndex = 2;
	
	// извлекаем аргументы для теста и пробуем его создать
	while ( !test && argIndex < argc )
	{
		char *testName = argv[ argIndex ];
		char *value;
		if ( !strcmp( testName, "xmldb" ) )
		{
			test = new XmlDbTest();
		}
		else if ( !strcmp( testName, "config" ) )
		{
			test = new ConfigTest( TEST_CONF_ITEMS_MAX );
		}
		else if ( !strcmp( testName, "crypter" ) )
		{
			// custom params:
			//    alg
			//    key_size
			//    mode
			
			// sample:
			// test AES 256 CBC crypter encryptFile decryptFile
			CryptAlg alg = AES;
			int keySize;
			Crypter::CryptMode mode = Crypter::CBC;
			if ( argIndex > 2 )
			{
				// alg
				value = argv[ 2 ];
				if ( !strcmp( value, "AES" ) ) {
					alg = AES;
				}
				else if ( !strcmp( value, "IDEA" ) ) {
					alg = IDEA;
				}
				else if ( !strcmp( value, "Camellia" ) ) {
					alg = Camellia;
				}
			}
			if ( argIndex > 3 )
			{
				// key_size
				keySize = atoi( argv[ 3 ] );
				keySize = keySize > 0 ? keySize : 256;
			}
			if ( argIndex > 4 )
			{
				// mode
				value = argv[ 4 ];
				if ( !strcmp( value, "ECB" ) )
				{
					mode = Crypter::ECB;
				}
				else if ( !strcmp( value, "CBC" ) ) {
					mode = Crypter::CBC;
				}
				else if ( !strcmp( value, "CFB" ) ) {
					mode = Crypter::CFB;
				}
				else if ( !strcmp(value, "OFB" ) ) {
					mode = Crypter::OFB;
				}
			}
			qDebug( "alg: %d, key_size: %d, mode: %d\n",
				alg, keySize, mode );
			test = new CrypterTest( alg, keySize, mode );
		}
		argIndex++;
	}
	
	int result = 0;
	
	// запускаем тест, если его получилось создать
	if ( test  != NULL )
	{
		argc = argc - argIndex + 1;
		for ( int j = 1; j < argc; j++ )
		{
			argv[ j ] = argv[ j + argIndex - 1 ];
		}
		argv[ argc ] = 0;
		result = QTest::qExec( test, argc, argv );
		delete test;
	}
	else {
		result = 2;
	}
	
	return result;
}

