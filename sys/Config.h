#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <QString>
#include <map>
#include <vector>
#include <set>
#include "types.h"

class Config
{
public:
	typedef std::set< QString > KeysList;
	
	Config(): m_changed( false )
	{}
	
	// open/close methods
	bool load( cst_str path );
	
	bool save();
	
	void setNewPath( cst_str path )
	{
		if ( m_path.length() > 0 )
		{
			m_path = path;
		}
	}
	
	// check common values
	static bool isTrue( cst_str val )
	{
		QString lowerVal = val.toLower();
		return "true" == lowerVal || "1" == lowerVal;
	}
	
	// data manipulate methods
	bool exists( cst_str key ) const
	{
		return m_values.find( key ) != m_values.end();
	}
	
	void setValue( cst_str key, cst_str value );
	
	KeysList getKeys() const;
	
	QString getValue( cst_str key ) const
	{
		return exists( key ) ? m_values[ key ].m_value : "";
	}

private:
	// == types == 
	typedef std::vector< QString > LinesList;
	
	struct ValueInfo
	{
		ValueInfo() {}
		
		ValueInfo( cst_str value, int lineIndex ):
			m_value( value ), m_lineIndex( lineIndex )
		{
			;
		}
		
		QString m_value;
		int m_lineIndex;
	};
	
	// == methods ==
	
	bool checkKey( cst_str key ) const;
	// return true if the key is correct
	
	bool isComment( const QChar &ch ) const;
	// return true if the ch symbol is a symbol comment
	
	KeyVal parseLine( cst_str line ) const;
	
	QString makeLine( cst_str key, cst_str value ) const;
	
	
	// == members ==
	static const QString kBadKeyRegex;
	static const int kReserveSize = 100;
	
	bool m_changed;
	LinesList m_lines;
	QString m_path;
	mutable std::map< QString, ValueInfo > m_values;
};

#endif // _CONFIG_H_

