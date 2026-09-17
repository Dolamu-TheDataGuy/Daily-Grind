def invert_roster(roster: dict[str, list[str]]) -> dict[str, list[str]]:
    players_to_teams = {}
    for team_name in roster:
        for player_name in roster[team_name]:
            if player_name not in players_to_teams:
                players_to_teams[player_name] = []
            players_to_teams[player_name].append(team_name)
    return players_to_teams


def best_total(score_groups: list[list[int]]) -> None | int:
    if len(score_groups) == 0:
        return None

    highest_total = None
    for group in score_groups:
        total = 0
        for score in group:
            total += score
        if highest_total is None or total > highest_total:
            highest_total = total
    return highest_total


def first_notes(notes_by_day: dict[str, list[str | None]]) -> dict[str, str | None]:
    result = {}
    for day in notes_by_day:
        first_note = None
        for note in notes_by_day[day]:
            if note is not None:
                first_note = note
                break
        result[day] = first_note
    return result
