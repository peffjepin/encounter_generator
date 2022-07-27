import dataclasses


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
