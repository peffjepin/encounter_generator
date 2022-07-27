#!/usr/bin/env python3

import util
import re

text_file = util.datadir / "associations.txt"
config_file = util.datadir / "associations.conf"


def main():
    name_to_id = {
        monster.name.lower(): monster.id for monster in util.load()
    }
    human_friendly_source = text_file.read_text()
    machine_friendly_source = re.sub(
        r"[^:,0-9\n]+",
        lambda m: str(name_to_id[m.group().strip().lower()]),
        human_friendly_source,
    ).strip()
    util.backup(config_file)
    config_file.write_text(machine_friendly_source)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
