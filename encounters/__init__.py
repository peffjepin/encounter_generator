import pathlib
import _encounters

datadir = pathlib.Path(__file__).parent / "data"
config_filepath = str((datadir / "associations.conf").absolute())
_encounters.init(config_filepath)

from .api import *
