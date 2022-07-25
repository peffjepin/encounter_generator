#!/usr/bin/env python3

import model


def main():
    monsters = model.Monster.load()
    for i, m in enumerate(monsters):
        m.id = i
    model.Monster.dump(monsters)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
