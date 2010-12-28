/*

   Copyright [2008] [Trevor Hogan]

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   CODE PORTED FROM THE ORIGINAL GHOST PROJECT: http://ghost.pwner.org/

*/

#ifndef GHOST_H
#define GHOST_H

#include "includes.h"

//
// CGHost
//

class CUDPSocket;
class CTCPServer;
class CTCPSocket;
class CGPSProtocol;
class CCRC32;
class CSHA1;
class CBNET;
class CBaseGame;
class CAdminGame;
class CGHostDB;
class CBaseCallable;
class CLanguage;
class CMap;
class CSaveGame;
class CConfig;
class CCallableCountrySkipList;
class CCallableVouchList;

typedef pair<string, string> VouchPair;

class CGHost
{
public:
	CUDPSocket *m_UDPSocket;				// a UDP socket for sending broadcasts and other junk (used with !sendlan)
	CTCPServer *m_ReconnectSocket;			// listening socket for GProxy++ reliable reconnects
	vector<CTCPSocket *> m_ReconnectSockets;// vector of sockets attempting to reconnect (connected but not identified yet)
	CGPSProtocol *m_GPSProtocol;
	CCRC32 *m_CRC;							// for calculating CRC's
	CSHA1 *m_SHA;							// for calculating SHA1's
	vector<CBNET *> m_BNETs;				// all our battle.net connections (there can be more than one)
	CBaseGame *m_CurrentGame;				// this game is still in the lobby state
	CAdminGame *m_AdminGame;				// this "fake game" allows an admin who knows the password to control the bot from the local network
	vector<CBaseGame *> m_Games;			// these games are in progress
	CGHostDB *m_DB;							// database
	CGHostDB *m_DBLocal;					// local database (for temporary data)
	vector<CBaseCallable *> m_Callables;	// vector of orphaned callables waiting to die
	vector<BYTEARRAY> m_LocalAddresses;		// vector of local IP addresses
	CLanguage *m_Language;					// language
	CMap *m_Map;							// the currently loaded map
	CMap *m_AdminMap;						// the map to use in the admin game
	CMap *m_AutoHostMap;					// the map to use when autohosting
	CSaveGame *m_SaveGame;					// the save game to use
	vector<PIDPlayer> m_EnforcePlayers;		// vector of pids to force players to use in the next game (used with saved games)
	bool m_Exiting;							// set to true to force ghost to shutdown next update (used by SignalCatcher)
	bool m_ExitingNice;						// set to true to force ghost to disconnect from all battle.net connections and wait for all games to finish before shutting down
	bool m_Enabled;							// set to false to prevent new games from being created
	string m_Version;						// GHost++ version string
	uint32_t m_HostCounter;					// the current host counter (a unique number to identify a game, incremented each time a game is created)
	string m_AutoHostGameName;				// the base game name to auto host with
	string m_AutoHostOwner;
	string m_AutoHostServer;
	uint32_t m_AutoHostMaximumGames;		// maximum number of games to auto host
	uint32_t m_AutoHostAutoStartPlayers;	// when using auto hosting auto start the game when this many players have joined
	uint32_t m_LastAutoHostTime;			// GetTime when the last auto host was attempted
	bool m_AutoHostMatchMaking;
	double m_AutoHostMinimumScore;
	double m_AutoHostMaximumScore;
	bool m_AllGamesFinished;				// if all games finished (used when exiting nicely)
	uint32_t m_AllGamesFinishedTime;		// GetTime when all games finished (used when exiting nicely)
	string m_LanguageFile;					// config value: language file
	string m_Warcraft3Path;					// config value: Warcraft 3 path
	bool m_TFT;								// config value: TFT enabled or not
	string m_BindAddress;					// config value: the address to host games on
	uint16_t m_HostPort;					// config value: the port to host games on
	bool m_Reconnect;						// config value: GProxy++ reliable reconnects enabled or not
	uint16_t m_ReconnectPort;				// config value: the port to listen for GProxy++ reliable reconnects on
	uint32_t m_ReconnectWaitTime;			// config value: the maximum number of minutes to wait for a GProxy++ reliable reconnect
	uint32_t m_MaxGames;					// config value: maximum number of games in progress
	char m_CommandTrigger;					// config value: the command trigger inside games
	string m_MapCFGPath;					// config value: map cfg path
	string m_SaveGamePath;					// config value: savegame path
	string m_MapPath;						// config value: map path
	bool m_SaveReplays;						// config value: save replays
	string m_ReplayPath;					// config value: replay path
	string m_VirtualHostName;				// config value: virtual host name
	bool m_HideIPAddresses;					// config value: hide IP addresses from players
	bool m_CheckMultipleIPUsage;			// config value: check for multiple IP address usage
	uint32_t m_SpoofChecks;					// config value: do automatic spoof checks or not
	bool m_RequireSpoofChecks;				// config value: require spoof checks or not
	bool m_ReserveAdmins;					// config value: consider admins to be reserved players or not
	bool m_RefreshMessages;					// config value: display refresh messages or not (by default)
	bool m_AutoLock;						// config value: auto lock games when the owner is present
	bool m_AutoSave;						// config value: auto save before someone disconnects
	uint32_t m_AllowDownloads;				// config value: allow map downloads or not
	bool m_PingDuringDownloads;				// config value: ping during map downloads or not
	uint32_t m_MaxDownloaders;				// config value: maximum number of map downloaders at the same time
	uint32_t m_MaxDownloadSpeed;			// config value: maximum total map download speed in KB/sec
	bool m_LCPings;							// config value: use LC style pings (divide actual pings by two)
	uint32_t m_AutoKickPing;				// config value: auto kick players with ping higher than this
	uint32_t m_BanMethod;					// config value: ban method (ban by name/ip/both)
	string m_IPBlackListFile;				// config value: IP blacklist file (ipblacklist.txt)
	uint32_t m_LobbyTimeLimit;				// config value: auto close the game lobby after this many minutes without any reserved players
	uint32_t m_Latency;						// config value: the latency (by default)
	uint32_t m_SyncLimit;					// config value: the maximum number of packets a player can fall out of sync before starting the lag screen (by default)
	bool m_VoteKickAllowed;					// config value: if votekicks are allowed or not
	uint32_t m_VoteKickPercentage;			// config value: percentage of players required to vote yes for a votekick to pass
	string m_DefaultMap;					// config value: default map (map.cfg)
	string m_MOTDFile;						// config value: motd.txt
	string m_GameLoadedFile;				// config value: gameloaded.txt
	string m_GameOverFile;					// config value: gameover.txt
	bool m_LocalAdminMessages;				// config value: send local admin messages or not
	bool m_AdminGameCreate;					// config value: create the admin game or not
	uint16_t m_AdminGamePort;				// config value: the port to host the admin game on
	string m_AdminGamePassword;				// config value: the admin game password
	string m_AdminGameMap;					// config value: the admin game map config to use
	unsigned char m_LANWar3Version;			// config value: LAN warcraft 3 version
	uint32_t m_ReplayWar3Version;			// config value: replay warcraft 3 version (for saving replays)
	uint32_t m_ReplayBuildNumber;			// config value: replay build number (for saving replays)
	bool m_TCPNoDelay;						// config value: use Nagle's algorithm or not
	uint32_t m_MatchMakingMethod;			// config value: the matchmaking method
	
	
	/*
		NordicLeague - @begin - define our config variables
	*/
	string 		m_ApprovedCountries;			// custom value: approved countries 
	bool 		m_DisplayAdminInGameMessage;	// config value: Display a message that admins are present in the game when game is loaded.
	string 		m_DisplayAdminInGameFile;		// config value: adminingame.txt
	bool 		m_SendDownloadLink;				// config value: send a download link if map downloads are disabled
	string 		m_MapDownloadLink;				// config value: the download link to send
	string 		m_RegisterEmailRegEx;
	bool		m_AdminCanAlwaysJoin;			// config value: should we kick a non-reserved player to make room for admins?
	bool		m_EnforceBalance;				// config value: force balance before gamestart?
	bool		m_VoteEndAllowed;				// config value: do we allow !voteend ?
	uint32_t 	m_VoteEndPercentage;			// config value: percentage of players required to vote yes for a voteend to pass
	uint32_t 	m_GamesReq;						// minimum amount of games required to be able to join safe games
	uint32_t 	m_StayReq;						// minimum amount of stay % to be able to join
	bool 		m_SafeGames;					// are we hosting safe games?
	bool 		m_EnableFF;
	bool 		m_Debug;
	bool		m_LinkEnabled;
	
	set<string> 				m_BypassEnforcer;
	set<VouchPair> 				m_VouchPairs;

	CCallableCountrySkipList 	*m_UpdateSkipList;
	void 						LoadEnforcerSkiplist();
	CCallableVouchList			*m_UpdateVouchList;
	uint32_t					m_NextListUpdateTime;	// keep track of perodical updates for country skiplist/vouched player list.
	
	CTCPServer 					*m_BroadcastListener; 	// listening socket for connecting broadcasters
	vector<CTCPSocket *> 		m_Broadcaster;			// vector of connected broadcasters
	uint16_t					m_BroadcastPort;		// port to listen on, set by config bot_broadcasterport
	
	bool						m_AutoCloseAfterLeave;
	uint32_t					m_AutoCloseTime;
	/*
		NordicLeague - @end - define our config variables
	*/
	

	CGHost( CConfig *CFG );
	~CGHost( );

	// processing functions

	bool Update( long usecBlock );

	// events

	void EventBNETConnecting( CBNET *bnet );
	void EventBNETConnected( CBNET *bnet );
	void EventBNETDisconnected( CBNET *bnet );
	void EventBNETLoggedIn( CBNET *bnet );
	void EventBNETGameRefreshed( CBNET *bnet );
	void EventBNETGameRefreshFailed( CBNET *bnet );
	void EventBNETConnectTimedOut( CBNET *bnet );
	void EventBNETWhisper( CBNET *bnet, string user, string message );
	void EventBNETChat( CBNET *bnet, string user, string message );
	void EventBNETEmote( CBNET *bnet, string user, string message );
	void EventGameDeleted( CBaseGame *game );

	// other functions

	void ReloadConfigs( );
	void ReloadMap( );
	void SetConfigs( CConfig *CFG );
	void ExtractScripts( );
	void LoadIPToCountryData( );
	void CreateGame( CMap *map, unsigned char gameState, bool saveGame, string gameName, string ownerName, string creatorName, string creatorServer, bool whisper );
};

#endif
