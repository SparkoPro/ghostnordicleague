#include <dlfcn.h>
#include "ghost.h"
#include "util.h"
#include "game_base.h"
#include "pluginmgr.h"
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

using namespace boost :: filesystem; 
typedef void* (*arbitrary)();

CPlugin :: CPlugin(string nName, void *nHandle)
{
	CONSOLE_Print("[PLUGIN] Loaded plugin [" + nName + "]" );
	m_Name = nName;
	m_Handle = nHandle;
}

CPlugin :: ~CPlugin()
{
	dlclose(m_Handle);
	CONSOLE_Print("[PLUGIN] Unloading [" + m_Name + "]" );
}

void CPlugin :: Execute(string function)
{
	if (!m_Handle)
	{
		CONSOLE_Print("[PLUGIN: " + m_Name + "] Execute aborted, called [" + function + "] but we have no handle!" );
		return;
	}

	char *error;	
	typedef void (*function_t)();
	function_t PluginFunction = (function_t) dlsym(m_Handle, function.c_str());

	if ((error = dlerror()) != NULL)
	{
		CONSOLE_Print("[PLUGIN: " + m_Name + "] Can not finding symbol for [" + function + "]" );
		dlclose(m_Handle);
		return;
	}

	(*PluginFunction)();
}

CPluginMgr :: CPluginMgr()
{
	ReloadPlugins( );
}

CPluginMgr :: ~CPluginMgr()
{
	UnloadPlugins( );
}

void CPluginMgr :: ReloadPlugins( )
{
	if (!m_LoadedPlugins.empty())
		UnloadPlugins();
		
	CONSOLE_Print("[PLUGINMGR] Searching for plugins...");
		
	FindPlugins(path("./plugins"), true);
	
	CONSOLE_Print("[PLUGINMGR] Loaded " + UTIL_ToString( m_LoadedPlugins.size() ) + " plugins." );
}

void CPluginMgr :: FindPlugins( const path & directory, bool recurse_into_subdirs )
{
	if( exists( directory ) )
	{
		directory_iterator end;
		for( directory_iterator iter(directory) ; iter != end ; ++iter )
		{
			if ( is_directory( *iter ) )
			{
				if ( recurse_into_subdirs )
					FindPlugins(*iter);
			}
			else
			{
				CONSOLE_Print(iter->leaf());
				boost::regex expression("^.*(so)$");
				if (boost::regex_match(iter->leaf(),expression))
					LoadPlugin( iter->string() );
			}
		}
	}
}

void CPluginMgr :: LoadPlugin( string filename )
{
	char *error;
	void* handle = dlopen(filename.c_str(), RTLD_LAZY);

	CONSOLE_Print("[PLUGINMGR] Trying to load: " + filename);

	if (!handle)
	{
		CONSOLE_Print( "[PLUGINMGR] Error loading plugin: " + filename );
		return;
	}

	typedef string (*getname_t)();
	getname_t GetName = (getname_t) dlsym(handle, "GetName");

	if ((error = dlerror()) != NULL)
	{
		CONSOLE_Print("[PLUGINMGR] Error finding symbol [GetName]" );
		dlclose(handle);
		return;
	}

	string PluginName = (*GetName)();

	m_LoadedPlugins.push_back(new CPlugin(PluginName, handle));
}

void CPluginMgr :: UnloadPlugins( )
{
	CONSOLE_Print("[PLUGINMGR] Unloading " + UTIL_ToString( m_LoadedPlugins.size() ) + " plugins." );

	for( vector<CPlugin *> :: iterator i = m_LoadedPlugins.begin( ); i != m_LoadedPlugins.end( ); )
	{
		delete *i;
		i = m_LoadedPlugins.erase( i );
	}
}

void CPluginMgr :: Execute(string function)
{
	for( vector<CPlugin *> :: iterator i = m_LoadedPlugins.begin( ); i != m_LoadedPlugins.end( ); )
	{
		(*i)->Execute(function);
	}
}

//vector<CPlugin *> m_LoadedPlugins;
