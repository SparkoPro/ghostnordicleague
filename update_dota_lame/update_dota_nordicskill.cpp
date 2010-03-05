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

string DBTable = "dota_lame_scores";
string DBControllTable = "dota_lame_games_scored";
string DBGainTable = "dota_lame_gains";
string DBScoreCategory = "dota_lame";

float gain_kill = 0.65; //UTIL_ToFloat( CFG.GetString( "formula_kill_gain", "" ) );
float gain_death = -1.2; //UTIL_ToFloat( CFG.GetString( "formula_death_gain", "" ) );
float gain_assist = 0.5; //UTIL_ToFloat( CFG.GetString( "formula_assist_gain", "" ) );
float gain_tower = 0.275; //UTIL_ToFloat( CFG.GetString( "formula_tower_gain", "" ) );
float gain_rax = 0.29; //UTIL_ToFloat( CFG.GetString( "formula_rax_gain", "" ) );
float gain_creepkill = 0.01; //= UTIL_ToFloat( CFG.GetString( "formula_creepkill_gain", "" ) );
float gain_creepdenie = 0.1; //UTIL_ToFloat( CFG.GetString( "formula_creepdenie_gain", "" ) );
float gain_neutral = 0.05; //UTIL_ToFloat( CFG.GetString( "formula_neutral_gain", "" ) );


float GetHeroBonus(string hero, MYSQL *Connection);

float CalculateGain(int k, int d, int a, int ck, int cd, int t, int r, int n)
{
	float gain = 0;
	float total_gain = 0;

	gain = k * gain_kill;
	total_gain += gain;

	gain = d * gain_death;
	total_gain += gain;

	gain = a * gain_assist;
	total_gain += gain;

	gain = ck * gain_creepkill;
	total_gain += gain;

	gain = cd * gain_creepdenie;
	total_gain += gain;

	gain = t * gain_tower;
	total_gain += gain;

	gain = r * gain_rax;
	total_gain += gain;

	gain = n * gain_neutral;
	total_gain += gain;
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

int32_t UTIL_ToInt32( string &s )
{
        int32_t result;
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

/*	cout << "Gains:" << endl;
	cout << "Kill: " << gain_kill << endl;
	cout << "Death: " << gain_death << endl;
	cout << "Assist: " << gain_assist << endl;
	cout << "Tower: " << gain_tower << endl;
	cout << "Rax: " << gain_rax << endl;
	cout << "CK: " << gain_creepkill << endl;
	cout << "CD: " << gain_creepdenie << endl;
	cout << "Neutral: " << gain_neutral << endl;
*/

	//cout << "connecting to database server" << endl;
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

	//GetHeroBonus("H00H", Connection);

	//cout << "connected" << endl;
	//cout << "beginning transaction" << endl;

	string QBegin = "BEGIN";

	if( mysql_real_query( Connection, QBegin.c_str( ), QBegin.size( ) ) != 0 )
	{
		cout << "error: " << mysql_error( Connection ) << endl;
		return 1;
	}

	//cout << "creating tables" << endl;

	string QCreate1 = "CREATE TABLE IF NOT EXISTS " + DBTable + " ( id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, name VARCHAR(15) NOT NULL, server VARCHAR(100) NOT NULL, score REAL NOT NULL )";
	string QCreate2 = "CREATE TABLE IF NOT EXISTS " + DBControllTable + " ( last_gameid INT NOT NULL PRIMARY KEY )";

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

	//cout << "getting unscored games" << endl;
	queue<uint32_t> UnscoredGames;

	string QSelectUnscored = "SELECT id FROM games WHERE id > ( SELECT last_gameid FROM " + DBControllTable + " LIMIT 1 ) ORDER BY id";

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

/*		string QSelectPlayers = "SELECT " + DBTable + ".id, gameplayers.name, spoofedrealm, newcolour, winner, score, kills, deaths, assists, creepkills, creepdenies, towerkills, raxkills, 
neutralkills, dotaplayers.botid, dotaplayers.hero, gameplayers.left, min, sec FROM dotaplayers LEFT JOIN dotagames ON dotagames.gameid=dotaplayers.gameid LEFT JOIN gameplayers ON 
gameplayers.gameid=dotaplayers.gameid AND gameplayers.colour=dotaplayers.colour LEFT JOIN " + DBTable + " ON LOWER(" + DBTable + ".name)=LOWER(gameplayers.name) AND server=spoofedrealm WHERE 
dotaplayers.gameid=" + UTIL_ToString( GameID );
*/

		string QSelectPlayers = "SELECT " + DBTable + ".id, gameplayers.name, spoofedrealm, newcolour, winner, score, kills, deaths, assists, creepkills, creepdenies, towerkills, raxkills,neutralkills, dotaplayers.botid, dotaplayers.hero, gameplayers.left, min, sec, games.duration FROM dotaplayers LEFT JOIN dotagames ON dotagames.gameid=dotaplayers.gameid LEFT JOIN gameplayers ON gameplayers.gameid=dotaplayers.gameid AND gameplayers.colour=dotaplayers.colour LEFT JOIN " + DBTable + " ON LOWER(" + DBTable + ".name)=LOWER(gameplayers.name) AND server='europe.battle.net' LEFT JOIN games on games.id=dotaplayers.gameid WHERE dotaplayers.gameid=" + UTIL_ToString( GameID );

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
				
				uint32_t BotID;

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

				uint32_t team_leavers[2];

				team_leavers[0] = 0;
				team_leavers[1] = 0;

				uint32_t player_kills[10];
				uint32_t player_deaths[10];
				uint32_t player_assists[10];
				uint32_t player_creepkills[10];
				uint32_t player_creepdenies[10];
				uint32_t player_towerkills[10];
				uint32_t player_raxkills[10];
				uint32_t player_neutralkills[10];
				uint32_t player_colours[10];

				string	 player_heroes[10];

				bool	 player_isleaver[10];
				float	 player_left[10];

				float	player_scale[10];

				
				vector<string> Row = MySQLFetchRow( Result );

				while( Row.size( ) == 20 )
				{
					if( num_players >= 10 )
					{
						cout << "gameid " << UTIL_ToString( GameID ) << " has more than 10 players, ignoring" << endl;
						ignore = true;
						break;
					}

					uint32_t Winner = UTIL_ToUInt32( Row[4] );
					//cout << "kills: " << Row[6] << " Deaths: " << Row[7] << " Assists: " << Row[8] << " CK: " << Row[9] << " CD: " << Row[10] << " TK: " << Row[11] << " RK: " << Row[12] << " NK: " << Row[13] << endl;

					player_colours[num_players] = UTIL_ToUInt32( Row[3] );
					player_kills[num_players] = UTIL_ToUInt32( Row[6] );
					player_deaths[num_players] = UTIL_ToUInt32( Row[7] );
					player_assists[num_players] = UTIL_ToUInt32( Row[8] );
					player_creepkills[num_players] = UTIL_ToUInt32( Row[9] );
					player_creepdenies[num_players] = UTIL_ToUInt32( Row[10] );
					player_towerkills[num_players] = UTIL_ToUInt32( Row[11] );
					player_raxkills[num_players] = UTIL_ToUInt32( Row[12] );
					player_neutralkills[num_players] = UTIL_ToUInt32( Row[13] );
					BotID = UTIL_ToUInt32( Row[14] );


					player_heroes[num_players] = Row[15];
					player_left[num_players] = UTIL_ToFloat(Row[16]);


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
						player_ratings[num_players] = 0.0; //1000.0;
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


					// We want to keep track of leavers and adjust score for loosing team if they are less than winning team..

					//float game_min = UTIL_ToFloat(Row[17]);
					//float game_sec = UTIL_ToFloat(Row[18]);

					float game_duration = UTIL_ToFloat(Row[19]);
					
					//cout << "Game duration: " + UTIL_ToString(game_duration, 0) << endl;

					// ok we got game duration
					// flag player as leaver if he left more than X minutes before game end

					if ((game_duration - (60 * 5)) > player_left[num_players])
					{
						cout << "We got a premature leaver." << endl;
						player_isleaver[num_players] = true;
						team_leavers[player_teams[num_players]]++;
					}
					else
					{
						player_isleaver[num_players] = false;
					}

					if (player_left[num_players] >= game_duration)
						player_scale[num_players] = 1.0;
					else if (player_left[num_players] > 0)
					{
						player_scale[num_players] = player_left[num_players] / game_duration;
						if (player_scale[num_players] > 0.99)
							player_scale[num_players] = 1.0;
					}
					else
						player_scale[num_players] = 0;

					num_players++;
					Row = MySQLFetchRow( Result );
				}

				cout << "Sentinel leavers: " + UTIL_ToString(team_leavers[0]) + " Scourge Leavers: " + UTIL_ToString(team_leavers[1]) << endl;


				/*
				if (team_leavers[0] > team_leavers[1])
					// give sent some kind of reduction
				else if (team_leavers[1] < team_leavers[0])
					// give scourge some kind if reduction
				else
					// no leavers or equal teams.
				*/

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
						float scourge_bonus, sentinel_bonus;
						float scourge_hero_bonus, sentinel_hero_bonus;

						for (int i = 0; i < num_players; i++)
						{
							if (player_teams[i] == 0)
								sentinel_hero_bonus -= GetHeroBonus(player_heroes[i], Connection);
							else
								scourge_hero_bonus -= GetHeroBonus(player_heroes[i], Connection);
						}
						
						sentinel_bonus = sentinel_hero_bonus - scourge_hero_bonus;			
						scourge_bonus = scourge_hero_bonus - sentinel_hero_bonus;


						cout << "Sentinel hero bonus: " << UTIL_ToString(sentinel_bonus, 2) << endl;			
						cout << "Scourge hero bonus: " << UTIL_ToString(scourge_bonus, 2) << endl;			

						for( int i = 0; i < num_players; i++ )
						{

							//cout << "Hero bonus: " + UTIL_ToString(GetHeroBonus(player_heroes[i], Connection), 2) << endl;

							if (player_ratings[i] < 0)
								player_ratings[i] = 0;

							string EscName = MySQLEscapeString( Connection, names[i] );
							player_gain[i] = CalculateGain(player_kills[i], player_deaths[i], player_assists[i], player_creepkills[i], player_creepdenies[i], player_towerkills[i], player_raxkills[i], player_neutralkills[i]);

							player_gain[i] -= GetHeroBonus(player_heroes[i], Connection);

							if (player_teams[i] == 0)
								player_gain[i] += sentinel_bonus;
							else
								player_gain[i] += scourge_bonus;


							if (player_ratings[i] == 0 && player_gain[i] < 0)
							{
								player_gain[i] = 0;
							}
							else if ((player_ratings[i] + player_gain[i]) < 0)
							{
								cout << "Gain would get rating under 0. Gain: " + UTIL_ToString(player_gain[i], 2);
								float n = (player_ratings[i] + player_gain[i]) * -1;
								player_gain[i] += n;
								cout << " Diff: " + UTIL_ToString(n, 2) + " New Gain: " + UTIL_ToString(player_gain[i], 2);
								cout << " New score: " + UTIL_ToString(player_ratings[i] + player_gain[i], 2) << endl;
							}

							float new_gain = (player_ratings[i] - old_player_ratings[i]) + player_gain[i];

							/*
							 * Compensate for leavers, leavers will still loose full.
							 */
							// todo: have to come up with a good algorithm for loss-reduction.
							
							// if (team_leavers[player_teams[i]])

							float unscaled_score = (player_ratings[i] - old_player_ratings[i] + player_gain[i]);
							float scaled_score = (player_ratings[i] - old_player_ratings[i] + player_gain[i]) * player_scale[i];

							cout << "Scaled score (scale: " + UTIL_ToString(player_scale[i], 8) + ") would give: " + UTIL_ToString(scaled_score, 2) + " Instead of: " + UTIL_ToString(unscaled_score, 2) << endl;

							//player_ratings[i] -= old_player_ratings[i];
							player_ratings[i] += player_gain[i];
							/*player_ratings[i] *= player_scale[i];
							player_ratings[i] += old_player_ratings[i];*/

							string QAddToGaintable = "INSERT INTO " + DBGainTable + " (gameid, botid, name, colour, score, gain) VALUES ( " + UTIL_ToString(GameID) + ", " + UTIL_ToString(BotID) + ", '" + EscName + "', " + UTIL_ToString(player_colours[i]) + ", " + UTIL_ToString(old_player_ratings[i], 2) + ", " + UTIL_ToString(new_gain, 2) + " )";
							cout << QAddToGaintable << endl;
							
							if( mysql_real_query( Connection, QAddToGaintable.c_str( ), QAddToGaintable.size( ) ) != 0 )
							{
								cout << "error: " << mysql_error( Connection ) << endl;
								return 1;
							}
							



							if( exists[i] )
							{
								string QUpdateScore = "UPDATE " + DBTable + " SET score=" + UTIL_ToString( player_ratings[i], 2 ) + " WHERE id=" + UTIL_ToString( rowids[i] );

								if( mysql_real_query( Connection, QUpdateScore.c_str( ), QUpdateScore.size( ) ) != 0 )
								{
									cout << "error: " << mysql_error( Connection ) << endl;
									return 1;
								}
							}
							else
							{
								string EscServer = MySQLEscapeString( Connection, servers[i] );
								string QInsertScore = "INSERT INTO " + DBTable + " ( name, server, score ) VALUES ( '" + EscName + "', 'europe.battle.net', " + UTIL_ToString( player_ratings[i], 2 ) + " )";

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

		string QInsertScored = "UPDATE " + DBControllTable + " set last_gameid = " + UTIL_ToString( GameID );

		if( mysql_real_query( Connection, QInsertScored.c_str( ), QInsertScored.size( ) ) != 0 )
		{
			cout << "error: " << mysql_error( Connection ) << endl;
			return 1;
		}
	}

	if (CopyScores)
	{
		cout << "copying dota lame scores to scores table" << endl;

		string QCopyScores1 = "DELETE FROM scores WHERE category='" + DBScoreCategory + "'";
		string QCopyScores2 = "INSERT INTO scores ( category, name, server, score ) SELECT '" + DBScoreCategory + "', name, server, score FROM " + DBTable;

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
	}

	//cout << "committing transaction" << endl;

	string QCommit = "COMMIT";

	if( mysql_real_query( Connection, QCommit.c_str( ), QCommit.size( ) ) != 0 )
	{
		cout << "error: " << mysql_error( Connection ) << endl;
		return 1;
	}

	//cout << "Running score decay.." << endl;

	/*
	 * Execute score decay once per day
	 * (also rebuilds hero-balance table)
	 * 
	 */

	string QLastDecay = "SELECT UNIX_TIMESTAMP() - UNIX_TIMESTAMP(last_update) FROM dota_score_decay";

        if( mysql_real_query( Connection, QLastDecay.c_str( ), QLastDecay.size( ) ) != 0 )
        {
                cout << "error: " << mysql_error( Connection ) << endl;
                return 1;
        }
        else
        {

		MYSQL_RES *dResult = mysql_store_result( Connection );
		
		if ( dResult )
		{
			
			vector<string> dRow = MySQLFetchRow( dResult );

			if (!dRow.empty() && UTIL_ToUInt32(dRow[0]) > 21600)
			{
				cout << "Executing score decay algorithm..." << endl;
				string QFindScoreDecays = "select name, score, (score - (score - (score * 0.01))) * -1 as gain from scores where name IN (select distinct(name) from " + DBGainTable + " where name NOT IN (select DISTINCT(name) from " + DBGainTable + " where UNIX_TIMESTAMP(timestamp) > (UNIX_TIMESTAMP() - 345600)) and name NOT LIKE '') AND category = '" + DBScoreCategory + "'";
	
				if( mysql_real_query( Connection, QFindScoreDecays.c_str( ), QFindScoreDecays.size( ) ) != 0 )
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
							string QUpdateScore = "CALL AddScoreDecay('" + Row[0] + "', " + Row[1] + ", " + Row[2] + ")"; 
				
							if( mysql_real_query( Connection, QUpdateScore.c_str( ), QUpdateScore.size( ) ) != 0 )
	        					{
               							cout << "error: " << mysql_error( Connection ) << endl;
               							return 1;
        						}
							else
							{
								cout << "Score decay for " + Row[0] + " added (" + Row[2] + ")" << endl;
							}
	
							Row = MySQLFetchRow( Result );
						}

						mysql_free_result( Result );

						string QSetLastUpdate = "UPDATE dota_score_decay SET last_update = NOW()";

						if( mysql_real_query( Connection, QSetLastUpdate.c_str( ), QSetLastUpdate.size( ) ) != 0 )
                                                {
                                                	cout << "error: " << mysql_error( Connection ) << endl;
                                                        return 1;
                                                }
						else
							cout << "Updating score decay last_update to current time." << endl;
					

					}
					else
					{
						cout << "error: " << mysql_error( Connection ) << endl;
						return 1;
					}


					/*
					 * Update herostats bonus system
					 */

					string QHeroStatsClean = "DELETE FROM herostats";
					string QHeroStats = "INSERT INTO herostats (count, name, hero, result) SELECT count(dota_entitys.name) as ncount, dota_entitys.name, hero, SUM(IF(winner > 0, IF((newcolour <= 5 && dotagames.winner = 1) || (newcolour >= 7 && dotagames.winner=2), 1, -1), 0)) as nresult FROM `dotaplayers` LEFT JOIN dotagames on dotagames.gameid = dotaplayers.gameid LEFT JOIN dota_entitys on dota_entitys.entity_id = hero WHERE name NOT LIKE '' group by name";

					if ( mysql_real_query( Connection, QHeroStatsClean.c_str( ), QHeroStatsClean.size( ) ) != 0 )
                                        {
                                                cout << "error: " << mysql_error( Connection ) << endl;
   						return 1;
                                        }
					else
						cout << "Cleaned herostats, trying to repopulate... ";

					if ( mysql_real_query( Connection, QHeroStats.c_str( ), QHeroStats.size( ) ) != 0 )
                                        {
                                                cout << "error: " << mysql_error( Connection ) << endl;
   						return 1;
                                        }
					else
						cout << "success!" << endl;

				}
			}
			else
				cout << "Score decay not ready to run yet." << endl;
		}
	}

	//cout << "done" << endl;

	return 0;
}


float GetHeroBonus(string hero, MYSQL *Connection)
{
	int sum, count;
	float bonus;
	
        string QGetHero = "select result,count from dota_entitys LEFT JOIN herostats ON herostats.name = dota_entitys.name where dota_entitys.entity_id LIKE '" + hero + "' LIMIT 1";
	//cout << "Hero sql: " << QGetHero << endl;

        if( mysql_real_query( Connection, QGetHero.c_str( ), QGetHero.size( ) ) != 0 )
        {
                cout << "error: " << mysql_error( Connection ) << endl;
                return 0;
        }
        else
        {
                MYSQL_RES *Result = mysql_store_result( Connection );
		//cout << "trying.";
                if( Result )
                {
			//cout << ".";
                        vector<string> Row = MySQLFetchRow( Result );

			if ( Row.size() == 2)
			{
				float res, count;
				
				res = UTIL_ToFloat(Row[0]);
				count = UTIL_ToFloat(Row[1]);
				//cout << ". done." << endl;
				bonus = (res / count) * 4;

				//cout << "result: " + UTIL_ToString(res, 2) + " Count: " + UTIL_ToString(count, 2) + " Bonus: " + UTIL_ToString(bonus, 2) << endl;
			}
			mysql_free_result(Result);
		}
	}

	return bonus;
}
