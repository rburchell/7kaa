/*
 * Seven Kingdoms: Ancient Adversaries
 *
 * Copyright 1997,1998 Enlight Software Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Filename    : netplay_none.cpp
// Description : MultiPlayerSDL, SDL_net based multiplayer class
// Onwer       : Gilbert

#include <netplay.h>
#include <ALL.h>
#include <string.h>
#include <OVGALOCK.h>
#include <OBLOB.h>
#include <stdint.h>
#include <dbglog.h>

DBGLOG_DEFAULT_CHANNEL(NetPlay);

SDLSessionDesc::SDLSessionDesc()
{
	ERR("[SDLSessionDesc::SDLSessionDesc] calling unimplemented method\n");
}

SDLSessionDesc::SDLSessionDesc(const SDLSessionDesc &SDLSessionDesc) //: SDLSessionDesc2(SDLSessionDesc)
{
	ERR("[SDLSessionDesc::SDLSessionDesc(const SDLSessionDesc &)] calling unimplemented method\n");
}

SDLSessionDesc& SDLSessionDesc::operator= (const SDLSessionDesc &src)
{
	ERR("[SDLSessionDesc::operator=] calling unimplemented method\n");
	return *this;
}

// to start a multiplayer game, first check if it is called from a
// lobbied (MultiPlayerSDL::is_lobbied)

// if it is a lobbied, call init_lobbied before create_player

// if not, call poll_service_provider; display them and let
// user to select, call init and pass the guid of the selected
// service; create_session or poll_sessions+join_session;
// finally create_player.

MultiPlayerSDL::MultiPlayerSDL() :
	current_sessions(sizeof(SDLSessionDesc), 10 ), player_pool(sizeof(SDLPlayer), 8 ),
	recv_buffer(NULL)
{
	ERR("[MultiPlayerSDL::MultiPlayerSDL] calling unimplemented method\n");
}

MultiPlayerSDL::~MultiPlayerSDL()
{
	ERR("[MultiPlayerSDL::~MultiPlayerSDL] calling unimplemented method\n");
}

void MultiPlayerSDL::init(ProtocolType protocol_type)
{
	ERR("[MultiPlayerSDL::init()] calling unimplemented method\n");
}

void MultiPlayerSDL::deinit()
{
	ERR("[MultiPlayerSDL::deinit] calling unimplemented method\n");	
}

void MultiPlayerSDL::init_lobbied(int maxPlayers, char *)
{
	ERR("[MultiPlayerSDL::init_lobbied] calling unimplemented method\n");
}

// return 0=not lobbied, 1=auto create, 2=auto join, 4=selectable
int MultiPlayerSDL::is_lobbied()
{
	return lobbied_flag;
}

char *MultiPlayerSDL::get_lobbied_name()
{
	ERR("[MultiPlayerSDL::get_lobbied_name] calling unimplemented method\n");
	return NULL;
}

void MultiPlayerSDL::poll_supported_protocols()
{
	supported_protocols = TCPIP;
}

bool MultiPlayerSDL::is_protocol_supported(ProtocolType protocol)
{
	return (protocol & supported_protocols) != 0;
}

int MultiPlayerSDL::poll_sessions()
{
	ERR("[MultiPlayerSDL::poll_sessions] calling unimplemented method\n");
	return FALSE;
}

// return a session description
//
// <int> i			i-th session (i start from 1)
// return pointer to a session, NULL if no more
SDLSessionDesc *MultiPlayerSDL::get_session(int i)
{
	ERR("[MultiPlayerSDL::get_session] calling unimplemented method\n");
	return NULL;
}

// create a new session
//
// <char *> sessionName      arbitary name to identify a session, input from user
// <int>    maxPlayers       maximum no. of players in a session
//
// return TRUE if success
int MultiPlayerSDL::create_session(char *sessionName, int maxPlayers)
{
	ERR("[MultiPlayerSDL::create_session] calling unimplemented method\n");
	return FALSE;
}

// join a session, by passing the index passed into get_session()
// note : do not call poll_sessions between get_session and join_session
//
// <int> currentSessionIndex       the index passed into get_session()
//
// currentSessionIndex start from 1
int MultiPlayerSDL::join_session(int currentSessionIndex)
{
	ERR("[MultiPlayerSDL::join_session] calling unimplemented method\n");
	return FALSE;
}

void MultiPlayerSDL::close_session()
{
	ERR("[MultiPlayerSDL::close_session] calling unimplemented method\n");
}

void MultiPlayerSDL::disable_join_session()
{
	ERR("[MultiPlayerSDL::disable_join_session] calling unimplemented method\n");
}

// create a local player
//
// <char *> friendlyName          short name of the player, best to be one word only
// [char *] formalName            long name of the player, take friendlyName if NULL (default: NULL)
// return TRUE if success
//
int MultiPlayerSDL::create_player(char *friendlyName, char *formalName)
{
	ERR("[MultiPlayerSDL::create_player] calling unimplemented method\n");
	return FALSE;
}

void MultiPlayerSDL::poll_players()
{
	ERR("[MultiPlayerSDL::poll_players] calling unimplemented method\n");
}

SDLPlayer *MultiPlayerSDL::get_player(int i)
{
	if( i <= 0 || i > player_pool.size() )
		return NULL;
	return (SDLPlayer *)player_pool.get(i);
}

SDLPlayer *MultiPlayerSDL::search_player(uint32_t playerId)
{
	SDLPlayer *player;
	int i = 0;
	while( (player = get_player(++i)) != NULL )
		if( player->player_id == playerId )
			return player;
	return NULL;
}

// determine whether a player is lost
//
// MultiPlayerSDL::received must be called (or remote.poll_msg) , 
// so if a player is really lost, the system message from 
// directPlay is received
//
int MultiPlayerSDL::is_player_connecting(uint32_t playerId)
{
	for( int p = 1; p <= player_pool.size(); ++p)
	{
		SDLPlayer * nonePlayer = (SDLPlayer *) player_pool.get(p);
		if( nonePlayer->player_id == playerId )
		{
			return nonePlayer->connecting;
		}
	}
	return 0;
}

// send message
//
// pass BROADCAST_PID as toId to all players
//
// return TRUE on success
//
int MultiPlayerSDL::send(uint32_t toId, void * lpData, uint32_t dataSize)
{
	ERR("[MultiPlayerSDL::send] calling unimplemented method\n");
	return FALSE;
}

// send message
//
// pass BROADCAST_PID as toId to all players
//
// return TRUE on success
//
int MultiPlayerSDL::send_stream(uint32_t toId, void * lpData, uint32_t dataSize)
{
	ERR("[MultiPlayerSDL::send_stream] calling unimplemented method\n");
	return FALSE;
}

// return NULL if fails
// sysMsgCount records how many system messages have been handled
// notice : *sysMsgCount may be != 0, but return NULL
//
char *MultiPlayerSDL::receive(uint32_t * from, uint32_t * to, uint32_t * dSize, int *sysMsgCount)
{
	ERR("[MultiPlayerSDL::receive] calling unimplemented method\n");
	return NULL;
}

/*
static int sort_session_id(const void *a, const void *b)
{
	return memcmp( &((SDLSessionDesc *)a)->guidInstance, &((SDLSessionDesc *)b)->guidInstance,
		sizeof(GUID) );
}
*/

static int sort_session_name(const void *a, const void *b)
{
	return strcmp( ((SDLSessionDesc *)a)->name_str(), ((SDLSessionDesc *)b)->name_str() );
}

// sort current_sessions
// <int> sortType, 1=sort by GUID, 2=sort by session name
void MultiPlayerSDL::sort_sessions(int sortType )
{
	ERR("[MultiPlayerSDL::sort_sessions] calling partially implemented method\n");

	// BUGHERE : quick_sort is a DynArray function but current_sessions is DynArrayB
	switch(sortType)
	{
	case 1:
		//current_sessions.quick_sort(sort_session_id);
		break;
	case 2:
		current_sessions.quick_sort(sort_session_name);
		break;
	default:
		err_here();
	}
}

