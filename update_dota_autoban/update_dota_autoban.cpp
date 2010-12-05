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

#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#ifdef WIN32
 #include "ms_stdint.h"
#else
 #include <stdint.h>
#endif

#include "config.h"

#include <string.h>

#ifdef WIN32
 #include <winsock.h>
#endif

#include <mysql/mysql.h>

void CONSOLE_Print( string message )
{
	cout << message << endl;
}

string MySQLEscapeString( MYSQL *conn, string str )
{
	char *to = new char[str.size( ) * 2 + 1];
	unsigned long size = mysql_real_escape_string( conn, to, str.c_str( ), str.size( ) );
	string result( to, size );
	delete [] to;
	return result;
}

vector<string> MySQLFetchRow( MYSQL_RES *res )
{
	vector<string> Result;

	MYSQL_ROW Row = mysql_fetch_row( res );

	if( Row )
	{
		unsigned long *Lengths;
		Lengths = mysql_fetch_lengths( res );

		for( unsigned int i = 0; i < mysql_num_fields( res ); i++ )
		{
			if( Row[i] )
				Result.push_back( string( Row[i], Lengths[i] ) );
			else
				Result.push_back( string( ) );
		}
	}

	return Result;
}

string UTIL_ToString( uint32_t i )
{
	string result;
	stringstream SS;
	SS << i;
	SS >> result;
	return result;
}

string UTIL_ToString( float f, int digits )
{
	string result;
	stringstream SS;
	SS << std :: fixed << std :: setprecision( digits ) << f;
	SS >> result;
	return result;
}

uint32_t UTIL_ToUInt32( string &s )
{
	uint32_t result;
	stringstream SS;
	SS << s;
	SS >> result;
	return result;
}

float UTIL_ToFloat( string &s )
{
	float result;
	stringstream SS;
	SS << s;
	SS >> result;
	return result;
}

int main( int argc, char **argv )
{
	string CFGFile = "update_dota_autoban.cfg";

	if( argc > 1 && argv[1] )
		CFGFile = argv[1];

	CConfig CFG;
	CFG.Read( CFGFile );
	string Server = CFG.GetString( "db_mysql_server", string( ) );
	string Database = CFG.GetString( "db_mysql_database", "ghost" );
	string User = CFG.GetString( "db_mysql_user", string( ) );
	string Password = CFG.GetString( "db_mysql_password", string( ) );
	int Port = CFG.GetInt( "db_mysql_port", 0 );

	cout << "connecting to database server" << endl;
	MYSQL *Connection = NULL;

	if( !( Connection = mysql_init( NULL ) ) )
	{
		cout << "error: " << mysql_error( Connection ) << endl;
		return 1;
	}

	my_bool Reconnect = true;
	mysql_options( Connection, MYSQL_OPT_RECONNECT, &Reconnect );

	if( !( mysql_real_connect( Connection, Server.c_str( ), User.c_str( ), Password.c_str( ), Database.c_str( ), Port, NULL, 0 ) ) )
	{
		cout << "error: " << mysql_error( Connection ) << endl;
		return 1;
	}

	cout << "connected" << endl;
	cout << "beginning transaction" << endl;

	string QBegin = "BEGIN";

	if( mysql_real_query( Connection, QBegin.c_str( ), QBegin.size( ) ) != 0 )
	{
		cout << "error: " << mysql_error( Connection ) << endl;
		return 1;
	}

	cout << "creating tables" << endl;

	string QCreate2 = "CREATE TABLE IF NOT EXISTS dota_autoban_games_checked ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, gameid INT NOT NULL, KEY gameid (gameid) )";

	if( mysql_real_query( Connection, QCreate2.c_str( ), QCreate2.size( ) ) != 0 )
	{
		cout << "error: " << mysql_error( Connection ) << endl;
		return 1;
	}

	cout << "getting unchecked games" << endl;
	queue<uint32_t> UnscoredGames;

	string QSelectUnscored = "SELECT id FROM games WHERE id NOT IN ( SELECT gameid FROM dota_autoban_games_checked ) ORDER BY id";

	if( mysql_real_query( Connection, QSelectUnscored.c_str( ), QSelectUnscored.size( ) ) != 0 )
	{
		cout << "error: " << mysql_error( Connection ) << endl;
		return 1;
	}
	else
	{
		MYSQL_RES *Result = mysql_store_result( Connection );

		if( Result )
		{
			vector<string> Row = MySQLFetchRow( Result );

			while( !Row.empty( ) )
			{
				UnscoredGames.push( UTIL_ToUInt32( Row[0] ) );
				Row = MySQLFetchRow( Result );
			}

			mysql_free_result( Result );
		}
		else
		{
			cout << "error: " << mysql_error( Connection ) << endl;
			return 1;
		}
	}

	cout << "found " << UnscoredGames.size( ) << " unscored games" << endl;
	
	bool CopyScores = false;
	
	if (UnscoredGames.size( ) > 0)
		CopyScores = true;

	while( !UnscoredGames.empty( ) )
	{
		uint32_t GameID = UnscoredGames.front( );
		UnscoredGames.pop( );

		string QSelectPlayers = "SELECT dota_elo_scores.id, gameplayers.name, spoofedrealm, newcolour, winner, score, games.datetime, gameplayers.left, games.duration, gameplayers.leftreason, gameplayers.ip, games.gamename FROM dotaplayers LEFT JOIN dotagames ON dotagames.gameid=dotaplayers.gameid LEFT JOIN gameplayers ON gameplayers.gameid=dotaplayers.gameid AND gameplayers.colour=dotaplayers.colour LEFT JOIN dota_elo_scores ON dota_elo_scores.name=gameplayers.name LEFT JOIN games ON games.id=dotaplayers.gameid WHERE dotaplayers.gameid=" + UTIL_ToString( GameID );

		if( mysql_real_query( Connection, QSelectPlayers.c_str( ), QSelectPlayers.size( ) ) != 0 )
		{
			cout << "error: " << mysql_error( Connection ) << endl;
			return 1;
		}
		else
		{
			MYSQL_RES *Result = mysql_store_result( Connection );

			if( Result )
			{
				cout << "gameid " << UTIL_ToString( GameID ) << " found" << endl;

				bool ignore = false;
				uint32_t rowids[10];
				string names[10];
				string servers[10];
				bool exists[10];
				int num_players = 0;
				float player_ratings[10];
				int player_teams[10];
				int num_teams = 2;
				float team_ratings[2];
				float team_winners[2];
				int team_numplayers[2];
				team_ratings[0] = 0.0;
				team_ratings[1] = 0.0;
				team_numplayers[0] = 0;
				team_numplayers[1] = 0;
				string gametime;
				
				int		team_leavers[2];
				bool	player_isleaver[10];
				float	player_left[10];

				float	player_scale[10];
				
				team_leavers[0] = 0;
				team_leavers[1] = 0;


				vector<string> Row = MySQLFetchRow( Result );

				while( Row.size( ) == 12 )
				{
					if( num_players >= 10 )
					{
						cout << "gameid " << UTIL_ToString( GameID ) << " has more than 10 players, ignoring" << endl;
						ignore = true;
						break;
					}

					uint32_t Winner = UTIL_ToUInt32( Row[4] );

					if( Winner != 1 && Winner != 2 )
					{
						cout << "gameid " << UTIL_ToString( GameID ) << " has no winner, ignoring" << endl;
						ignore = true;
						break;
					}
					else if( Winner == 1 )
					{
						team_winners[0] = 1.0;
						team_winners[1] = 0.0;
					}
					else
					{
						team_winners[0] = 0.0;
						team_winners[1] = 1.0;
					}

					if( !Row[0].empty( ) )
						rowids[num_players] = UTIL_ToUInt32( Row[0] );
					else
						rowids[num_players] = 0;

					names[num_players] = Row[1];
					servers[num_players] = Row[2];

					uint32_t Colour = UTIL_ToUInt32( Row[3] );

					if( Colour >= 1 && Colour <= 5 )
					{
						player_teams[num_players] = 0;
						team_ratings[0] += player_ratings[num_players];
						team_numplayers[0]++;
					}
					else if( Colour >= 7 && Colour <= 11 )
					{
						player_teams[num_players] = 1;
						team_ratings[1] += player_ratings[num_players];
						team_numplayers[1]++;
					}
					else
					{
						cout << "gameid " << UTIL_ToString( GameID ) << " has a player with an invalid newcolour, ignoring" << endl;
						ignore = true;
						break;
					}
					
					float game_duration = UTIL_ToFloat(Row[8]);
					
					player_left[num_players] = UTIL_ToUInt32(Row[7]);
					
					if ((game_duration - (60 * 5)) > player_left[num_players])
					{
						bool doban=false;
						string ban = "INSERT INTO bans (botid, server, name, ip, date, gamename, admin, reason, ipban, expires)";
						string warn;
						if (Row[9] == "has left the game voluntarily")
						{
							doban=true;
							cout << "Leaver: " + Row[1] << endl;
							ban += " VALUES ('2', 'europe.battle.net', '" + Row[1] + "', '" + Row[10] + "', NOW(), '" + Row[11] + "', 'Autoban', 'Autoban: Leaver', '1', UNIX_TIMESTAMP()+259200)";
							cout << ban << endl;

							warn = "INSERT INTO warnings (name, weight, warning_id, date, admin, game, note) VALUES ('" + Row[1] + "', '30', '1', NOW(), 'Autoban', '" + Row[11] + "', '')";
							cout << warn << endl;


						}
						else if (Row[9] == "lagged out (dropped by vote)" || Row[9] == "lagged out (dropped by admin)")
						{
							doban=true;
							ban += " VALUES ('2', 'europe.battle.net', '" + Row[1] + "', '" + Row[10] + "', NOW(), '" + Row[11] + "', 'Autoban', 'Autoban: Disconnect', '1', UNIX_TIMESTAMP()+129600)";
							cout << ban << endl;

							warn = "INSERT INTO warnings (name, weight, warning_id, date, admin, game, note) VALUES ('" + Row[1] + "', '30', '8', NOW(), 'Autoban', '" + Row[11] + "', '')";
							cout << warn << endl;
						}


						if (doban)
						{
							if( mysql_real_query( Connection, ban.c_str( ), ban.size( ) ) != 0 )
								cout << "error: " << mysql_error( Connection ) << endl;

							if( mysql_real_query( Connection, warn.c_str( ), warn.size( ) ) != 0 )
								cout << "error: " << mysql_error( Connection ) << endl;
						}
					}
					else
					{
						player_isleaver[num_players] = false;
					}

					gametime = Row[6];

					num_players++;
					Row = MySQLFetchRow( Result );
				}
				
				mysql_free_result( Result );
				
				if( !ignore )
				{
					if( num_players == 0 )
						cout << "gameid " << UTIL_ToString( GameID ) << " has no players, ignoring" << endl;
					else if( team_numplayers[0] == 0 )
						cout << "gameid " << UTIL_ToString( GameID ) << " has no Sentinel players, ignoring" << endl;
					else if( team_numplayers[1] == 0 )
						cout << "gameid " << UTIL_ToString( GameID ) << " has no Scourge players, ignoring" << endl;
				}				
			}
			else
			{
				cout << "error: " << mysql_error( Connection ) << endl;
				return 1;
			}
		}


		string QInsertScored = "INSERT INTO dota_autoban_games_checked ( gameid ) VALUES ( " + UTIL_ToString( GameID ) + " )";

		if( mysql_real_query( Connection, QInsertScored.c_str( ), QInsertScored.size( ) ) != 0 )
		{
			cout << "error: " << mysql_error( Connection ) << endl;
			return 1;
		}
	}

	cout << "committing transaction" << endl;

	string QCommit = "COMMIT";

	if( mysql_real_query( Connection, QCommit.c_str( ), QCommit.size( ) ) != 0 )
	{
		cout << "error: " << mysql_error( Connection ) << endl;
		return 1;
	}
	
	cout << "done" << endl;
	return 0;
}
