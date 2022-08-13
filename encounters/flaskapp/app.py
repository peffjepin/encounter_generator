#!/usr/bin/env python3

import flask
import encounters

from . import util

encounters_blueprint = flask.Blueprint(
    "encounter_generator",
    __name__,
    template_folder="templates",
    static_folder="static",
)


def validate_encounter_arguments():
    validator = util.Validator()

    @validator.argument("player_levels", required=True)
    def validate_levels(levels_arg):
        level_strings = levels_arg.split(",")
        try:
            levels = list(map(int, level_strings))
        except Exception:
            raise AssertionError(
                "`player_levels` should be valued 1-20 and separated by commas"
            )
        else:
            assert all(
                0 < lvl <= 20 for lvl in levels
            ), "`player_levels` out of range"

        return levels

    @validator.argument("difficulty", required=True)
    def validate_difficulty(diff_arg):
        assert (
            diff_arg in util.DIFFICULTY_MAP
        ), f"`difficulty` should be one of: {tuple(util.DIFFICULTY_MAP.keys())}"
        return util.DIFFICULTY_MAP.get(diff_arg)

    @validator.argument("seed_monsters")
    def validate_seed_monsters(arg):
        try:
            monster_names = list(arg.split(","))
        except Exception:
            raise AssertionError(
                "`seed_monsters` should be ',' separated monster names"
            )
        else:
            ids = []
            for name in monster_names:
                monster = encounters.monsters_by_name.get(name, None)
                assert (
                    monster is not None
                ), f"seed monster name {name!r} is not recognized"
                ids.append(monster.id)
            return ids

    return validator


@encounters_blueprint.route("/api/encounter", methods=("GET",))
def generate_encounter_api():
    validator = validate_encounter_arguments()

    if validator.errors:
        return flask.jsonify({"error": validator.error_messages})

    # encounter generation can back itself into a corner and fail
    # with the way it's currently configured. In most cases retrying
    # should be sufficient.
    for _ in range(10):
        try:
            monsters = sorted(
                encounters.generate(**validator.args),
                key=lambda m: -float(eval(m.cr.split()[0])),
            )
            break
        except encounters.EncounterError:
            pass
    else:
        return flask.jsonift({"error": "whoops, encounter generation failed"})

    table_entries = [
        {
            "name": m.name,
            "description": f"{m.size} {m.type}, {m.alignment}",
            "challenge": m.cr,
        }
        for m in monsters
    ]
    return flask.jsonify(table_entries)


@encounters_blueprint.route("/", methods=("GET",))
def mainpage():
    return flask.render_template(
        "index.html", monsters_json_object=util.monster_html_cards
    )
