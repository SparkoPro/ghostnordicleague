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

/*
Table definition ---
string QCreate1 = "CREATE TABLE IF NOT EXISTS dota_elo_scores ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, name VARCHAR(15) NOT NULL, server VARCHAR(100) NOT NULL, score REAL NOT NULL )";
string QCreate2 = "CREATE TABLE IF NOT EXISTS dota_elo_games_scored ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, gameid INT NOT NULL )";
*/

#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <math.h>

using namespace std;

#ifdef WIN32
#include "ms_stdint.h"
#else
#include <stdint.h>
#endif

#include "config.h"
#include "elo.h"

#include <string.h>

#ifdef WIN32
#include <winsock.h>
#endif

#include <mysql/mysql.h>

void CONSOLE_Print( string message ) { cout << message << endl; }

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
	string CFGFile = "update_dota_elo.cfg";

	if( argc > 1 && argv[1] )
		CFGFile = argv[1];

	CConfig CFG;
	CFG.Read( CFGFile );
	string Server = CFG.GetString( "db_mysql_server", string( ) );
	string Database = CFG.GetString( "db_mysql_database", "ghost" );
	string User = CFG.GetString( "db_mysql_user", string( ) );
	string Password = CFG.GetString( "db_mysql_password", string( ) );
	int Port = CFG.GetInt( "db_mysql_port", 0 );

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

	string QBegin = "BEGIN";

	if( mysql_real_query( Connection, QBegin.c_str( ), QBegin.size( ) ) != 0 )
	{
		cout << "error: " << mysql_error( Connection ) << endl;
		return 1;
	}

	queue<uint32_t> UnscoredGames;

	//string QSelectUnscored = "SELECT id FROM games WHERE id NOT IN ( SELECT gameid FROM dota_elo_games_scored ) ORDER BY id LIMIT 1";
	string QSelectUnscored = "SELECT id FROM games WHERE scored = 0 ORDER BY id";

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

	while( !UnscoredGames.empty( ) )
	{
		uint32_t GameID = UnscoredGames.front( );
		UnscoredGames.pop( );

		string QSelectPlayers = "SELECT dota_elo_scores.id, gameplayers.name, spoofedrealm, newcolour, winner, score, games.datetime, gameplayers.left, games.duration, gameplayers.leftreason, gameplayers.ip, games.gamename FROM dotaplayers LEFT JOIN dotagames ON dotagames.gameid=dotaplayers.gameid LEFT JOIN gameplayers ON gameplayers.gameid=dotaplayers.gameid AND gameplayers.colour=dotaplayers.colour LEFT JOIN dota_elo_scores ON (dota_elo_scores.name=gameplayers.name AND dota_elo_scores.season = 2) LEFT JOIN games ON games.id=dotaplayers.gameid WHERE dotaplayers.gameid=" + UTIL_ToString( GameID );

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

				bool 		ignore = false;
				string 		gametime;
				uint32_t 	rowids[10];
				string 		names[10];
				int 		num_players = 0;
				float 		player_ratings[10];
				int 		player_teams[10];
				int 		num_teams = 2;
				float 		team_ratings[2];
				float 		team_winners[2];
				int 		team_numplayers[2];
				int		team_leavers[2];
				float		team_bonus[2];
				bool		player_isleaver[10];
				float		player_left[10];

				team_ratings[0] = 0.0;
				team_ratings[1] = 0.0;
				team_numplayers[0] = 0;
				team_numplayers[1] = 0;
				team_leavers[0] = 0;
				team_leavers[1] = 0;
				team_bonus[0] = 0;
				team_bonus[1] = 0;

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
					else
					{
						team_winners[0] = (Winner == 1) ? 1.0 : 0.0;
						team_winners[1] = (Winner == 2) ? 1.0 : 0.0;
					}

					if( !Row[0].empty( ) )
						rowids[num_players] = UTIL_ToUInt32( Row[0] );
					else
						rowids[num_players] = 0;

					names[num_players] = Row[1];
					player_ratings[num_players] = 1000.0;

					if( !Row[5].empty( ) )
						player_ratings[num_players] = UTIL_ToFloat( Row[5] );

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

					player_isleaver[num_players] = false;

					float game_duration = UTIL_ToFloat(Row[8]);
					player_left[num_players] = UTIL_ToUInt32(Row[7]);

					if ((game_duration - (60 * 5)) > player_left[num_players])
					{
						player_isleaver[num_players] = true;
						team_leavers[player_teams[num_players]]++;

                                                bool doban = false;
                                                string ban = "INSERT INTO bans (botid, server, name, ip, date, gamename, admin, reason, ipban, expires)";
                                                string warn;
                                                if (Row[9] == "has left the game voluntarily")
                                                {
                                                        doban=true;
                                                        cout << "Autoban (leaver): " + names[num_players] << endl;
                                                        ban += " VALUES ('2', 'europe.battle.net', '" + names[num_players] + "', '" + Row[10] + "', CURRENT_TIME, '" + Row[11] + "', 'Autoban', 'Autoban: Leaver', 1, CURRENT_TIME + INTERVAL 3 DAY)";
                                                        warn = "INSERT INTO warnings (name, weight, warning_id, date, admin, game, note) VALUES ('" + names[num_players] + "', 30, 1, CURRENT_TIME, 'Autoban', '" + Row[11] + "', '" + Row[11] + "')";

                                                }
                                                else if (Row[9] == "lagged out (dropped by vote)" || Row[9] == "lagged out (dropped by admin)")
                                                {
                                                        doban=true;
                                                        cout << "Warning (disconnect): " << names[num_players] << endl;
                                                        //ban += " VALUES ('2', 'europe.battle.net', '" + names[num_players] + "', '" + Row[10] + "', NOW(), '" + Row[11] + "', 'Autoban', 'Autoban: Disconnect', '1', CURRENT_TIME + INTERVAL 1 HOUR)";
                                                        ban = "";
                                                        warn = "INSERT INTO warnings (name, weight, warning_id, date, admin, game, note) VALUES ('" + names[num_players] + "', 30, 8, CURRENT_TIME, 'Autoban', '" + Row[11] + "', '" + Row[11] + "')";
							cout << warn << endl;
                                                }

						if (doban)
                                                {
                                                        if (!ban.empty())
							{
								cout << ban << endl;
                                                                if( mysql_real_query( Connection, ban.c_str( ), ban.size( ) ) != 0 )
                                                                        cout << "error: " << mysql_error( Connection ) << endl;
							}

							cout << warn << endl;
                                                        if( mysql_real_query( Connection, warn.c_str( ), warn.size( ) ) != 0 )
                                                                cout << "error: " << mysql_error( Connection ) << endl;
                                                }


					}

					gametime = Row[6];

					num_players++;
					Row = MySQLFetchRow( Result );
				}

				mysql_free_result( Result );

				if (abs(team_leavers[0] - team_leavers[1]) > 0)
				{
					cout << "Sentinel leavers: " + UTIL_ToString(team_leavers[0]) + " Scourge Leavers: " + UTIL_ToString(team_leavers[1]) << endl;

					if (team_leavers[0] > team_leavers[1])
						team_bonus[0] = team_leavers[0] - team_leavers[1];
					else if (team_leavers[1] > team_leavers[0])
						team_bonus[1] = team_leavers[1] - team_leavers[0];
				}

				if( !ignore )
				{
					if( num_players == 0 )
						cout << "gameid " << UTIL_ToString( GameID ) << " has no players, ignoring" << endl;
					else if( team_numplayers[0] == 0 )
						cout << "gameid " << UTIL_ToString( GameID ) << " has no Sentinel players, ignoring" << endl;
					else if( team_numplayers[1] == 0 )
						cout << "gameid " << UTIL_ToString( GameID ) << " has no Scourge players, ignoring" << endl;
					else
					{
						cout << "gameid " << UTIL_ToString( GameID ) << " is calculating" << endl;

						float old_player_ratings[10];
						memcpy( old_player_ratings, player_ratings, sizeof( float ) * 10 );
						team_ratings[0] /= team_numplayers[0];
						team_ratings[1] /= team_numplayers[1];
						elo_recalculate_ratings( num_players, player_ratings, player_teams, num_teams, team_ratings, team_winners );

						for( int i = 0; i < num_players; i++ )
						{
							float gain = fabs(player_ratings[i] - old_player_ratings[i]);
							if (player_isleaver[i])
							{
								cout << "Player [" << names[i] << "] is a leaver, removing his score.." << endl;

								if (player_ratings[i] > old_player_ratings[i])
									player_ratings[i] = old_player_ratings[i] - (gain/2);
								else
									player_ratings[i] -= (gain/2);
							}				
							else if (!player_isleaver[i] && team_bonus[player_teams[i]] > 0)
							{
								float bonus = (team_bonus[player_teams[i]] == 1) ? gain/2 : gain;
								player_ratings[i] += bonus;	
								cout << " player [" << names[i] << "] is given a bonus of " << UTIL_ToString(bonus, 2) << endl;
							}

							cout << "player [" << names[i] << "] rating " << UTIL_ToString( (uint32_t)old_player_ratings[i] ) << " -> " << UTIL_ToString( (uint32_t)player_ratings[i] ) << endl;

							string QAddToGaintable = "INSERT INTO dota_elo_gains (gameid, timestamp, name, score, gain) VALUES ( " + UTIL_ToString(GameID) + ", '" + gametime + "', '" + names[i] + "', " +UTIL_ToString(player_ratings[i], 2) + ", " + UTIL_ToString(player_ratings[i] - old_player_ratings[i], 2) + " )";
							if( mysql_real_query( Connection, QAddToGaintable.c_str( ), QAddToGaintable.size( ) ) != 0 )
								cout << "error: " << mysql_error( Connection ) << endl;

							string EscName = MySQLEscapeString( Connection, names[i] );
							string QInsertScore = "INSERT INTO dota_elo_scores ( name, server, score, season ) VALUES ( '" + EscName + "', 'europe.battle.net', " + UTIL_ToString( player_ratings[i], 2 ) + ", 2 ) ON duplicate KEY UPDATE score = " + UTIL_ToString( player_ratings[i], 2 );
							if( mysql_real_query( Connection, QInsertScore.c_str( ), QInsertScore.size( ) ) != 0 )
								cout << "error: " << mysql_error( Connection ) << endl;

							string QUpdateScore2 = "UPDATE dotastats SET score=" + UTIL_ToString( player_ratings[i], 2 ) + " WHERE player_name LIKE '" + names[i] + "' AND season = 2";
							if( mysql_real_query( Connection, QUpdateScore2.c_str( ), QUpdateScore2.size( ) ) != 0 )
								cout << "error: " << mysql_error( Connection ) << endl;
						}
					}
				}				
			}
			else
				cout << "error: " << mysql_error( Connection ) << endl;
		}

		string QInsertScored = "UPDATE games SET scored = 1 WHERE id = " + UTIL_ToString( GameID );
		if( mysql_real_query( Connection, QInsertScored.c_str( ), QInsertScored.size( ) ) != 0 )
			cout << "error: " << mysql_error( Connection ) << endl;

		string QInsertScored2 = "INSERT INTO dota_elo_games_scored (gameid) VALUES (" + UTIL_ToString(GameID) + ")";
		if( mysql_real_query( Connection, QInsertScored2.c_str( ), QInsertScored2.size( ) ) != 0 )
			cout << "error: " << mysql_error( Connection ) << endl;
	}

	string QCommit = "COMMIT";

	if( mysql_real_query( Connection, QCommit.c_str( ), QCommit.size( ) ) != 0 )
	{
		cout << "error: " << mysql_error( Connection ) << endl;
	}

	return 0;
}
