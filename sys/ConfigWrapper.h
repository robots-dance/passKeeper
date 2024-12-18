#ifndef _CONFIG_WRAPPER_H_
#define _CONFIG_WRAPPER_H_

#include "Config.h"

class ConfigWrapper: public Config
{

public:
	static ConfigWrapper &Instance()
	{
		static ConfigWrapper obj;
		return obj;
	}

private:
	ConfigWrapper(): Config() {}
	
	ConfigWrapper( const ConfigWrapper & ) {}
	
	const ConfigWrapper &operator=( ConfigWrapper & ) {
		return *this;
	}
};

#endif // _CONFIG_WRAPPER_H_

