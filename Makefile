SOURCES := $(wildcard src/*.c)
VENV = .testvenv
MAKE_VENV = python3 -m venv $(VENV)
PYTHON = $(VENV)/bin/python3

all: clean generate test

generate:
	./scripts/generate_monster_ids.py
	./scripts/generate_baked_data.py
	./scripts/generate_associations_config.py

test:
	rm -f testmain
	gcc -o testmain -DENCOUNTERS_MAIN -DBAKED_MONSTER_DATA -g $(SOURCES)
	./testmain | ./scripts/id_to_name.py

clean:
	./scripts/clean.py
	rm -f testmain
	rm -rf $(VENV)

pytest: clean
	$(MAKE_VENV);
	$(PYTHON) -m pip install .
	$(PYTHON) scripts/test_python_module.py

