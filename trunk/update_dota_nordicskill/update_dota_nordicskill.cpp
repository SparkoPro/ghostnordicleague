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
#include "elo.h"

#include <string.h>

#ifdef WIN32
 #include <winsock.h>
#endif

#include <mysql/mysql.h>

float gain_kill = 1; //UTIL_ToFloat( CFG.GetString( "formula_kill_gain", "" ) );
float gain_death = -2; //UTIL_ToFloat( CFG.GetString( "formula_death_gain", "" ) );
float gain_assist = 0.75; //UTIL_ToFloat( CFG.GetString( "formula_assist_gain", "" ) );
float gain_tower = 0.4; //UTIL_ToFloat( CFG.GetString( "formula_tower_gain", "" ) );
float gain_rax = 0.5; //UTIL_ToFloat( CFG.GetString( "formula_rax_gain", "" ) );
float gain_creepkill = 0.01; //= UTIL_ToFloat( CFG.GetString( "formula_creepkill_gain", "" ) );
float gain_creepdenie = 0.04; //UTIL_ToFloat( CFG.GetString( "formula_creepdenie_gain", "" ) );
float gain_neutral = 0.02; //UTIL_ToFloat( CFG.GetString( "formula_neutral_gain", "" ) );

float CalculateGain(int k, int d, int a, int ck, int cd, int t, int r, int n)
{
	float gain = 0;
	float total_gain = 0;

//	cout << "Calculating NordicSkill Rate.." << endl;

	gain = k * gain_kill;
	total_gain += gain;
//	cout << "Kill gain: " << gain << " Total: " << total_gain << endl;

	gain = d * gain_death;
	total_gain += gain;
//	cout << "Death gain: " << gain << " Total: " << total_gain << endl;

	gain = a * gain_assist;
	total_gain += gain;
//	cout << "Assist gain: " << gain << " Total: " << total_gain << endl;

	gain = ck * gain_creepkill;
	total_gain += gain;
//	cout << "Creepkill gain: " << gain << " Total: " << total_gain << endl;

	gain = cd * gain_creepdenie;
	total_gain += gain;
//	cout << "CreepDenie gain: " << gain << " Total: " << total_gain << endl;

	gain = t * gain_tower;
	total_gain += gain;
//	cout << "TowerKill gain: " << gain << " Total: " << total_gain << endl;

	gain = r * gain_rax;
	total_gain += gain;
//	cout << "Rax gain: " << gain << " Total: " << total_gain << endl;

	gain = n * gain_neutral;
	total_gain += gain;
//	cout << "NeutralKill gain: " << gain << " Total: " << total_gain << endl;


	return total_gain;
}



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

float UTIL_ToFloat( string s )
{
	float result;
	stringstream SS;
	SS << s;
	SS >> result;
	return result;
}

int main( int argc, char **argv )
{
	string CFGFile = "update_dota_nordicskill.cfg";

	if( argc > 1 && argv[1] )
		CFGFile = argv[1];

	CConfig CFG;
	CFG.Read( CFGFile );
	string Server = CFG.GetString( "db_mysql_server", string( ) );
	string Database = CFG.GetString( "db_mysql_database", "ghost" );
	string User = CFG.GetString( "db_mysql_user", string( ) );
	string Password = CFG.GetString( "db_mysql_password", string( ) );
	int Port = CFG.GetInt( "db_mysql_port", 0 );

	cout << "Gains:" << endl;
	cout << "Kill: " << gain_kill << endl;
	cout << "Death: " << gain_death << endl;
	cout << "Assist: " << gain_assist << endl;
	cout << "Tower: " << gain_tower << endl;
	cout << "Rax: " << gain_rax << endl;
	cout << "CK: " << gain_creepkill << endl;
	cout << "CD: " << gain_creepdenie << endl;
	cout << "Neutral: " << gain_neutral << endl;


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

	string QCreate1 = "CREATE TABLE IF NOT EXISTS dota_nskill_scores ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, name VARCHAR(15) NOT NULL, server VARCHAR(100) NOT NULL, score REAL NOT NULL )";
	string QCreate2 = "CREATE TABLE IF NOT EXISTS dota_nskill_games_scored ( last_gameid INT NOT NULL PRIMARY KEY )";

	if( mysql_real_query( Connection, QCreate1.c_str( ), QCreate1.size( ) ) != 0 )
	{
		cout << "error: " << mysql_error( Connection ) << endl;
		return 1;
	}

	if( mysql_real_query( Connection, QCreate2.c_str( ), QCreate2.size( ) ) != 0 )
	{
		cout << "error: " << mysql_error( Connection ) << endl;
		return 1;
	}

	cout << "getting unscored games" << endl;
	queue<uint32_t> UnscoredGames;

	string QSelectUnscored = "SELECT id FROM games WHERE id > ( SELECT last_gameid FROM dota_nskill_games_scored LIMIT 1 ) ORDER BY id";

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

	while( !UnscoredGames.empty( ) )
	{
		uint32_t GameID = UnscoredGames.front( );
		UnscoredGames.pop( );

		string QSelectPlayers = "SELECT dota_nskill_scores.id, gameplayers.name, spoofedrealm, newcolour, winner, score, kills, deaths, assists, creepkills, creepdenies, towerkills, raxkills, neutralkills FROM dotaplayers LEFT JOIN dotagames ON dotagames.gameid=dotaplayers.gameid LEFT JOIN gameplayers ON gameplayers.gameid=dotaplayers.gameid AND gameplayers.colour=dotaplayers.colour LEFT JOIN dota_nskill_scores ON dota_nskill_scores.name=gameplayers.name AND server=spoofedrealm WHERE dotaplayers.gameid=" + UTIL_ToString( GameID );

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
				float player_gain[10];
				int player_teams[10];

				int num_teams = 2;
				float team_ratings[2];
				float team_winners[2];
				int team_numplayers[2];
				team_ratings[0] = 0.0;
				team_ratings[1] = 0.0;
				team_numplayers[0] = 0;
				team_numplayers[1] = 0;

				uint32_t player_kills[10];
				uint32_t player_deaths[10];
				uint32_t player_assists[10];
				uint32_t player_creepkills[10];
				uint32_t player_creepdenies[10];
				uint32_t player_towerkills[10];
				uint32_t player_raxkills[10];
				uint32_t player_neutralkills[10];

				vector<string> Row = MySQLFetchRow( Result );

				while( Row.size( ) == 14 )
				{
					if( num_players >= 10 )
					{
						cout << "gameid " << UTIL_ToString( GameID ) << " has more than 10 players, ignoring" << endl;
						ignore = true;
						break;
					}

					uint32_t Winner = UTIL_ToUInt32( Row[4] );
					//cout << "kills: " << Row[6] << " Deaths: " << Row[7] << " Assists: " << Row[8] << " CK: " << Row[9] << " CD: " << Row[10] << " TK: " << Row[11] << " RK: " << Row[12] << " NK: " << Row[13] << endl;

					player_kills[num_players] = UTIL_ToUInt32( Row[6] );
					player_deaths[num_players] = UTIL_ToUInt32( Row[7] );
					player_assists[num_players] = UTIL_ToUInt32( Row[8] );
					player_creepkills[num_players] = UTIL_ToUInt32( Row[9] );
					player_creepdenies[num_players] = UTIL_ToUInt32( Row[10] );
					player_towerkills[num_players] = UTIL_ToUInt32( Row[11] );
					player_raxkills[num_players] = UTIL_ToUInt32( Row[12] );
					player_neutralkills[num_players] = UTIL_ToUInt32( Row[13] );

					if( Winner != 1 && Winner != 2 )
					{
						//cout << "gameid " << UTIL_ToString( GameID ) << " has no winner, ignoring" << endl;
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

					if( !Row[5].empty( ) )
					{
						exists[num_players] = true;
						player_ratings[num_players] = UTIL_ToFloat( Row[5] );
					}
					else
					{
						//cout << "new player [" << Row[1] << "] found" << endl;
						exists[num_players] = false;
						player_ratings[num_players] = 1000.0;
					}

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
						//cout << "gameid " << UTIL_ToString( GameID ) << " has a player with an invalid newcolour, ignoring" << endl;
						ignore = true;
						break;
					}

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
							player_gain[i] = CalculateGain(player_kills[i], player_deaths[i], player_assists[i], player_creepkills[i], player_creepdenies[i], player_towerkills[i], player_raxkills[i], player_neutralkills[i]);

							//cout << "player [" << names[i] << "] rating " << UTIL_ToString( (uint32_t)old_player_ratings[i] ) << " -> " << UTIL_ToString( (uint32_t)player_ratings[i] ) << " Gain: " << player_gain[i] << endl;
							player_ratings[i] += player_gain[i];


							if( exists[i] )
							{
								string QUpdateScore = "UPDATE dota_nskill_scores SET score=" + UTIL_ToString( player_ratings[i], 2 ) + " WHERE id=" + UTIL_ToString( rowids[i] );

								if( mysql_real_query( Connection, QUpdateScore.c_str( ), QUpdateScore.size( ) ) != 0 )
								{
									cout << "error: " << mysql_error( Connection ) << endl;
									return 1;
								}
							}
							else
							{
								string EscName = MySQLEscapeString( Connection, names[i] );
								string EscServer = MySQLEscapeString( Connection, servers[i] );
								string QInsertScore = "INSERT INTO dota_nskill_scores ( name, server, score ) VALUES ( '" + EscName + "', '" + EscServer + "', " + UTIL_ToString( player_ratings[i], 2 ) + " )";

								if( mysql_real_query( Connection, QInsertScore.c_str( ), QInsertScore.size( ) ) != 0 )
								{
									cout << "error: " << mysql_error( Connection ) << endl;
									return 1;
								}
							}
						}
					}
				}				
			}
			else
			{
				cout << "error: " << mysql_error( Connection ) << endl;
				return 1;
			}
		}

		string QInsertScored = "UPDATE dota_nskill_games_scored set last_gameid = " + UTIL_ToString( GameID );

		if( mysql_real_query( Connection, QInsertScored.c_str( ), QInsertScored.size( ) ) != 0 )
		{
			cout << "error: " << mysql_error( Connection ) << endl;
			return 1;
		}
	}

	cout << "copying dota nskill scores to scores table" << endl;

	string QCopyScores1 = "DELETE FROM scores WHERE category='dota_nskill'";
	string QCopyScores2 = "INSERT INTO scores ( category, name, server, score ) SELECT 'dota_nskill', name, server, score FROM dota_nskill_scores";

	if( mysql_real_query( Connection, QCopyScores1.c_str( ), QCopyScores1.size( ) ) != 0 )
	{
		cout << "error: " << mysql_error( Connection ) << endl;
		return 1;
	}

	if( mysql_real_query( Connection, QCopyScores2.c_str( ), QCopyScores2.size( ) ) != 0 )
	{
		cout << "error: " << mysql_error( Connection ) << endl;
		return 1;
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


