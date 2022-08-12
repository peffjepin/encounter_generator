#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "monster_data.h"

#ifdef BAKED_MONSTER_DATA
#include "_monster_data.h"
#endif

#define EXIT(...) { fprintf(stderr, "ERROR: ");     \
                    fprintf(stderr, __VA_ARGS__);   \
                    fprintf(stderr, "\n");          \
                    exit(1); }

size_t number_monsters = 0;
static AssociationWeights* weights_table = NULL;

#define MAX_ASSOCIATIONS 32

typedef struct {
    size_t length;
    size_t* ids;
} MonstersByCR;

static MonstersByCR monster_ids_by_cr_lookup[CR_MAX] = {0};
static MonsterData* monster_data_array = NULL;

void
init_monster_data(MonsterData* data, size_t length)
{
    // low hanging fruit for optimization, however in the projects current
    // stage this will only be executed once as a setup step so is negligible.
    monster_data_array = data;
    number_monsters = length;

    // accumulate which CR values are represented in the dataset
    bool cr_present[CR_MAX] = {false};
    for (size_t i=0; i < length; i++) cr_present[data[i].cr] = true;

    // create lookup arrays of monster ids keyed by challenge rating
    for (size_t cr=0; cr < CR_MAX; cr++) {
        if (!cr_present[cr]) continue;

        size_t ids_buffer[length];
        size_t buffer_index = 0;
        for (size_t id=0; id < length; id++) {
            if (data[id].cr != cr) continue;
            ids_buffer[buffer_index++] = id;
        }
        size_t array_nbytes = sizeof(size_t) * buffer_index;
        size_t* ids_array = malloc(array_nbytes);
        if (!ids_array) EXIT("failed to malloc monster ids by cr lookup array");
        memcpy(ids_array, ids_buffer, array_nbytes);
        monster_ids_by_cr_lookup[cr].length = buffer_index;
        monster_ids_by_cr_lookup[cr].ids = ids_array;
    }
}

void free_monster_data(void)
{
    for (size_t cr=0; cr < CR_MAX; cr++) free(monster_ids_by_cr_lookup[cr].ids);
#ifndef BAKED_MONSTER_DATA
    free(monster_data_array);
#endif
}

void init_weights_table(char* filepath)
{
#ifdef BAKED_MONSTER_DATA
    init_monster_data(BAKED_IN_MONSTER_DATA, BAKED_IN_LENGTH);
#endif
    if (number_monsters == 0) EXIT("no monster data available");

    free_weights_table();
    weights_table = malloc(sizeof(AssociationWeights) * number_monsters);
    if (!weights_table) EXIT("failed to allocate weights table");

    FILE* config = fopen(filepath, "r");
    if (!config) EXIT("failed opening %s", filepath);

    char line_buffer[512];
    MonsterAssociation associations_buffer[MAX_ASSOCIATIONS];

    while (fgets(line_buffer, 512, config)) {
        // NOTE: does not support for empty lines
        size_t monster_id = atoi(strtok(line_buffer, ","));
        if (monster_id >= number_monsters) EXIT("monster id (%zu) out of range", monster_id);
        size_t association_index = 0;

        // parse each association into the associations buffer
        for (;;association_index++) {
            if (association_index == 512)
                EXIT("max associations exceeded for monster_id: %zu", monster_id);
            char* associated_id_text = strtok(NULL, ":");
            if (!associated_id_text) break;
            associations_buffer[association_index].monster_id = atoi(associated_id_text);
            unsigned associated_value = atoi(strtok(NULL, ","));
            associations_buffer[association_index].weight = associated_value * associated_value;
        }

        // place associations in heap allocated memory
        size_t associations_size = sizeof(MonsterAssociation) * association_index;
        MonsterAssociation* associations = malloc(associations_size);
        if (!associations) EXIT("could not malloc monster associations");
        memcpy(associations, associations_buffer, associations_size);

        weights_table[monster_id].length = association_index;
        weights_table[monster_id].associations = associations;
    }

    fclose(config);
}

void free_weights_table(void)
{
    if (!weights_table) return;
    for (size_t i=0; i < number_monsters; i++) {
        free(weights_table[i].associations);
    }
    free(weights_table);
    weights_table = NULL;
}

AssociationWeights*
get_association_weights(size_t monster_id)
{
    if (monster_id >= number_monsters)
        EXIT("monster_id (%zu) out of range", monster_id);
    return weights_table + monster_id;
}

size_t *
ids_by_cr(Challenge cr, size_t* length)
{
    MonstersByCR entry = monster_ids_by_cr_lookup[cr];
    if (length != NULL) *length = entry.length;
    return entry.ids;
}


MonsterData*
monster_data_by_id(size_t monster_id)
{
    if (!monster_data_array) EXIT("no monster data found");
    if (monster_id >= number_monsters) EXIT("monster id out of range: %zu", monster_id);
    return monster_data_array + monster_id;
}
