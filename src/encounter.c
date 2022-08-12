#include "encounter.h"

#include <assert.h>

#include "monster_data.h"
#include "noise.h"

static const unsigned CHARACTER_XP_TABLE[20][4] = {
    {25, 50, 75, 100},         {50, 100, 150, 200},       {75, 150, 225, 400},
    {125, 250, 375, 500},      {250, 500, 750, 1100},     {300, 600, 900, 1400},
    {350, 750, 1100, 1700},    {450, 900, 1400, 2100},    {550, 1100, 1600, 2400},
    {600, 1200, 1900, 2800},   {800, 1600, 2400, 3600},   {1000, 2000, 3000, 4500},
    {1100, 2200, 3400, 5100},  {1250, 2500, 3800, 5700},  {1400, 2800, 4300, 6400},
    {1600, 3200, 4800, 7200},  {2000, 3900, 5900, 8800},  {2100, 4200, 6300, 9500},
    {2400, 4900, 7300, 10900}, {2800, 5700, 8500, 12700},
};

typedef struct {
    float value;
    unsigned experience;
} ChallengeEntry;

const ChallengeEntry CHALLENGE_TABLE[CR_MAX] = {
    [CR0] = {0.0, 10},       [CR1_8] = {0.125, 25},   [CR1_4] = {0.25, 50},
    [CR1_2] = {0.5, 100},    [CR1] = {1.0, 200},      [CR2] = {2.0, 450},
    [CR3] = {3.0, 700},      [CR4] = {4.0, 1100},     [CR5] = {5.0, 1800},
    [CR6] = {6.0, 2300},     [CR7] = {7.0, 2900},     [CR8] = {8.0, 3900},
    [CR9] = {9.0, 5000},     [CR10] = {10.0, 5900},   [CR11] = {11.0, 7200},
    [CR12] = {12.0, 8400},   [CR13] = {13.0, 10000},  [CR14] = {14.0, 11500},
    [CR15] = {15.0, 13000},  [CR16] = {16.0, 15000},  [CR17] = {17.0, 18000},
    [CR18] = {18.0, 20000},  [CR19] = {19.0, 22000},  [CR20] = {20.0, 25000},
    [CR21] = {21.0, 33000},  [CR22] = {22.0, 41000},  [CR23] = {23.0, 50000},
    [CR24] = {24.0, 62000},  [CR25] = {25.0, 75000},  [CR26] = {26.0, 90000},
    [CR27] = {27.0, 105000}, [CR28] = {28.0, 120000}, [CR29] = {29.0, 135000},
    [CR30] = {30.0, 155000}};

static void
set_budget(Encounter* encounter, EncounterInputs* inputs)
{
    encounter->budget_target = 0;
    encounter->budget_max = 0;
    for (size_t i = 0; i < inputs->number_of_characters; i++) {
        if (inputs->difficulty == Deadly) {
            unsigned allotment =
                CHARACTER_XP_TABLE[inputs->character_levels[i] - 1][inputs->difficulty];
            encounter->budget_target += allotment;
            encounter->budget_max += allotment * 1.5;
        }
        else {
            encounter->budget_target +=
                CHARACTER_XP_TABLE[inputs->character_levels[i] - 1][inputs->difficulty];
            encounter->budget_max += CHARACTER_XP_TABLE[inputs->character_levels[i] - 1]
                                                       [inputs->difficulty + 1];
        }
    }
    const float budget_multiplier = 1.25;
    encounter->budget_target *= budget_multiplier;
    encounter->budget_max *= budget_multiplier;
}

#define EXP_COST(monster_id)                                                             \
    CHALLENGE_TABLE[monster_data_by_id(monster_id)->cr].experience
#define ENCOUNTER_SIZE_MULT_MAX 15
static const float ENCOUNTER_SIZE_MULT[ENCOUNTER_SIZE_MULT_MAX] = {
    1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 2.5, 2.5, 2.5, 2.5, 3.0, 3.0, 3.0, 3.0, 4.0};

// returns non 0 on failure
static int
add_monster_to_encounter(size_t monster_id, Encounter* encounter)
{
    if (encounter->number_of_enemies >= MAX_ENEMIES) return 1;
    encounter->monster_ids[encounter->number_of_enemies++] = monster_id;
    encounter->cost += EXP_COST(monster_id);
    // the rule is to apply the multiplier table value as a multiplier
    // on monster exp cost as the number of monsters in the encounter
    // increases. it's easier to apply the inverse to the budget in this case.
    // the rule also states not to use the multiplier on very low challenge
    // monsters relative to the players, I may just allocate a slightly larger
    // budget to account for this.
    if (encounter->number_of_enemies < ENCOUNTER_SIZE_MULT_MAX &&
        encounter->number_of_enemies > 1) {
        size_t current_index = encounter->number_of_enemies - 1;
        float inverse_factor =
            ENCOUNTER_SIZE_MULT[current_index - 1] / ENCOUNTER_SIZE_MULT[current_index];
        encounter->budget_target *= inverse_factor;
        encounter->budget_max *= inverse_factor;
    }
    return 0;
}

static size_t running_totals_by_cr[CR_MAX] = {MAX_MONSTERS};

static void
precompute_running_totals_by_cr_array(void)
{
    size_t total = 0;
    for (Challenge cr = CR0; cr < CR_MAX; cr++) {
        size_t count;
        ids_by_cr(cr, &count);
        total += count;
        running_totals_by_cr[cr] = total;
    }
}

#define ASSERT_VALID_MONSTER(monster_id)                                                 \
    assert(monster_id >= 0 && monster_id < number_monsters)

static size_t
select_seed_monster(Challenge lower_bound, Challenge upper_bound, RNG* rng)
{
    if (running_totals_by_cr[0] == MAX_MONSTERS) precompute_running_totals_by_cr_array();
    size_t start = (lower_bound == 0) ? 0 : running_totals_by_cr[lower_bound - 1];
    size_t diff = running_totals_by_cr[upper_bound] - start;
    size_t index = start + RNG_INDEX(rng, diff - 1);
    Challenge cr = lower_bound;
    while (running_totals_by_cr[cr] <= index) cr++;
    size_t prev = (cr == 0) ? 0 : running_totals_by_cr[cr - 1];
    size_t selected_monster_id = ids_by_cr(cr, NULL)[index - prev];
    ASSERT_VALID_MONSTER(selected_monster_id);
    return selected_monster_id;
}

typedef struct {
    Challenge lower_bound;
    Challenge upper_bound;
    size_t options_count;
    size_t options[MAX_MONSTERS];
    unsigned total_weight;
    unsigned weights[MAX_MONSTERS];
} WeightedChoice;

static void
accumulate_monster_weights(size_t monster_id, WeightedChoice* choice)
{
    AssociationWeights* weights = get_association_weights(monster_id);
    for (size_t i = 0; i < weights->length; i++) {
        MonsterAssociation assoc = weights->associations[i];
        MonsterData* data = monster_data_by_id(assoc.monster_id);
        if (data->cr > choice->upper_bound || data->cr < choice->lower_bound) continue;
        if (!choice->weights[assoc.monster_id])
            choice->options[(choice->options_count)++] = assoc.monster_id;
        choice->weights[assoc.monster_id] += assoc.weight;
        choice->total_weight += assoc.weight;
    }
}

#define MAX_RETRIES 5

// returns -1 on failure
// this might happen if the only possible choices would
// violate the encounters maximum budget
static int
make_weighted_choice(WeightedChoice* choice, Encounter* encounter, RNG* rng)
{
    for (int attempts = 1; attempts <= MAX_RETRIES; attempts++) {
        size_t index = RNG_INDEX(rng, choice->total_weight);
        int selected_monster_id = -1;
        unsigned running_weight = 0;
        for (size_t i = 0; i < choice->options_count; i++) {
            size_t monster_id = choice->options[i];
            running_weight += choice->weights[monster_id];
            if (running_weight >= index) {
                selected_monster_id = monster_id;
                break;
            }
        }
        ASSERT_VALID_MONSTER(selected_monster_id);

        if (encounter->cost + EXP_COST(selected_monster_id) <= encounter->budget_max)
            return selected_monster_id;
    }
    return -1;
}

int
generate_encounter(EncounterInputs* inputs, Encounter* encounter, RNG* rng)
{
    set_budget(encounter, inputs);

    unsigned total_player_level = 0;
    for (size_t i = 0; i < inputs->number_of_characters; i++) {
        total_player_level += inputs->character_levels[i];
    }
    unsigned average_player_level = total_player_level / inputs->number_of_characters;

    // CR0, CR1_8, CR1_4, CR1_2, CR1, CR2
    // 0    1      2      3      4    5
    // stop using CR1_8 at player level 5 then progress linearly
    // level 5 - 4 = CR1_8
    Challenge lower_bound =
        (average_player_level <= 4) ? 1 : ((int)average_player_level - 4);
    // use maximum of 2 CR stage higher than average player level
    // if player level == 1, (1 + 5) = CR3
    // upper bound is inclusive
    Challenge upper_bound = average_player_level + 5;
    // for an easier encounter we need to make sure the exp cost
    // associated with the upper bound wont violate encounter->budget_max
    while (CHALLENGE_TABLE[upper_bound].experience > encounter->budget_max) upper_bound--;

    WeightedChoice choice = {.lower_bound = lower_bound, .upper_bound = upper_bound};

    // If seed monsters aren't given we must choose one here
    if (inputs->number_of_seed_monsters == 0) {
        size_t seed_monster_id;
        unsigned attempts = 0;
        do {
            seed_monster_id =
                select_seed_monster(choice.lower_bound, choice.upper_bound, rng);
            accumulate_monster_weights(seed_monster_id, &choice);
        } while (choice.options_count == 0 && ++attempts <= MAX_RETRIES);
        add_monster_to_encounter(seed_monster_id, encounter);
    }
    // Otherwise just populate the weighed choice data structure with the given
    // seed monster ids
    else {
        for (size_t i = 0; i < inputs->number_of_seed_monsters; i++) {
            size_t seed_monster_id = inputs->seed_monster_ids[i];
            accumulate_monster_weights(seed_monster_id, &choice);
            if (add_monster_to_encounter(seed_monster_id, encounter)) return 1;
        }
    }
    // unless I enforce some specific constraints on my configuration data
    // it is possible to select a seed monster which only has associations
    // outside of the CR boundaries.
    if (choice.options_count == 0) return 1;

    // make weighted choices based on associations data until budget is broken
    while (encounter->cost < encounter->budget_target) {
        int monster_id = make_weighted_choice(&choice, encounter, rng);
        if (monster_id < 0 || add_monster_to_encounter(monster_id, encounter)) return 1;
        accumulate_monster_weights(monster_id, &choice);
    };
    return 0;
}

#ifdef ENCOUNTERS_MAIN
#include <stdio.h>
#include <time.h>

#include "_monster_data.h"

int
main(void)
{
    init_monster_data(BAKED_IN_MONSTER_DATA, BAKED_IN_LENGTH);
    init_weights_table(STATIC_CONFIG_FILEPATH);

    EncounterInputs inputs = {{1, 2, 3, 5}, 4, Deadly};
    unsigned seed = time(0);
    RNG rng = {.seed = seed};

    printf("Starting with seed %u\n", seed);
    printf("-----------------------\n");
    for (int times = 0; times < 10; times++) {
        Encounter encounter = {};
        if (generate_encounter(&inputs, &encounter, &rng)) {
            printf("generation exited with errorcode\n");
            return 1;
        }
        for (size_t i = 0; i < encounter.number_of_enemies; i++) {
            printf("%zu\n", encounter.monster_ids[i]);
        }
        printf("-----------------------\n");
    }

    free_weights_table();
    free_monster_data();

    return 0;
}
#endif
