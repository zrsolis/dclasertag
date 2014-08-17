#define MAX_HEALTH            8
#define NUM_TEAMS             2        // number of teams playing
#define NUM_TEAM_BITS         1
#define NUM_HIT_TYPE_BITS     2
#define NUM_PLAYERID_BITS     8
#define NUM_GAME_BITS         3
#define NUM_CHECKSUM_BITS     1
#define NUM_RANDOM_BITS       1
#define NUM_BITS_FOR_CHECKSUM 14       //the first X bits to use when calculating the checksum
#define RATE_OF_FIRE          1000     //one shot every X milliseconds (1000 milliseconds = 1 second)
#define FRIENDLY_FIRE         0