#include <QApplication>
#include <QBoxLayout>
#include <QClipboard>
#include <QDialog>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>

#include "AuthWgt.h"
#include "ConfigWrapper.h"
#include "EventsCenter.h"
#include "FieldsWgt.h"
#include "KeyEntEvent.h"
#include "SecClipboard.h"
#include "SettingsWgt.h"
#include "SleepThread.h"
#include "TreeWgt.h"
#include "XmlDb.h"
#include "consts.h"
#include "messages.h"
#include "types.h"
#include "wsizes.h"

#include "MainWgt.h"

MainWgt::MainWgt( QWidget *parent ) : QWidget( parent ),
	m_lastCrypter( 0 )
{
	EventsCenter::Instance()->addPort( this, KEQuestEvent::myType() );
	
	QMenuBar *mbar = new QMenuBar();
	
	// создаем меню файла ( File )
	QMenu *fileMenu = new QMenu( "&File" );
	
	fileMenu->addAction( "&New file", this, SLOT( newFile() ),
		Qt::CTRL + Qt::Key_N );
	
	fileMenu->addAction( "&Open file...", this, SLOT( openFile() ),
		Qt::CTRL + Qt::Key_O );
	
	fileMenu->addAction( "&Save", this, SLOT( saveFile() ),
		Qt::CTRL + Qt::Key_S );
	
	fileMenu->addAction( "Close file", this, SLOT( closeFile() ),
		Qt::CTRL + Qt::Key_W );
	
	fileMenu->addAction( "&Delete file", this, SLOT( deleteFile() ),
		Qt::CTRL + Qt::Key_D );
	
	fileMenu->addAction( "&Quit", this, SLOT( quit() ),
		Qt::CTRL + Qt::Key_Q );
	
	mbar->addMenu( fileMenu );
	
	m_treeWgt = new TreeWgt();
	
	
	// создаем меню просмотра ( View )
	QMenu *viewMenu = new QMenu( "&View" );
	viewMenu->addAction( "Show description", m_treeWgt, SLOT( showDescr() ),
		Qt::CTRL + Qt::Key_R );
	mbar->addMenu( viewMenu );
	
	
	// создаем меню редактирования ( Edit )
	QMenu *editMenu = new QMenu( "&Edit" );
	
	editMenu->addAction( "Cut", m_treeWgt, SLOT( cutItem() ),
		Qt::CTRL + Qt::Key_X );
	
	editMenu->addAction( "Paste", m_treeWgt, SLOT( pasteItem() ),
		Qt::CTRL + Qt::Key_V );
	
	editMenu->addAction( "Set desc&ription...", m_treeWgt, SLOT( setDescr() ),
		Qt::ALT + Qt::Key_R );
	
	m_cryptAction = editMenu->addAction( "Change security...", this,
		SLOT( changeSecurity() ), Qt::CTRL + Qt::Key_P );
	m_cryptAction->setEnabled( false );
	
	editMenu->addAction( "Se&ttings...", this, SLOT( openSettings() ),
		Qt::CTRL + Qt::Key_T );
	
	editMenu->addAction( "&Import struc", this, SLOT( importStruc() ) );
	
	mbar->addMenu( editMenu );
	
	
	// создаем меню инструментов ( Tools )
	QMenu *toolsMenu = new QMenu( "&Tools" );
	
	toolsMenu->addAction( "&Export keys", this, SLOT( exportKeys() ),
		Qt::CTRL + Qt::Key_E );
	
	toolsMenu->addAction( "Export struc", this, SLOT( exportStruc() ) );
	
	mbar->addMenu( toolsMenu );
	
	
	// создаем меню справки ( Help )
	QMenu *aboutMenu = new QMenu( "&Help" );
	
	aboutMenu->addAction( "&About...", this, SLOT( about() ) );
	
	createAboutDialog();
	
	mbar->addMenu( aboutMenu );
	
	
	// создаем разделитель между виджетами дерева и таблицы
	QSplitter *spl = new QSplitter();
	spl->setOrientation( Qt::Horizontal );
	
	// настраиваем виджет дерева
	spl->addWidget( m_treeWgt );
	connect( m_treeWgt, SIGNAL( changed() ), SLOT( changed() ) );
	
	// создаем виджет таблицы
	m_tableWgt = new FieldsWgt();
	spl->addWidget( m_tableWgt );
	connect( m_tableWgt, SIGNAL( changed() ), SLOT( changed() ) );
	
	// связываем дерево и таблицу
	connect( m_treeWgt, SIGNAL( openEntity( KeyEntity * ) ),
		m_tableWgt, SLOT( openEntity( KeyEntity * ) ) );
	
	// размещаем меню и менеджер компоновки
	QVBoxLayout *vrtl = new QVBoxLayout();
	vrtl->addWidget( mbar, 0 );
	// vrtl->addSpacing( 10 );
	vrtl->addWidget( spl, 1 );
	setLayout( vrtl );
	
	m_db = new XmlDb();
	
	// создаем окно авторизации
	m_authWgt = new AuthWgt( this );
	m_authWgt->resize( AUTH_WIDTH, AUTH_HEIGHT );
	m_authWgt->setFixedSize( m_authWgt->size() );
	connect( m_authWgt,
		SIGNAL( newCrypter( Crypter * ) ),
		SLOT( newCrypter( Crypter * ) ),
		Qt::DirectConnection );
	m_authWgt->setModal( true );
}

MainWgt::~MainWgt()
{
	delete m_aboutDlg;
	delete m_db;
}

void MainWgt::askForUnsaved()
{
	if ( m_db->isChanged() )
	{
		// спрашиваем у пользователя - нужно ли сохранить файл
		int resCode = QMessageBox::warning( 0, "Unsaved file",
			"Current file doesn't saved. Save it?",
			QMessageBox::Ok, QMessageBox::Cancel );
		if ( QMessageBox::Ok == resCode )
		{
			saveFile();
		}
	}
}

void MainWgt::clear()
{
	if ( m_db->isOpened() ) {
		m_db->close();
	}
	m_authWgt->resetCrypter();
	m_tableWgt->reset();
	m_lastCrypter = NULL;
	updateTreeWidget( 0 );
	setWindowTitle( "" );
	m_cryptAction->setEnabled( false );
}

void MainWgt::createAboutDialog()
{
	QString aboutText = "<b>PassKeeper</b> ";
	
	aboutText += PROGRAM_VERSION;
	aboutText += "<br>";
	aboutText += "<p align='justify'>";
	aboutText += "You can use a compiled version of this program ";
	aboutText += "without any payments and an author mention. ";
	aboutText += "You can't limit or prohibit an usage of PassKeeper ";
	aboutText += "for someone. You may research this binary code with ";
	aboutText += "any disassemblers, debuggers and other tools. ";
	aboutText += "</p>";
	aboutText += "<p align='justify'>";
	aboutText += "This program no contains any network code or ";
	aboutText += "backdoors for transmit user files to a 3rd side.";
	aboutText += "</p>";
	aboutText += "<br>========<br>Developed by <i>anonimous</i>";
	
	QLabel *aboutLbl = new QLabel();
	aboutLbl->setWordWrap( true );
	aboutLbl->setText( aboutText );
	
	m_aboutDlg = new QDialog();
	m_aboutDlg->resize( ABOUT_WIDTH, ABOUT_HEIGHT );
	m_aboutDlg->setFixedSize( m_aboutDlg->size() );
	m_aboutDlg->setLayout( ( new QVBoxLayout() ) );
	m_aboutDlg->layout()->addWidget( aboutLbl );
}

void MainWgt::closeEvent( QCloseEvent * )
{
	QString checkOnClose = ConfigWrapper::Instance().getValue(
		"check_on_win_close" );
	if ( Config::isTrue( checkOnClose ) )
	{
		askForUnsaved();
	}
	else {
		saveFile();
	}
}

void MainWgt::customEvent( QEvent *event )
{
	if ( event->type() == KEQuestEvent::myType() )
	{
		KEQuestEvent *keEvent = static_cast< KEQuestEvent* >( event );
		m_db->addEntity( keEvent->entity(), 0 );
		KEAnswerEvent event( true );
		QApplication::sendEvent( EventsCenter::Instance(), &event );
		m_treeWgt->acceptKey();
	}
}

void MainWgt::updateTreeWidget( TreeNode *tree )
{
	m_treeWgt->setTree( tree );
	m_treeWgt->update();
}


// ======== slots ======== 
void MainWgt::about()
{
	m_aboutDlg->exec();
}

void MainWgt::changeSecurity()
{
	m_authWgt->show();
}

void MainWgt::changed()
{
	if ( m_openPath.length() > 0 )
	{
		setWindowTitle( QFileInfo( m_openPath ).fileName() );
		m_openPath = "";
		return;
	}
	m_db->setChanged();
	QString title = m_db->getPath();
	if ( title.length() == 0 )
	{
		title = DEF_FILE_TITLE;
	}
	else {
		title = QFileInfo( title ).fileName();
	}
	setWindowTitle( title + " *" );
}

void MainWgt::closeFile()
{
	if ( m_db->isOpened() )
	{
		if ( m_db->isChanged() ) {
			saveFile();
		}
		clear();
	}
	else {
		QMessageBox::warning( 0, "Close File", FILE_WASNT_OPENED );
	}
}

void MainWgt::deleteFile()
{
	DEF_TITLE( "Delete file" );
	if ( m_db->isOpened() )
	{
		if ( !QFile::remove( m_db->getPath() ) )
		{
			QMessageBox::critical( 0, GET_TITLE(), "Cannot delete file!" );
		}
		else {
			clear();
		}
	}
	else {
		QMessageBox::warning( 0, GET_TITLE(), FILE_WASNT_OPENED );
	}
}

void MainWgt::exportKeys()
{
	m_treeWgt->exportKeys();
}

void MainWgt::exportStruc()
{
	DEF_TITLE( "Export struc" );
	if ( !m_db->isOpened() )
	{
		QMessageBox::warning( 0, GET_TITLE(), FILE_WASNT_OPENED );
		return;
	}
	QString content = m_db->getContent();
	SecClipboard::Instance().pasteText( content );
}

void MainWgt::importStruc()
{
	DEF_TITLE( "Import struc" );
	if ( m_db->isOpened() )
	{
		QMessageBox::warning( 0, GET_TITLE(),
			"Close a current file before a this operation" );
		return;
	}
	QString content = QApplication::clipboard()->text();
	QByteArray data;
	data.append( content );
	TreeNode *tree = m_db->create( data ); 
	if ( tree != NULL )
	{
		updateTreeWidget( tree );
		this->setWindowTitle( DEF_FILE_TITLE );
		m_cryptAction->setEnabled( true );
		QApplication::clipboard()->clear();
	}
	else
	{
		QMessageBox::critical( 0, GET_TITLE(),
			"Incorrect an internal structure" );
	}
}

void MainWgt::newCrypter( Crypter *crypter )
{
	m_lastCrypter = crypter;
	if ( m_openPath.length() == 0 ) {
		changed();
	}
}

void MainWgt::newFile()
{
	askForUnsaved();
	clear();
	updateTreeWidget( m_db->create() );
	this->setWindowTitle( DEF_FILE_TITLE );
	m_cryptAction->setEnabled( true );
}

void MainWgt::openFile()
{
	DEF_TITLE( "Open File" );
	
	askForUnsaved();
	clear();
	
	m_openPath = QFileDialog::getOpenFileName( 0, GET_TITLE(), "",
		DEF_FILTER );
	if ( 0 == m_openPath.length() )
	{
		QMessageBox::warning( 0, GET_TITLE(), "File is not selected!" );
		return;
	}
	
	// поиск максимального значения
	int attemptsMax = 0;
	QString attemptsMaxStr = ConfigWrapper::Instance().getValue(
		"attempts_max" );
	if ( attemptsMaxStr != "" )
	{
		attemptsMax = attemptsMaxStr.toInt();
	}
	if ( attemptsMax <= 0 )
	{
		attemptsMax = kAttemptsMax;
	}
	
	int attemptsCounter = 0;
	bool success = false;
	while ( !success && attemptsCounter < attemptsMax )
	{
		m_lastCrypter = NULL;
		m_authWgt->resetCrypter();
		m_authWgt->exec();
		if ( NULL == m_lastCrypter )
		{
			QMessageBox::warning( 0, GET_TITLE(), ENC_PARAMS_ARE_BAD );
			break;
		}
		
		m_db->setCrypter( m_lastCrypter );
		if ( !m_db->open( m_openPath ) )
		{
			QMessageBox::warning( 0, GET_TITLE(), "Cannot open file!" );
			break;
		}
		
		TreeNode *tree = m_db->load();
		if ( tree != NULL )
		{
			success = true;
			updateTreeWidget( tree );
			m_cryptAction->setEnabled( true );
		}
		else
		{
			attemptsCounter++;
			m_db->close();
			QMessageBox::warning( 0, GET_TITLE(), "Cannot parse file!" );
		}
	}
}

void MainWgt::openSettings()
{
	QMessageBox::information( 0, "Settings", "No implemented now" );
}

void MainWgt::quit()
{
	QCoreApplication::exit( 0 );
}

void MainWgt::saveFile()
{
	DEF_TITLE( "Save File" );
	if ( !m_db->isOpened() || !m_db->isChanged() )
	{
		return;
	}
	
	if ( NULL == m_lastCrypter )
	{
		m_authWgt->exec();
	}
	if ( NULL == m_lastCrypter )
	{
		QMessageBox::warning( 0, GET_TITLE(), ENC_PARAMS_ARE_BAD );
		return;
	}
	m_db->setCrypter( m_lastCrypter );
	
	QString path;
	if ( m_db->isCreated() )
	{
		path = QFileDialog::getSaveFileName( 0, GET_TITLE(),
			"", DEF_FILTER );
	}
	else
	{
		QString srcCopyPath = m_db->getPath();
		QString dstCopyPath = srcCopyPath + DEF_BACK_SUFF;
		bool status = true;
		if ( QFile::exists( dstCopyPath ) )
		{
			status = QFile::remove( dstCopyPath );
		}
		status = QFile::copy( srcCopyPath, dstCopyPath );
		path = "";
		if ( !status )
		{
			QMessageBox::warning( 0, GET_TITLE(),
				"Cannot create a backup copy" );
		}
	}
	if ( !m_db->save( path ) )
	{
	// TODO: добавить цикл
		QMessageBox::warning( 0, GET_TITLE(), "Cannot save file!" );
	}
	else
	{
		path = m_db->getPath();
		setWindowTitle( QFileInfo( path ).fileName() );
	}
}
