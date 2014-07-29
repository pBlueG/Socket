/**
 *
 * SA:MP Server protocol
 * (c) BlueG
 * requires the Socket Plugin v0.2b
 */

#include <a_samp>
#include <socket>

#define NULL 0

#define PRTCL_RULES 		'r'     // Server rules such as lagcomp, weburl etc.
#define PRTCL_PLAYERLIST 	'c'     // basic playerlist with score
#define PRTCL_PLAYERINFO 	'd'     // playerlist including playerid, score & ping
#define PRTCL_SERVERINFO    'i'     // Server information such as hostname, players/maxplayers
#define PRTCL_RCON          'x'     // rcon commands -> sendPacket(ip, port, PRTCL_RCON, "gmx")

new Socket:StatsHandle;

public OnFilterScriptInit()
{
	new IP[20] = "188.226.221.218";
	new Port = 7777;
	StatsHandle = socket_create(UDP);
	if(udpConnect(IP, Port))
		sendPacket(IP, Port, PRTCL_RULES);
	return 1;
}

stock udpConnect(ip[], port)
{
	StatsHandle = socket_create(UDP);
	return socket_connect(StatsHandle, ip, port);
}

stock sendPacket(ip[], port, prtcl_type, extra[] = "")
{
	new sPacket[60], aIP[4][4], idx;
	aIP[0] = strtok(ip, idx, '.');
	aIP[1] = strtok(ip, idx, '.');
	aIP[2] = strtok(ip, idx, '.');
	aIP[3] = strtok(ip, idx, '.');
	format(sPacket, sizeof sPacket, "SAMP%c%c%c%c%c%c%c", strval(aIP[0]), strval(aIP[1]), strval(aIP[2]), strval(aIP[3]), (port & 0xFF), (port >> 8 & 0xFF), prtcl_type);
	if(strlen(extra) > 0)
	    strcat(sPacket, extra);
	return socket_send(StatsHandle, sPacket, strlen(sPacket));
}

public onUDPReceiveData(Socket:id, data[], data_len, remote_client_ip[], remote_client_port)
{
	if(id == StatsHandle && data_len > 11) {
        new prtcl_type = data[10];
		array_slice(data, 11, data_len);
		switch(prtcl_type) {
		    // Server information
		    case PRTCL_SERVERINFO: {
				//kinda a mess to parse this in PAWN
		        new bool:passworded = (data[0] != NULL ? true : false),
		            players[4], sLen[4],
					iHost, iMod, iMap, iPlayers, iMaxPlayers,
					sHostname[80], sModename[40], sMapname[40];
				#pragma unused passworded
				format(players, 4, "%c%c\0\0", data[1], data[2]);
				iPlayers = toInteger(players);
				format(players, 4, "%c%c\0\0", data[3], data[4]);
				iMaxPlayers = toInteger(players);
				format(sLen, 4, "%c%c%c%c", data[5], data[6], data[7], data[8]);
				iHost = toInteger(sLen);
				array_slice(data, 9, (data_len-11));
				array_clean(data, 'x', (data_len-20));
				strmid(sHostname, data, 0, iHost);
				strmid(sLen, data, iHost, iHost+4);
				iMod = toInteger(sLen);
				array_slice(data, (iHost+4), (data_len-20));
				strmid(sModename, data, 0, iMod);
				strmid(sLen, data, iMod, iMod+4);
				iMap = toInteger(sLen);
				array_slice(data, (iMod+4), (data_len-20-iMod));
				strmid(sMapname, data, 0, iMap);
				printf("Players %d/%d", iPlayers, iMaxPlayers);
				printf("Passworded %i (0 = No, 1 = Yes)", passworded);
				printf("Hostname %s", sHostname);
				printf("Modename %s", sModename);
				printf("Map %s", sMapname);
			}
			// Server rules
			case PRTCL_RULES: {
				// TODO
			}
			// Player list + score
			case PRTCL_PLAYERLIST: {
			    new players[4], iPlayers,
					sPlayer[MAX_PLAYER_NAME], iLen, iScore,
					score[4];
			    format(players, 4, "%c%c\0\0", data[0], data[1]);
			    iPlayers = toInteger(players);
			    array_slice(data, 2, data_len-11);
			    for(new i;i < iPlayers;i++) {
			        iLen = data[0];
			        strmid(sPlayer, data, 1, iLen+1);
					format(score, 4, "%c%c%c%c", data[iLen+1], data[iLen+2], data[iLen+3], data[iLen+4]);
			        iScore = toInteger(score);
			        array_slice(data, strlen(sPlayer)+5, (data_len-13));
			        printf("Player %s (Score %d)", sPlayer, iScore);
				}
			}
			// Player list + playerid + ping + score
			case PRTCL_PLAYERINFO: {
			    // TODO
			}
			// RCON response
			case PRTCL_RCON: {
			    // TODO
			}
			default: {
			    printf("Invalid SA:MP protocol.");
			}
		}
	}
	return 1;
}

stock toInteger(bytes[], s=sizeof(bytes))
{
	new ret = 0;
	ret += bytes[0];
	if(s > 1 && bytes[1] != NULL)
	    ret += bytes[1] << 8;
	if(s > 2 && bytes[2] != NULL)
	    ret += bytes[2] << 16;
	if(s > 3 && bytes[3] != NULL)
	    ret += bytes[3] << 24;
	return ret;
}

array_slice(arr[], idx, size = sizeof(arr))
{
	for(new i, a;i < size;i++) {
 		if(i >= idx)
   			arr[a++] = arr[i];
	}
}

array_clean(arr[], replace, size = sizeof(arr))
{
	for(new i, a = size;i < a;i++) {
	    if(arr[i] < 0)
	        arr[i] = replace;
	}
}

strtok(const string[], &index, delimiter)
{
	new length = strlen(string);
	while ((index < length) && (string[index] <= delimiter))
	{
		index++;
	}

	new offset = index;
	new result[4];
	while ((index < length) && (string[index] > delimiter) && ((index - offset) < (sizeof(result) - 1)))
	{
		result[index - offset] = string[index];
		index++;
	}
	result[index - offset] = EOS;
	return result;
}
