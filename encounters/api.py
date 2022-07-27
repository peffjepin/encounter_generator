import _encounters
import time
import pathlib
from .model import Monster

datadir = pathlib.Path(__file__).parent / "data"
monsters = [eval(ln) for ln in (datadir / "reprs").read_text().splitlines()]
monster_lookup = {m.id: m for m in monsters}


def generate(player_levels, difficulty, seed=None):
    if seed is None:
        seed = int(time.time() * 10000000)
    ids = _encounters.generate(player_levels, difficulty, seed)
    return tuple(monster_lookup.get(id).name for id in ids)
