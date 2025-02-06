
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <menu.h>
#include <unistd.h>


typedef struct Position{
int x;
int y;
} Position;

typedef struct Spell {
    Position position;
    bool taken;  
} Spell;

typedef struct Trap {
    Position position;
    bool triggered; 
} Trap;

typedef struct{
    char username[60];
    char password[60];
    char email[50];
} Menu;



typedef struct Weapon {
    char symbol;     
    Position position; 
    bool taken;       
} Weapon;

typedef struct {
    int mace;     
    int dagger;    
    int wand;       
    int arrow;     
    int sword;      
} WeaponInventory;


typedef struct Player {
    Position *position;
    int health;
    int gold;
    int maxhealth;
    int exp;
    int attack;
    int food;

    // Weapon inventory
    int mace_count;
    int dagger_count;
    int wand_count;
    int arrow_count;
    int sword_count;
} Player;

typedef struct Level{
char ** tiles;
int level;
int numberofrooms;
struct Room **rooms;
struct Monster ** monsters;
int numberOfMonsters;
Player * user;
Trap traps[10];
Spell speed;
} Level;

typedef struct Stair {
    Position position;
} Stair;

typedef struct Food {
    Position position;
    bool eaten;
} Food;

typedef struct Door
{
  Position position;
  int connected;


} Door;

typedef struct Room {
Position position;
int height;
int width;
Door **doors;

int numberofdoors;

} Room; 

 
typedef struct Monster {
    char symbol;
    Position *position;
    int health;
    int attack;
    int move_type; // 0 for random, 1 for seeking player
    bool alive;
} Monster;


//gameloops
int gameLoop1(Player * player);
int gameLoop2(Player * player);
int gameLoop3(Player * player);
int gameLoop4(Player * player);

//weapons
void spawn_weapons(Level *level);
void check_for_weapons(Level *level);
void displayInventory(Player *player);
void spawn_speed_spell(Level *level);

//menu
int ScreenSetUp();


int mainMenu (int numberitems, char *choices[]);
void get_player_info(Menu *u, Menu *p, Menu *e);
bool is_valid_email(const char *email);
bool password_validation(char * password);

//level & map functions 
Room ** roomssetup();
Level * createlevel(int level, Player * existingPlayer);
char ** savelevelpositions();
int printgamehub();
int placeplayer(Room ** rooms, Player * user);
void spawn_traps(Level *level);
void place_stair(Level *level);

//player functions
Player *playersetup();
Position * handleinput(int input, Player *user);
int playermove(Position * newPosition, Player *user, char ** level);
int checkposition1(Position * newPosition, Level * level);
int checkposition2(Position *newPosition, Level *level);
int checkposition3(Position *newPosition, Level *level);
int checkposition4(Position *newPosition, Level *level);



//room functions
Room* createroom(int grid, int numberofdoors);
int drawroom(Room *room);
void connectdoors(Level * level);
void pathfind(Position* start, Position * end);
void spawn_gold(Level *level, int gold_count);
void spawn_health_spell(Level *level);
void activate_health_spell(Player * user);
void check_for_health_spell(Level *level);

void spawn_food(Level *level, int food_count);





//globals
int speedTimer = 0;  // Global variable for speed boost timer
int speedDuration = 10;  // Duration of speed boost in seconds
bool speedBoostActive = false;

time_t start_time; 


int get_elapsed_time() {
    return (int)(time(NULL) - start_time);
}



int gameLoop1(Player * player) {
    int ch;
    Position *newPosition;
    Level *level;
    bool inInventory = false; // Tracks whether we're in the inventory menu

    level = createlevel(1, player);

   
    spawn_food(level, 2);
    spawn_gold(level, 2);
    spawn_weapons(level);
    spawn_speed_spell(level);
    spawn_health_spell(level);
    start_time = time(NULL); 
    

    printgamehub(level);

    int health_timer = 0;
    const int health_decrease_interval = 50;

    while ((ch = getch()) != 'q') {
        if (inInventory) { 
        // If in the inventory menu
            if (ch == 'i') {
                inInventory = false; // Return to game
                clear();
                printgamehub(level);
                gameLoop1(player);
                refresh();
            }
            continue;
        }

        if (ch == 'i') { // Open inventory menu
            inInventory = true;
            displayInventory(level->user);
            continue;
        }

        printgamehub(level);
        mvprintw(level->user->position->y, level->user->position->x, "@");
        refresh();


        newPosition = handleinput(ch, level->user);
        checkposition1(newPosition, level);
    

        if (speedBoostActive && time(NULL) - speedTimer >= speedDuration) {
            speedBoostActive = false;
            mvprintw(28, 0, "Speed boost has expired!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                             ");
            refresh();
        }
        move(level->user->position->y, level->user->position->x);
        

        // Decrease health periodically
        health_timer++;
        if (health_timer >= health_decrease_interval) {
            level->user->health--;
            health_timer = 0;

            if (level->user->health <= 0) {
                clear();
                mvprintw(14, 40, "You died! Game over.Your Exp: %d, your Gold:%d",level->user->exp, level->user->gold);
                napms(10000);
                refresh();
                return -1;
            }
        }
        refresh();
    }
    return 0;
}

int gameLoop2(Player * player) {
    int ch;
    Position *newPosition;
    Level *level;
    bool inInventory = false; 

    level = createlevel(2, player);


    spawn_food(level, 2);
    spawn_gold(level, 2);
    spawn_weapons(level);
    spawn_health_spell(level);
    

    printgamehub(level);

    int health_timer = 0;
    const int health_decrease_interval = 50;

    while ((ch = getch()) != 'q') {
        if (inInventory) { 
        // If in the inventory menu
            if (ch == 'i') {
                inInventory = false; // Return to game
                clear();
                gameLoop2(player);
                printgamehub(level);
                refresh();
            }
            continue;
        }

        if (ch == 'i') { // Open inventory menu
            inInventory = true;
            displayInventory(level->user);
            continue;
        }

        printgamehub(level);

        newPosition = handleinput(ch, level->user);
        checkposition2(newPosition, level);
       

        move(level->user->position->y, level->user->position->x);

     

        // Decrease health periodically
        health_timer++;
        if (health_timer >= health_decrease_interval) {
            level->user->health--;
            health_timer = 0;

            if (level->user->health <= 0) {
                mvprintw(10, 10, "You died! Game over.");
                clear();
               mvprintw(14, 40, "You died! Game over.Your Exp: %d, your Gold:%d",level->user->exp, level->user->gold);
                napms(10000);
                refresh();
                return -1;
            }
        }
        refresh();
    }
    return 0;
}

int gameLoop3(Player * player) {
    int ch;
    Position *newPosition;
    Level *level;
    bool inInventory = false; 

    level = createlevel(3, player);


    spawn_food(level, 2);
    spawn_gold(level, 2);
    spawn_weapons(level);
    spawn_health_spell(level);

    printgamehub(level);

  
    int health_timer = 0;
    const int health_decrease_interval = 50;

    while ((ch = getch()) != 'q') {
        if (inInventory) { 
        // If in the inventory menu
            if (ch == 'i') {
                inInventory = false; // Return to game
                clear();
                printgamehub(level);
                gameLoop3(player);
                refresh();
            }
            continue;
        }

        if (ch == 'i') { // Open inventory menu
            inInventory = true;
            displayInventory(level->user);
            continue;
        }

        printgamehub(level);

        newPosition = handleinput(ch, level->user);
        checkposition3(newPosition, level);
       

        move(level->user->position->y, level->user->position->x);
   

        // Decrease health periodically
        health_timer++;
        if (health_timer >= health_decrease_interval) {
            level->user->health--;
            health_timer = 0;

            if (level->user->health <= 0) {
                mvprintw(10, 10, "You died! Game over.");
                clear();
            mvprintw(14, 40, "You died! Game over.Your Exp: %d, your Gold:%d",level->user->exp, level->user->gold);
                napms(10000);
                refresh();
                return -1;
            }
        }
        refresh();
    }
    return 0;
}

int gameLoop4(Player * player) {
    int ch;
    Position *newPosition;
    Level *level;
    bool inInventory = false; 

    level = createlevel(4, player);

    spawn_food(level, 2);
    spawn_gold(level, 2);
    spawn_weapons(level);
    spawn_health_spell(level);

    printgamehub(level);

    
    int health_timer = 0;
    const int health_decrease_interval = 50;

    while ((ch = getch()) != 'q') {
        if (inInventory) { 
       
            if (ch == 'i') {
                inInventory = false; 
                clear();
                printgamehub(level);
                gameLoop4(player);
                refresh();
            }
            continue;
        }

        if (ch == 'i') { 
            inInventory = true;
            displayInventory(level->user);
            continue;
        }

        printgamehub(level);

        newPosition = handleinput(ch, level->user);
        checkposition4(newPosition, level);
       
        move(level->user->position->y, level->user->position->x);

      

      
        health_timer++;
        if (health_timer >= health_decrease_interval) {
            level->user->health--;
            health_timer = 0;

            if (level->user->health <= 0) {
                mvprintw(10, 10, "You died! Game over.");
                clear();
            mvprintw(14, 40, "You died! Game over.Your Exp: %d, your Gold:%d",level->user->exp, level->user->gold);
                napms(10000);
                refresh();
                return -1;
            }
        }
        refresh();
    }
    return 0;
}



void menuLoop()
{
    Menu u;
    Menu p;
    Menu e;
    int choice;
    char * choices[] = {"New Game", "Resume Game", "Setting", "ScoreBoard", "End Game"};
      Player *player = playersetup();
    mainMenu(5, choices);

    while (true)
    {
        choice = mainMenu(5, choices);

        switch (choice)
        {
            case 0:
            {
             
                
                 get_player_info(&u, &p, &e);
                 clear();
               
            
                gameLoop1(player);
                clear();
            }
            break;

            case 1:
                return;
                break;
            
            case 2:
               int difficulty;
                char * difficulty_choices[] = {"Easy", "Medium", "Hard"};
                difficulty = mainMenu(3, difficulty_choices);
                
                int initial_health;
                switch (difficulty)
                {
                    case 0:
                        initial_health = 50; // Easy 
                        break;
                    case 1:
                        initial_health = 40; // Medium 
                        break;
                    case 2:
                        initial_health = 30; // Hard
                        break;
                    default:
                        initial_health = 50; // Default to Easy
                        break;
                }
                 player->health = initial_health;
                player->maxhealth = initial_health;
                break;

            case 3:
                break;
            case 4:
                break;
        }
    }
}



int main() {
    

    ScreenSetUp();
    menuLoop();
    
    refresh();
    endwin();
    return 0;
}


int ScreenSetUp() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);


    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK); 
    init_pair(2, COLOR_BLUE, COLOR_BLACK);   
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    refresh();
    srand(time(NULL));
    return 1;
}


int printgamehub(Level * level){
level->user->exp = get_elapsed_time();
start_color();
attron(COLOR_PAIR(3));
mvprintw(26, 4,"Level:%d", level->level);
mvprintw(26,14 ,"Gold:%d", level->user->gold);
mvprintw(26, 24,"Hp:%d", level->user->health);
mvprintw(26, 34,"Exp:%d", level->user->exp);
mvprintw(26, 44, "food:%d",level->user->food);
mvprintw(26, 54,"    ");
attroff(COLOR_PAIR(3));

return 1;
}

Level * createlevel(int level, Player * existingPlayer){
    Level *newlevel;
    newlevel = malloc(sizeof(Level));

    newlevel->level = level; 
    newlevel->numberofrooms = 6;
    newlevel->rooms = roomssetup();

    connectdoors(newlevel);
    newlevel->tiles = savelevelpositions();


    newlevel->user = existingPlayer; 
  
    placeplayer(newlevel->rooms, newlevel->user);

  
    
     spawn_traps(newlevel);

     place_stair(newlevel);
   

    return newlevel;
}

Room ** roomssetup() {

    int x;
  Room **rooms = (Room **)malloc(sizeof(Room *) * (7));

    
    for(x=0; x<6; x++){
    rooms[x]= createroom(x, 4);
        drawroom(rooms[x]);
    }
    
    return rooms;
}


void connectdoors(Level * level)
 {
  int i, j;
  int randomroom, randomdoor;
  int count;

  for( i=0; i < level->numberofrooms; i++)
  {
    for( j=0; j< level->rooms[i]->numberofdoors; j++)
    {
        if(level->rooms[i]->doors[j]->connected == 1)
        {
        continue;
        }

        count = 0;

        while(count < 10)
        {
        randomroom = rand()  %  level->numberofrooms;
        randomdoor = rand()  %  level->rooms[randomroom]->numberofdoors;

        if(level->rooms[randomroom]->doors[randomdoor]->connected == 1 || randomroom == i) 
        {
            count++;
            continue;
        }

        pathfind(&level->rooms[randomroom]->doors[randomdoor]->position, &level->rooms[i]->doors[j]->position);
        
        level->rooms[randomroom]->doors[randomdoor]->connected = 1;
        level->rooms[i]->doors[j]->connected = 1;
        break;
        }
    }
    
  }
 }
char ** savelevelpositions(){
    int x, y;
    char ** positions;

positions = malloc(sizeof(char*)*35);
    for(y=0; y<35; y++){

        positions[y]= malloc(sizeof(char)*120);
        for(x=0; x<120; x++){

            positions[y][x]=mvinch(y, x);
        }
    }
    refresh();
    return positions;
}
Room* createroom(int grid, int numberofdoors){

    Room *newroom;
    newroom= malloc(sizeof(Room));
    newroom->numberofdoors = numberofdoors;
    
    switch(grid){

        case 0:
        newroom->position.x=0;
        newroom->position.y=0;
        break;
        case 1:
        newroom->position.x=33;
        newroom->position.y=0;
        break;
        case 2:
        newroom->position.x=66;
        newroom->position.y=0;
        break;
        case 3:
        newroom->position.x=0;
        newroom->position.y=14;
        break;
        case 4:
        newroom->position.x=33;
        newroom->position.y=14;
        break;
        case 5:
        newroom->position.x=66;
        newroom->position.y=14;
    
        break;
    }

    newroom->height= rand() % 6 + 4;
    newroom->width= rand() % 14 + 4;

    newroom->position.x += rand() % (30- newroom->width) +1;
    newroom->position.y += rand() % (10- newroom->height) +1;

    newroom->doors= malloc(sizeof(Door *)*numberofdoors);

    for(int i=0; i<numberofdoors; i++)
    {
        newroom->doors[i] = malloc(sizeof(Door));
        newroom->doors[i]->connected = 0;
    }

    //top door
    newroom->doors[0]->position.x = rand() % (newroom->width-2) + newroom->position.x+1;
    newroom->doors[0]->position.y = newroom->position.y;

    //left door
    newroom->doors[1]->position.y = rand() % (newroom->height-2) + newroom->position.y+1;
    newroom->doors[1]->position.x = newroom->position.x;

    //bottom door
 
    newroom->doors[2]->position.x = rand() % (newroom->width -2)+ newroom->position.x+1;
    newroom->doors[2]->position.y = newroom->position.y + newroom->height-1;

    //right door
   
    newroom->doors[3]->position.y = rand() % (newroom->height-2) + newroom->position.y+1;
    newroom->doors[3]->position.x = newroom->position.x + newroom->width-1;
    return newroom;
}
int drawroom(Room * room){
int x;
int y;

for(x=room->position.x; x < room->position.x + room->width; x++){
    attron(COLOR_PAIR(1));
    mvprintw(room->position.y, x, "-");
    mvprintw(room->position.y+ room->height-1, x, "-");
    attroff(COLOR_PAIR(1));

}

for(y=room->position.y+1; y< room->position.y+room->height-1; y++){
attron(COLOR_PAIR(1));
mvprintw(y, room->position.x, "|");
mvprintw(y, room->position.x+room->width-1, "|");
attroff(COLOR_PAIR(1));


for(x=room->position.x+1; x< room->position.x+ room->width -1 ; x++){

    mvprintw(y,x, ".");
}
}
mvprintw(room->doors[0]->position.y, room->doors[0]->position.x, "+");
mvprintw(room->doors[1]->position.y, room->doors[1]->position.x, "+");
mvprintw(room->doors[2]->position.y, room->doors[2]->position.x, "+");
mvprintw(room->doors[3]->position.y, room->doors[3]->position.x, "+");

refresh();
return 1;
}


Player *playersetup() {
    Player *newPlayer;
    Position *startPosition;

   
    newPlayer = malloc(sizeof(Player));
    startPosition = malloc(sizeof(Position));

   
    startPosition->x = 0;
    startPosition->y = 0;

   
    newPlayer->position = startPosition;
    newPlayer->health = 50;
    newPlayer->maxhealth = 50;
    newPlayer->gold = 0;
    newPlayer->exp = 0;
    newPlayer->attack = 1;
    newPlayer->food = 0;

    
    newPlayer->mace_count = 0;
    newPlayer->dagger_count = 0;
    newPlayer->wand_count = 0;
    newPlayer->arrow_count = 0;
    newPlayer->sword_count = 0;

    return newPlayer;
}


int placeplayer(Room ** rooms, Player * user)
{
user->position->x = rooms[0]->position.x + 1;
user->position->y = rooms[0]->position.y + 1;

    mvprintw(user->position->y, user->position->x, "@");
    move(user->position->y, user->position->x);
    refresh();

}

Position * handleinput(int input, Player *user) {
    Position * newPosition;
    newPosition = malloc(sizeof(Position));


    // Check if speed boost is active and adjust movement accordingly
    int speedMultiplier = (speedBoostActive) ? 2 : 1;  // Double speed if boost is active

   switch (input) {
        case KEY_UP:
            newPosition->y = user->position->y - speedMultiplier;
            newPosition->x = user->position->x;
            break;
        case KEY_DOWN:
            newPosition->y = user->position->y + speedMultiplier;
            newPosition->x = user->position->x;
            break;
        case KEY_LEFT:
            newPosition->y = user->position->y;
            newPosition->x = user->position->x - speedMultiplier;
            break;
        case KEY_RIGHT:
            newPosition->y = user->position->y;
            newPosition->x = user->position->x + speedMultiplier;
            break;
        case KEY_END:
            newPosition->y = user->position->y + 1;
            newPosition->x = user->position->x - 1;
            break;
        case KEY_HOME:
            newPosition->y = user->position->y - 1;
            newPosition->x = user->position->x - 1;
            break;
        case KEY_PPAGE:
            newPosition->y = user->position->y - 1;
            newPosition->x = user->position->x + 1;
            break;
        case KEY_NPAGE:
            newPosition->y = user->position->y + 1;
            newPosition->x = user->position->x + 1;
            break;
        case 'b':
         if(user->health <=45){
        user->food--;
        user->health+= 5;}
        break;
        default:
            break;
    }


    return newPosition;

}

int checkposition1(Position *newPosition, Level *level) {
    Player *user = level->user;
    char tile = mvinch(newPosition->y, newPosition->x) & A_CHARTEXT;

     for (int i = 0; i < 10; i++) {
        Trap *trap = &level->traps[i];
        if (!trap->triggered && 
            trap->position.x == newPosition->x && 
            trap->position.y == newPosition->y) {
            // Trigger the trap
            trap->triggered = true;
             mvprintw(28, 0, "You stepped on a trap!                ");
            refresh();
            napms(1000);

            mvprintw(28, 0, "                                   ");
            refresh();
            level->user->health -= 5; 
        
            refresh();

        
        }
    }
    

    switch (tile) {
        case '.': 
        case '#': 
        case '+': 
            playermove(newPosition, user, level->tiles);
            break;
        case '<':
        mvprintw(28,0, "You found the stairs! Moving to the next level...");
        refresh();
        napms(1000);
        clear();
         gameLoop2(level->user);

        case 'O': 
            user->food += 1; 
            if(user->food>=10){
            user->food=2;
            user->health= 50;
            }
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh();
            playermove(newPosition, user, level->tiles);
            mvprintw(28, 0, "You ate food!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case '$': // Common gold
            user->gold += 1;
            level->tiles[newPosition->y][newPosition->x] = '.';
            mvprintw(newPosition->y, newPosition->x, ".");
            refresh();
            playermove(newPosition, user, level->tiles);
             mvprintw(28, 0, "You got a common gold!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 'B': // Black gold
            user->gold += 5;
            level->tiles[newPosition->y][newPosition->x] = '.';
            mvprintw(newPosition->y, newPosition->x, ".");
            refresh();
            playermove(newPosition, user, level->tiles);
            mvprintw(28, 0, "You you got a black gold!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 'm': // Mace
            user->mace_count++;
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You picked up a Mace!                ");
            refresh();
            napms(1000);

            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 'd': // Dagger
            user->dagger_count++;
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles);
            mvprintw(28, 0, "You picked up a Dagger!                ");
            refresh();
            napms(1000);

            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 'w': // Magic Wand
            user->wand_count++;
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You picked up a Magic Wand!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;
        case 'a': // Arrow
            user->arrow_count++;
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You picked up an Arrow!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 's': // Sword
            user->sword_count++;
            level->tiles[newPosition->y][newPosition->x] = '.';
            mvprintw(newPosition->y, newPosition->x, ".");
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You picked up a Sword!           ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

            case 'Q':  // Speed spell
            if (!level->speed.taken) {
                level->speed.taken = true;
                speedBoostActive = true;
                speedTimer = time(NULL);  // Set the current time as the start of the boost
                mvprintw(newPosition->y, newPosition->x, ".");
                refresh();
                mvprintw(28, 0, "You took the speed spell! Speed doubled for 10 seconds.");
                refresh();
                napms(1000);
                mvprintw(28, 0, "                                                               ");
                refresh();
            }
            playermove(newPosition, user, level->tiles);
            break;
            case 'H':
            activate_health_spell(user);
            level->tiles[newPosition->y][newPosition->x] = '.';
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You took the Health spell! Speed doubled for 10 seconds.");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                                          ");
            refresh();
            break;
               

        default: 
            move(user->position->y, user->position->x);
            break;
    }

    mvprintw(user->position->y, user->position->x, "@");
    refresh();
    return 1;
}

int checkposition2(Position *newPosition, Level *level) {
    Player *user = level->user;
    char tile = mvinch(newPosition->y, newPosition->x) & A_CHARTEXT;

     for (int i = 0; i < 10; i++) {
        Trap *trap = &level->traps[i];
        if (!trap->triggered && 
            trap->position.x == newPosition->x && 
            trap->position.y == newPosition->y) {
            // Trigger the trap
            trap->triggered = true;
             mvprintw(28, 0, "You stepped on a trap!                ");
            refresh();
            napms(1000);

            mvprintw(28, 0, "                                   ");
            refresh();
            level->user->health -= 5; 
            refresh();

        
        }
    }

    switch (tile) {
        case '.': 
        case '#': 
        case '+':
            playermove(newPosition, user, level->tiles);
            break;
        case '<':
        mvprintw(28, 0, "You found the stairs! Moving to the next level...");
        refresh();
        napms(1000);
        clear();
        gameLoop3(level->user);

        case 'O': 
            user->food += 1; 
            if(user->food>=2){
            user->food=2;
            user->health= 50;
            }
            level->tiles[newPosition->y][newPosition->x] = '.';
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You ate food!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case '$': // Common gold
            user->gold += 1;
            level->tiles[newPosition->y][newPosition->x] = '.';
            mvprintw(newPosition->y, newPosition->x, ".");
            refresh();
            playermove(newPosition, user, level->tiles);
             mvprintw(28, 0, "You got a common gold!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 'B': // Black gold
            user->gold += 5;
            level->tiles[newPosition->y][newPosition->x] = '.';
            mvprintw(newPosition->y, newPosition->x, ".");
            refresh();
            playermove(newPosition, user, level->tiles);
            mvprintw(28, 0, "You you got a black gold!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 'm': // Mace
            user->mace_count++;
            level->tiles[newPosition->y][newPosition->x] = '.';
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh();
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You picked up a Mace!                ");
            refresh();
            napms(1000);

            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 'd': // Dagger
            user->dagger_count++;
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You picked up a Dagger!                ");
            refresh();
            napms(1000);

            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 'w': // Magic Wand
            user->wand_count++;
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You picked up a Magic Wand!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;
        case 'a': // Arrow
            user->arrow_count++;
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You picked up an Arrow!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 's': // Sword
            user->sword_count++;
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh();
            playermove(newPosition, user, level->tiles);
            mvprintw(28, 0, "You picked up a Sword!           ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

            case 'Q':  // Speed spell
            if (!level->speed.taken) {
                level->speed.taken = true;
                speedBoostActive = true;
                speedTimer = time(NULL);  
                mvprintw(newPosition->y, newPosition->x, ".");
                refresh();
                mvprintw(28, 0, "You took the speed spell! Speed doubled for 10 seconds.");
                refresh();
                napms(1000);
                mvprintw(28, 0, "                                             ");
                refresh();
            }
            playermove(newPosition, user, level->tiles);
            break;

            case 'H':
            activate_health_spell(user);
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You took the Health spell! Speed doubled for 10 seconds.");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                                          ");
            refresh();
            break;

        default: 
            move(user->position->y, user->position->x);
            break;
    }
    return 1;
}

int checkposition3(Position *newPosition, Level *level) {
    Player *user = level->user;
    char tile = mvinch(newPosition->y, newPosition->x) & A_CHARTEXT;

     for (int i = 0; i < 10; i++) {
        Trap *trap = &level->traps[i];
        if (!trap->triggered && 
            trap->position.x == newPosition->x && 
            trap->position.y == newPosition->y) {
            // Trigger the trap
            trap->triggered = true;
             mvprintw(28, 0, "You stepped on a trap!                ");
            refresh();
            napms(1000);

            mvprintw(28, 0, "                                   ");
            refresh();
            level->user->health -= 5; 
        
            refresh();

        
        }
    }

    switch (tile) {
        case '.': 
        case '#':
        case '+': 
            playermove(newPosition, user, level->tiles);
            break;
        case '<':
        mvprintw(28, 0, "You found the stairs! Moving to the next level...");
        refresh();
        napms(1000);
        clear();
        gameLoop4(level->user);

        case 'O': 
            user->food += 1; 
            if(user->food>=2){
            user->food=2;
            user->health= 50;
            }
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You ate food!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case '$': // Common gold
            user->gold += 1;
            level->tiles[newPosition->y][newPosition->x] = '.';
            mvprintw(newPosition->y, newPosition->x, ".");
            refresh();
            playermove(newPosition, user, level->tiles);
             mvprintw(28, 0, "You got a common gold!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 'B': // Black gold
            user->gold += 5;
            level->tiles[newPosition->y][newPosition->x] = '.';
            mvprintw(newPosition->y, newPosition->x, ".");
            refresh();
            playermove(newPosition, user, level->tiles);
            mvprintw(28, 0, "You you got a black gold!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 'm': // Mace
            user->mace_count++;
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh();
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You picked up a Mace!                ");
            refresh();
            napms(1000);

            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 'd': // Dagger
            user->dagger_count++;
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You picked up a Dagger!                ");
            refresh();
            napms(1000);

            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 'w': // Magic Wand
            user->wand_count++;
            level->tiles[newPosition->y][newPosition->x] = '.';
            mvprintw(newPosition->y, newPosition->x, ".");
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You picked up a Magic Wand!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;
        case 'a': // Arrow
            user->arrow_count++;
            level->tiles[newPosition->y][newPosition->x] = '.';
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh();
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You picked up an Arrow!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 's': // Sword
            user->sword_count++;
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You picked up a Sword!           ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

            case 'Q':  // Speed spell
            if (!level->speed.taken) {
                level->speed.taken = true;
                speedBoostActive = true;
                speedTimer = time(NULL); 
                mvprintw(newPosition->y, newPosition->x, ".");
                refresh();
                mvprintw(28, 0, "You took the speed spell! Speed doubled for 10 seconds.");
                refresh();
                napms(1000);
                mvprintw(28, 0, "                                             ");
                refresh();
            }
            playermove(newPosition, user, level->tiles);
            break;

            case 'H':
            activate_health_spell(user);
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You took the Health spell! Speed doubled for 10 seconds.");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                                          ");
            refresh();
            break;

        default:
            move(user->position->y, user->position->x);
            break;
    }
    return 1;
}

int checkposition4(Position *newPosition, Level *level) {
    Player *user = level->user;
    char tile = mvinch(newPosition->y, newPosition->x) & A_CHARTEXT;

     for (int i = 0; i < 10; i++) {
        Trap *trap = &level->traps[i];
        if (!trap->triggered && 
            trap->position.x == newPosition->x && 
            trap->position.y == newPosition->y) {
            // Trigger the trap
            trap->triggered = true;
             mvprintw(28, 0, "You stepped on a trap!                ");
            refresh();
            napms(1000);

            mvprintw(28, 0, "                                   ");
            refresh();
            level->user->health -= 5; 
        
            refresh();
        
        }
    }

    switch (tile) {
        case '.':
        case '#': 
        case '+': 
            playermove(newPosition, user, level->tiles);
            break;
        case '<':
        clear();
        mvprintw(14, 40,"You won! Your Exp:%d , Your Gold:%d", level->user->exp, level->user->gold);
        break;
        case 'O': 
            user->food += 1; 
            if(user->food>=2){
            user->food=2;
            user->health= 50;
            }
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You ate food!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case '$': // Common gold
            user->gold += 1;
            level->tiles[newPosition->y][newPosition->x] = '.';
            mvprintw(newPosition->y, newPosition->x, ".");
            refresh();
            playermove(newPosition, user, level->tiles);
             mvprintw(28, 0, "You got a common gold!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 'B': // Black gold
            user->gold += 5;
            level->tiles[newPosition->y][newPosition->x] = '.';
            mvprintw(newPosition->y, newPosition->x, ".");
            refresh();
            playermove(newPosition, user, level->tiles);
            mvprintw(28, 0, "You you got a black gold!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 'm': // Mace
            user->mace_count++;
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You picked up a Mace!                ");
            refresh();
            napms(1000);

            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 'd': // Dagger
            user->dagger_count++;
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, ".");
            refresh(); 
            playermove(newPosition, user, level->tiles);
            mvprintw(28, 0, "You picked up a Dagger!                ");
            refresh();
            napms(1000);

            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 'w': // Magic Wand
            user->wand_count++;
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, ".");
            refresh(); 
            playermove(newPosition, user, level->tiles);
            mvprintw(28, 0, "You picked up a Magic Wand!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;
        case 'a': // Arrow
            user->arrow_count++;
            level->tiles[newPosition->y][newPosition->x] = '.';
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You picked up an Arrow!                ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

        case 's': // Sword
            user->sword_count++;
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh();
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You picked up a Sword!           ");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                   ");
            refresh();
            break;

            case 'Q':  // Speed spell
            if (!level->speed.taken) {
                level->speed.taken = true;
                speedBoostActive = true;
                speedTimer = time(NULL);  // Set the current time as the start of the boost
                mvprintw(newPosition->y, newPosition->x, ".");
                refresh();
                mvprintw(28, 0, "You took the speed spell! Speed doubled for 10 seconds.");
                refresh();
                napms(1000);
                mvprintw(28, 0, "                                             ");
                refresh();
            }
            playermove(newPosition, user, level->tiles);
            break;

            case 'H':
            activate_health_spell(user);
            level->tiles[newPosition->y][newPosition->x] = '.'; 
            mvprintw(newPosition->y, newPosition->x, "."); 
            refresh(); 
            playermove(newPosition, user, level->tiles); 
            mvprintw(28, 0, "You took the Health spell! Speed doubled for 10 seconds.");
            refresh();
            napms(1000);
            mvprintw(28, 0, "                                                          ");
            refresh();
            break;

        default:
            move(user->position->y, user->position->x);
            break;
    }
    return 1;
}


int playermove(Position *newPosition, Player *user, char **level) {
    mvprintw(user->position->y, user->position->x, "%.1s", &level[user->position->y][user->position->x]);
    user->position = newPosition;
    mvprintw(user->position->y, user->position->x, "@");
    move(user->position->y, user->position->x);
    return 0;
     
}
void addpositionYX(int ** frontier,int frontiercount, int y, int x){
frontier[frontiercount][0] = y;
frontier[frontiercount][1] = x;
}

int checkPosition(int y ,int x){
 
 char temp = mvinch(y,x);
  
  if(temp == '.' || temp == '|' || temp == '-'){
   return 0;
  }
  else{
    return 1;
  }
}

int addneighbors(int **frontier, int frontiercount, int ***camefrom, int y, int x) {

 
    if (y >= 0 && y < 25 && x >= 0 && x < 100) {
        // North
        if (y > 0 && camefrom[y - 1][x][0] < 0 && checkPosition(y - 1, x)) {
            addpositionYX(frontier, frontiercount, y - 1, x);
            frontiercount++;
            camefrom[y - 1][x][0] = y;
            camefrom[y - 1][x][1] = x;
        }
        // South
        if (y < (25 - 1) && camefrom[y + 1][x][0] < 0 && checkPosition(y + 1, x)) {
            addpositionYX(frontier, frontiercount, y + 1, x);
            frontiercount++;
            camefrom[y + 1][x][0] = y;
            camefrom[y + 1][x][1] = x;
        }
        // East
        if (x < (100 - 1) && camefrom[y][x + 1][0] < 0 && checkPosition(y, x + 1)) {
            addpositionYX(frontier, frontiercount, y, x + 1);
            frontiercount++;
            camefrom[y][x + 1][0] = y;
            camefrom[y][x + 1][1] = x;
        }
        // West
        if (x > 0 && camefrom[y][x - 1][0] < 0 && checkPosition(y, x - 1)) {
            addpositionYX(frontier, frontiercount, y, x - 1);
            frontiercount++;
            camefrom[y][x - 1][0] = y;
            camefrom[y][x - 1][1] = x;
        }
    } 

    return frontiercount;
}


void pathfind(Position* start, Position* end) {
    int i, j;
    int x, y;
    int tempY;
    int **frontier = malloc(sizeof(int*) * 100 * 25);
    int ***camefrom = malloc(sizeof(int**) * 25);

    int frontierindex = 0;
    int frontiercount = 0;
    for (i = 0; i < 25 * 100; i++) {
        frontier[i] = malloc(sizeof(int) * 2);
    }
    for (i = 0; i < 25; i++) {
        camefrom[i] = malloc(sizeof(int*) * 100);
        for (j = 0; j < 100; j++) {
            camefrom[i][j] = malloc(sizeof(int) * 2);
            camefrom[i][j][0] = -1;
            camefrom[i][j][1] = -1;
        }
    }

    camefrom[start->y][start->x][0] = -9;
    camefrom[start->y][start->x][1] = -9;

    addpositionYX(frontier, frontiercount, start->y, start->x);
    frontiercount++;

    while (frontierindex < frontiercount) {
        y = frontier[frontierindex][0];
        x = frontier[frontierindex][1];
        frontierindex++;

        if (y == end->y && x == end->x) {
            break;
        }

        frontiercount = addneighbors(frontier, frontiercount, camefrom, y, x);
    }

    y = end->y;
    x = end->x;

    while (y != start->y || x != start->x) {
        tempY = y;
    
        
        if (tempY >= 0 && tempY < 25 && x >= 0 && x < 100) {
            if (camefrom[tempY] != NULL && camefrom[tempY][x] != NULL) {
                y = camefrom[tempY][x][0];
                x = camefrom[tempY][x][1];
                
                mvprintw(y, x, "#");
             
            } else {
             
                break;
            }
        } else {
            
            break;
        }
    }

    for (i = 0; i < 25 * 100; i++) {
        free(frontier[i]);
    }
    for (i = 0; i < 25; i++) {
        for (j = 0; j < 100; j++) {
            free(camefrom[i][j]);
        }
        free(camefrom[i]);
    }
    free(frontier);
    free(camefrom);
}


void closeMenu(int numberitems, MENU *menu, ITEM ** items){

    unpost_menu(menu);
    free_menu(menu);
    for(int i = 0; i < numberitems; i++){

        free_item(items[i]);
    }
}



int mainMenu(int numberitems, char *choices[]) {
    int i, c;
    int value;
    MENU *menu;
    ITEM **items = (ITEM **)malloc(sizeof(ITEM *) * (numberitems + 1)); // Allocate space for NULL terminator
    ITEM *current;

    if (items == NULL) {
        fprintf(stderr, "Memory allocation failed for items\n");
        return -1;
    }

    for (i = 0; i < numberitems; i++) {
        items[i] = new_item(choices[i], "");
    }
    items[numberitems] = (ITEM *)NULL; // Null-terminate the array

    menu = new_menu((ITEM **)items);
    post_menu(menu);
    refresh();

    while (true) {
        c = getch();
        switch (c) {
        case KEY_DOWN:
            menu_driver(menu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(menu, REQ_UP_ITEM);
            break;
        case 10:
            current = current_item(menu);
            value = item_index(current);
            closeMenu(numberitems, menu, items);
            return value;
        }
    }
}


void draw_menu_border()
{
    clear();
    for (int x = 0; x < COLS; ++x)
    {
        mvprintw(0, x, "-");
        mvprintw(LINES - 1, x, "-");
    }
    for (int y = 0; y < LINES; ++y)
    {
        mvprintw(y, 0, "|");
        mvprintw(y, COLS - 1, "|");
    }
}

void get_player_info(Menu *u, Menu *p, Menu *e)
{
    bool hasLower, hasUpper, hasDigit;

    echo();
    draw_menu_border();

    mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Enter your Username: ");
    getnstr(u->username, 60);

    while (true) {
        hasLower = hasUpper = hasDigit = false;

        mvprintw(LINES / 2 - 1, COLS / 2 - 15, "Enter your Password: ");
        getnstr(p->password, 60);

        if (strlen(p->password) < 7) {
            mvprintw(LINES / 2 + 8, COLS / 2 - 24, "Your password should have at least 7 characters");
              mvprintw(LINES / 2 - 1, COLS / 2 +6, "                                     ");
            continue;
        }

        for (int i = 0; p->password[i] != '\0'; i++) {
            if (islower(p->password[i])) hasLower = true;
            else if (isupper(p->password[i])) hasUpper = true;
            else if (isdigit(p->password[i])) hasDigit = true;
        }

        if (!(hasLower && hasUpper && hasDigit)) {
            mvprintw(LINES / 2 + 9, COLS / 2 - 37, "Password must contain at least one uppercase, one lowercase, and one digit");
             mvprintw(LINES / 2 - 1, COLS / 2 +6, "                                     ");
            continue;
        }

        break;
    }

    while (true) {
        mvprintw(LINES / 2, COLS / 2 - 15, "Enter your Email: ");
        getnstr(e->email, 50);

        if (!is_valid_email(e->email)) {
            mvprintw(LINES / 2 + 10, COLS / 2 - 24, "Invalid Email. Please try again.");
            mvprintw(LINES / 2, COLS / 2 +3, "                                  ");
            continue;
        }
    
        break;
    }

    noecho();
}


bool is_valid_email(const char *email) {
    const char *at_sign = strchr(email, '@');
    const char *dot_sign = strrchr(email, '.');

    if (at_sign == NULL || dot_sign == NULL || at_sign > dot_sign) {
        return false;
    }

    if (at_sign == email) {
        return false;
    }

    if (dot_sign - at_sign <= 1) {
        return false;
    }
   
    if (strlen(dot_sign + 1) < 2) {
        return false;
    }
    
    for (const char *p = email; p < at_sign; p++) {
        if (!isalnum(*p) && *p != '.' && *p != '_' && *p != '%' && *p != '+' && *p != '-') {
            return false;
        }
    }

    for (const char *p = at_sign + 1; p < dot_sign; p++) {
        if (!isalnum(*p) && *p != '.' && *p != '-') {
            return false;
        }
    }

    return true;
}


void spawn_food(Level *level, int food_count) {

    level->numberOfMonsters = food_count;  // Reusing the monster count variable to store food count
    level->monsters = malloc(sizeof(Food *) * food_count); 

    for (int i = 0; i < food_count; i++) {
        Food *food = malloc(sizeof(Food));
        Room *room = level->rooms[rand() % 5];

        food->position.x = rand() % (room->width - 2) + room->position.x + 1;
        food->position.y = rand() % (room->height - 2) + room->position.y + 1;
        food->eaten = false;

        attron(COLOR_PAIR(4));
        mvprintw(food->position.y, food->position.x, "O");
        attroff(COLOR_PAIR(4));
        level->monsters[i] = (Monster *)food; 
    }

    refresh();
}



void spawn_gold(Level *level, int gold_count) {
    int i;
    for (i = 0; i < 5; i++) {
        Room *room = level->rooms[i];

      
        for (int j = 0; j < gold_count; j++) {
            int is_black_gold = rand() % 4 == 0; 
            int x = rand() % (room->width - 2) + room->position.x + 1;
            int y = rand() % (room->height - 2) + room->position.y + 1;

            if (is_black_gold) {
                attron(COLOR_PAIR(2));
                mvprintw(y, x, "B");
                attroff(COLOR_PAIR(2));
                level->tiles[y][x] = 'B'; 
                attron(COLOR_PAIR(1)); 
                mvprintw(y, x, "$");
                attroff(COLOR_PAIR(1));
                level->tiles[y][x] = '$'; 
            }
        }
    }
    refresh();
}

void spawn_weapons(Level *level) {
    const char weapon_symbols[] = {'m', 'd', 'w', 'a', 's'}; // Mace, Dagger, Wand, Arrow, Sword
    int weapon_count = sizeof(weapon_symbols) / sizeof(weapon_symbols[0]);

    for (int i = 0; i < weapon_count; i++) {
        Weapon *new_weapon = malloc(sizeof(Weapon));
        new_weapon->symbol = weapon_symbols[i];
        new_weapon->taken = false;

        Room *room = level->rooms[rand() % 5];
        new_weapon->position.x = room->position.x + rand() % (room->width - 2) + 1;
        new_weapon->position.y = room->position.y + rand() % (room->height - 2) + 1;

    
        mvprintw(new_weapon->position.y, new_weapon->position.x, "%c", new_weapon->symbol);
        level->tiles[new_weapon->position.y][new_weapon->position.x] = new_weapon->symbol;
    }
    refresh();
}


void displayInventory(Player *player) {
    clear();
    mvprintw(5, 10, "=== Inventory ===");
    mvprintw(7, 10, "Mace: %d", player->mace_count);
    mvprintw(8, 10, "Dagger: %d", player->dagger_count);
    mvprintw(9, 10, "Magic Wand: %d", player->wand_count);
    mvprintw(10, 10, "Arrow: %d", player->arrow_count);
    mvprintw(11, 10, "Sword: %d", player->sword_count);
    mvprintw(13, 10, "Press 'i' to return to the game.");
    refresh();
}

void spawn_traps(Level *level) {
    int i, roomIndex;
    Trap *trap;

    for (i = 0; i < 10; i++) {
        trap = &level->traps[i];
        trap->triggered = false;
        roomIndex = rand() % level->numberofrooms;
        Room *room = level->rooms[roomIndex];

     
        trap->position.x = room->position.x + (rand() % (room->width - 2)) + 1;
        trap->position.y = room->position.y + (rand() % (room->height - 2)) + 1;

     
        while (level->tiles[trap->position.y][trap->position.x] != '.') {
            trap->position.x = room->position.x + (rand() % (room->width - 2)) + 1;
            trap->position.y = room->position.y + (rand() % (room->height - 2)) + 1;
        }
    }
}


void place_stair(Level *level) {
    Room *lastRoom = level->rooms[5]; 
    int stair_x, stair_y;

    
    do {
        stair_x = lastRoom->position.x + 1 + rand() % (lastRoom->width - 2);
        stair_y = lastRoom->position.y + 1 + rand() % (lastRoom->height - 2);
    } while (mvinch(stair_y, stair_x) != '.'); 

   
    mvprintw(stair_y, stair_x, "<");
    level->tiles[stair_y][stair_x] = '<'; 
}

void spawn_speed_spell(Level *level) {
    Room *room = level->rooms[rand() % 5]; 
    level->speed.position.x = rand() % (room->width - 2) + room->position.x + 1;
    level->speed.position.y = rand() % (room->height - 2) + room->position.y + 1;
    level->speed.taken = false;


    mvprintw(level->speed.position.y, level->speed.position.x, "Q");
    refresh();
}


void spawn_health_spell(Level *level)
{
     Room *room = level->rooms[rand() % 5]; 
    Position spell_position;
    spell_position.x = rand() % (room->width - 2) + room->position.x + 1;
    spell_position.y = rand() % (room->height - 2) + room->position.y + 1;
    
    mvprintw(spell_position.y, spell_position.x, "H");
    refresh();
}

void activate_health_spell(Player *user)
{
    for (int i = 0; i < 10; i++)
    {
        sleep(1);
        user->health += 1;
        if (user->health > user->maxhealth)
        {
            user->health = user->maxhealth;
            break;
        }
        mvprintw(28, 0, "Healing... Health: %d", user->health);
        refresh();
    }
    mvprintw(28, 0, "Healing complete!       ");
    refresh();
}
