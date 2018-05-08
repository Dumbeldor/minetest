/*
Minetest
Copyright (C) 2018 Dumbeldor, Vincent Glize <vincent.glize@live.fr>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "client\discord.h"
#include <ctime>
#include "discord_rpc.h"
#include <string>
#include <assert.h>
#include <log.h>

std::unique_ptr<Discord> Discord::s_pDiscord;
const std::string Discord::s_applicationId = "443156798510333954";

Discord::Discord()
{
	m_data.startTimestamp = time(0);
}

Discord::~Discord()
{
	assert(s_pDiscord);
	Discord_Shutdown();
}

void Discord::create()
{
	assert(!s_pDiscord);
	s_pDiscord = std::unique_ptr<Discord>(new Discord());
}

void Discord::init()
{
	DiscordEventHandlers handlers = {};
	/*
		Handlers list :
		ready, disconnected, errored, joinGame, spectateGame, joinRequest
	*/
	handlers.ready = Discord::handleDiscordReady;
	handlers.errored = Discord::handleDiscordError;
	Discord_Shutdown();
	Discord_Initialize(s_applicationId.c_str(), &handlers, 1, NULL);
}

Discord *Discord::getInstance()
{
	assert(s_pDiscord);
	return s_pDiscord.get();
}

void Discord::setState(const std::string &state)
{
	m_data.state = state;
}

void Discord::setDetails(const std::string &details)
{
	m_data.details = details;
}

void Discord::updatePresence()
{
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));

	discordPresence.state = m_data.state.c_str();
	discordPresence.details = m_data.details.c_str();
	discordPresence.startTimestamp = m_data.startTimestamp;
	discordPresence.largeImageKey = m_data.largeImageKey.c_str();
	discordPresence.smallImageKey = m_data.smallImageKey.c_str();
	discordPresence.partyId = m_data.partyId.c_str();
	discordPresence.partySize = m_data.partySize;
	discordPresence.partyMax = m_data.partyMax;
	// discordPresence.matchSecret = "4b2fdce12f639de8bfa7e3591b71a0d679d7c93f";
	// discordPresence.spectateSecret = "e7eb30d2ee025ed05c71ea495f770b76454ee4e0";
	// discordPresence.instance = 1;
	Discord_UpdatePresence(&discordPresence);
}

void Discord::handleDiscordReady()
{
	infostream << "Discord is ready" << std::endl;
}

void Discord::handleDiscordError(int errcode, const char *message)
{
	errorstream << "Error discord : " << std::to_string(errcode) << " " << message
		    << std::endl;
}
