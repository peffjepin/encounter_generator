#ifndef MONSTER_DATA_H
#define MONSTER_DATA_H

#include <stddef.h>
#define MAX_MONSTERS 256

enum Challenge {
    CR0,
    CR1_8,
    CR1_4,
    CR1_2,
    CR1,
    CR2,
    CR3,
    CR4,
    CR5,
    CR6,
    CR7,
    CR8,
    CR9,
    CR10,
    CR11,
    CR12,
    CR13,
    CR14,
    CR15,
    CR16,
    CR17,
    CR18,
    CR19,
    CR20,
    CR21,
    CR22,
    CR23,
    CR24,
    CR25,
    CR26,
    CR27,
    CR28,
    CR29,
    CR30,
    CR_MAX
};

typedef struct {
    size_t monster_id;
    unsigned weight;
} MonsterAssociation;

typedef struct {
    size_t length;
    MonsterAssociation* associations;
} AssociationWeights;

typedef struct {
    size_t id;  // monster ids should start at 0
    enum Challenge cr;
} MonsterData;

extern size_t number_monsters;

AssociationWeights* get_association_weights(size_t monster_id);
size_t* ids_by_cr(enum Challenge cr, size_t* length);
MonsterData* monster_data_by_id(size_t monster_id);

void init_monster_data(MonsterData* data, size_t length);
void free_monster_data(void);
void init_weights_table(void);
void free_weights_table(void);

#endif // MONSTER_DATA_H
