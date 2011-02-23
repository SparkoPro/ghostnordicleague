/*

Copyright [2008] [Fredrik Wallin]

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

using namespace std;

#ifdef WIN32
#include "ms_stdint.h"
#else
#include <stdint.h>
#endif

#include "config.h"

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
	string Season = CFG.GetString( "db_current_season", "2" );

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

        string QCleanEvents = "DELETE FROM `dotaevents` WHERE timestamp < CURRENT_TIMESTAMP - INTERVAL 1 DAY";
        if( mysql_real_query( Connection, QCleanEvents.c_str( ), QCleanEvents.size( ) ) != 0 )
	{
                cout << "error: QCleanEvents :" << mysql_error( Connection ) << endl;
	}


	///
	// Begin score decay
	///

	string QBegin = "BEGIN";

	if( mysql_real_query( Connection, QBegin.c_str( ), QBegin.size( ) ) != 0 )
	{
		cout << "error: " << mysql_error( Connection ) << endl;
		return 1;
	}

	string QSelectDecayed = "SELECT player_name, score FROM dotastats WHERE last_activity < CURRENT_TIMESTAMP - INTERVAL 7 DAY AND season = 2 AND score > 1000";
	//cout << QSelectDecayed << endl;

	if( mysql_real_query( Connection, QSelectDecayed.c_str( ), QSelectDecayed.size( ) ) != 0 )
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

			while( Row.size( ) == 2 )
			{
				float score = UTIL_ToFloat(Row[1]);
				float gain = (score * 0.01) * -1;

				string QUpdateScore = "CALL AddScoreDecayELO('" + Row[0] + "', " + UTIL_ToString(score + gain, 2) + ", " + UTIL_ToString(gain, 2) + ")";

				if( mysql_real_query( Connection, QUpdateScore.c_str( ), QUpdateScore.size( ) ) != 0 )
				{
					cout << "error: " << mysql_error( Connection ) << endl;

					mysql_free_result( Result );
					return 1;
				}
				else
					cout << "Decaying score for [" + Row[0] + "] " + UTIL_ToString(score, 2) + " > " + UTIL_ToString(score+gain, 2) + " [" + UTIL_ToString(gain, 2) + "]" << endl;

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

	string QSetLastUpdate = "UPDATE nordicleague SET last_score_decay = CURRENT_TIMESTAMP";
	if( mysql_real_query( Connection, QSetLastUpdate.c_str( ), QSetLastUpdate.size( ) ) != 0 )
		cout << "error: " << mysql_error( Connection ) << endl;

	string QCommit = "COMMIT";

	if( mysql_real_query( Connection, QCommit.c_str( ), QCommit.size( ) ) != 0 )
		cout << "error: " << mysql_error( Connection ) << endl;


	///
	// Update herostats bonus system
	///

	/*
	string QHeroStatsClean = "DELETE FROM herostats";
	string QHeroStats = "INSERT INTO herostats (count, name, hero, result) SELECT count(dota_entitys.name) as ncount, dota_entitys.name, hero, SUM(IF(winner > 0, IF((newcolour <= 5 && dotagames.winner = 1) || (newcolour >= 7 && dotagames.winner=2), 1, -1), 0)) as nresult FROM `dotaplayers` LEFT JOIN dotagames on dotagames.gameid = dotaplayers.gameid LEFT JOIN dota_entitys on dota_entitys.entity_id = hero WHERE name NOT LIKE '' group by name";

	if ( mysql_real_query( Connection, QHeroStatsClean.c_str( ), QHeroStatsClean.size( ) ) != 0 )
	{
		cout << "error: " << mysql_error( Connection ) << endl;
	}
	else
		cout << "Cleaned herostats, trying to repopulate... ";

	if ( mysql_real_query( Connection, QHeroStats.c_str( ), QHeroStats.size( ) ) != 0 )
	{
		cout << "error: " << mysql_error( Connection ) << endl;
	}
	else
		cout << "success!" << endl;
	*/

	return 0;
}
