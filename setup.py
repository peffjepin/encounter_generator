import contextlib
import pathlib
import setuptools
import shutil

root = pathlib.Path(__file__).parent
srcdir = root / "src"
sources = [
    str(p.relative_to(root)) for p in srcdir.iterdir() if p.name.endswith(".c")
]
sources.append("src/python/encounters_module.c")


@contextlib.contextmanager
def include_necessary_data_files_in_package():
    data_files_to_copy = ("associations.conf", "reprs")
    package_data_dir = root / "encounters/data"
    root_data_dir = root / "data"
    for filename in data_files_to_copy:
        shutil.copy(root_data_dir / filename, package_data_dir)
    yield
    for filename in data_files_to_copy:
        (package_data_dir / filename).unlink()


encounters_module = setuptools.Extension(
    "_encounters",
    sources=sources,
    define_macros=[("BAKED_MONSTER_DATA", None)],
)

if __name__ == "__main__":
    with include_necessary_data_files_in_package():
        setuptools.setup(
            ext_modules=[encounters_module],
        )
