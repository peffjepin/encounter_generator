#!/usr/bin/env python3

import encounters


def main():
    for _ in range(20):
        print("----------------------------------")
        for monster in encounters.generate((3, 3, 3), 3):
            print(monster.name)
        print("----------------------------------")

    print("==================================")
    print("using seed monsters")
    print("==================================")
    for n in range(5):
        print("----------------------------------")
        seed_monster_ids = [0] * (n+1)
        for monster in encounters.generate((1, 2, 3), 3, seed_monsters=seed_monster_ids):
            print(monster.name)
        print("----------------------------------")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
