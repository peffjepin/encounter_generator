#!/usr/bin/env python3

import util


def main():
    paths = list(util.root.iterdir())
    while paths:
        p = paths.pop()
        if p.is_dir():
            paths.extend(p.iterdir())
        else:
            if p.name.endswith(".old"):
                p.unlink()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
