#!/usr/bin/env python3

import util

monster_data_header = util.srcdir / "_monster_data.h"
monster_data_source = util.srcdir / "_monster_data.c"


class MonsterDataStruct:
    def __init__(self, monster):
        self.monster = monster

    def __str__(self):
        return f"{{{self.monster.id},{self.cr}}}"

    @property
    def cr(self):
        return "CR" + self.monster.cr.split()[0].replace("/", "_")


def main():
    monsters = util.load()
    header_source = f"""
#ifndef BAKED_IN_DATA_H
#define BAKED_IN_DATA_H
#include "monster_data.h"
#define BAKED_IN_LENGTH {len(monsters)}
extern MonsterData BAKED_IN_MONSTER_DATA[BAKED_IN_LENGTH];
#endif
    """.strip()
    structs = [MonsterDataStruct(m) for m in monsters]
    source_source = f"""
#include "_monster_data.h"
MonsterData BAKED_IN_MONSTER_DATA[BAKED_IN_LENGTH] = {{{",".join(map(str, structs))}}};
    """.strip()
    util.backup(monster_data_header)
    util.backup(monster_data_source)
    monster_data_header.write_text(header_source)
    monster_data_source.write_text(source_source)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
