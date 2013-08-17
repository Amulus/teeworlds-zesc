/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/shared/config.h>
#include <game/mapitems.h>
#include <game/generated/protocol.h>

#include "entities/pickup.h"
#include "entities/door.h"
#include "gamecontroller.h"
#include "gamecontext.h"
#include "gamemodes/zesc.h"


IGameController::IGameController(class CGameContext *pGameServer)
{
	m_pGameServer = pGameServer;
	m_pServer = m_pGameServer->Server();
	m_pGameType = "unknown";

	//
	m_ZombWarmup = 0;
	m_LastZomb = -1;
	m_LastZomb2 = -1;
	m_GameOverTick = -1;
	m_SuddenDeath = 0;
	m_RoundStartTick = Server()->Tick();
	m_RoundCount = 0;
	m_GameFlags = 0;
	m_aTeamscore[TEAM_RED] = 0;
	m_aTeamscore[TEAM_BLUE] = 0;
	m_aMapWish[0] = 0;

	m_UnbalancedTick = -1;
	m_ForceBalanced = false;

	m_aNumSpawnPoints = 0;
	m_aZSpawn = vec2(-42, -42);

	mem_zero(m_apTriggeredEvents, sizeof(m_apTriggeredEvents));
	mem_zero(m_apCustomTeleport, sizeof(m_apCustomTeleport));
	m_lTimedEvents.clear();
	m_aaOnTeamWinEvent[TEAM_RED][0] = 0;
	m_aaOnTeamWinEvent[TEAM_BLUE][0] = 0;
	m_aaOnTeamWinEvent[2][0] = 0; // on restart
}

IGameController::~IGameController()
{
	for(int i = 0; i < 256; i++)
	{
		delete m_apTriggeredEvents[i];
		delete m_apCustomTeleport[i];
	}
}

void IGameController::EvaluateSpawnType(CSpawnEval *pEval)
{
	// get spawn point
	for(int i = 0; i < m_aNumSpawnPoints; i++)
	{
		// check if the position is occupado
		CCharacter *aEnts[MAX_CLIENTS];
		int Num = GameServer()->m_World.FindEntities(m_aaSpawnPoints[i], 64, (CEntity**)aEnts, MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);
		vec2 Positions[5] = { vec2(0.0f, 0.0f), vec2(-2.0f, 0.0f), vec2(0.0f, -2.0f), vec2(2.0f, 0.0f), vec2(0.0f, 2.0f) };	// the original spawns too many players on one spawn and sometime they spawn in earth...
		int Result = -1;
		for(int Index = 0; Index < 5 && Result == -1; ++Index)
		{
			Result = Index;
			for(int c = 0; c < Num; ++c)
				if(GameServer()->Collision()->CheckPoint(m_aaSpawnPoints[i]+Positions[Index]) ||
					distance(aEnts[c]->m_Pos, m_aaSpawnPoints[i]+Positions[Index]) <= aEnts[c]->m_ProximityRadius)
				{
					Result = -1;
					break;
				}
		}
		if(Result == -1)
			continue;	// try next spawn point

		vec2 P = m_aaSpawnPoints[i]+Positions[Result];
		if(!pEval->m_Got)
		{
			pEval->m_Got = true;
			pEval->m_Pos = P;
		}
	}
}

bool IGameController::CanSpawn(int Team, vec2 *pOutPos)
{
	CSpawnEval Eval;

	// spectators can't spawn, zombies also
	if(Team == TEAM_SPECTATORS || Team == TEAM_RED)
		return false;

	Eval.m_FriendlyTeam = Team;

	EvaluateSpawnType(&Eval);
	if(!Eval.m_Got)
		EvaluateSpawnType(&Eval);
	if(!Eval.m_Got)
		EvaluateSpawnType(&Eval);
	if(!Eval.m_Got)
		EvaluateSpawnType(&Eval);


	*pOutPos = Eval.m_Pos;
	return Eval.m_Got;
}

bool IGameController::ZombieSpawn(vec2 *pOutPos)
{
	if(m_aZSpawn == vec2(-42, -42))
		return false;

	*pOutPos = m_aZSpawn;
	return true;
}

bool IGameController::OnEntity(int Index, vec2 Pos)
{
	int Type = -1;
	int SubType = 0;

	int Item = -1;
	int RealWeapon = 0;

	if(Index == ENTITY_SPAWN)
		m_aaSpawnPoints[m_aNumSpawnPoints++] = Pos;
	if(Index == ENTITY_SPAWN_RED)
		m_aZSpawn = Pos;
	else if(Index == ENTITY_WEAPON_SHOTGUN)
	{
		Type = POWERUP_WEAPON;
		SubType = WEAPON_SHOTGUN;
	}
	else if(Index == ENTITY_WEAPON_GRENADE)
	{
		Type = POWERUP_WEAPON;
		SubType = WEAPON_GRENADE;
	}
	else if(Index == ENTITY_WEAPON_RIFLE)
	{
		Type = POWERUP_WEAPON;
		SubType = WEAPON_RIFLE;
	}
	else if(Index == ENTITY_POWERUP_NINJA && g_Config.m_SvPowerups)
	{
		Type = POWERUP_NINJA;
		SubType = WEAPON_NINJA;
	}
	else if(Index == ENTITY_ZHAMMER_UPGRADE)
	{
		Item = ZITEM_HAMMER;
		RealWeapon = WEAPON_HAMMER;
	}
	else if(Index == ENTITY_HHAMMER_UPGRADE)
	{
		Item = HITEM_HAMMER;
		RealWeapon = WEAPON_HAMMER;
	}
	else if(Index == ENTITY_GUN_UPGRADE)
	{
		Item = HITEM_GUN;
		RealWeapon = WEAPON_GUN;
	}
	else if(Index == ENTITY_SHOTGUN_UPGRADE)
	{
		Item = HITEM_SHOTGUN;
		RealWeapon = WEAPON_SHOTGUN;
	}
	else if(Index == ENTITY_GRENADE_UPGRADE)
	{
		Item = HITEM_GRENADE;
		RealWeapon = WEAPON_GRENADE;
	}
	else if(Index == ENTITY_RIFLE_UPGRADE)
	{
		Item = HITEM_RIFLE;
		RealWeapon = WEAPON_RIFLE;
	}
	else if((Index >= ENTITY_DOOR_BEGIN && Index <= ENTITY_DOOR_END) || (Index >= ENTITY_ZDOOR_BEGIN && Index <= ENTITY_ZDOOR_END))
	{
		CDoor *pDoor = new CDoor(&GameServer()->m_World, Index-17, -1);
		pDoor->m_Pos = Pos;
		return true;
	}
	if(Type != -1)
	{
		CPickup *pPickup = new CPickup(&GameServer()->m_World, Type, SubType);
		pPickup->m_Pos = Pos;
		return true;
	}
	if(Item != -1)
	{
		CItem *pItem = new CItem(&GameServer()->m_World, Item, RealWeapon);
		pItem->m_Pos = Pos;
		return true;
	}

	return false;
}

void IGameController::EndRound()
{
	if(m_ZombWarmup || m_GameOverTick != -1) // game can't end when we are running warmup
		return;

	GameServer()->zESCController()->OnEndRound();

	GameServer()->m_World.m_Paused = true;
	m_GameOverTick = Server()->Tick()+Server()->TickSpeed()*5;
	m_SuddenDeath = 0;
}

void IGameController::ResetGame()
{
	GameServer()->m_World.m_ResetRequested = true;
}

const char *IGameController::GetTeamName(int Team)
{
	if(IsTeamplay())
	{
		if(Team == TEAM_RED)
			return "zombie team";
		else if(Team == TEAM_BLUE)
			return "human team";
	}
	else
	{
		if(Team == 0)
			return "game";
	}

	return "spectators";
}

static bool IsSeparator(char c) { return c == ';' || c == ' ' || c == ',' || c == '\t'; }

void IGameController::StartRound()
{
	if(m_aTeamscore[TEAM_RED] >= 100 && m_aTeamscore[TEAM_BLUE] < 100 && m_aaOnTeamWinEvent[TEAM_RED][0])
		GameServer()->Console()->ExecuteLine(m_aaOnTeamWinEvent[TEAM_RED]);
	else if(m_aTeamscore[TEAM_BLUE] >= 100 && m_aTeamscore[TEAM_RED] < 100 && m_aaOnTeamWinEvent[TEAM_BLUE][0])
		GameServer()->Console()->ExecuteLine(m_aaOnTeamWinEvent[TEAM_BLUE]);
	if(m_aaOnTeamWinEvent[2][0])
		GameServer()->Console()->ExecuteLine(m_aaOnTeamWinEvent[2]);
	ResetGame();

	m_ZombWarmup = 0;
	m_RoundStartTick = Server()->Tick();
	m_SuddenDeath = 0;
	m_GameOverTick = -1;

	GameServer()->zESCController()->StartZomb(false);
	GameServer()->zESCController()->CheckZomb();

	GameServer()->m_World.m_Paused = false;
	m_aTeamscore[TEAM_RED] = 0;
	m_aTeamscore[TEAM_BLUE] = 0;
	m_ForceBalanced = false;
	Server()->DemoRecorder_HandleAutoStart();
	ResetEvents();
	char aBuf[256];
	str_format(aBuf, sizeof(aBuf), "start round type='%s' teamplay='%d'", m_pGameType, m_GameFlags&GAMEFLAG_TEAMS);
	GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);
}

void IGameController::ChangeMap(const char *pToMap)
{
	str_copy(m_aMapWish, pToMap, sizeof(m_aMapWish));
	EndRound();
}

void IGameController::CycleMap()
{
	if(m_aMapWish[0] != 0)
	{
		char aBuf[256];
		str_format(aBuf, sizeof(aBuf), "rotating map to %s", m_aMapWish);
		GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);
		str_copy(g_Config.m_SvMap, m_aMapWish, sizeof(g_Config.m_SvMap));
		m_aMapWish[0] = 0;
		m_RoundCount = 0;
		return;
	}
	if(!str_length(g_Config.m_SvMaprotation))
		return;

	// handle maprotation
	const char *pMapRotation = g_Config.m_SvMaprotation;
	const char *pCurrentMap = g_Config.m_SvMap;

	int CurrentMapLen = str_length(pCurrentMap);
	const char *pNextMap = pMapRotation;
	while(*pNextMap)
	{
		int WordLen = 0;
		while(pNextMap[WordLen] && !IsSeparator(pNextMap[WordLen]))
			WordLen++;

		if(WordLen == CurrentMapLen && str_comp_num(pNextMap, pCurrentMap, CurrentMapLen) == 0)
		{
			// map found
			pNextMap += CurrentMapLen;
			while(*pNextMap && IsSeparator(*pNextMap))
				pNextMap++;

			break;
		}

		pNextMap++;
	}

	// restart rotation
	if(pNextMap[0] == 0)
		pNextMap = pMapRotation;

	// cut out the next map
	char aBuf[512] = {0};
	for(int i = 0; i < 511; i++)
	{
		aBuf[i] = pNextMap[i];
		if(IsSeparator(pNextMap[i]) || pNextMap[i] == 0)
		{
			aBuf[i] = 0;
			break;
		}
	}

	// skip spaces
	int i = 0;
	while(IsSeparator(aBuf[i]))
		i++;

	m_RoundCount = 0;

	char aBufMsg[256];
	str_format(aBufMsg, sizeof(aBufMsg), "rotating map to %s", &aBuf[i]);
	GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);
	str_copy(g_Config.m_SvMap, &aBuf[i], sizeof(g_Config.m_SvMap));
}

void IGameController::PostReset()
{
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(GameServer()->m_apPlayers[i])
		{
			GameServer()->m_apPlayers[i]->Respawn();
			//GameServer()->m_apPlayers[i]->m_Score = 0;
			//GameServer()->m_apPlayers[i]->m_ScoreStartTick = Server()->Tick();
			GameServer()->m_apPlayers[i]->m_RespawnTick = Server()->Tick()+Server()->TickSpeed()/2;
		}
	}
}

void IGameController::OnPlayerInfoChange(class CPlayer *pP)
{
	const int aTeamColors[2] = {65387, 10223467};
	if(IsTeamplay())
	{
		pP->m_TeeInfos.m_UseCustomColor = 1;
		if(pP->GetTeam() >= TEAM_RED && pP->GetTeam() <= TEAM_BLUE)
		{
			pP->m_TeeInfos.m_ColorBody = aTeamColors[pP->GetTeam()];
			pP->m_TeeInfos.m_ColorFeet = aTeamColors[pP->GetTeam()];
		}
		else
		{
			pP->m_TeeInfos.m_ColorBody = 12895054;
			pP->m_TeeInfos.m_ColorFeet = 12895054;
		}
	}
}


int IGameController::OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon)
{
	// do scoreing
	if(!pKiller || Weapon == WEAPON_GAME)
		return 0;
	if(pKiller == pVictim->GetPlayer())
		pVictim->GetPlayer()->m_Score--; // suicide
	else
	{
		if(IsTeamplay() && pVictim->GetPlayer()->GetTeam() == pKiller->GetTeam())
			pKiller->m_Score--; // teamkill
		else
			pKiller->m_Score++; // normal kill
	}
	if(Weapon == WEAPON_SELF)
		pVictim->GetPlayer()->m_RespawnTick = Server()->Tick()+Server()->TickSpeed()*3.0f;
	return 0;
}

void IGameController::OnCharacterSpawn(class CCharacter *pChr)
{
	// default health
	pChr->IncreaseHealth(10);

	// give default weapons
	pChr->GiveWeapon(WEAPON_HAMMER, -1);
	pChr->GiveWeapon(WEAPON_GUN, 10);
}

bool IGameController::IsFriendlyFire(int ClientID1, int ClientID2)
{
	if(ClientID1 == ClientID2)
		return false;

	if(IsTeamplay())
	{
		if(!GameServer()->m_apPlayers[ClientID1] || !GameServer()->m_apPlayers[ClientID2])
			return false;

		if(GameServer()->m_apPlayers[ClientID1]->GetTeam() == GameServer()->m_apPlayers[ClientID2]->GetTeam())
			return true;
	}

	return false;
}

bool IGameController::IsForceBalanced()
{
	if(m_ForceBalanced)
	{
		m_ForceBalanced = false;
		return true;
	}
	else
		return false;
}

bool IGameController::CanBeMovedOnBalance(int ClientID)
{
	return true;
}

void IGameController::Tick()
{
	// Zomb warmup
	if(m_ZombWarmup)
	{
		m_ZombWarmup--;
		if(!m_ZombWarmup)
		{
			// Make players sad :> / mad :D
			if(GameServer()->zESCController()->NumPlayers() > 1 && g_Config.m_SvZombieRatio)
			{
				int ZAtStart = 1;
				ZAtStart = (int)GameServer()->zESCController()->NumPlayers()/(int)g_Config.m_SvZombieRatio;
				if(!ZAtStart)
					ZAtStart = 1;
				for(; ZAtStart; ZAtStart--)
					RandomZomb(-1);
			}
			else
				GameServer()->zESCController()->StartZomb(false);
		}
	}

	if(m_GameOverTick != -1)
	{
		// game over.. wait for restart
		if(Server()->Tick() > m_GameOverTick)
		{
			CycleMap();
			m_RoundCount++;
			StartRound();
		}
	}

	// game is Paused
	if(GameServer()->m_World.m_Paused)
		++m_RoundStartTick;

	// check for inactive players
	if(g_Config.m_SvInactiveKickTime > 0)
	{
		for(int i = 0; i < MAX_CLIENTS; ++i)
		{
		#ifdef CONF_DEBUG
			if(g_Config.m_DbgDummies)
			{
				if(i >= MAX_CLIENTS-g_Config.m_DbgDummies)
					break;
			}
		#endif
			if(GameServer()->m_apPlayers[i] && GameServer()->m_apPlayers[i]->GetTeam() != TEAM_SPECTATORS && !Server()->IsAuthed(i))
			{
				if(Server()->Tick() > GameServer()->m_apPlayers[i]->m_LastActionTick+g_Config.m_SvInactiveKickTime*Server()->TickSpeed()*60)
				{
					switch(g_Config.m_SvInactiveKick)
					{
					case 0:
						{
							// move player to spectator
							GameServer()->m_apPlayers[i]->SetTeam(TEAM_SPECTATORS);
						}
						break;
					case 1:
						{
							// move player to spectator if the reserved slots aren't filled yet, kick him otherwise
							int Spectators = 0;
							for(int j = 0; j < MAX_CLIENTS; ++j)
								if(GameServer()->m_apPlayers[j] && GameServer()->m_apPlayers[j]->GetTeam() == TEAM_SPECTATORS)
									++Spectators;
							if(Spectators >= g_Config.m_SvSpectatorSlots)
								Server()->Kick(i, "Kicked for inactivity");
							else
								GameServer()->m_apPlayers[i]->SetTeam(TEAM_SPECTATORS);
						}
						break;
					case 2:
						{
							// kick the player
							Server()->Kick(i, "Kicked for inactivity");
						}
					}
				}
			}
		}
	}
	if(GameServer()->zESCController()->NumPlayers())
	{
		for(int i = 0; i < m_lTimedEvents.size(); i++)
		{
			if(m_lTimedEvents[i].m_Tick && m_lTimedEvents[i].m_Tick <= Server()->Tick())
			{
				m_lTimedEvents[i].m_Tick = 0;
				char aBuf[512];
				str_copy(aBuf, m_lTimedEvents[i].m_pAction, sizeof(aBuf));
				// Parse the exec() command
				ParseExec(aBuf, sizeof(aBuf));
				GameServer()->Console()->ExecuteLine(aBuf);
			}
		}
	}
}

int IGameController::ParseExec(char *pCommand, int Size)
{
	int Replaced = 0;
	const char *pExecStart = 0;
	const char *pExecEnd = 0;
	while((pExecStart = str_find(pCommand, "exec(")))
	{
		if((pExecEnd = str_find(pExecStart, ")")))
		{
			char *pBuf = new char[pExecEnd-pExecStart+2]; // +1 for the last char ")" and +1 for null-termination
			char *pToExec = new char[pExecEnd-pExecStart-4]; // -5 for "exec(" +1 for null-termination
			str_copy(pBuf, pExecStart, pExecEnd-pExecStart+2);
			str_copy(pToExec, pExecStart+5, pExecEnd-pExecStart-4);
			str_replace(pCommand, Size, pBuf, GameServer()->Console()->ExecuteLineEx(pToExec));
			delete[] pBuf;
			delete[] pToExec;
			Replaced++;
		}
		else
			return -1;
	}
	return Replaced;
}

bool IGameController::RegisterTimedEvent(float Time, const char *pCommand)
{
	if(pCommand[0] == 0 || Time < 0.1f) // Why use an event with no command or that gets executed immediately?
		return false;

	m_lTimedEvents.add(CTimedEvent(Time, Server()->Tick() + Time*Server()->TickSpeed(), pCommand));

	return true;
}

void IGameController::ResetEvents()
{
	for(int i = 0; i < m_lTimedEvents.size(); i++)
		m_lTimedEvents[i].m_Tick = Server()->Tick() + m_lTimedEvents[i].m_Time*Server()->TickSpeed();
	for(int i = 0; i < 256; i++)
	{
		if(m_apTriggeredEvents[i])
			m_apTriggeredEvents[i]->Reset(false);
		if(m_apCustomTeleport[i])
			m_apCustomTeleport[i]->Reset();
	}
}

bool IGameController::RegisterTriggeredEvent(int ID, int Type, const char *pCommand)
{
	if(ID < 0 || ID >= 256) // Notify the user that he's stupid...
		return false;

	if(m_apTriggeredEvents[ID])
		m_apTriggeredEvents[ID]->Reset(true);
	else
		m_apTriggeredEvents[ID] = new CTriggeredEvent();

	str_copy(m_apTriggeredEvents[ID]->m_aAction, pCommand, sizeof(m_apTriggeredEvents[ID]->m_aAction));
	m_apTriggeredEvents[ID]->m_State = false;
	m_apTriggeredEvents[ID]->m_Type = Type;

	return true;
}

void IGameController::OnTrigger(int ID, int TriggeredBy)
{
	if(!m_apTriggeredEvents[ID] || !m_apTriggeredEvents[ID]->m_aAction[0] || m_apTriggeredEvents[ID]->m_State || m_apTriggeredEvents[ID]->m_aPlayerState[TriggeredBy])
		return;

	char aBuf[512];
	str_copy(aBuf, m_apTriggeredEvents[ID]->m_aAction, sizeof(aBuf));

	// Replace keywords with values
	if(str_find(aBuf, "!activator"))
	{
		str_replace(aBuf, sizeof(aBuf), "!activator.name", Server()->ClientName(TriggeredBy));
		str_replace(aBuf, sizeof(aBuf), "!activator.team", int_tostr(GameServer()->m_apPlayers[TriggeredBy]->GetTeam()));
		str_replace(aBuf, sizeof(aBuf), "!activator.score", int_tostr(GameServer()->m_apPlayers[TriggeredBy]->m_Score));
		str_replace(aBuf, sizeof(aBuf), "!activator.pos_x", int_tostr(GameServer()->m_apPlayers[TriggeredBy]->m_ViewPos.x/32));
		str_replace(aBuf, sizeof(aBuf), "!activator.pos_y", int_tostr(GameServer()->m_apPlayers[TriggeredBy]->m_ViewPos.y/32));
		str_replace(aBuf, sizeof(aBuf), "!activator", int_tostr(TriggeredBy));
	}

	// Parse the exec() command
	ParseExec(aBuf, sizeof(aBuf));

	GameServer()->Console()->ExecuteLine(aBuf);
	if(m_apTriggeredEvents[ID]->m_Type == TRIGGER_ONCE)
		m_apTriggeredEvents[ID]->m_State = true;
	else if(m_apTriggeredEvents[ID]->m_Type == TRIGGER_PLAYERONCE)
		m_apTriggeredEvents[ID]->m_aPlayerState[TriggeredBy] = true;
}

bool IGameController::RegisterCustomTeleport(int ID, int ToX, int Team)
{
	if(ID < 0 || ID >= 256) // Notify the user that he's stupid...
		return false;

	if(m_apCustomTeleport[ID])
		m_apCustomTeleport[ID]->Reset();
	else
		m_apCustomTeleport[ID] = new CCustomTeleport();

	m_apCustomTeleport[ID]->m_Teleport = ToX;
	m_apCustomTeleport[ID]->m_Team = Team;

	return true;
}

int IGameController::OnCustomTeleporter(int ID, int Team)
{
	if(!m_apCustomTeleport[ID] || m_apCustomTeleport[ID]->m_Teleport == -1) // Check if there's any teleport assigned
		return -1;

	if(m_apCustomTeleport[ID]->m_Team != -1 && Team != m_apCustomTeleport[ID]->m_Team) // Check if there's any team assigned to this teleporter, if yes check if it matches with the players team
		return -1;

	return m_apCustomTeleport[ID]->m_Teleport;
}

bool IGameController::IsTeamplay() const
{
	return m_GameFlags&GAMEFLAG_TEAMS;
}

void IGameController::Snap(int SnappingClient)
{
	CNetObj_GameInfo *pGameInfoObj = (CNetObj_GameInfo *)Server()->SnapNewItem(NETOBJTYPE_GAMEINFO, 0, sizeof(CNetObj_GameInfo));
	if(!pGameInfoObj)
		return;

	pGameInfoObj->m_GameFlags = m_GameFlags;
	pGameInfoObj->m_GameStateFlags = 0;
	if(m_GameOverTick != -1)
		pGameInfoObj->m_GameStateFlags |= GAMESTATEFLAG_GAMEOVER;
	if(m_SuddenDeath)
		pGameInfoObj->m_GameStateFlags |= GAMESTATEFLAG_SUDDENDEATH;
	if(GameServer()->m_World.m_Paused)
		pGameInfoObj->m_GameStateFlags |= GAMESTATEFLAG_PAUSED;
	pGameInfoObj->m_RoundStartTick = m_RoundStartTick;

	pGameInfoObj->m_WarmupTimer = m_ZombWarmup;

	pGameInfoObj->m_ScoreLimit = g_Config.m_SvScorelimit;
	pGameInfoObj->m_TimeLimit = g_Config.m_SvTimelimit*60;

	pGameInfoObj->m_RoundNum = (str_length(g_Config.m_SvMaprotation) && g_Config.m_SvRoundsPerMap) ? g_Config.m_SvRoundsPerMap : 0;
	pGameInfoObj->m_RoundCurrent = m_RoundCount+1;
}

int IGameController::GetAutoTeam(int NotThisID)
{
	if(GameServer()->zESCController()->ZombStarted() && !m_ZombWarmup)
		return TEAM_RED;
	else
		return TEAM_BLUE;
}

bool IGameController::CanJoinTeam(int Team, int NotThisID)
{
	if(Team == TEAM_SPECTATORS || (GameServer()->m_apPlayers[NotThisID] && GameServer()->m_apPlayers[NotThisID]->GetTeam() != TEAM_SPECTATORS))
		return true;

	int aNumplayers[2] = {0,0};
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(GameServer()->m_apPlayers[i] && i != NotThisID)
		{
			if(GameServer()->m_apPlayers[i]->GetTeam() >= TEAM_RED && GameServer()->m_apPlayers[i]->GetTeam() <= TEAM_BLUE)
				aNumplayers[GameServer()->m_apPlayers[i]->GetTeam()]++;
		}
	}

	return (aNumplayers[0] + aNumplayers[1]) < Server()->MaxClients()-g_Config.m_SvSpectatorSlots;
}

int IGameController::ClampTeam(int Team)
{
	if(Team < 0)
		return TEAM_SPECTATORS;
	if(IsTeamplay())
		return Team&1;
	return 0;
}

void IGameController::ZombWarmup(int W)
{
	m_ZombWarmup = W*Server()->TickSpeed();
}

void IGameController::RandomZomb(int Mode)
{
	int ZombCID = rand()%MAX_CLIENTS;
	int WTF = 100; // 100 Trys should be enough
	while(!GameServer()->m_apPlayers[ZombCID] || (GameServer()->m_apPlayers[ZombCID] && GameServer()->m_apPlayers[ZombCID]->GetTeam() == TEAM_SPECTATORS) ||
			m_LastZomb == ZombCID || (GameServer()->zESCController()->NumPlayers() > 2 && m_LastZomb2 == ZombCID) || !GameServer()->m_apPlayers[ZombCID]->GetCharacter() ||
			(GameServer()->m_apPlayers[ZombCID]->GetCharacter() && !GameServer()->m_apPlayers[ZombCID]->GetCharacter()->IsAlive()))	// <- End ^^
	{
		ZombCID = rand()%MAX_CLIENTS;
		WTF--;
		if(!WTF) // Anti 100% CPU :D (Very crappy, but it's a fix :P)
		{
			StartRound();
			return;
		}
	}
	GameServer()->m_apPlayers[ZombCID]->SetZomb(Mode);
	GameServer()->zESCController()->StartZomb(true);
	m_LastZomb2 = m_LastZomb;
	m_LastZomb = ZombCID;
}
