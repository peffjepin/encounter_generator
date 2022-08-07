#!/usr/bin/env python3

import encounters


def main():
    for _ in range(20):
        print(encounters.generate((1, 2, 3), 3))

    print("using seed monsters")
    for n in range(5):
        seed_ids = [0] * (n + 1)
        print(encounters.generate((1, 2, 3), 3, seed_monsters=seed_ids))

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
