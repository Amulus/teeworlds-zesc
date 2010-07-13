#ifndef GAME_VARIABLES_H
#define GAME_VARIABLES_H
#undef GAME_VARIABLES_H // this file will be included several times


// client
MACRO_CONFIG_INT(ClPredict, cl_predict, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Predict client movements")
MACRO_CONFIG_INT(ClNameplates, cl_nameplates, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show nameplates")
MACRO_CONFIG_INT(ClNameplatesAlways, cl_nameplates_always, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Always show nameplats disregarding of distance")
MACRO_CONFIG_INT(ClAutoswitchWeapons, cl_autoswitch_weapons, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Auto switch weapon on pickup")

MACRO_CONFIG_INT(ClShowfps, cl_showfps, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show ingame FPS counter")

MACRO_CONFIG_INT(ClAirjumpindicator, cl_airjumpindicator, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClThreadsoundloading, cl_threadsoundloading, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(ClWarningTeambalance, cl_warning_teambalance, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Warn about team balance")

MACRO_CONFIG_INT(ClMouseDeadzone, cl_mouse_deadzone, 300, 0, 0, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClMouseFollowfactor, cl_mouse_followfactor, 60, 0, 200, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClMouseMaxDistance, cl_mouse_max_distance, 800, 0, 0, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(EdShowkeys, ed_showkeys, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(ClFlow, cl_flow, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")

MACRO_CONFIG_INT(ClShowWelcome, cl_show_welcome, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "")
MACRO_CONFIG_INT(ClMotdTime, cl_motd_time, 10, 0, 100, CFGFLAG_CLIENT|CFGFLAG_SAVE, "How long to show the server message of the day")

MACRO_CONFIG_STR(ClVersionServer, cl_version_server, 100, "version.teeworlds.com", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Server to use to check for new versions")

MACRO_CONFIG_STR(ClLanguagefile, cl_languagefile, 255, "", CFGFLAG_CLIENT|CFGFLAG_SAVE, "What language file to use")
MACRO_CONFIG_STR(ClFontfile, cl_fontfile, 255, "", CFGFLAG_CLIENT|CFGFLAG_SAVE, "What font file to use")

/* other stuff :\ */
MACRO_CONFIG_INT(ClNameplateClientId, cl_nameplate_client_id, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Shows the client id above the player")
MACRO_CONFIG_INT(ClScoreboardClientId, cl_scoreboard_client_id, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Shows the client id in scoreboard")
MACRO_CONFIG_INT(ClShowGhost, cl_show_ghost, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Shows the ghost tee")

//Hud-Mod
MACRO_CONFIG_INT(ClRenderTime, cl_render_time, 1, 0, 1, CFGFLAG_CLIENT, "Turnes the Server time on or off")
MACRO_CONFIG_INT(ClRenderScoreboard, cl_render_scoreboard, 1, 0, 1, CFGFLAG_CLIENT, "Turnes the game over scoreboard on or off")
MACRO_CONFIG_INT(ClRenderWarmup, cl_render_warmup, 1, 0, 1, CFGFLAG_CLIENT, "Turnes warmup timer on or off")
MACRO_CONFIG_INT(ClRenderBroadcast, cl_render_broadcast, 1, 0, 1, CFGFLAG_CLIENT, "Turnes broadcast on or off")
MACRO_CONFIG_INT(ClRenderHp, cl_render_hp, 1, 0, 1, CFGFLAG_CLIENT, "Turnes the hp display on or off")
MACRO_CONFIG_INT(ClRenderAmmo, cl_render_ammo, 1, 0, 1, CFGFLAG_CLIENT, "Turnes the ammo display on or off")
MACRO_CONFIG_INT(ClRenderCrosshair, cl_render_crosshair, 1, 0, 1, CFGFLAG_CLIENT, "Turnes the crosshair on or off")
MACRO_CONFIG_INT(ClRenderScore, cl_render_score, 1, 0, 1, CFGFLAG_CLIENT, "Turnes the small score board on or off")
MACRO_CONFIG_INT(ClRenderServermsg, cl_render_servermsg, 1, 0, 1, CFGFLAG_CLIENT, "Turnes in game server messages on or off")
MACRO_CONFIG_INT(ClRenderChat, cl_render_chat, 1, 0, 1, CFGFLAG_CLIENT, "Turnes in game chat on or off")
MACRO_CONFIG_INT(ClRenderKill, cl_render_kill, 1, 0, 1, CFGFLAG_CLIENT, "Turnes kill messages on or off")
MACRO_CONFIG_INT(ClRenderVote, cl_render_vote, 1, 0, 1, CFGFLAG_CLIENT, "Turnes Vote messages on or off")
MACRO_CONFIG_INT(ClRenderViewmode, cl_render_viewmode, 1, 0, 1, CFGFLAG_CLIENT, "Turnes viewmode display on or off")
MACRO_CONFIG_INT(ClRenderEmotes, cl_render_emotes, 1, 0, 1, CFGFLAG_CLIENT, "Turnes emoticons on or off")

MACRO_CONFIG_INT(ClClearHud, cl_clear_hud, 0, 0, 1, CFGFLAG_CLIENT, "Turnes hud on or off")
MACRO_CONFIG_INT(ClClearAll, cl_clear_all, 0, 0, 1, CFGFLAG_CLIENT, "Cleans the whole screen")

MACRO_CONFIG_INT(ClServermsgsound, cl_servermsgsound, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Turnes servermessage sound on or off")
MACRO_CONFIG_INT(ClChatsound, cl_chatsound, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Turnes chat sound on or off")
MACRO_CONFIG_INT(ClSpreesounds, cl_spreesounds, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Turnes spreesounds on or off")

/* lvlx */
MACRO_CONFIG_STR(ClLvlxName, cl_lvlx_name, 16, "", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Set lvlx account name")
MACRO_CONFIG_STR(ClLvlxPass, cl_lvlx_pass, 32, "", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Set lvlx account pass")
MACRO_CONFIG_INT(ClShortHpDisplay, cl_short_hp_display, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Use short HP display")
MACRO_CONFIG_INT(ClShowExpBar, cl_show_exp_bar, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show Exp bar")
MACRO_CONFIG_INT(ClExpBarColor, cl_exp_bar_color, 10026844, 0, 0xFFFFFF, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show Exp bar")
MACRO_CONFIG_INT(ClShowSpreeMessages, cl_show_spree_messages, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show spree messages")

/* Beep-Mod */
MACRO_CONFIG_INT(ClSoundId, cl_sound_id, 0, 0, 38, CFGFLAG_CLIENT, "Plays a special sound")
MACRO_CONFIG_INT(ClChangeSound, cl_change_sound, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Turns special chat sound on or off")
MACRO_CONFIG_INT(ClChangeColor, cl_change_color, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Changes the color in messages if it includes the name")
MACRO_CONFIG_STR(ClSearchName, cl_search_name, 64, "nameless tee", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Names to search for in chat messages")
MACRO_CONFIG_INT(ClAntiSpam, cl_anti_spam, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Disables multiple messages")
MACRO_CONFIG_INT(ClBlockSpammer, cl_block_spammer, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Disables chat messages for player you want to ignore")
MACRO_CONFIG_STR(ClSpammerName, cl_spammer_name, 64, "nameless tee", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Names of players you want to ignore")

/* Laser Color */
MACRO_CONFIG_INT(ClLaserColor, cl_laser_color, 11665217, 0, 16777215, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Laser color")

/* Anti rainbow */
MACRO_CONFIG_INT(ClAntiRainbowCount, cl_anti_rainbow_count, 2, 0, 20, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Set how many colorchanges are allowed")
MACRO_CONFIG_INT(ClAntiRainbow, cl_anti_rainbow, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Enable anti rainbow")

/* race - client */
MACRO_CONFIG_INT(ClAutoRecord, cl_auto_record, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Save the best demo of each race")
MACRO_CONFIG_INT(ClShowOthers, cl_show_others, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Turn other players off in race")
MACRO_CONFIG_INT(ClShowCheckpointDiff, cl_show_checkpoint_diff, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show checkpoint diff")
MACRO_CONFIG_INT(ClShowRecords, cl_show_records, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show records")
MACRO_CONFIG_INT(ClShowServerRecord, cl_show_server_record, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show server record")
MACRO_CONFIG_INT(ClShowLocalRecord, cl_show_local_record, 1, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Show personal best")

MACRO_CONFIG_INT(PlayerUseCustomColor, player_use_custom_color, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Toggles usage of custom colors")
MACRO_CONFIG_INT(PlayerColorBody, player_color_body, 65408, 0, 0xFFFFFF, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Player body color")
MACRO_CONFIG_INT(PlayerColorFeet, player_color_feet, 65408, 0, 0xFFFFFF, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Player feet color")
MACRO_CONFIG_STR(PlayerSkin, player_skin, 64, "default", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Player skin")

MACRO_CONFIG_INT(UiPage, ui_page, 5, 0, 9, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface page")
MACRO_CONFIG_INT(UiToolboxPage, ui_toolbox_page, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Toolbox page")
MACRO_CONFIG_STR(UiServerAddress, ui_server_address, 25, "localhost:8303", CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface server address")
MACRO_CONFIG_INT(UiScale, ui_scale, 100, 1, 100000, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface scale")

MACRO_CONFIG_INT(UiColorHue, ui_color_hue, 160, 0, 255, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface color hue")
MACRO_CONFIG_INT(UiColorSat, ui_color_sat, 70, 0, 255, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface color saturation")
MACRO_CONFIG_INT(UiColorLht, ui_color_lht, 175, 0, 255, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface color lightness")
MACRO_CONFIG_INT(UiColorAlpha, ui_color_alpha, 228, 0, 255, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Interface alpha")

MACRO_CONFIG_INT(GfxNoclip, gfx_noclip, 0, 0, 1, CFGFLAG_CLIENT|CFGFLAG_SAVE, "Disable clipping")

/* TeeComp */
#include "teecomp_vars.h"

// server
MACRO_CONFIG_INT(SvWarmup, sv_warmup, 0, 0, 0, CFGFLAG_SERVER, "Number of seconds to do warpup before round starts")
MACRO_CONFIG_STR(SvMotd, sv_motd, 900, "", CFGFLAG_SERVER, "Message of the day to display for the clients")
MACRO_CONFIG_INT(SvTeamdamage, sv_teamdamage, 0, 0, 1, CFGFLAG_SERVER, "Team damage")
MACRO_CONFIG_STR(SvMaprotation, sv_maprotation, 768, "", CFGFLAG_SERVER, "Maps to rotate between")
MACRO_CONFIG_INT(SvRoundsPerMap, sv_rounds_per_map, 1, 1, 100, CFGFLAG_SERVER, "Number of rounds on each map before rotating")
MACRO_CONFIG_INT(SvPowerups, sv_powerups, 1, 0, 1, CFGFLAG_SERVER, "Allow powerups like ninja")
MACRO_CONFIG_INT(SvScorelimit, sv_scorelimit, 20, 0, 1000, CFGFLAG_SERVER, "Score limit (0 disables)")
MACRO_CONFIG_INT(SvTimelimit, sv_timelimit, 0, 0, 1000, CFGFLAG_SERVER, "Time limit in minutes (0 disables)")
MACRO_CONFIG_STR(SvGametype, sv_gametype, 32, "dm", CFGFLAG_SERVER, "Game type (dm, tdm, ctf)")
MACRO_CONFIG_INT(SvTournamentMode, sv_tournament_mode, 0, 0, 1, CFGFLAG_SERVER, "Tournament mode. When enabled, players joins the server as spectator")
MACRO_CONFIG_INT(SvSpamprotection, sv_spamprotection, 1, 0, 1, CFGFLAG_SERVER, "Spam protection")

MACRO_CONFIG_INT(SvSpectatorSlots, sv_spectator_slots, 0, 0, MAX_CLIENTS, CFGFLAG_SERVER, "Number of slots to reserve for spectators")
MACRO_CONFIG_INT(SvTeambalanceTime, sv_teambalance_time, 1, 0, 1000, CFGFLAG_SERVER, "How many minutes to wait before autobalancing teams")

MACRO_CONFIG_INT(SvVoteKick, sv_vote_kick, 1, 0, 1, CFGFLAG_SERVER, "Allow voting to kick players")
MACRO_CONFIG_INT(SvVoteKickBantime, sv_vote_kick_bantime, 5, 0, 1440, CFGFLAG_SERVER, "The time to ban a player if kicked by vote. 0 makes it just use kick")
MACRO_CONFIG_INT(SvVoteScorelimit, sv_vote_scorelimit, 0, 0, 1, CFGFLAG_SERVER, "Allow voting to change score limit")
MACRO_CONFIG_INT(SvVoteTimelimit, sv_vote_timelimit, 0, 0, 1, CFGFLAG_SERVER, "Allow voting to change time limit")

// debug
#ifdef CONF_DEBUG // this one can crash the server if not used correctly
	MACRO_CONFIG_INT(DbgDummies, dbg_dummies, 0, 0, 15, CFGFLAG_SERVER, "")
#endif

MACRO_CONFIG_INT(DbgFocus, dbg_focus, 0, 0, 1, CFGFLAG_CLIENT, "")
MACRO_CONFIG_INT(DbgTuning, dbg_tuning, 0, 0, 1, CFGFLAG_CLIENT, "")
#endif