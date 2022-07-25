#!/usr/bin/env python3

import model


def main():
    monsters = model.Monster.load()
    lookup = {monster.id: monster.name for monster in monsters}
    import sys

    for ln in sys.stdin.read().splitlines():
        if not ln:
            continue
        try:
            print(lookup[int(ln.strip())])
        except Exception:
            print(ln)


if __name__ == "__main__":
    raise SystemExit(main())
