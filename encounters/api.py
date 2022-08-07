import _encounters
import time
import pathlib

from .model import Monster

datadir = pathlib.Path(__file__).parent / "data"
monsters = [eval(ln) for ln in (datadir / "reprs").read_text().splitlines()]
name_by_id = {m.id: m for m in monsters}
id_by_name = {m.name.lower(): m.id for m in monsters}


def generate(player_levels, difficulty, seed=None, seed_monsters=None):
    if seed is None:
        seed = int(time.time() * 10000000)
    ids = _encounters.generate(player_levels, difficulty, seed, seed_monsters)
    return tuple(name_by_id.get(id).name for id in ids)


__all__ = (
    "monsters",
    "name_by_id",
    "id_by_name",
    "generate",
)
