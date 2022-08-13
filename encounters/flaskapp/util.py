import encounters
import flask


DIFFICULTY_MAP = {"easy": 0, "medium": 1, "hard": 2, "deadly": 3}


def labeled_p(label, content):
    return f"<p><strong><em>{label}: </em></strong>{content}</p>"


def render_html_display(monster):
    base = f"""
<h1>{monster.name}</h1>
<p>{monster.size} {monster.type}, {monster.alignment}</p>
<hr>
{labeled_p("Armor Class", monster.ac)}
{labeled_p("Hit Points", monster.hp)}
{labeled_p("Speed", monster.speed)}
<hr>
<p>STR: {monster.STR}, DEX: {monster.DEX}, CON: {monster.CON}, INT: {monster.INT}, WIS: {monster.WIS}, CHA: {monster.CHA}</p>
<hr>
""".lstrip()
    remainder = []

    if monster.saving_throws:
        remainder.append(labeled_p("Saving Throws", monster.saving_throws))
    if monster.senses:
        remainder.append(labeled_p("Senses", monster.senses))
    if monster.skills:
        remainder.append(labeled_p("Skills", monster.skills))
    if monster.condition_immunities:
        remainder.append(
            labeled_p("Condition Immunities", monster.condition_immunities)
        )
    if monster.damage_resistances:
        remainder.append(
            labeled_p("Damage Resistances", monster.damage_resistances)
        )
    if monster.damage_immunities:
        remainder.append(
            labeled_p("Damage Immunities", monster.damage_immunities)
        )
    if monster.damage_vulnerabilities:
        remainder.append(
            labeled_p("Damage Vulnerabilities", monster.damage_vulnerabilities)
        )
    if monster.languages:
        remainder.append(labeled_p("Languages", monster.languages))
    if monster.cr:
        remainder.append(labeled_p("Challenge", monster.cr))
    remainder.append("<hr>")
    for name, desc in monster.features.items():
        remainder.append(labeled_p(name, desc))
    if monster.actions:
        remainder.append("<h3>Actions</h3>")
        remainder.append("<hr>")
        for name, desc in monster.actions.items():
            remainder.append(labeled_p(name, desc))
    if monster.reactions:
        remainder.append("<h3>Reactions</h3>")
        remainder.append("<hr>")
        for name, desc in monster.reactions.items():
            remainder.append(labeled_p(name, desc))
    if monster.legendary_actions:
        remainder.append("<h3>Legendary Actions</h3>")
        remainder.append("<hr>")
        remainder.append(
            f"<p>{monster.legendary_actions.get('preamble', '')}</p>"
        )
        for name, desc in monster.legendary_actions.items():
            remainder.append(labeled_p(name, desc))

    return base + "\n".join(remainder)


monster_html_cards = {
    m.name: render_html_display(m) for m in encounters.monsters
}


class Validator:
    def __init__(self):
        self.url_arguments = flask.request.args
        self.errors = []
        self.args = {}

    def argument(self, argument_name, required=False):
        def wrapper(func):
            try:
                arg = self.url_arguments.get(argument_name, None)
                if arg is None:
                    if required:
                        self.errors.append(AssertionError(
                            f"`{argument_name}` param is required"))
                    return
                value = func(arg)
                if value is not None:
                    self.args[argument_name] = value
            except AssertionError as exc:
                self.errors.append(exc)

        return wrapper

    @property
    def error_messages(self):
        return tuple(map(str, self.errors))
