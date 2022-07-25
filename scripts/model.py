import dataclasses
import pathlib
import shutil

root = pathlib.Path(__file__).parent.parent
srcdir = root / "src"
datadir = root / "data"
reprs = datadir / "reprs"


@dataclasses.dataclass
class Monster:
    name: str
    size: str
    type: str
    alignment: str
    ac: str
    hp: str
    speed: str
    ability_scores: str
    saving_throws: str
    skills: str
    damage_resistances: str
    damage_immunities: str
    damage_vulnerabilities: str
    condition_immunities: str
    senses: str
    languages: str
    cr: str
    features: tuple
    actions: dict
    legendary_actions: dict
    id: int
    associations: dict
    reactions: dict

    def __str__(self):
        return f"{self.name}: CR: {self.cr}"

    @classmethod
    def load(cls, filename=None):
        if filename is None:
            src = reprs.read_text()
        else:
            src = pathlib.Path(filename).read_text()
        return [eval(ln) for ln in src.splitlines()]

    @classmethod
    def dump(cls, monsters, filename=None):
        if filename is None:
            path = reprs
        else:
            path = pathlib.Path(filename)

        if path.exists():
            shutil.move(path, path.parent / f"{path.name}.old")
        with open(path, "w") as f:
            for monster in monsters:
                f.write(repr(monster) + "\n")


def backup(path):
    if not isinstance(path, pathlib.Path):
        path = pathlib.Path(path)
    if path.exists():
        shutil.move(path, path.parent / f"{path.name}.old")
