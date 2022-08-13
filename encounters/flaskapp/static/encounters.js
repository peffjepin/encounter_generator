const MONSTER_ENTRY_LOCKED = 'monster_entry_locked';
const MONSTER_ENTRY_UNLOCKED = 'monster_entry';
const MONSTER_JSON_DUMP = JSON.parse(document.querySelector("#monsters_json_dump").innerHTML);

const unlocked_icon = document.querySelector("#unlocked_lock");
const locked_icon = document.querySelector("#locked_lock");
const level_inputs = document.querySelector("#character_level_inputs");
const table_body = document.querySelector('#monster_table_body');
const monster_entry_template = document.querySelector('#monster_entry_template');
const monster_card_modal_content = document.querySelector("#monster_card_content");
const monster_card_modal = document.querySelector("#monster_card_modal");


function encounter_fetch_uri(difficulty, locked_in_monster_names) {
    const levels =
        Array.from(level_inputs.children).map(div => div.children[1].innerHTML).join(',');

    var uri = window.location + 'api/encounter?player_levels=' + levels + '&difficulty=' + difficulty;
    if (locked_in_monster_names.length > 0)
        uri = uri + '&seed_monsters=' + locked_in_monster_names.join(',');
    return encodeURI(uri);
}


async function generate_encounter(difficulty) {
    // get a list of monsters that have been locked
    var locked_in = Array.from(table_body.children)
        .filter(elem => elem.className == MONSTER_ENTRY_LOCKED)
        .map(elem => elem.querySelector('.monster_name').innerHTML)

    // clear monsters list ui
    while (table_body.firstChild) table_body.removeChild(table_body.lastChild);

    // attempt to fetch new data
    fetch(encounter_fetch_uri(difficulty, locked_in)).then(async response => {
        // display error message if we didn't get back json
        const json = await response.json();
        if ("error" in json) {
            alert(await json.error);
            return;
        }
        // update ui with new data if request appropriately returns json
        for (let i = 0; i < json.length; i++) {
            const monster_table_entry = json[i];
            const element = monster_entry_template.cloneNode(true);
            const icon_div = element.querySelector(".monster_lock_icon");
            icon_div.innerHTML = "";

            // make sure locked in choices remain locked
            const locked_in_index = locked_in.indexOf(monster_table_entry.name);
            if (locked_in_index > -1) {
                locked_in.splice(locked_in_index, 1);
                element.className = MONSTER_ENTRY_LOCKED;
                icon_div.appendChild(locked_icon.cloneNode(true));
            }
            else {
                element.className = MONSTER_ENTRY_UNLOCKED;
                icon_div.appendChild(unlocked_icon.cloneNode(true));
            }

            // init table row values
            element.querySelector('.monster_name').innerHTML = monster_table_entry.name;
            element.querySelector('.monster_description').innerHTML = monster_table_entry.description;
            element.querySelector('.monster_challenge').innerHTML = monster_table_entry.challenge;
            table_body.appendChild(element);
        }
    });
}


function add_character() {
    const new_input = level_inputs.firstElementChild.cloneNode(true);
    level_inputs.appendChild(new_input);
}


function remove_character() {
    if (level_inputs.children.length > 1) {
        level_inputs.removeChild(level_inputs.lastChild);
    }
}


function change_character_level(element, change) {
    const value_span = element.parentElement.children[1]
    var value = parseInt(value_span.innerHTML) + change;
    value = Math.max(Math.min(10, value), 1);
    value_span.innerHTML = value;
}


function toggle_monster_lock(element) {
    const table_row = element.parentElement.parentElement.parentElement;
    const lock_icon_div = table_row.querySelector(".monster_lock_icon");
    lock_icon_div.removeChild(lock_icon_div.firstElementChild);

    if (table_row.className == MONSTER_ENTRY_LOCKED) {
        table_row.className = MONSTER_ENTRY_UNLOCKED;
        lock_icon_div.appendChild(unlocked_icon.cloneNode(false));
    }
    else {
        table_row.className = MONSTER_ENTRY_LOCKED;
        lock_icon_div.appendChild(locked_icon.cloneNode(false));
    }
}


function show_monster_card(button) {
    monster_card_modal_content.innerHTML = MONSTER_JSON_DUMP[button.innerHTML];
    monster_card_modal.style.display = "block";
}


function close_monster_card() {
    monster_card_modal.style.display = "none";
}

window.onclick = function(event) {
    if (event.target == monster_card_modal) {
        close_monster_card();
    }
}
