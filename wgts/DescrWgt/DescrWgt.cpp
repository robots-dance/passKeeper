#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include "TreeNode.h"

#include "DescrWgt.h"

DescrWgt::DescrWgt( QWidget *parent ): QDialog( parent ),
	m_node( 0 ), m_readOnly( true ) 
{
	m_editor = new QTextEdit();
	m_editor->setReadOnly( true );
	
	QPushButton *btAccept = new QPushButton( "Accept" );
	connect( btAccept, SIGNAL( clicked() ), SLOT( accept() ) );
	QPushButton *btCancel = new QPushButton( "Cancel" );
	connect( btCancel, SIGNAL( clicked() ), SLOT( cancel() ) );
	
	QHBoxLayout *hrsl = new QHBoxLayout();
	hrsl->addWidget( btAccept );
	hrsl->addWidget( btCancel );
	
	QVBoxLayout *vrtl = new QVBoxLayout();
	vrtl->addWidget( m_editor );
	vrtl->addLayout( hrsl );
	setLayout( vrtl );
	
	setWindowTitle( "Edit node description" );
}

void DescrWgt::setNode( TreeNode *node )
{
	m_node = node;
	m_editor->setPlainText( m_node->descr() );
}

void DescrWgt::setReadOnly( bool readOnly )
{
	m_readOnly = readOnly;
	m_editor->setReadOnly( readOnly );
}


// ======== slots ========
void DescrWgt::accept()
{
	QString curText = m_editor->toPlainText();
	if ( curText != m_node->descr() )
	{
		m_node->setDescr( curText );
		emit changed();
	}
	close();
}

void DescrWgt::cancel()
{
	close();
}
