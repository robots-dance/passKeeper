#ifndef _MAIN_WIDGET_H_
#define _MAIN_WIDGET_H_

#include <QWidget>
#include "types.h"

class QDialog;

class AuthWgt;
class Crypter;
class FieldsWgt;
class SettingsWgt;
class TreeNode;
class TreeWgt;
class XmlDb;

class QAction;

class MainWgt: public QWidget
{
	Q_OBJECT

public:
	explicit MainWgt( QWidget *parent = 0 );
	~MainWgt();

protected:
	void closeEvent( QCloseEvent * );
	void customEvent( QEvent * );

private:
	void askForUnsaved();
	/*
		Если текущий файл не сохранен, спрашивает у пользователя, сделать ли
		это в текущий момент или потерять все изменения с момента последнего
		сохранения
	*/
	
	void clear();
	// закрывает файл данных и очищает все виджеты от данных
	
	void createAboutDialog();
	
	void updateTreeWidget( TreeNode * );
	// обновляет виджет дерева аргументом метода
	
	static const int kAttemptsMax = 4;
	
	QDialog *m_aboutDlg;
	AuthWgt *m_authWgt; // диалог авторизации
	QAction *m_cryptAction; // пункт меню изменения пароля
	XmlDb *m_db; // база данных с паролями
	Crypter *m_lastCrypter; // актуальный шифровальщик
	QString m_openPath; // путь открываемого файла
	SettingsWgt *m_settWgt; // виджет настроек приложения
	FieldsWgt *m_tableWgt; // виджет редактирования полей записи
	TreeWgt *m_treeWgt; // виджет дерева

public slots:

	void about();
	
	void changeSecurity();
	
	void changed();
	
	void closeFile();
	
	void deleteFile();
	
	void exportKeys();
	
	void exportStruc();
	
	void importStruc();
	
	void newCrypter( Crypter * );
	
	void newFile();
	
	void openFile();
	
	void openSettings();
	
	void quit();
	
	void saveFile();
};

#endif // _MAIN_WIDGET_H_

