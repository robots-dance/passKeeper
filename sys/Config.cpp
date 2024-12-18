#include <QFile>
#include <QRegExp>
#include <QTextStream>

#include "Config.h"

const QString Config::kBadKeyRegex = "[\\W]+";

bool Config::checkKey( cst_str key ) const
{
	return key.indexOf( ( QRegExp( kBadKeyRegex ) ) ) < 0;
}

bool Config::isComment( const QChar &ch ) const
{
	return QChar( ';' ) == ch || QChar( '#' ) == ch;
}

Config::KeysList Config::getKeys() const
{
	KeysList keys;
	auto valIter = m_values.begin();
	for ( ; valIter != m_values.end(); ++valIter )
	{
		keys.insert( valIter->first );
	}
	return keys;
}

bool Config::load( cst_str path )
{
	QFile inpFile( path );
	if ( !inpFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
	{
		return false;
	}
	
	m_lines.clear();
	m_lines.reserve( kReserveSize );
	m_values.clear();
	QTextStream inpStream( &inpFile );
	while ( !inpStream.atEnd() )
	{
		QString line = inpStream.readLine();
		m_lines.push_back( line );
		line = line.trimmed();
		bool noEmpty = line.length() > 0;
		if ( noEmpty && !isComment( line[ 0 ] ) )
		{
			KeyVal pair = parseLine( line );
			if ( pair.first.length() > 0 )
			{
				m_values[ pair.first ] = ValueInfo( pair.second,
					m_lines.size() - 1 );
			}
		}
	}
	m_path = path;
	inpFile.close();
	
	return true;
}

QString Config::makeLine( cst_str key, cst_str value ) const
{
	return key + " = " + value;
}

KeyVal Config::parseLine( cst_str line ) const
{
	KeyVal result;
	int delimPos = line.indexOf( "=" );
	if ( delimPos > 0 )
	{
		QString key = line.left( delimPos ).trimmed();
		if ( checkKey( key ) )
		{
			result.first = key;
			result.second = line.right( line.length() -
				delimPos - 1 ).trimmed();
		}
	}
	return result;
}

bool Config::save()
{
	bool result = false;
	
	if ( !m_changed || m_path.length() == 0 )
	{
		return false;
	}
	
	QFile outFile( m_path );
	if ( outFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
	{
		QTextStream outStream( &outFile );
		
		auto lineIter = m_lines.begin();
		for ( ; lineIter != m_lines.end(); ++lineIter )
		{
			outStream << *lineIter << endl;
		}
		
		outFile.close();
		m_changed = false;
		result = true;
	}
	
	return result;
}

void Config::setValue( cst_str key, cst_str value )
{
	if ( checkKey( key ) )
	{
		bool existed = exists( key );
		if ( !existed || m_values[ key ].m_value != value )
		{
			m_changed = true;
		}
		if ( m_changed )
		{
			m_values[ key ].m_value = value;
			QString newLine = makeLine( key, value );
			if ( existed )
			{
				m_lines[ m_values[ key ].m_lineIndex ] = newLine; 
			}
			else
			{
				m_lines.push_back( newLine );
				m_values[ key ].m_lineIndex = m_lines.size() - 1;
			}
		}
	}
}
