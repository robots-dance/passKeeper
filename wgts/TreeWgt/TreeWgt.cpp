#include <QBoxLayout>
#include <QContextMenuEvent>
#include <QInputDialog>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QTreeWidget>

#include <algorithm>
#include <iterator>
#include <queue>
#include <vector>

#include "DescrWgt.h"
#include "KeyEntity.h"
#include "NewKeyWgt.h"
#include "SecClipboard.h"
#include "TreeNode.h"
#include "TreeWgtItem.h"
#include "consts.h"
#include "debug.h"
#include "messages.h"
#include "types.h"
#include "wsizes.h"

#include "TreeWgt.h"

#define BASE_ITEM_FLAGS ( Qt::ItemIsEnabled | Qt::ItemIsSelectable \
	| Qt::ItemIsEditable )

#if TREE_WGT_DEBUG
#define CHECK_ITEM( item ) assert( item )
#else
#define CHECK_ITEM( item ) do { \
	if ( NULL == ( item ) ) { \
		throw TreeWgt::BadSelectedItem(); \
	}	\
} while(0);
#endif

TreeWgt::TreeWgt( QWidget *parent ) : QWidget( parent ),
	m_clippedNode( 0 ), m_tree( 0 )
{
	// создаем окно описания узла
	m_descrWgt = new DescrWgt( this );
	m_descrWgt->resize( DESCR_WIDTH, DESCR_HEIGHT );
	m_descrWgt->setFixedSize( m_descrWgt->size() );
	m_descrWgt->setModal( true );
	connect( m_descrWgt, SIGNAL( changed() ), this,
		SLOT( descriptionChanged() ) );
	
	// создаем диалог создания нового ключа
	m_newKeyWgt = new NewKeyWgt( this );
	m_newKeyWgt->resize( NK_WIDTH, NK_HEIGHT );
	m_newKeyWgt->setFixedSize( m_newKeyWgt->size() );
	m_newKeyWgt->setModal( true );
	
	// готовим виджет дерева
	m_treeWgt = new QTreeWidget();
	m_treeWgt->setAutoScroll( false );
	m_treeWgt->setHeaderLabel( "" );
	m_treeWgt->setSortingEnabled( false );
	
	m_treeWgt->header()->setHorizontalScrollMode(
		QAbstractItemView::ScrollPerPixel );
	m_treeWgt->header()->setResizeMode( QHeaderView::Interactive );
	
	connect( m_treeWgt, SIGNAL( itemChanged( QTreeWidgetItem *, int ) ),
		SLOT( itemChanged( QTreeWidgetItem *, int ) ) );
	
	connect( m_treeWgt, SIGNAL( itemSelectionChanged() ),
		SLOT( itemSelectionChanged() ) );
	
	connect( m_treeWgt, SIGNAL( itemCollapsed( QTreeWidgetItem * ) ),
		SLOT( itemCollapsed( QTreeWidgetItem * ) ) );
	
	connect( m_treeWgt, SIGNAL( itemExpanded( QTreeWidgetItem * ) ),
		SLOT( itemExpanded( QTreeWidgetItem * ) ) );
	
	m_treeWgt->installEventFilter( this );
	
	// == готовим кнопки управления ==
	QBoxLayout *hrsl = new QBoxLayout( QBoxLayout::LeftToRight );
	
	// кнопка добавления папки
	QPushButton *addFolderBtn = new QPushButton( QIcon( ADD_FOLDER_ICON ), "" );
	addFolderBtn->setToolTip( "Add new folder" );
	connect( addFolderBtn, SIGNAL( clicked() ), SLOT( addFolder() ) );
	hrsl->addWidget( addFolderBtn );
	hrsl->addSpacing( DEF_BTN_ALIGN );
	
	// кнопка добавления ключа
	QPushButton *addKeyBtn = new QPushButton( QIcon( ADD_ICON ), "" );
	addKeyBtn->setToolTip( "Add new key" );
	connect( addKeyBtn, SIGNAL( clicked() ), SLOT( addKey() ) );
	hrsl->addWidget( addKeyBtn );
	hrsl->addSpacing( DEF_BTN_ALIGN );
	
	// кнопка удаления item'a
	QPushButton *removeBtn = new QPushButton( QIcon( REMOVE_ICON ), "" );
	removeBtn->setToolTip( "Remove item" );
	connect( removeBtn, SIGNAL( clicked() ), SLOT( removeItem() ) );
	hrsl->addWidget( removeBtn );
	hrsl->addSpacing( DEF_BTN_ALIGN );
	
	// кнопка вырезания item'a
	QPushButton *cutBtn = new QPushButton( QIcon( CUT_ICON ), "" );
	cutBtn->setToolTip( "Cut item" );
	connect( cutBtn, SIGNAL( clicked() ), SLOT( cutItem() ) );
	hrsl->addWidget( cutBtn );
	hrsl->addSpacing( DEF_BTN_ALIGN );
	
	// кнопка вставки item'a
	QPushButton *pasteBtn = new QPushButton( QIcon( PASTE_ICON ), "" );
	pasteBtn->setToolTip( "Paste item" );
	connect( pasteBtn, SIGNAL( clicked() ), SLOT( pasteItem() ) );
	hrsl->addWidget( pasteBtn );
	
	hrsl->addStretch();
	// ----
	
	// создаем контекстного меню
	m_ctxMenu = new QMenu();
	m_ctxMenu->addAction( "Copy password" )->setData(
		TreeWgtItem::CopyPassword );
	
	m_ctxMenu->addAction( "Set password..." )->setData(
		TreeWgtItem::SetPassword );
	
	m_ctxMenu->addAction( "Show Info" )->setData( TreeWgtItem::ShowInfo );
	
	connect( m_ctxMenu, SIGNAL( triggered( QAction * ) ),
		SLOT( triggered( QAction * ) ) );
	
	// создаем вертикальную компановку
	QBoxLayout *vrtl = new QBoxLayout( QBoxLayout::TopToBottom );
	vrtl->addWidget( m_treeWgt );
	vrtl->addLayout( hrsl );
	setLayout( vrtl );
}

void TreeWgt::acceptKey()
{
	TreeWgtItem *item = getSelectedItem();
	CHECK_ITEM( item );
	
	// получение последнего созданного ключа
	KeyEntity *key = m_newKeyWgt->lastEntity();
	
	// обновление табличного представления
	emit openEntity( key );
	
	QString keyText = key->getf( "key" );
	
	// настройка узлов
	TreeNode *newNode = new TreeNode();
	newNode->setEntity( key );
	TreeNode *node = item->node();
	QString title = m_newKeyWgt->title();
	node->setChild( title.length() ? title : keyText, newNode );
	
	// создание нового графического узла дерева
	TreeWgtItem *newItem = new TreeWgtItem( item );
	newItem->setNode( newNode );
	newItem->setText( 0, newNode->name() );
	newItem->setIcon( 0, QIcon( KEY_ICON ) );
	newItem->setFlags( BASE_ITEM_FLAGS );
	
	update( item );
	
	m_treeWgt->setItemExpanded( item, true );
	m_treeWgt->resizeColumnToContents( 0 );
	
	emit changed();
}

void TreeWgt::exportKeys()
{
	DEF_TITLE( "Export keys" );
	
	if ( NULL == m_tree )
	{
		QMessageBox::warning( 0, GET_TITLE(), TREE_ISNT_LOADED );
		return;
	}
	
	QString output;
	std::queue< TreeNode* > stack;
	stack.push( m_tree );
	while ( !stack.empty() )
	{
		TreeNode *node = stack.front();
		stack.pop();
		KeyEntity *ent = node->entity();
		if ( ent != NULL )
		{
			QString loginStr = ent->getf( "login" );
			if ( loginStr.length() > 0 ) {
				loginStr = "Login: [" + loginStr + "]\n";
			}
			output += QString("Path: [%1]\nTitle: [%2]"
				"\n%3Password: [%4]\n" )
					.arg( node->path() )
					.arg( node->name() )
					.arg( loginStr )
					.arg( ent->getf( "password" ) );
			auto fieldIter = ent->cbegin();
			for ( ; fieldIter != ent->cend(); ++fieldIter )
			{
				QString keyName = fieldIter->first;
				output += QString("%1: [%2]\n")
					.arg( keyName ).arg( ent->getf( keyName ) );
			}
			output += "========\n";
		}
		
		// добавление детей в стек
		auto childIter = node->cbegin();
		for ( ; childIter != node->cend(); ++childIter )
		{
			stack.push( node->getNode( childIter->first ) );
		}
	}
	
	SecClipboard::Instance().pasteText( output );
}

bool TreeWgt::eventFilter( QObject *, QEvent *event )
{
	if ( event->type() == QEvent::ContextMenu )
	{
		QContextMenuEvent *ctxEvent = static_cast<
			QContextMenuEvent* >( event );
		TreeWgtItem *selItem = getSelectedItem();
		if ( selItem != NULL )
		{
			TreeNode *node = selItem->node();
			if ( node->entity() != NULL )
			{
				m_ctxMenu->exec( ctxEvent->globalPos() );
				return true;
			}
		}
	}
	return false;
}

TreeWgtItem *TreeWgt::getSelectedItem()
{
	QList< QTreeWidgetItem * > list = m_treeWgt->selectedItems();
	if ( !list.empty() )
	{
		return dynamic_cast< TreeWgtItem * >( list.at( 0 ) );
	}
	else {
		return NULL;
	}
}

QString TreeWgt::getPath( const QTreeWidgetItem *item )
{
	QString result = "";
	QTreeWidgetItem *parent = item->parent();
	if ( parent != NULL )
	{
		result = getPath( parent );
		if ( result.length() > 0 ) {
			result += ".";
		}
		result += item->text( 0 );
	}
	return result;
}

bool TreeWgt::isSelected() const
{
	return !m_treeWgt->selectedItems().empty();
}

void TreeWgt::openDescrWindow( bool forChange )
{
	if ( !isSelected() )
	{
		return;
	}
	
	TreeWgtItem *item = getSelectedItem();
	CHECK_ITEM( item );
	m_descrWgt->setReadOnly( !forChange );
	m_descrWgt->setNode( item->node() );
	m_descrWgt->show();
}

void TreeWgt::update()
{
	m_treeWgt->clear();
	if ( m_tree != NULL )
	{
		// создаем root-элемент
		TreeWgtItem *rootItem = new TreeWgtItem( m_treeWgt );
		rootItem->setNode( m_tree );
		rootItem->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
		rootItem->setText( 0, "/" );
		rootItem->setIcon( 0, QIcon( FOLDER_ICON ) );
		
		update( rootItem );
		m_treeWgt->setItemExpanded( rootItem, true );
	}
	m_treeWgt->resizeColumnToContents( 0 );
}

void TreeWgt::sortColumn( TreeWgtItem *item )
{
	item->sortChildren( 0, Qt::AscendingOrder );
}

void TreeWgt::update( TreeWgtItem *item )
{
	item->takeChildren();
	
	std::vector< QString > dirs, lists;
	
	// собираем имена всех узлов дерева
	TreeNode *node = item->node();
	auto nodeIter = node->cbegin();
	for ( ; nodeIter != node->cend(); ++nodeIter )
	{
		QString name = nodeIter->first;
		TreeNode *subNode = node->getNode( name );
		if ( subNode->entity() == NULL )
		{
			dirs.push_back( name );
		}
		else {
			lists.push_back( name );
		}
	}
	
	// объединяем полученные результаты
	int nodesCount = dirs.size() + lists.size(); 
	std::vector< QString > nodeNames( nodesCount );
	nodeNames.assign( dirs.begin(), dirs.end() );
	std::copy( lists.begin(), lists.end(),
		std::back_inserter( nodeNames ) );
	
	for ( int nodeIndex = 0; nodeIndex < nodesCount; nodeIndex++ )
	{
		QString name = nodeNames[ nodeIndex ];
		TreeNode *subNode = node->getNode( name );
		
		// создаем новый графический узел дерева
		TreeWgtItem *subItem = new TreeWgtItem( item );
		subItem->setNode( subNode );
		subItem->setFlags( BASE_ITEM_FLAGS );
		if ( subNode->descr().length() > 0 )
		{
			QFont curFont = subItem->font( 0 );
			curFont.setBold( true );
			subItem->setFont( 0, curFont );
		}
		subItem->setText( 0, name );
		item->addChild( subItem );
		
		KeyEntity *ent = subNode->entity();
		if ( ent != NULL )
		{
			// добавляем ключ
			subItem->setIcon( 0, QIcon( KEY_ICON ) );
		}
		else
		{
			// оказалась директория
			subItem->setIcon( 0, QIcon( FOLDER_ICON ) );
			update( subItem );
		}
	}
}


// ======== slots ======== 
void TreeWgt::addFolder()
{
	DEF_TITLE( "Add Folder" );
	
	if ( NULL == m_tree )
	{
		QMessageBox::warning( 0, GET_TITLE(), TREE_ISNT_LOADED );
		return;
	}
	
	if ( !isSelected() )
	{
		return;
	}
	
	// извлекаем необходимые поля
	TreeWgtItem *item = getSelectedItem();
	CHECK_ITEM( item );
	TreeNode *node = item->node();
	if ( node->entity() != NULL )
	{
		QMessageBox::warning( 0, GET_TITLE(), "It's a key!" );
		return;
	}
	
	// просим выбрать имя новой директории 
	TreeNode *newNode = new TreeNode();
	QString name;
	bool finded = false;
	bool okClicked = true;
	while ( !finded && okClicked )
	{
		name = QInputDialog::getText( 0, "Input", "Dir name:",
			QLineEdit::Normal, "", &okClicked );
		finded = node->getNode( name ) == NULL;
		if ( !finded )
		{
			QMessageBox::warning( 0, GET_TITLE(),
				"The node with this name already exists!" );
		}
	}
	if ( !okClicked ) {
		return;
	}
	
	if ( name.length() == 0 )
	// ищем подходящее уникальное имя по умолчанию
	{
		name = DEF_FOLDER_NAME;
		finded = node->getNode( name ) == NULL;
		int dirIndex = 0;
		while ( !finded && ++dirIndex )
		{
			name = QString( DEF_FOLDER_NAME ) + " " +
				QString::number( dirIndex );
			finded = node->getNode( name ) == NULL;
		}
	}
	node->setChild( name, newNode );
	
	// создаем новый графический узел дерева
	TreeWgtItem *newItem = new TreeWgtItem( item );
	newItem->setNode( newNode );
	newItem->setText( 0, name );
	newItem->setIcon( 0, QIcon( FOLDER_ICON ) );
	newItem->setFlags( BASE_ITEM_FLAGS );
	
	update( item );
	
	// частично обновляем дерево
	m_treeWgt->setItemExpanded( item, true );
	m_treeWgt->resizeColumnToContents( 0 );
	
	emit changed();
}

void TreeWgt::addKey()
{
	DEF_TITLE( "Add Key" );
	
	if ( NULL == m_tree )
	{
		QMessageBox::warning( 0, GET_TITLE(), TREE_ISNT_LOADED );
		return;
	}
	
	if ( !isSelected() )
	{
		return;
	}
	
	TreeWgtItem *item = getSelectedItem();
	if ( item != NULL )
	{
		if ( item->node()->entity() == NULL )
		{
			m_newKeyWgt->clear();
			m_newKeyWgt->show();
		}
		else
		{
			QMessageBox::warning( 0, GET_TITLE(),
				"You can add keys only to folders" );
		}
	}
}

void TreeWgt::cutItem()
{
	DEF_TITLE( "Cut item" );
	
	if ( NULL == m_tree )
	{
		QMessageBox::warning( 0, GET_TITLE(), TREE_ISNT_LOADED );
		return;
	}
	
	if ( !isSelected() )
	{
		return;
	}
	
	// извлекаем необходимые поля
	TreeWgtItem *item = getSelectedItem();
	CHECK_ITEM( item );
	TreeNode *curNode = item->node();
	TreeNode *parent = curNode->parent();
	
	if ( NULL == parent )
	// проверка на root
	{
		QMessageBox::warning( 0, GET_TITLE(), "Can't cut root element" );
		return;
	}
	
	// вырезаем узел
	parent->cutNode( curNode->name() );
	m_clippedNode = curNode;
	
	item->parent()->removeChild( item );
	emit changed();
}

void TreeWgt::descriptionChanged()
{
	TreeWgtItem *item = getSelectedItem();
	CHECK_ITEM( item );
	QFont curFont = item->font( 0 );
	bool boldNeeded = item->node()->descr().length() > 0;
	curFont.setBold( boldNeeded );
	item->setFont( 0, curFont );
	emit changed();
}

void TreeWgt::itemChanged( QTreeWidgetItem *qtwItem, int )
{
	TreeWgtItem *item = static_cast< TreeWgtItem * >( qtwItem );
	TreeNode *node = item->node();
	QString newName = qtwItem->text( 0 );
	if ( newName.length() > 0 )
	{
		if ( node->setName( newName ) )
		{
			TreeWgtItem *parent = item->parent();
			if ( parent != NULL )
			{
				update( parent );
			}
			emit changed();
		}
		else {
			// восстанавливаем старое значение
			item->setText( 0, node->name() );
		}
	}
	else {
		// восстанавливаем старое значение
		item->setText( 0, node->name() );
	}
	m_treeWgt->resizeColumnToContents( 0 );
}

void TreeWgt::itemCollapsed( QTreeWidgetItem * )
{
	m_treeWgt->resizeColumnToContents( 0 );
}

void TreeWgt::itemExpanded( QTreeWidgetItem * )
{
	m_treeWgt->resizeColumnToContents( 0 );
}

void TreeWgt::itemSelectionChanged()
{
	TreeWgtItem *item = getSelectedItem();
	if ( NULL == item )
	{
		return;
	}
	
	KeyEntity *ent = item->node()->entity();
	emit openEntity( ent );
	
	QRect rect = m_treeWgt->visualItemRect( item );
	QModelIndex modelIndex = m_treeWgt->indexAt( QPoint(
		rect.x(), rect.y() ) );
	m_treeWgt->scrollTo( modelIndex );
}

void TreeWgt::pasteItem()
{
	DEF_TITLE( "Paste item" );
	
	if ( NULL == m_tree )
	{
		QMessageBox::warning( 0, GET_TITLE(), TREE_ISNT_LOADED );
		return;
	}
	
	if ( !isSelected() )
	{
		return;
	}
	
	if ( NULL == m_clippedNode )
	{
		QMessageBox::warning( 0, GET_TITLE(), "Nodes buffer is empty" );
		return;
	}
	
	TreeWgtItem *item = getSelectedItem();
	CHECK_ITEM( item );
	TreeNode *curNode = item->node();
	if ( curNode->entity() != NULL )
	{
		QMessageBox::warning( 0, GET_TITLE(),
			"Can't paste an item to a key" );
		return;
	}
	
	curNode->setChild( m_clippedNode->name(), m_clippedNode );
	m_clippedNode->setParent( curNode );
	m_clippedNode = NULL;
	
	update( item );
	
	emit changed();
}

void TreeWgt::removeItem()
{
	DEF_TITLE( "Remove Item" );
	
	if ( NULL == m_tree )
	{
		QMessageBox::warning( 0, GET_TITLE(), TREE_ISNT_LOADED );
		return;
	}
	
	if ( !isSelected() )
	{
		return;
	}
	
	TreeWgtItem *item = getSelectedItem();
	if ( item != NULL )
	{
		if ( item->flags() & Qt::ItemIsEditable )
		// проверяем что это не корневой узел - его удалять нельзя
		{
			TreeNode *node = item->node();
			node->parent()->removeNode( node->name() );
			delete item;
			emit changed();
		}
		else
		{
			QMessageBox::warning( 0, GET_TITLE(),
				"Can't remove this node" );
		}
	}
}

void TreeWgt::setDescr()
{
	DEF_TITLE( "Set description" );
	
	if ( NULL == m_tree )
	{
		QMessageBox::warning( 0, GET_TITLE(), TREE_ISNT_LOADED );
	}
	else {
		openDescrWindow( true );
	}
}

void TreeWgt::showDescr()
{
	DEF_TITLE( "Show description" );
	
	if ( NULL == m_tree )
	{
		QMessageBox::warning( 0, GET_TITLE(), TREE_ISNT_LOADED );
	}
	else {
		openDescrWindow( false );
	}
}

void TreeWgt::triggered( QAction *action )
{
	TreeWgtItem *item = getSelectedItem();
	if ( NULL == item )
	{
		return;
	}
	
	KeyEntity *ent = item->node()->entity();
	if ( NULL == ent )
	{
		return;
	}
	
	switch ( action->data().toInt() )
	{
		case TreeWgtItem::CopyPassword:
		{
			SecClipboard::Instance().pasteText( ent->getf( "password" ) );
		}
		break;
		
		case TreeWgtItem::SetPassword:
		{
			const QString title = "Set Password";
			bool okClicked;
			
			QString oldPass = ent->getf( "password" );
			QString newPass = QInputDialog::getText( 0,
				title,
				"New password:",
				QLineEdit::Password,
				"",
				&okClicked );
			
			if ( okClicked && newPass != oldPass )
			{
				if ( newPass.length() > 0 )
				{
					ent->setf( "password", newPass );
					emit changed();
				}
				else
				{
					QMessageBox::warning( 0, title,
						"Password cannot be empty!" );
				}
			}
			else
			{
				QMessageBox::information( 0, title,
					"Password is not changed" );
			}
		}
		break;
		
		case TreeWgtItem::ShowInfo:
		{
			QMessageBox::information( 0, "key info",
				"This function is deprecated." );
		}
		break;
	}
}
