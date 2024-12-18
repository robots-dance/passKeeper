#ifndef _TREEWGT_H_
#define _TREEWGT_H_

#include <QWidget>

class QMenu;
class QTimer;
class QTreeWidget;
class QTreeWidgetItem;

class DescrWgt;
class KeyEntity;
class NewKeyWgt;
class TreeNode;
class TreeWgtItem;

class TreeWgt: public QWidget
{
	Q_OBJECT

public:
	class BadSelectedItem {};
	/*
		выкидывается в релиз-версии в случае некорректно выбранного
		item''; по-хорошему просто спасательный жилет, ошибка не
		должна появляться
	*/
	
	explicit TreeWgt( QWidget *parent = 0 );
	
	void acceptKey();
	// разрешить добавить выделенный ключ
	
	void exportKeys();
	// копирует в буфер обмена все ключи со всеми полями
	
	void setTree( TreeNode *tree )
	// устанавливает новое дерево (не обновляет граф. интерфейс!)
	{
		this->m_tree = tree;
	}
	
	void update();
	// обновляет виджет после смены дерева (полная перерисовка)

protected:
	bool eventFilter( QObject *, QEvent * );

private:
	TreeWgtItem *getSelectedItem();
	/*
		возвращает текущий выделенный item или 0, если выделения
		нет (или произошла ошибка)
	*/
	
	QString getPath( const QTreeWidgetItem * );
	//  возвращает путь к узлу item (берутся titles в качестве
	//  имен узлов)
	
	bool isSelected() const;
	
	void openDescrWindow( bool forChange );
	
	void sortColumn( TreeWgtItem * );
	/*
		сортирует 0-й столбец предложенного item'a
		
		Note: на текущий момент не используется из-за того, что
		сортировать надо не только по имени узла, но и по его типу
	*/
	
	void update( TreeWgtItem * );
	// служебный метод для рекурсивного обновления дерева
	
	TreeNode *m_clippedNode; // вырезанный узел
	QMenu *m_ctxMenu; // контекстное меню
	DescrWgt *m_descrWgt; // окно описания узла
	NewKeyWgt *m_newKeyWgt; // окно создания нового ключа
	TreeNode *m_tree; // представление
	QTreeWidget *m_treeWgt; // виджет, отображающий дерево

signals:
	void changed();
	// высылается при любом изменении структуры дерева
	
	void openEntity( KeyEntity * );
	/*
		высылается при выделении новой сущности
		(его графического представления)
	*/

public slots:
	// возникают при нажатии на соотв. кнопки внизу m_treeWgt
	void addFolder();
	void addKey();
	void cutItem();
	void pasteItem();
	void removeItem();
	void setDescr();
	void showDescr();

private slots:
	void descriptionChanged();
	// возникает при изменении описании узла
	
	void itemChanged( QTreeWidgetItem *, int );
	// возникает при переименовании узла в m_treeWgt 
	
	void itemCollapsed( QTreeWidgetItem * );
	// возникает при сворачивании узла
	
	void itemExpanded( QTreeWidgetItem * );
	// возникает при разворачивании узла
	
	void itemSelectionChanged();
	// возникает при выделении нового элемента в виджете дерева
	
	void triggered( QAction * );
	// возникает при выборе пункта из контекстного меню
};

#endif // _TREEWGT_H_

