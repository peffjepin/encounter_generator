#!/usr/bin/env python3

import encounters


def main():
    for _ in range(20):
        print(encounters.generate((1, 2, 3), 3))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
