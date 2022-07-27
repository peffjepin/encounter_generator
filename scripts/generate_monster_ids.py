#!/usr/bin/env python3

import util


def main():
    monsters = util.load()
    for i, m in enumerate(monsters):
        m.id = i
    util.dump(monsters)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
