/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <new>
#include <engine/shared/config.h>
#include "player.h"
#include "gamemodes/zesc.h"


MACRO_ALLOC_POOL_ID_IMPL(CPlayer, MAX_CLIENTS)

IServer *CPlayer::Server() const { return m_pGameServer->Server(); }

CPlayer::CPlayer(CGameContext *pGameServer, int ClientID, int Team)
{
	m_pGameServer = pGameServer;
	m_RespawnTick = Server()->Tick();
	m_DieTick = Server()->Tick();
	m_Score = 0;
	m_ScoreStartTick = Server()->Tick();
	m_pCharacter = 0;
	m_ClientID = ClientID;
	m_Team = GameServer()->m_pController->ClampTeam(Team);
	m_SpectatorID = SPEC_FREEVIEW;
	m_LastActionTick = Server()->Tick();
	m_TeamChangeTick = Server()->Tick();
	m_ShowHP = true;
	m_LastCheckpoint = vec2(0, 0);
}

CPlayer::~CPlayer()
{
	delete m_pCharacter;
	m_pCharacter = 0;
}

void CPlayer::Tick()
{
#ifdef CONF_DEBUG
	if(!g_Config.m_DbgDummies || m_ClientID < MAX_CLIENTS-g_Config.m_DbgDummies)
#endif
	if(!Server()->ClientIngame(m_ClientID))
		return;

	Server()->SetClientScore(m_ClientID, m_Score);

	// do latency stuff
	{
		IServer::CClientInfo Info;
		if(Server()->GetClientInfo(m_ClientID, &Info))
		{
			m_Latency.m_Accum += Info.m_Latency;
			m_Latency.m_AccumMax = max(m_Latency.m_AccumMax, Info.m_Latency);
			m_Latency.m_AccumMin = min(m_Latency.m_AccumMin, Info.m_Latency);
		}
		// each second
		if(Server()->Tick()%Server()->TickSpeed() == 0)
		{
			m_Latency.m_Avg = m_Latency.m_Accum/Server()->TickSpeed();
			m_Latency.m_Max = m_Latency.m_AccumMax;
			m_Latency.m_Min = m_Latency.m_AccumMin;
			m_Latency.m_Accum = 0;
			m_Latency.m_AccumMin = 1000;
			m_Latency.m_AccumMax = 0;
		}
	}

	if(!GameServer()->m_World.m_Paused)
	{
		if(!m_pCharacter && m_Team == TEAM_SPECTATORS && m_SpectatorID == SPEC_FREEVIEW)
			m_ViewPos -= vec2(clamp(m_ViewPos.x-m_LatestActivity.m_TargetX, -500.0f, 500.0f), clamp(m_ViewPos.y-m_LatestActivity.m_TargetY, -400.0f, 400.0f));

		if(!m_pCharacter && m_DieTick+Server()->TickSpeed()*3 <= Server()->Tick())
			m_Spawning = true;

		if(m_pCharacter)
		{
			if(m_pCharacter->IsAlive())
			{
				m_ViewPos = m_pCharacter->m_Pos;
			}
			else
			{
				delete m_pCharacter;
				m_pCharacter = 0;
			}
		}
		else if(m_Spawning && m_RespawnTick <= Server()->Tick() && !GameServer()->zESCController()->m_NukeLaunched)
			TryRespawn();
	}
	else
	{
		++m_RespawnTick;
		++m_DieTick;
		++m_ScoreStartTick;
		++m_LastActionTick;
		++m_TeamChangeTick;
 	}
}

void CPlayer::PostTick()
{
	// update latency value
	if(m_PlayerFlags&PLAYERFLAG_SCOREBOARD)
	{
		for(int i = 0; i < MAX_CLIENTS; ++i)
		{
			if(GameServer()->m_apPlayers[i] && GameServer()->m_apPlayers[i]->GetTeam() != TEAM_SPECTATORS)
				m_aActLatency[i] = GameServer()->m_apPlayers[i]->m_Latency.m_Min;
		}
	}

	// update view pos for spectators
	if(m_Team == TEAM_SPECTATORS && m_SpectatorID != SPEC_FREEVIEW && GameServer()->m_apPlayers[m_SpectatorID])
		m_ViewPos = GameServer()->m_apPlayers[m_SpectatorID]->m_ViewPos;
}

void CPlayer::Snap(int SnappingClient)
{
#ifdef CONF_DEBUG
	if(!g_Config.m_DbgDummies || m_ClientID < MAX_CLIENTS-g_Config.m_DbgDummies)
#endif
	if(!Server()->ClientIngame(m_ClientID))
		return;

	CNetObj_ClientInfo *pClientInfo = static_cast<CNetObj_ClientInfo *>(Server()->SnapNewItem(NETOBJTYPE_CLIENTINFO, m_ClientID, sizeof(CNetObj_ClientInfo)));
	if(!pClientInfo)
		return;

	StrToInts(&pClientInfo->m_Name0, 4, Server()->ClientName(m_ClientID));
	StrToInts(&pClientInfo->m_Clan0, 3, Server()->ClientClan(m_ClientID));
	pClientInfo->m_Country = Server()->ClientCountry(m_ClientID);
	StrToInts(&pClientInfo->m_Skin0, 6, m_TeeInfos.m_SkinName);
	pClientInfo->m_UseCustomColor = m_TeeInfos.m_UseCustomColor;
	pClientInfo->m_ColorBody = m_TeeInfos.m_ColorBody;
	pClientInfo->m_ColorFeet = m_TeeInfos.m_ColorFeet;

	CNetObj_PlayerInfo *pPlayerInfo = static_cast<CNetObj_PlayerInfo *>(Server()->SnapNewItem(NETOBJTYPE_PLAYERINFO, m_ClientID, sizeof(CNetObj_PlayerInfo)));
	if(!pPlayerInfo)
		return;

	pPlayerInfo->m_Latency = SnappingClient == -1 ? m_Latency.m_Min : GameServer()->m_apPlayers[SnappingClient]->m_aActLatency[m_ClientID];
	pPlayerInfo->m_Local = 0;
	pPlayerInfo->m_ClientID = m_ClientID;
	pPlayerInfo->m_Score = m_Score;
	pPlayerInfo->m_Team = m_Team;

	if(m_ClientID == SnappingClient)
		pPlayerInfo->m_Local = 1;

	if(m_ClientID == SnappingClient && m_Team == TEAM_SPECTATORS)
	{
		CNetObj_SpectatorInfo *pSpectatorInfo = static_cast<CNetObj_SpectatorInfo *>(Server()->SnapNewItem(NETOBJTYPE_SPECTATORINFO, m_ClientID, sizeof(CNetObj_SpectatorInfo)));
		if(!pSpectatorInfo)
			return;

		pSpectatorInfo->m_SpectatorID = m_SpectatorID;
		pSpectatorInfo->m_X = m_ViewPos.x;
		pSpectatorInfo->m_Y = m_ViewPos.y;
	}
}

void CPlayer::OnDisconnect(const char *pReason)
{
	KillCharacter();

	if(Server()->ClientIngame(m_ClientID))
	{
		char aBuf[512];
		if(pReason && *pReason)
			str_format(aBuf, sizeof(aBuf), "'%s' has left the game (%s)", Server()->ClientName(m_ClientID), pReason);
		else
			str_format(aBuf, sizeof(aBuf), "'%s' has left the game", Server()->ClientName(m_ClientID));
		GameServer()->SendChat(-1, CGameContext::CHAT_ALL, aBuf);

		str_format(aBuf, sizeof(aBuf), "leave player='%d:%s'", m_ClientID, Server()->ClientName(m_ClientID));
		GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "game", aBuf);
	}
}

void CPlayer::OnPredictedInput(CNetObj_PlayerInput *NewInput)
{
	// skip the input if chat is active
	if((m_PlayerFlags&PLAYERFLAG_CHATTING) && (NewInput->m_PlayerFlags&PLAYERFLAG_CHATTING))
		return;

	if(m_pCharacter)
		m_pCharacter->OnPredictedInput(NewInput);
}

void CPlayer::OnDirectInput(CNetObj_PlayerInput *NewInput)
{
	if(NewInput->m_PlayerFlags&PLAYERFLAG_CHATTING)
	{
		// skip the input if chat is active
		if(m_PlayerFlags&PLAYERFLAG_CHATTING)
			return;

		// reset input
		if(m_pCharacter)
			m_pCharacter->ResetInput();

		m_PlayerFlags = NewInput->m_PlayerFlags;
 		return;
	}

	m_PlayerFlags = NewInput->m_PlayerFlags;

	if(m_pCharacter)
		m_pCharacter->OnDirectInput(NewInput);

	if(!m_pCharacter && m_Team != TEAM_SPECTATORS && (NewInput->m_Fire&1))
		m_Spawning = true;

	// check for activity
	if(NewInput->m_Direction || m_LatestActivity.m_TargetX != NewInput->m_TargetX ||
		m_LatestActivity.m_TargetY != NewInput->m_TargetY || NewInput->m_Jump ||
		NewInput->m_Fire&1 || NewInput->m_Hook)
	{
		m_LatestActivity.m_TargetX = NewInput->m_TargetX;
		m_LatestActivity.m_TargetY = NewInput->m_TargetY;
		m_LastActionTick = Server()->Tick();
	}
}

CCharacter *CPlayer::GetCharacter()
{
	if(m_pCharacter && m_pCharacter->IsAlive())
		return m_pCharacter;
	return 0;
}

void CPlayer::KillCharacter(int Weapon)
{
	if(m_pCharacter)
	{
		m_pCharacter->Die(m_ClientID, Weapon);
		delete m_pCharacter;
		m_pCharacter = 0;
	}
}

void CPlayer::Respawn()
{
	if(m_Team != TEAM_SPECTATORS)
		m_Spawning = true;
}

void CPlayer::SetTeam(int Team, bool DoChatMsg)
{
	// clamp the team
	Team = GameServer()->m_pController->ClampTeam(Team);
	if(m_Team == Team)
		return;
	if(m_Nuked) {
		GameServer()->SendBroadcast("You were nuked, you can't rejoin.", m_ClientID);
		return; }
	if(GameServer()->zESCController()->m_NukeLaunched) {
		GameServer()->SendBroadcast("Nuke already launched, wait for gameend.", m_ClientID);
		Nuke();
		return; }
	if(m_Team == TEAM_RED && Team != TEAM_SPECTATORS) {
		GameServer()->SendBroadcast("Zombies can't change team.", m_ClientID);
		return; }
	if(GameServer()->zESCController()->ZombStarted() && !GameServer()->m_pController->m_ZombWarmup && Team == TEAM_BLUE) {
		GameServer()->SendBroadcast("You only can join the human team when round hasn't started.", m_ClientID);
		return; }
	if(Team == TEAM_RED && ((GameServer()->zESCController()->ZombStarted() && GameServer()->m_pController->m_ZombWarmup) || !GameServer()->zESCController()->ZombStarted())) {
		GameServer()->SendBroadcast("Zombie will be chosen randomly.", m_ClientID);
		return; }
	if(m_Team == TEAM_BLUE && GameServer()->zESCController()->ZombStarted()) {
		GameServer()->SendBroadcast("You can't join the zombie team.", m_ClientID);
		return; }
	if(m_Team == TEAM_RED && GameServer()->zESCController()->NumZombs() < 2 && GameServer()->zESCController()->ZombStarted()) {
		GameServer()->SendBroadcast("You are the only zombie.", m_ClientID);
		return; }

	char aBuf[512];
	if(DoChatMsg)
	{
		str_format(aBuf, sizeof(aBuf), "'%s' joined the %s", Server()->ClientName(m_ClientID), GameServer()->m_pController->GetTeamName(Team));
		GameServer()->SendChat(-1, CGameContext::CHAT_ALL, aBuf);
	}
	if(Team == TEAM_RED)
	{
		if(m_Team == TEAM_SPECTATORS)
		{
			m_Team = TEAM_RED;
			TryRespawn();
		}
			SetZomb(-3);
		return;
	}

	KillCharacter();

	m_Team = Team;
	m_LastActionTick = Server()->Tick();
	// we got to wait 0.5 secs before respawning
	m_RespawnTick = Server()->Tick()+Server()->TickSpeed()/2;
	str_format(aBuf, sizeof(aBuf), "team_join player='%d:%s' m_Team=%d", m_ClientID, Server()->ClientName(m_ClientID), m_Team);
	GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);

	GameServer()->m_pController->OnPlayerInfoChange(GameServer()->m_apPlayers[m_ClientID]);
	GameServer()->zESCController()->CheckZomb();

	if(Team == TEAM_SPECTATORS)
	{
		// update spectator modes
		for(int i = 0; i < MAX_CLIENTS; ++i)
		{
			if(GameServer()->m_apPlayers[i] && GameServer()->m_apPlayers[i]->m_SpectatorID == m_ClientID)
				GameServer()->m_apPlayers[i]->m_SpectatorID = SPEC_FREEVIEW;
		}
	}
}

void CPlayer::TryRespawn()
{
	vec2 SpawnPos;

	if(m_Team == TEAM_RED)
	{
		if(!GameServer()->m_pController->ZombieSpawn(&SpawnPos))
		{
			if(!GameServer()->m_pController->CanSpawn(TEAM_BLUE, &SpawnPos))
				return;
		}
		if(m_LastCheckpoint != vec2(0, 0))
			SpawnPos = m_LastCheckpoint;
	}
	else if(m_Team == TEAM_BLUE)
	{
		if(!GameServer()->m_pController->CanSpawn(TEAM_BLUE, &SpawnPos))
			return;
	}
	else if(m_Team == TEAM_SPECTATORS)
		return;

	m_Spawning = false;
	m_pCharacter = new(m_ClientID) CCharacter(&GameServer()->m_World);
	m_pCharacter->Spawn(this, SpawnPos);
	GameServer()->CreatePlayerSpawn(SpawnPos);
}

void CPlayer::SetZomb(int From)
{
	// send a nice message
	CNetMsg_Sv_KillMsg Msg;
	if(From > -1)
	{
		Msg.m_Killer = From;
		Msg.m_Victim = m_ClientID;
		Msg.m_Weapon = WEAPON_HAMMER;
		GameServer()->m_apPlayers[From]->m_Score++;
	}
	else
	{
		Msg.m_Killer = m_ClientID;
		Msg.m_Victim = m_ClientID;
		Msg.m_Weapon = WEAPON_HAMMER;
	}
	Msg.m_ModeSpecial = 0;
	Server()->SendPackMsg(&Msg, MSGFLAG_VITAL, -1);

	m_Team = TEAM_RED;
	if(From == -1 || From == -3)
	{
		if(From == -1)
		{
			char aBuf[512];
			GameServer()->CreateSound(m_ViewPos, SOUND_PLAYER_PAIN_LONG);
			str_format(aBuf, sizeof(aBuf), "'%s' wants your brain! Run away.", Server()->ClientName(m_ClientID));
			GameServer()->SendChat(-1, CGameContext::CHAT_ALL, aBuf);
		}

		vec2 SpawnPos;
		if(GameServer()->m_pController->ZombieSpawn(&SpawnPos))
		{
			m_pCharacter->m_Core.m_HookedPlayer = -1;
			m_pCharacter->m_Core.m_HookState = HOOK_RETRACTED;
			m_pCharacter->m_Core.m_TriggeredEvents |= COREEVENT_HOOK_RETRACT;
			m_pCharacter->m_Core.m_HookState = HOOK_RETRACTED;
			m_pCharacter->m_Core.m_Pos = SpawnPos;
			m_pCharacter->m_Core.m_Vel = vec2(0,0);
			m_pCharacter->m_Core.m_HookPos = m_pCharacter->m_Core.m_Pos;
			GameServer()->CreatePlayerSpawn(SpawnPos);
		}
	}

	str_copy(m_TeeInfos.m_SkinName, "zomb", sizeof(m_TeeInfos.m_SkinName));
	m_pCharacter->SetZomb();
	GameServer()->m_pController->OnPlayerInfoChange(GameServer()->m_apPlayers[m_ClientID]);
	GameServer()->zESCController()->CheckZomb();
	GameServer()->SendChatTarget(m_ClientID, "You are now a zombie! Eat some brains.");
	if(g_Config.m_SvZombieHp)
		GameServer()->SendChatTarget(m_ClientID, "Write !hp to disable or enable the hp display.");
}

void CPlayer::ResetZomb()
{
	if(m_Team == TEAM_SPECTATORS && !m_Nuked)
		return;
	str_copy(m_TeeInfos.m_SkinName, m_OriginSkinName, sizeof(m_TeeInfos.m_SkinName));
	m_Team = TEAM_BLUE;
	GameServer()->m_pController->OnPlayerInfoChange(GameServer()->m_apPlayers[m_ClientID]);
	m_Nuked = false;
	m_LastCheckpoint = vec2(0, 0);
}

void CPlayer::Nuke()
{
	KillCharacter(WEAPON_WORLD);
	m_Nuked = true;
	m_Team = TEAM_SPECTATORS;
	GameServer()->m_pController->OnPlayerInfoChange(GameServer()->m_apPlayers[m_ClientID]);
	m_RespawnTick = Server()->Tick()+Server()->TickSpeed()/2;
	// update spectator modes
	for(int i = 0; i < MAX_CLIENTS; ++i)
	{
		if(GameServer()->m_apPlayers[i] && GameServer()->m_apPlayers[i]->m_SpectatorID == m_ClientID)
			GameServer()->m_apPlayers[i]->m_SpectatorID = SPEC_FREEVIEW;
	}
	GameServer()->zESCController()->CheckZomb();
}
