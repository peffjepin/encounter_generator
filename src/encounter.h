#include <assert.h>

#include "monster_data.h"
#include "noise.h"

typedef enum { Easy, Medium, Hard, Deadly } Difficulty;

#define MAX_CHARACTERS 10
#define MAX_ENEMIES 20

typedef struct {
    unsigned character_levels[MAX_CHARACTERS];
    size_t number_of_characters;
    size_t seed_monster_ids[MAX_ENEMIES];
    size_t number_of_seed_monsters;
    Difficulty difficulty;
} EncounterInputs;

typedef struct {
    size_t monster_ids[MAX_ENEMIES];
    size_t number_of_enemies;
    unsigned cost;
    unsigned budget_target;
    unsigned budget_max;
} Encounter;

int generate_encounter(EncounterInputs* inputs, Encounter* encounter, RNG* rng);
