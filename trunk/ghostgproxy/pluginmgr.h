#ifndef PLUGINMGR_H
#define PLUGINMGR_H

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>


class CGHost;
class CGame;
class CBaseGame;

using namespace boost::filesystem;
class CPlugin
{
private:
	void 	*m_Handle;
	string	m_Name;
	
public:
	CPlugin(string nName, void *nHandle);
	~CPlugin();
	
	void Execute(string function);
	string GetName() { return m_Name; }
};

class CPluginMgr
{
public:
	CPluginMgr();
	~CPluginMgr();
	
	void ReloadPlugins( );
	void FindPlugins( const path & directory, bool recurse_into_subdirs = false );
	void LoadPlugin( string filename );
	void UnloadPlugins( );
	
	void Execute(string function);

private:
	vector<CPlugin *> m_LoadedPlugins;
};

#endif
