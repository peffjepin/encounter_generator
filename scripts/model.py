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
    features: dict
    actions: dict
    legendary_actions: dict
    id: int
    associations: dict
    reactions: dict

    def _ability_score(self, index):
        value = self.ability_scores.split()[index]
        return f"{value} ({self.bonus_from_ability_score(value)})"

    @property
    def STR(self):
        return self._ability_score(0)

    @property
    def DEX(self):
        return self._ability_score(1)

    @property
    def CON(self):
        return self._ability_score(2)

    @property
    def INT(self):
        return self._ability_score(3)

    @property
    def WIS(self):
        return self._ability_score(4)

    @property
    def CHA(self):
        return self._ability_score(5)

    def bonus_from_ability_score(self, value):
        return (int(value) - 10) // 2
