#ifndef _DESCR_WIDGET_H_
#define _DESCR_WIDGET_H_

#include <QDialog>

class QTextEdit;

class TreeNode;


class DescrWgt: public QDialog
{
	Q_OBJECT

public:
	explicit DescrWgt( QWidget *parent = 0 );
	
	void setNode( TreeNode *node );
	
	void setReadOnly( bool readOnly );

signals:
	void changed();

private slots:
	void accept();
	void cancel();

private:
	QTextEdit *m_editor;
	TreeNode *m_node;
	bool m_readOnly;
};

#endif // _DESCR_WIDGET_H_

