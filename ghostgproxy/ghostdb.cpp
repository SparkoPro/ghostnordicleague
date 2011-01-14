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

#include "ghost.h"
#include "util.h"
#include "config.h"
#include "ghostdb.h"
#include "packed.h"
#include "replay.h"
#include "gameprotocol.h"

//
// CGHostDB
//

CGHostDB :: CGHostDB( CConfig *CFG )
{
	m_HasError = false;
}

CGHostDB :: ~CGHostDB( )
{

}

void CGHostDB :: RecoverCallable( CBaseCallable *callable )
{

}

bool CGHostDB :: Begin( )
{
	return true;
}

bool CGHostDB :: Commit( )
{
	return true;
}

//
// nordicleague
//

uint32_t CGHostDB :: RegisterPlayerAdd( string name, string email, string ip )
{
	return 0;
}

uint32_t CGHostDB :: DotAEventAdd( uint32_t gameid, string gamename, string killer, string victim, uint32_t kcolour, uint32_t vcolour  )
{
	return 0;
}

bool CGHostDB :: UpdateGameInfo( string name, uint32_t players, bool ispublic, vector<string> m_Slots )
{
	return false;
}

bool CGHostDB :: SaveReplay( CReplay *replay )
{
	return false;
}

CCallableDotAEventAdd *CGHostDB :: ThreadedDotAEventAdd( uint32_t gameid, string gamename, string killer, string victim, uint32_t kcolour, uint32_t vcolour  )
{
	return NULL;
}

CCallableUpdateGameInfo *CGHostDB :: ThreadedUpdateGameInfo( string name, uint32_t players, bool ispublic, vector<string> m_Slots)
{
	return NULL;
}

CDBLastSeenPlayer *CGHostDB :: LastSeenPlayer( string name )
{
	return NULL;
}

CCallableLastSeenPlayer *CGHostDB :: ThreadedLastSeenPlayer( string name )
{
	return NULL;
}

CCallableSaveReplay *CGHostDB :: ThreadedSaveReplay( CReplay *replay )
{
	return NULL;
}

set<string> CGHostDB :: CountrySkipList()
{
	return set<string>( );
}

CCallableCountrySkipList *CGHostDB :: ThreadedCountrySkipList( )
{
	return NULL;
}

set<VouchPair> CGHostDB :: VouchList( )
{
	return set<VouchPair>( );
}

CCallableVouchList *CGHostDB :: ThreadedVouchList( )
{
	return NULL;
}

CCallableRegisterPlayerAdd :: ~CCallableRegisterPlayerAdd( )
{

}

CCallableDotAEventAdd :: ~CCallableDotAEventAdd( )
{

}

CCallableUpdateGameInfo :: ~CCallableUpdateGameInfo( )
{
	
}

CCallableLastSeenPlayer :: ~CCallableLastSeenPlayer( )
{

}

CCallableSaveReplay :: ~CCallableSaveReplay( )
{
	
}

CCallableCountrySkipList :: ~CCallableCountrySkipList()
{
	
}

CCallableVouchList :: ~CCallableVouchList()
{
	
}

//
// end
//



uint32_t CGHostDB :: AdminCount( string server )
{
	return 0;
}

bool CGHostDB :: AdminCheck( string server, string user )
{
	return false;
}

bool CGHostDB :: AdminAdd( string server, string user )
{
	return false;
}

bool CGHostDB :: AdminRemove( string server, string user )
{
	return false;
}

vector<string> CGHostDB :: AdminList( string server )
{
	return vector<string>( );
}

uint32_t CGHostDB :: BanCount( string server )
{
	return 0;
}

CDBBan *CGHostDB :: BanCheck( string server, string user, string ip )
{
	return NULL;
}

bool CGHostDB :: BanAdd( string server, string user, string ip, string gamename, string admin, string reason, uint32_t bantime, uint32_t ipban )
{
	return false;
}

bool CGHostDB :: BanRemove( string server, string user, string admin, string reason )
{
	return false;
}

bool CGHostDB :: BanRemove( string user, string admin, string reason )
{
	return false;
}

vector<CDBBan *> CGHostDB :: BanList( string server )
{
	return vector<CDBBan *>( );
}

uint32_t CGHostDB :: GameAdd( string server, string map, string gamename, string ownername, uint32_t duration, uint32_t gamestate, string creatorname, string creatorserver )
{
	return 0;
}

uint32_t CGHostDB :: GamePlayerAdd( uint32_t gameid, string name, string ip, uint32_t spoofed, string spoofedrealm, uint32_t reserved, uint32_t loadingtime, uint32_t left, string leftreason, uint32_t team, uint32_t colour )
{
	return 0;
}

uint32_t CGHostDB :: GamePlayerCount( string name )
{
	return 0;
}

CDBGamePlayerSummary *CGHostDB :: GamePlayerSummaryCheck( string name )
{
	return NULL;
}

uint32_t CGHostDB :: DotAGameAdd( uint32_t gameid, uint32_t winner, uint32_t min, uint32_t sec )
{
	return 0;
}

uint32_t CGHostDB :: DotAPlayerAdd( uint32_t gameid, string name, uint32_t colour, uint32_t kills, uint32_t deaths, uint32_t creepkills, uint32_t creepdenies, uint32_t assists, uint32_t gold, uint32_t neutralkills, string item1, string item2, string item3, string item4, string item5, string item6, string hero, uint32_t newcolour, uint32_t towerkills, uint32_t raxkills, uint32_t courierkills, uint32_t outcome, uint32_t level, uint32_t apm )
{
	return 0;
}

uint32_t CGHostDB :: DotAPlayerCount( string name )
{
	return 0;
}

CDBDotAPlayerSummary *CGHostDB :: DotAPlayerSummaryCheck( string name )
{
	return NULL;
}

string CGHostDB :: FromCheck( uint32_t ip )
{
	return "??";
}

bool CGHostDB :: FromAdd( uint32_t ip1, uint32_t ip2, string country )
{
	return false;
}

bool CGHostDB :: DownloadAdd( string map, uint32_t mapsize, string name, string ip, uint32_t spoofed, string spoofedrealm, uint32_t downloadtime )
{
	return false;
}

uint32_t CGHostDB :: W3MMDPlayerAdd( string category, uint32_t gameid, uint32_t pid, string name, string flag, uint32_t leaver, uint32_t practicing )
{
	return 0;
}

bool CGHostDB :: W3MMDVarAdd( uint32_t gameid, map<VarP,int32_t> var_ints )
{
	return false;
}

bool CGHostDB :: W3MMDVarAdd( uint32_t gameid, map<VarP,double> var_reals )
{
	return false;
}

bool CGHostDB :: W3MMDVarAdd( uint32_t gameid, map<VarP,string> var_strings )
{
	return false;
}


void CGHostDB :: CreateThread( CBaseCallable *callable )
{
	callable->SetReady( true );
}

CCallableAdminCount *CGHostDB :: ThreadedAdminCount( string server )
{
	return NULL;
}

CCallableAdminCheck *CGHostDB :: ThreadedAdminCheck( string server, string user )
{
	return NULL;
}

CCallableAdminAdd *CGHostDB :: ThreadedAdminAdd( string server, string user )
{
	return NULL;
}

CCallableAdminRemove *CGHostDB :: ThreadedAdminRemove( string server, string user )
{
	return NULL;
}

CCallableAdminList *CGHostDB :: ThreadedAdminList( string server )
{
	return NULL;
}

CCallableBanCount *CGHostDB :: ThreadedBanCount( string server )
{
	return NULL;
}

CCallableBanCheck *CGHostDB :: ThreadedBanCheck( string server, string user, string ip )
{
	return NULL;
}

CCallableBanAdd *CGHostDB :: ThreadedBanAdd( string server, string user, string ip, string gamename, string admin, string reason, uint32_t bantime, uint32_t ipban )
{
	return NULL;
}

CCallableBanRemove *CGHostDB :: ThreadedBanRemove( string server, string user, string admin, string reason )
{
	return NULL;
}

CCallableBanRemove *CGHostDB :: ThreadedBanRemove( string user, string admin, string reason )
{
	return NULL;
}

CCallableBanList *CGHostDB :: ThreadedBanList( string server )
{
	return NULL;
}

CCallableGameAdd *CGHostDB :: ThreadedGameAdd( string server, string map, string gamename, string ownername, uint32_t duration, uint32_t gamestate, string creatorname, string creatorserver, vector<string> chatlog )
{
	return NULL;
}

CCallableGamePlayerAdd *CGHostDB :: ThreadedGamePlayerAdd( uint32_t gameid, string name, string ip, uint32_t spoofed, string spoofedrealm, uint32_t reserved, uint32_t loadingtime, uint32_t left, string leftreason, uint32_t team, uint32_t colour )
{
	return NULL;
}

CCallableGamePlayerSummaryCheck *CGHostDB :: ThreadedGamePlayerSummaryCheck( string name, uint32_t season )
{
	return NULL;
}

CCallableDotAGameAdd *CGHostDB :: ThreadedDotAGameAdd( uint32_t gameid, uint32_t winner, uint32_t min, uint32_t sec )
{
	return NULL;
}

CCallableDotAPlayerAdd *CGHostDB :: ThreadedDotAPlayerAdd( uint32_t gameid, string name, uint32_t colour, uint32_t kills, uint32_t deaths, uint32_t creepkills, uint32_t creepdenies, uint32_t assists, uint32_t gold, uint32_t neutralkills, string item1, string item2, string item3, string item4, string item5, string item6, string hero, uint32_t newcolour, uint32_t towerkills, uint32_t raxkills, uint32_t courierkills, uint32_t outcome, uint32_t level, uint32_t apm )
{
	return NULL;
}

CCallableDotAPlayerSummaryCheck *CGHostDB :: ThreadedDotAPlayerSummaryCheck( string name, uint32_t season )
{
	return NULL;
}

CCallableDownloadAdd *CGHostDB :: ThreadedDownloadAdd( string map, uint32_t mapsize, string name, string ip, uint32_t spoofed, string spoofedrealm, uint32_t downloadtime )
{
	return NULL;
}

CCallableScoreCheck *CGHostDB :: ThreadedScoreCheck( string category, string name, string server, uint32_t season )
{
	return NULL;
}

CCallableW3MMDPlayerAdd *CGHostDB :: ThreadedW3MMDPlayerAdd( string category, uint32_t gameid, uint32_t pid, string name, string flag, uint32_t leaver, uint32_t practicing )
{
	return NULL;
}

CCallableW3MMDVarAdd *CGHostDB :: ThreadedW3MMDVarAdd( uint32_t gameid, map<VarP,int32_t> var_ints )
{
	return NULL;
}

CCallableW3MMDVarAdd *CGHostDB :: ThreadedW3MMDVarAdd( uint32_t gameid, map<VarP,double> var_reals )
{
	return NULL;
}

CCallableW3MMDVarAdd *CGHostDB :: ThreadedW3MMDVarAdd( uint32_t gameid, map<VarP,string> var_strings )
{
	return NULL;
}

CCallableRegisterPlayerAdd *CGHostDB :: ThreadedRegisterPlayerAdd( string name, string email, string ip )
{
	return NULL;
}

//
// Callables
//

void CBaseCallable :: Init( )
{
	m_StartTicks = GetTicks( );
}

void CBaseCallable :: Close( )
{
	m_EndTicks = GetTicks( );
	m_Ready = true;
}

CCallableAdminCount :: ~CCallableAdminCount( )
{

}

CCallableAdminCheck :: ~CCallableAdminCheck( )
{

}

CCallableAdminAdd :: ~CCallableAdminAdd( )
{

}

CCallableAdminRemove :: ~CCallableAdminRemove( )
{

}

CCallableAdminList :: ~CCallableAdminList( )
{

}

CCallableBanCount :: ~CCallableBanCount( )
{

}

CCallableBanCheck :: ~CCallableBanCheck( )
{
	delete m_Result;
}

CCallableBanAdd :: ~CCallableBanAdd( )
{

}

CCallableBanRemove :: ~CCallableBanRemove( )
{

}

CCallableBanList :: ~CCallableBanList( )
{
	// don't delete anything in m_Result here, it's the caller's responsibility
}

CCallableGameAdd :: ~CCallableGameAdd( )
{

}

CCallableGamePlayerAdd :: ~CCallableGamePlayerAdd( )
{

}

CCallableGamePlayerSummaryCheck :: ~CCallableGamePlayerSummaryCheck( )
{
	delete m_Result;
}

CCallableDotAGameAdd :: ~CCallableDotAGameAdd( )
{

}

CCallableDotAPlayerAdd :: ~CCallableDotAPlayerAdd( )
{

}

CCallableDotAPlayerSummaryCheck :: ~CCallableDotAPlayerSummaryCheck( )
{
	delete m_Result;
}

CCallableDownloadAdd :: ~CCallableDownloadAdd( )
{

}

CCallableScoreCheck :: ~CCallableScoreCheck( )
{

}

CCallableW3MMDPlayerAdd :: ~CCallableW3MMDPlayerAdd( )
{

}

CCallableW3MMDVarAdd :: ~CCallableW3MMDVarAdd( )
{

}

//
// CDBBan
//

CDBBan :: CDBBan( string nServer, string nName, string nIP, string nDate, string nGameName, string nAdmin, string nReason )
{
	m_Server = nServer;
	m_Name = nName;
	m_IP = nIP;
	m_Date = nDate;
	m_GameName = nGameName;
	m_Admin = nAdmin;
	m_Reason = nReason;
	m_IPBan = false;
}

CDBBan :: CDBBan( string nServer, string nName, string nIP, string nDate, string nGameName, string nAdmin, string nReason, string nExpires )
{
	m_Server = nServer;
	m_Name = nName;
	m_IP = nIP;
	m_Date = nDate;
	m_GameName = nGameName;
	m_Admin = nAdmin;
	m_Reason = nReason;
	m_IPBan = false;
	m_Expires = nExpires;
}

CDBBan :: CDBBan( string nServer, string nName, string nIP, string nDate, string nGameName, string nAdmin, string nReason, uint32_t nIPBan )
{
	m_Server = nServer;
	m_Name = nName;
	m_IP = nIP;
	m_Date = nDate;
	m_GameName = nGameName;
	m_Admin = nAdmin;
	m_Reason = nReason;

	if (nIPBan == 0)
		m_IPBan = false;
	else
		m_IPBan = true;
}

CDBBan :: CDBBan( string nServer, string nName, string nIP, string nDate, string nGameName, string nAdmin, string nReason, uint32_t nIPBan, string nExpires )
{
	m_Server = nServer;
	m_Name = nName;
	m_IP = nIP;
	m_Date = nDate;
	m_GameName = nGameName;
	m_Admin = nAdmin;
	m_Reason = nReason;

	if (nIPBan == 0)
		m_IPBan = false;
	else
		m_IPBan = true;

	m_Expires = nExpires;
}

CDBBan :: ~CDBBan( )
{

}

//
// CDBGame
//

CDBGame :: CDBGame( uint32_t nID, string nServer, string nMap, string nDateTime, string nGameName, string nOwnerName, uint32_t nDuration )
{
	m_ID = nID;
	m_Server = nServer;
	m_Map = nMap;
	m_DateTime = nDateTime;
	m_GameName = nGameName;
	m_OwnerName = nOwnerName;
	m_Duration = nDuration;
}

CDBGame :: ~CDBGame( )
{

}

//
// CDBGamePlayer
//

CDBGamePlayer :: CDBGamePlayer( uint32_t nID, uint32_t nGameID, string nName, string nIP, uint32_t nSpoofed, string nSpoofedRealm, uint32_t nReserved, uint32_t nLoadingTime, uint32_t nLeft, string nLeftReason, uint32_t nTeam, uint32_t nColour )
{
	m_ID = nID;
	m_GameID = nGameID;
	m_Name = nName;
	m_IP = nIP;
	m_Spoofed = nSpoofed;
	m_SpoofedRealm = nSpoofedRealm;
	m_Reserved = nReserved;
	m_LoadingTime = nLoadingTime;
	m_Left = nLeft;
	m_LeftReason = nLeftReason;
	m_Team = nTeam;
	m_Colour = nColour;
}

CDBGamePlayer :: ~CDBGamePlayer( )
{

}

//
// CDBGamePlayerSummary
//

CDBGamePlayerSummary :: CDBGamePlayerSummary( string nServer, string nName, string nFirstGameDateTime, string nLastGameDateTime, uint32_t nTotalGames, uint32_t nMinLoadingTime, uint32_t nAvgLoadingTime, uint32_t nMaxLoadingTime, uint32_t nMinLeftPercent, uint32_t nAvgLeftPercent, uint32_t nMaxLeftPercent, uint32_t nMinDuration, uint32_t nAvgDuration, uint32_t nMaxDuration )
{
	m_Server = nServer;
	m_Name = nName;
	m_FirstGameDateTime = nFirstGameDateTime;
	m_LastGameDateTime = nLastGameDateTime;
	m_TotalGames = nTotalGames;
	m_MinLoadingTime = nMinLoadingTime;
	m_AvgLoadingTime = nAvgLoadingTime;
	m_MaxLoadingTime = nMaxLoadingTime;
	m_MinLeftPercent = nMinLeftPercent;
	m_AvgLeftPercent = nAvgLeftPercent;
	m_MaxLeftPercent = nMaxLeftPercent;
	m_MinDuration = nMinDuration;
	m_AvgDuration = nAvgDuration;
	m_MaxDuration = nMaxDuration;
	m_Vouched = false;
	m_VouchedBy.clear();
	m_AllTotalGames = 0;
}

CDBGamePlayerSummary :: CDBGamePlayerSummary( string nServer, string nName, string nFirstGameDateTime, string nLastGameDateTime, uint32_t nTotalGames, uint32_t nMinLoadingTime, uint32_t nAvgLoadingTime, uint32_t nMaxLoadingTime, uint32_t nMinLeftPercent, uint32_t nAvgLeftPercent, uint32_t nMaxLeftPercent, uint32_t nMinDuration, uint32_t nAvgDuration, uint32_t nMaxDuration, bool nVouched, string nVouchedBy )
{
	m_Server = nServer;
	m_Name = nName;
	m_FirstGameDateTime = nFirstGameDateTime;
	m_LastGameDateTime = nLastGameDateTime;
	m_TotalGames = nTotalGames;
	m_MinLoadingTime = nMinLoadingTime;
	m_AvgLoadingTime = nAvgLoadingTime;
	m_MaxLoadingTime = nMaxLoadingTime;
	m_MinLeftPercent = nMinLeftPercent;
	m_AvgLeftPercent = nAvgLeftPercent;
	m_MaxLeftPercent = nMaxLeftPercent;
	m_MinDuration = nMinDuration;
	m_AvgDuration = nAvgDuration;
	m_MaxDuration = nMaxDuration;
	m_Vouched = nVouched;
	m_VouchedBy = nVouchedBy;
	m_AllTotalGames = 0;
}

CDBGamePlayerSummary :: ~CDBGamePlayerSummary( )
{

}

//
// CDBDotAGame
//

CDBDotAGame :: CDBDotAGame( uint32_t nID, uint32_t nGameID, uint32_t nWinner, uint32_t nMin, uint32_t nSec )
{
	m_ID = nID;
	m_GameID = nGameID;
	m_Winner = nWinner;
	m_Min = nMin;
	m_Sec = nSec;
}

CDBDotAGame :: ~CDBDotAGame( )
{

}

//
// CDBDotAPlayer
//

CDBDotAPlayer :: CDBDotAPlayer( )
{
	m_ID = 0;
	m_GameID = 0;
	m_Colour = 0;
	m_Kills = 0;
	m_Deaths = 0;
	m_CreepKills = 0;
	m_CreepDenies = 0;
	m_Assists = 0;
	m_Gold = 0;
	m_NeutralKills = 0;
	m_NewColour = 0;
	m_TowerKills = 0;
	m_RaxKills = 0;
	m_CourierKills = 0;
	m_Rank = 0;
	m_Score = 1000;
	m_Outcome = 0;
	m_Level = 1;
	m_Apm = 0;
}

CDBDotAPlayer :: CDBDotAPlayer( uint32_t nID, string nName, uint32_t nGameID, uint32_t nColour, uint32_t nKills, uint32_t nDeaths, uint32_t nCreepKills, uint32_t nCreepDenies, uint32_t nAssists, uint32_t nGold, uint32_t nNeutralKills, string nItem1, string nItem2, string nItem3, string nItem4, string nItem5, string nItem6, string nHero, uint32_t nNewColour, uint32_t nTowerKills, uint32_t nRaxKills, uint32_t nCourierKills, uint32_t nOutcome, uint32_t nLevel, uint32_t nApm )
{
	m_ID = nID;
	m_GameID = nGameID;
	m_Colour = nColour;
	m_Kills = nKills;
	m_Deaths = nDeaths;
	m_CreepKills = nCreepKills;
	m_CreepDenies = nCreepDenies;
	m_Assists = nAssists;
	m_Gold = nGold;
	m_NeutralKills = nNeutralKills;
	m_Items[0] = nItem1;
	m_Items[1] = nItem2;
	m_Items[2] = nItem3;
	m_Items[3] = nItem4;
	m_Items[4] = nItem5;
	m_Items[5] = nItem6;
	m_Hero = nHero;
	m_NewColour = nNewColour;
	m_TowerKills = nTowerKills;
	m_RaxKills = nRaxKills;
	m_CourierKills = nCourierKills;
	m_Outcome = 0;
	m_Name = nName;
	m_Level = nLevel;
	m_Apm = nApm;
}

CDBDotAPlayer :: CDBDotAPlayer( uint32_t nID, string nName, uint32_t nGameID,uint32_t nColour, uint32_t nKills, uint32_t nDeaths, uint32_t nCreepKills, uint32_t nCreepDenies, uint32_t nAssists, uint32_t nGold, uint32_t nNeutralKills, string nItem1, string nItem2, string nItem3, string nItem4, string nItem5, string nItem6, string nHero, uint32_t nNewColour, uint32_t nTowerKills, uint32_t nRaxKills, uint32_t nCourierKills, uint32_t nRank, uint32_t nScore, uint32_t nOutcome, uint32_t nLevel, uint32_t nApm )
{
	m_ID = nID;
	m_GameID = nGameID;
	m_Colour = nColour;
	m_Kills = nKills;
	m_Deaths = nDeaths;
	m_CreepKills = nCreepKills;
	m_CreepDenies = nCreepDenies;
	m_Assists = nAssists;
	m_Gold = nGold;
	m_NeutralKills = nNeutralKills;
	m_Items[0] = nItem1;
	m_Items[1] = nItem2;
	m_Items[2] = nItem3;
	m_Items[3] = nItem4;
	m_Items[4] = nItem5;
	m_Items[5] = nItem6;
	m_Hero = nHero;
	m_NewColour = nNewColour;
	m_TowerKills = nTowerKills;
	m_RaxKills = nRaxKills;
	m_CourierKills = nCourierKills;
	m_Rank = nRank;
	m_Score = nScore;
	m_Outcome = nOutcome;
	m_Name = nName;
	m_Level = nLevel;
	m_Apm = nApm;
}

CDBDotAPlayer :: ~CDBDotAPlayer( )
{

}

string CDBDotAPlayer :: GetItem( unsigned int i )
{
	if( i < 6 )
		return m_Items[i];

	return string( );
}

void CDBDotAPlayer :: SetItem( unsigned int i, string item )
{
	if( i < 6 )
		m_Items[i] = item;
}

//
// CDBDotAPlayerSummary
//

CDBDotAPlayerSummary :: CDBDotAPlayerSummary( string nServer, string nName, uint32_t nTotalGames, uint32_t nTotalWins, uint32_t nTotalLosses, uint32_t nTotalKills, uint32_t nTotalDeaths, uint32_t nTotalCreepKills, uint32_t nTotalCreepDenies, uint32_t nTotalAssists, uint32_t nTotalNeutralKills, uint32_t nTotalTowerKills, uint32_t nTotalRaxKills, uint32_t nTotalCourierKills )
{
	m_Server = nServer;
	m_Name = nName;
	m_TotalGames = nTotalGames;
	m_TotalWins = nTotalWins;
	m_TotalLosses = nTotalLosses;
	m_TotalKills = nTotalKills;
	m_TotalDeaths = nTotalDeaths;
	m_TotalCreepKills = nTotalCreepKills;
	m_TotalCreepDenies = nTotalCreepDenies;
	m_TotalAssists = nTotalAssists;
	m_TotalNeutralKills = nTotalNeutralKills;
	m_TotalTowerKills = nTotalTowerKills;
	m_TotalRaxKills = nTotalRaxKills;
	m_TotalCourierKills = nTotalCourierKills;
	m_Score = 0;
	m_Rank = 0;
	m_Streak = 0;
}

CDBDotAPlayerSummary :: CDBDotAPlayerSummary( string nServer, string nName, uint32_t nTotalGames, uint32_t nTotalWins, uint32_t nTotalLosses, uint32_t nTotalKills, uint32_t nTotalDeaths, uint32_t nTotalCreepKills, uint32_t nTotalCreepDenies, uint32_t nTotalAssists, uint32_t nTotalNeutralKills, uint32_t nTotalTowerKills, uint32_t nTotalRaxKills, uint32_t nTotalCourierKills, uint32_t nRank, double nScore)
{
	m_Server = nServer;
	m_Name = nName;
	m_TotalGames = nTotalGames;
	m_TotalWins = nTotalWins;
	m_TotalLosses = nTotalLosses;
	m_TotalKills = nTotalKills;
	m_TotalDeaths = nTotalDeaths;
	m_TotalCreepKills = nTotalCreepKills;
	m_TotalCreepDenies = nTotalCreepDenies;
	m_TotalAssists = nTotalAssists;
	m_TotalNeutralKills = nTotalNeutralKills;
	m_TotalTowerKills = nTotalTowerKills;
	m_TotalRaxKills = nTotalRaxKills;
	m_TotalCourierKills = nTotalCourierKills;
	m_Rank = nRank;
	m_Score	= nScore;
	m_Streak = 0;
}

CDBDotAPlayerSummary :: CDBDotAPlayerSummary( string nServer, string nName, uint32_t nTotalGames, uint32_t nTotalWins, uint32_t nTotalLosses, uint32_t nTotalKills, uint32_t nTotalDeaths, uint32_t nTotalCreepKills, uint32_t nTotalCreepDenies, uint32_t nTotalAssists, uint32_t nTotalNeutralKills, uint32_t nTotalTowerKills, uint32_t nTotalRaxKills, uint32_t nTotalCourierKills, uint32_t nRank, double nScore, uint32_t nStreak)
{
	m_Server = nServer;
	m_Name = nName;
	m_TotalGames = nTotalGames;
	m_TotalWins = nTotalWins;
	m_TotalLosses = nTotalLosses;
	m_TotalKills = nTotalKills;
	m_TotalDeaths = nTotalDeaths;
	m_TotalCreepKills = nTotalCreepKills;
	m_TotalCreepDenies = nTotalCreepDenies;
	m_TotalAssists = nTotalAssists;
	m_TotalNeutralKills = nTotalNeutralKills;
	m_TotalTowerKills = nTotalTowerKills;
	m_TotalRaxKills = nTotalRaxKills;
	m_TotalCourierKills = nTotalCourierKills;
	m_Rank = nRank;
	m_Score	= nScore;
	m_Streak = nStreak;
}

CDBDotAPlayerSummary :: ~CDBDotAPlayerSummary( )
{

}

CDBLastSeenPlayer :: CDBLastSeenPlayer( bool nSeen, string nName )
{
	m_Seen = nSeen;
	m_Name = nName;
}

CDBLastSeenPlayer :: CDBLastSeenPlayer( bool nSeen, string nName, string nDate, string nLastGame, string nLastHero, uint32_t nLastTeam, uint32_t nLastOutcome, double nLastGain, uint32_t nKills, uint32_t nDeaths, uint32_t nAssists )
{
	m_Seen = nSeen;
	m_Name = nName;
	m_Date = nDate;
	m_LastGame = nLastGame;
	m_LastHero = nLastHero;
	m_LastTeam = nLastTeam;
	m_LastOutcome = nLastOutcome;
	m_LastGain = nLastGain;
	m_Kills = nKills;
	m_Deaths = nDeaths;
	m_Assists = nAssists;
}

CDBLastSeenPlayer :: ~CDBLastSeenPlayer( )
{
	
}
