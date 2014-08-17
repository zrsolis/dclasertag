#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <propeller.h>
#include "eeprom.h"
#include "../settings/settings.h"
#include "../libs/BitArray.h"

class Player {
private:
  static void SetHealth(int health);
  static void SetChecksum();
  static bool IsEnemyPlayerIDEmpty();

  static int playerLock;
  static bool checksum[NUM_CHECKSUM_BITS];
  static bool my_team_id[NUM_TEAM_BITS];
  static bool my_player_id[NUM_PLAYERID_BITS];
  static bool game_id[NUM_GAME_BITS];
  static bool my_hit_type[NUM_HIT_TYPE_BITS];
  static int my_health;         // 0..8 (have separate count for shield powerup or 0..16 and normally subtract 2?)
  static int my_power_ups;      // if multiple allowed, use binary addition (similar to permissions)

  //eeprom vars
  static EEPROM_COGDRIVER state;
  static EEPROM *eeprom;
  static bool enemy_team[NUM_TEAM_BITS];
  static bool enemy_player_id[NUM_PLAYERID_BITS];

public:
  static volatile void Init();
  static volatile void Init(bool gameID[], bool playerID[], bool teamID[]);

  static volatile void SetPlayerGameID(bool data[]);
  static volatile void SetPlayerID(bool data[]);
  static volatile void SetTeamID(bool data[]);
  static volatile void RestoreHealth();

  static volatile bool* PlayerGameID();
  static volatile bool* PlayerID();
  static volatile bool* TeamID();
  static volatile bool* HitType();
  static volatile int GetHealth();
  static volatile int GetNumberTimesHit();
  static volatile bool GetChecksum(bool gameID[], bool hit[], bool teamID[], bool playerID[]);

  //returns the player id who hit them
  //passing in "2" will return the second player who has hit them this life.
  //will return all 0's if no player id found
  static volatile bool* GetEnemyPlayerID(int hitNumber);

  //we have been hit!
  static volatile void BadgeHit(bool teamID[], bool playerID[]);
  static volatile bool IsAlive();
};

#endif