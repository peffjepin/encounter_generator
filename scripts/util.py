import pathlib
import shutil

from model import Monster

root = pathlib.Path(__file__).parent.parent
srcdir = root / "src"
datadir = root / "data"
reprs = datadir / "reprs"


def load(filename=None):
    if filename is None:
        src = reprs.read_text()
    else:
        src = pathlib.Path(filename).read_text()
    return [eval(ln) for ln in src.splitlines()]


def dump(monsters, filename=None):
    if filename is None:
        path = reprs
    else:
        path = pathlib.Path(filename)

    backup(path)
    with open(path, "w") as f:
        for monster in monsters:
            f.write(repr(monster) + "\n")


def backup(path):
    if not isinstance(path, pathlib.Path):
        path = pathlib.Path(path)
    if path.exists():
        shutil.move(path, path.parent / f"{path.name}.old")
