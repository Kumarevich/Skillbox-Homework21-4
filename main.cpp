#include <iostream>
#include <time.h>
#include <fstream>

struct position {
    int x = 0;
    int y = 0;
};

struct character {
    std::string name;
    int life;
    int armor;
    int damage;
    position pos;
};

void init_enemies(character (&enemies)[5], char (&field)[40][40]) {
    srand(time(NULL));
    for (int i = 0; i < 5; ++i) {
        enemies[i].name = "Enemy#";
        enemies[i].name += (i + 1);
        enemies[i].life = rand() % 101 + 50;
        enemies[i].armor = rand() % 51;
        enemies[i].damage = rand() % 16 + 15;
        enemies[i].pos.x = rand() % 40;
        enemies[i].pos.y = rand() % 40;
        while (field[enemies[i].pos.x][enemies[i].pos.y] != '.') {
            enemies[i].pos.x = rand() % 40;
            enemies[i].pos.y = rand() % 40;
        }
        field[enemies[i].pos.x][enemies[i].pos.y] = 'E';
    }
}

void create_player(character& player) {
    std::cout << "Input your character parameters.\n";
    std::cout << "Name: ";
    std::cin >> player.name;
    std::cout << "Life: ";
    std::cin >> player.life;
    std::cout << "Armor: ";
    std::cin >> player.armor;
    std::cout << "Damage: ";
    std::cin >> player.damage;
    std::cout << "Position (x, y): ";
    std::cin >> player.pos.x >> player.pos.y;
}

void init_space(char (&field)[40][40]) {
    for (int i = 0; i < 40; ++i) {
        for (int j = 0; j < 40; ++j) {
            field[i][j] = '.';
        }
    }
}

void battle(character& attacker, character& defender, char (&field)[40][40]) {
    int attack = attacker.damage;
    if (defender.armor >= attack) {
        defender.armor -= attack;
    } else {
        attack -= defender.armor;
        defender.armor = 0;
        if (defender.life <= attack) {
            defender.life = 0;
            field[defender.pos.x][defender.pos.y] = '.';
        } else {
            defender.life -= attack;
        }
    }
}

void init_game (character& player,character (&enemies)[5], char (&field)[40][40]) {
    init_space(field);
    create_player(player);
    field[player.pos.x][player.pos.y] = 'P';
    init_enemies(enemies, field);
}

void save_game (character& player, character (&enemies)[5]) {
    std::ofstream game("..\\game.bin", std::ofstream::trunc);
    game << player.name << std::endl;
    game << player.life << std::endl;
    game << player.armor << std::endl;
    game << player.damage << std::endl;
    game << player.pos.x << std::endl;
    game << player.pos.y << std::endl;
    for (int i = 0; i < 5; ++i) {
        game << enemies[i].name << std::endl;
        game << enemies[i].life << std::endl;
        game << enemies[i].armor << std::endl;
        game << enemies[i].damage << std::endl;
        game << enemies[i].pos.x << std::endl;
        game << enemies[i].pos.y << std::endl;
    }
    game.close();
}

void load_game (character& player, character (&enemies)[5], char (&field)[40][40]) {
    std::ifstream game("..\\game.bin");
    if (!game.is_open() || game.peek() == EOF) return init_game(player, enemies, field);
    game >> player.name;
    game >> player.life;
    game >> player.armor;
    game >> player.damage;
    game >> player.pos.x;
    game >> player.pos.y;
    for (int i = 0; i < 5; ++i) {
        game >> enemies[i].name;
        game >> enemies[i].life;
        game >> enemies[i].armor;
        game >> enemies[i].damage;
        game >> enemies[i].pos.x;
        game >> enemies[i].pos.y;
    }
    game.close();
    init_space(field);
    field[player.pos.x][player.pos.y] = 'P';
    for (int i = 0; i < 5; ++i) {
        if (enemies[i].life > 0) field[enemies[i].pos.x][enemies[i].pos.y] = 'E';
    }
}

void move_character (character& character, char (&field)[40][40], char type, int x, int y) {
    field[x][y] = type;
    field[character.pos.x][character.pos.y] = '.';
    character.pos.x = x;
    character.pos.y = y;
}

void player_move(character& player, character (&enemies)[5], char (&field)[40][40]) {
    int x = player.pos.x;
    int y = player.pos.y;
    bool fault = false;
    std::string move;
    std::cout << "Your move direction(LEFT, RIGHT, UP, DOWN, SAVE, LOAD): ";
    std::cin >> move;
    if (move == "LEFT") x -= 1;
    else if (move == "RIGHT") x += 1;
    else if (move == "UP") y -= 1;
    else if (move == "DOWN") y += 1;
    else if (move == "SAVE" || move == "LOAD") {
        (move == "SAVE") ? save_game(player, enemies) : load_game(player, enemies, field);
        return player_move(player, enemies, field);
    }
    else fault = true;
    if (x < 0 || y < 0) {
        fault = true;
        x = player.pos.x;
        y = player.pos.y;
    }
    while (fault) {
        fault = false;
        std::cout << "Something goes wrong. Try again (LEFT, RIGHT, UP, DOWN, SAVE, LOAD): ";
        std::cin >> move;
        if (move == "LEFT") x -= 1;
        else if (move == "RIGHT") x += 1;
        else if (move == "UP") y -= 1;
        else if (move == "DOWN") y += 1;
        else if (move == "SAVE" || move == "LOAD") {
            (move == "SAVE") ? save_game(player, enemies) : load_game(player, enemies, field);
            return player_move(player, enemies, field);
        }
        else fault = true;
        if (x < 0 || y < 0) {
            fault = true;
            x = player.pos.x;
            y = player.pos.y;
        }
    }
    if (field[x][y] == 'E') {
        int enemy = 0;
        while (!(x == enemies[enemy].pos.x && y == enemies[enemy].pos.y)) {
            ++enemy;
        }
        battle(player, enemies[enemy], field);
    }
    if (field[x][y] == '.') move_character(player, field, 'P', x, y);
}

void enemies_moves(character (&enemies)[5], character& player, char (&field)[40][40]) {
    for (int i = 0; i < 5; ++i) {
        if (enemies[i].life > 0) {
            int x = enemies[i].pos.x;
            int y = enemies[i].pos.y;
            //move = 0 - left; 1 - right; 2 - up; 3 - down
            int move = rand() % 4;
            if (move == 0) x -= 1;
            else if (move == 1) x += 1;
            else if (move == 2) y += 1;
            else if (move == 3) y -= 1;

            if (x >= 0 && x < 40 && y >= 0 && y < 40) {
                if (field[x][y] == 'P') battle(enemies[i], player, field);
                if (field[x][y] == '.') move_character(enemies[i], field, 'E', x, y);
            }
        }
    }
}

void game_round (character& player,character (&enemies)[5], char (&field)[40][40]) {
    for (int i = 0; i < 40; ++i) {
        for (int j = 0; j < 40; ++j) {
            std::cout << field[j][i] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    player_move(player, enemies, field);
    enemies_moves(enemies, player, field);
}

bool enemies_life(character (&enemies)[5]) {
    if (enemies[0].life == 0 && enemies[1].life == 0 && enemies[2].life == 0
        && enemies[3].life == 0 && enemies[4].life == 0) return true;
    return false;
}

int main() {
    character player;
    character enemies[5];
    char field[40][40];
    std::cout << "Would you like to LOAD game? YES/NO: ";
    std::string answer;
    std::cin >> answer;
    if (answer == "YES") load_game(player, enemies, field);
    else {
        init_game(player, enemies, field);
    }
    while (player.life > 0 && !enemies_life(enemies)) {
        game_round(player, enemies, field);
    }
    if (player.life == 0) std::cout << "You lost!";
    else std::cout << "You Win!";
    return 0;
}
