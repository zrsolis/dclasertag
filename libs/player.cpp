#include "player.h"

#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif

#define EEPROM_ADDR     0xa0

int Player::playerLock = 0;

bool Player::my_team_id[NUM_TEAM_BITS] = {};
bool Player::my_player_id[NUM_PLAYERID_BITS];
bool Player::checksum[NUM_CHECKSUM_BITS];
bool Player::game_id[NUM_GAME_BITS];
bool Player::my_hit_type[NUM_HIT_TYPE_BITS];
int Player::my_health;         // 0..8 (have separate count for shield powerup or 0..16 and normally subtract 2?)
int Player::my_power_ups;      // if multiple allowed, use binary addition (similar to permissions)
EEPROM_COGDRIVER Player::state;
EEPROM* Player::eeprom = NULL;
bool Player::enemy_team[NUM_TEAM_BITS];
bool Player::enemy_player_id[NUM_PLAYERID_BITS];



void eepromInit(EEPROM *eeprom, I2C *dev, int address);
int eepromRead(EEPROM *eeprom, uint32_t address, uint8_t *buffer, int count);
int eepromWrite(EEPROM *eeprom, uint32_t address, uint8_t *buffer, int count);


volatile void Player::Init()
{
  //playerLock = locknew();

  //Get eeprom handle
  if ((eeprom = eepromOpen(&state, 28, 29, 400000, EEPROM_ADDR)) == NULL)
  {
    return;
  }

  //lockset(playerLock);
  SetHealth(MAX_HEALTH);
  my_power_ups = 0;

  memset(enemy_player_id, 0, NUM_PLAYERID_BITS);
  memset(enemy_team, 0, NUM_TEAM_BITS);
  memset(my_team_id, 0, NUM_TEAM_BITS);
  memset(my_player_id, 0, NUM_PLAYERID_BITS);
  memset(game_id, 0, NUM_GAME_BITS);
  
  print("EEPROM Read\n");
  eepromRead(eeprom, 0x8000, (uint8_t*)my_team_id, NUM_TEAM_BITS);
  eepromRead(eeprom, 0x8000 + NUM_TEAM_BITS, (uint8_t*)game_id, NUM_GAME_BITS);
  eepromRead(eeprom, 0x8000 + NUM_TEAM_BITS + NUM_GAME_BITS, (uint8_t*)my_player_id, NUM_PLAYERID_BITS);
  //print("EEPROM Read done\n");

//trying to be fancy with NUM_PLAYERID_BITS > 8, but the eeprom read is stalling the game
/*
  for(int i=0; (float)i < (float)(NUM_PLAYERID_BITS / 8); i++)
  {
    bool tmp_player_id_storage[NUM_PLAYERID_BITS] = {};
    int next_element = NUM_PLAYERID_BITS * i;

    eepromRead(eeprom, 0x8000 + NUM_TEAM_BITS + NUM_GAME_BITS + next_element, (uint8_t*)tmp_player_id_storage, NUM_PLAYERID_BITS);
    int remainder = NUM_PLAYERID_BITS % 8 > 0;

    //this is to catch the last loop
    if(remainder > 0 && i+1 > (float)(NUM_PLAYERID_BITS / 8))
    {
      tmp_player_id_storage = BitArray::ToBool(BitArray::ToInt(tmp_player_id_storage, NUM_PLAYERID_BITS) >> (NUM_PLAYERID_BITS - (NUM_PLAYERID_BITS % 8)), NUM_PLAYERID_BITS);
    }
    else
    {
      remainder = NUM_PLAYERID_BITS;
    }

    memcpy(my_player_id + next_element, tmp_player_id_storage, remainder);
  }
*/

  int hitNumber = 0;
  do
  {
    eepromRead(eeprom, 0x8000 + NUM_TEAM_BITS + NUM_GAME_BITS + NUM_PLAYERID_BITS + (hitNumber * NUM_PLAYERID_BITS), (uint8_t*)enemy_player_id, NUM_PLAYERID_BITS);
    hitNumber++;
  }
  while(IsEnemyPlayerIDEmpty() && hitNumber <= MAX_HEALTH);

  SetHealth(MAX_HEALTH - hitNumber + 1);
  SetChecksum();
  //lockclr(playerLock);
}

volatile void Player::Init(bool gameID[], bool playerID[], bool teamID[])
{
  Init();

  SetPlayerGameID(gameID);
  SetPlayerID(playerID);
  SetTeamID(teamID);
  SetChecksum();
}

volatile void Player::SetTeamID(bool data[])
{
  //lockset(playerLock);
  memcpy(my_team_id, data, NUM_TEAM_BITS);
  //lockclr(playerLock);

  if (eepromWrite(eeprom, 0x8000, (uint8_t*)my_team_id, NUM_TEAM_BITS) != 0) {
        return;
  }
}

volatile void Player::SetPlayerGameID(bool data[])
{

  //lockset(playerLock);
  memcpy(game_id, data, NUM_GAME_BITS);
  //lockclr(playerLock);

  if (eepromWrite(eeprom, 0x8000 + NUM_TEAM_BITS, (uint8_t*)game_id, NUM_GAME_BITS) != 0) {
          return;
  }
}

volatile void Player::SetPlayerID(bool data[])
{
  //lockset(playerLock);
  memcpy(my_player_id, data, NUM_PLAYERID_BITS);
  //lockclr(playerLock);

  if (eepromWrite(eeprom, 0x8000 + NUM_TEAM_BITS + NUM_GAME_BITS, (uint8_t*)my_player_id, NUM_PLAYERID_BITS) != 0) {
          return;
  }
}

// public wrapper
volatile void Player::RestoreHealth()
{
  SetHealth(MAX_HEALTH);
}

void Player::SetHealth(int health)
{
  //verify health is in normal bounds
  if(health >= 0 && health <= MAX_HEALTH)
  {
    my_health = health;
  }
}

void Player::SetChecksum()
{
  bool c = GetChecksum((bool*)PlayerGameID(), (bool*)HitType(), (bool*)TeamID(), (bool*)PlayerID());
  memcpy(checksum, &c, NUM_CHECKSUM_BITS);
}

volatile bool* Player::PlayerGameID()
{
  return game_id;
}

volatile bool* Player::PlayerID()
{
  return my_player_id;
}

volatile bool* Player::TeamID()
{
  return my_team_id;
}

volatile bool* Player::HitType()
{
  return my_hit_type;
}

volatile int Player::GetHealth()
{
  return my_health;
}

volatile bool Player::GetChecksum(bool gameID[], bool hit[], bool teamID[], bool playerID[])
{
  bool IRpartial[NUM_BITS_FOR_CHECKSUM];

  memcpy(IRpartial, gameID, NUM_GAME_BITS);
  memcpy(IRpartial + NUM_GAME_BITS, hit, NUM_HIT_TYPE_BITS);
  memcpy(IRpartial + NUM_GAME_BITS + NUM_HIT_TYPE_BITS, teamID, NUM_TEAM_BITS);
  memcpy(IRpartial + NUM_GAME_BITS + NUM_HIT_TYPE_BITS + NUM_TEAM_BITS, playerID, NUM_PLAYERID_BITS);

  //Number of 1 bits, GameID[3], Hit[2], Team[1], PID[8]
  int oneCount = 0;
  int boolAsInt = BitArray::ToInt(IRpartial, NUM_BITS_FOR_CHECKSUM);
  for(int i=0; i < NUM_BITS_FOR_CHECKSUM; i++)
  {
    if(boolAsInt & 1)
      ++oneCount;

    boolAsInt = boolAsInt >> 1;
  }

  return !(bool)(oneCount % 2);
}

volatile int Player::GetNumberTimesHit()
{
  return MAX_HEALTH - my_health;
}

volatile void Player::BadgeHit(bool teamID[], bool playerID[])
{
  if(IsAlive())
  {
    //check for friendly fire
    if(FRIENDLY_FIRE || !BitArray::bitArraySame(teamID, Player::TeamID(), NUM_TEAM_BITS))
    {
	    //lockset(playerLock);
       memcpy(enemy_player_id, playerID, NUM_PLAYERID_BITS);

       //save to eeprom

       if (eepromWrite(eeprom, 0x8000 + NUM_TEAM_BITS + NUM_GAME_BITS + NUM_PLAYERID_BITS + (GetNumberTimesHit() * (NUM_PLAYERID_BITS + NUM_TEAM_BITS)), (uint8_t*)enemy_team, NUM_TEAM_BITS) != 0) {
         return;
       }

       if (eepromWrite(eeprom, 0x8000 + NUM_TEAM_BITS + NUM_GAME_BITS + NUM_PLAYERID_BITS + (GetNumberTimesHit() * (NUM_PLAYERID_BITS + NUM_TEAM_BITS)) + NUM_TEAM_BITS, (uint8_t*)enemy_player_id, NUM_PLAYERID_BITS) != 0) {
         return;
       }

       SetHealth(GetHealth() - 1);
       print("Hit! Health is now %d\n", GetHealth());
       //lockclr(playerLock);
    }
    else
    {
      print("Hit! ... but on the same team :(\n");
    }
  }
}

volatile bool Player::IsAlive()
{
  return GetHealth() > 0;
}

volatile bool* Player::GetEnemyPlayerID(int hitNumber)
{
  //lockset(playerLock);
  //clear out enemy_player_id
  memset(enemy_player_id, 0, NUM_PLAYERID_BITS);

  if(hitNumber > 0 && hitNumber <= GetNumberTimesHit())
  {
    for(int i=0; (float)i < (float)(NUM_PLAYERID_BITS / 8); i++)
    {
      uint8_t* tmp_player_id_storage;
      int next_element = NUM_PLAYERID_BITS * i;

      eepromRead(eeprom, 0x8000 + NUM_TEAM_BITS + NUM_GAME_BITS + NUM_PLAYERID_BITS + (hitNumber * (NUM_PLAYERID_BITS + NUM_TEAM_BITS) ) + next_element, tmp_player_id_storage, NUM_PLAYERID_BITS);
      int remainder = NUM_PLAYERID_BITS % 8 > 0;

      //this is to catch the last loop
      if(remainder > 0 && i+1 > (float)(NUM_PLAYERID_BITS / 8))
      {
        tmp_player_id_storage = (uint8_t*)((int)tmp_player_id_storage >> (NUM_PLAYERID_BITS - (NUM_PLAYERID_BITS % 8)));
      }
      else
      {
        remainder = NUM_PLAYERID_BITS;
      }

      memcpy(enemy_player_id + next_element, tmp_player_id_storage, remainder);
    }
  }
  //lockclr(playerLock);

  return enemy_player_id;
}

bool Player::IsEnemyPlayerIDEmpty()
{
  for(int i = 0; i <= 9; i++)
  {
    if(enemy_player_id[i] == 1)
      return false;
  }

  return true;
}

/***************************************************/
/* These were copied from propgcc                  */
/***************************************************/
EEPROM *eepromOpen(EEPROM_COGDRIVER *eeprom, int scl, int sda, int freq, int address)
{
    I2C *dev;
    if (!(dev = i2cOpen(&eeprom->dev, scl, sda, freq)))
        return NULL;
    eepromInit(&eeprom->state, dev, address);
    return (EEPROM *)eeprom;
}

EEPROM *simple_eepromOpen(EEPROM_SIMPLE *eeprom, int scl, int sda, int address)
{
    I2C *dev;
    if (!(dev = simple_i2cOpen(&eeprom->dev, scl, sda)))
        return NULL;
    eepromInit(&eeprom->state, dev, address);
    return (EEPROM *)eeprom;
}

EEPROM *eepromBootOpen(EEPROM_BOOT *eeprom, int address)
{
    I2C *dev;
    if (!(dev = i2cBootOpen()))
        return NULL;
    eepromInit(&eeprom->state, dev, address);
    return (EEPROM *)eeprom;
}

void eepromInit(EEPROM *eeprom, I2C *dev, int address)
{
    eeprom->dev = dev;
    eeprom->address = address;
}

int eepromRead(EEPROM *eeprom, uint32_t address, uint8_t *buffer, int count)
{
    uint8_t buf[2] = { address >> 8, address };

    /* write the i2c header and the eeprom address */
    if (i2cWrite(eeprom->dev, eeprom->address, buf, 2, FALSE) != 0)
        return -1;

    /* read the eeprom data */
    if (i2cRead(eeprom->dev, eeprom->address, buffer, count, TRUE) != 0)
        return -1;

    return 0;
}

int eepromWrite(EEPROM *eeprom, uint32_t address, uint8_t *buffer, int count)
{
    uint8_t buf[2] = { address >> 8, address };

    /* write the i2c header and the eeprom address */
    if (i2cWrite(eeprom->dev, eeprom->address, buf, 2, FALSE) != 0)
        return -1;

    /* write the eeprom data */
    if (i2cWriteMore(eeprom->dev, buffer, count, TRUE) != 0)
        return -1;

    /* wait for the write to complete (maybe should have a timeout?) */
    while (i2cWrite(eeprom->dev, eeprom->address, NULL, 0, TRUE) != 0)
        ;

    return 0;
}