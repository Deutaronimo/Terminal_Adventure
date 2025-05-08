#include<iostream>
#include<map>
#include<cstdlib>
#include<string>
#include<vector>
#include<memory>
#include<thread>
#include<chrono>

using namespace std;

// World parameters
const int mapHeight = 256;
const int mapWidth = 256;
const int mapSize = mapWidth * mapHeight;
int playerStartPosition = 0;
int position_x = 0;
int position_y = 0;
int position = 0;
int textPrintSpeed = 125;

unsigned int seed = 0;

// Recorded stats
int moveCount = 0;
int worldPosition = 0;
const string currentMap = "overworld";

// Viewport screen setup
const int windowHeight = 14;
const int windowWidth = 32;
const int windowSize = windowHeight * windowWidth;
int windowPosition = 0;
int windowCenter = windowWidth / 2 + (windowHeight / 2) * windowWidth;

// Define graphic ASCII tiles
const char water = 126;
const char grass = 46;
const char island = 79;
const char enemyShip = 69;
const char playerIcon = 65;
const char wall = 35;
const char town = 84;

// unicode graphics
const string playerIcon2 = "\u263A";
const string lightning = "\u26A1";
const string star = "\u2605";
const string sun = "\u2600";
const string shield = "\u1f6e1";
const string swords = "\u2694";
const string bomb = "\u1f4a3";
const string bow = "\u1f3f9";
const string gem = "\u1f48e";
const string dagger = "\u1f5e1";
const string castle = "\u1f3f0";

struct vec2d
{
    int x = 0;
    int y = 0;
    int p = 0;
};

static void printWithDelay(const string& text)
{
    for (char c : text)
    {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(textPrintSpeed));
    }
    cout << endl;
}

static int getAbsolutePosition(vec2d pos)
{
    int answer = pos.x + pos.y * mapWidth;
    return answer;
}

static std::string generateNpcName()
{
    std::vector<std::string> syllables =
    {
        "om","mo","em","ori","czi","ist","ayx","aza","aso","tza","onn","dar","eff","axl","xy","ly","ae","itz",
        "ara","ix","xi","goy","al", "ar", "en", "dor","mir", "thal", "wyn", "eth", "ion", "ael", "ka", "rin",
        "tor", "el", "fen", "gal", "nor", "sil", "val", "zor", "bra", "sha", "dra", "lor", "mal", "nel", "oth", "pyr",
        "qua", "ryn", "sel", "tan", "uth", "vor", "xel", "yra", "zyn", "bel", "cal", "den", "eri", "fyn", "gor", "hal",
        "iri", "jal", "kel", "len", "mor", "ner", "ol", "pal", "quin", "rel", "sar", "tal", "ul", "ven", "wyr", "xen",
        "yel", "zar", "bal", "cer", "del", "er", "fal", "gel", "har", "il", "jar", "kor", "lan", "mel", "nel", "or",
        "par", "quor", "ras", "sel", "tar", "ur", "vel", "wal", "xor", "yal", "zor", "bran", "char", "dren", "lir",
        "q'","bla","oo","ob","omn","tyr","mar", "ner", "oryn", "pyr", "quin", "ral", "syth", "thar", "vyn"

    };




    std::string name;

    int maxSyllables = 2;
    int minSyllables = 2;

    int numSyllables = rand() % maxSyllables + minSyllables; // Randomly choose 2-4 syllables

    for (int i = 0; i < numSyllables; i++)
    {
        name += syllables[rand() % syllables.size()];
    }

    // Capitalize the first letter
    name[0] = toupper(name[0]);

    return name;
}

static std::string generatePlaceName()
{
    std::string firstPart = "";
    std::string secondPart = "";
    std::string fullName = "";

    std::vector<std::string> firstHalf =
    {
        "ever","chill","night","dark","deep",
        "front","grime","dirt","hallow","ember",
        "never","under","gloom","mist","shadow",
        "terra","spirit","ether","wind","water",
        "west","east","south","north","wither",
        "frost","sun","rain","snow","hail",
        "sayda","bore","over","ara","far",
        "mans","hells","heavens","gray","tarn",
        "sutton","earth","moon","star","gala",
        "ashen","earthen","celeste","molten","nether",
        "morden","pyro","zero","frozen","wither",
        "pike","hilts","dagger","bow","flail",
        "ghoul","lich","goblin","gnome","dragon",
        "silver","gold","wood","glass","potter",
        "gem","coin","trade","wonder","graven",
        "crystal","slates","rock","surface","day",
        "draken","mortal","fays","edge","farm",
        "tin","mortis","valis","glad","pad",
        "cron","bosh","agd","here","luds"
    };

    std::vector<std::string> secondHalf =
    {
        "glade","wood","dale","shade","hall",
        "moor","rend","mire","shire","crest",
        "vale","stead","wich","more","thton",
        "lyn","thol","enthal","orum","een",
        "ill","dul","ora","illa","ayn","barrow",
        "ten","fold","send","er","march",
        "strom","storm","bend","way","dore",
        "rest","break","tell","sond","beam",
        "tzer","peak","tellum","ray","reach",
        "berg","holt","heim","gart","still",
        "haven","trin","fen","sten","blend",
        "worth","wich","brook","holm","stow",
        "bury","stings","sham","mere","lech",
        "cairn","fal"
    };

    int random1 = rand() % firstHalf.size();
    int random2 = rand() % secondHalf.size();
    firstPart = firstHalf[random1];
    secondPart = secondHalf[random2];
    fullName = firstPart + secondPart;
    fullName[0] = toupper(fullName[0]);
    return fullName;
}

enum Direction
{
    NONE,
    NORTH,
    SOUTH,
    EAST,
    WEST
};

enum Color
{
    black,
    red,
    green,
    yellow,
    blue,
    magenta,
    cyan,
    white,

};

enum actionState
{
    idle,
    walking,
    mounted,
    aboard,
    flying,
    spirit
};

Direction direction;
Color textColor;

bool canMove = true;
bool isRunning = true;

char world[mapSize];
char window[windowSize];

struct Entity
{
    string name = "none";
    string shipName = "none";
    string description = "No description";
    // Base Stats
    int hitPoints = 0;
    int strength = 0;
    int dexterity = 0;
    int wisdom = 0;
    int attack = 0;
    int defense = 0;
    int speed = 0;
    int gold = 0;
    int xp = 0;
    int level = 0;

    int position = 0;
    int dammage = 0;
    int armor = 0;
    char icon = 0;
    bool isAlive = true;
    bool onShip = false;
    vec2d mapPosition;
};

struct Tile
{

    vec2d mapPosition;
    int teleportDestination = 0;
    int teleportSource = 0;
    int TeleportExit = 0;
    int absolutePosiiton = 0;
    string name = "none";
    string type = "none";
    string description = "No description";
    char graphic = 65;
    bool isPassible = true;
    Color color;
    char* map; //map that the tile occupies.

};

Tile* overWorldMap[mapSize];


static void setTextColor(Color _color)
{


    if (_color == green)
    {
        cout << "\u001b[32m";
    }

    if (_color == white)
    {
        cout << "\u001b[0m";
    }

    if (_color == red)
    {
        cout << "\u001b[31m";
    }

    if (_color == yellow)
    {
        cout << "\u001b[33m";
    }

    if (_color == blue)
    {
        cout << "\u001b[34m";
    }

    if (_color == magenta)
    {
        cout << "\u001b[35m";
    }

    if (_color == cyan)
    {
        cout << "\u001b[36m";
    }
}

static bool moveCheck(Direction _direction, int _position)
{
    bool borderCheckOk = false;
    bool noCollision = false;

    if (_direction == NORTH)
    {

        if (position_y - 1 < 0)
        {
            borderCheckOk = false;


        }
        else
        {
            position_y--;
            borderCheckOk = true;
        }

    }

    if (_direction == SOUTH)
    {

        if (position_y + 1 > mapHeight - windowHeight)
        {
            borderCheckOk = false;


        }
        else
        {
            position_y++;
            borderCheckOk = true;
        }
    }

    if (_direction == EAST)
    {

        if (position_x + 1 > mapWidth - windowWidth)
        {
            borderCheckOk = false;

        }
        else
        {
            position_x++;
            borderCheckOk = true;
        }

    }

    if (_direction == WEST)
    {

        if (position_x - 1 < 0)
        {
            borderCheckOk = false;
            return false;

        }
        else
        {
            position_x--;
            borderCheckOk = true;
        }
    }

    noCollision = true; // temporary until collision check is made.

    // After border check is ok and no collisions, move player.
    if (borderCheckOk == true && noCollision == true)
    {
        return true;
    }

    else
        return false;
}

static void populatePlaces()
{
    int randomTownCount = 20;


    // Create 20 random towns and place them on the map within an x and y of 100
    // Dont generate if its in the border area..

    for (int x = 0; x < randomTownCount; x++)
    {

        int ran_x = 15 + rand() % 100;
        int ran_y = 6 + rand() % 100;


        vec2d spot = { ran_x, ran_y, 0 };
        Tile* place = new Tile;
        place->name = generatePlaceName();
        place->type = "town";
        place->color = yellow;
        place->description = "A cozy small town, inhabitted by a small group of close residents.";
        place->mapPosition = spot;
        place->graphic = town;
        place->absolutePosiiton = getAbsolutePosition(place->mapPosition);

        delete overWorldMap[place->absolutePosiiton];

        overWorldMap[place->absolutePosiiton] = place;


    }
}

static void drawScreen()
{

    bool isPlayer = false;

    for (int y = 0; y < windowHeight; y++)
    {
        for (int x = 0; x < windowWidth; x++)
        {

            int position = windowPosition + x + (y * mapWidth);
            // do a place check here

            // If draw is center of window draw the player.
            if (y == 6 && x == 14)
            {
                setTextColor(yellow);
                cout << playerIcon2;
                isPlayer = true;
            }

            if (!isPlayer)
            {
                // Draw the current char tile.
                //cout << world[position];
                setTextColor(overWorldMap[position]->color);
                cout << overWorldMap[position]->graphic;


                // Reset text to white.
                setTextColor(white);
            }
            isPlayer = false;

        }
        cout << endl;

    }
    cout << "Location X:" << position_x + 15 << " Y:" << position_y + 7 << " Tile position: " << windowPosition
        << " Name: " << overWorldMap[windowPosition]->name << endl;
}

static void loadWorld(string type)
{

    // fill the map with tiles.
    for (int y = 0; y < mapHeight; y++)
    {
        for (int x = 0; x < mapWidth; x++)
        {

            if (x < 14 || x > mapWidth - windowWidth || y < 6 || y > mapHeight - windowHeight)
            {
                vec2d pos = { x,y,0 };
                Tile* tile = new Tile;

                tile->mapPosition = { x,y,0 };
                tile->type = "water";
                tile->graphic = water;
                tile->description = "water";
                tile->isPassible = false;
                tile->color = blue;
                tile->absolutePosiiton = getAbsolutePosition(tile->mapPosition);
                overWorldMap[tile->absolutePosiiton] = tile;
            }

            else
            {
                vec2d pos = { x,y,0 };
                Tile* tile = new Tile;

                tile->mapPosition = { x,y,0 };
                tile->type = "grassland";
                tile->name = "field";
                tile->graphic = grass;
                tile->description = "a typical grass area";
                tile->isPassible = true;
                tile->color = green;
                tile->absolutePosiiton = getAbsolutePosition(tile->mapPosition);
                overWorldMap[tile->absolutePosiiton] = tile;
            }
        }
    }
}

static void showMapIcons()
{

}

static void moveMenu()
{

    string choice = "none";
    cout << "______Action_____" << endl;
    cout << " Move  : N S E W " << endl;
    cout << " Status:  " << endl;
    cout << " Look  : L" << endl;
    cout << " Menu  : " << endl;
    cout << " Icons : " << endl;
    cout << " Quit  : Q " << endl;
    cout << "-----------------" << endl;
    cout << "Action:";
    cin >> choice;

    if (choice == "")
    {
        system("clear");
        drawScreen();
        moveMenu();
    }


    if (choice == "s")
    {

        if (moveCheck(SOUTH, windowPosition))
        {
            windowPosition = windowPosition + mapWidth;
        }
    }


    if (choice == "n")
    {
        if (moveCheck(NORTH, windowPosition))
        {
            windowPosition = windowPosition - mapWidth;
        }
    }


    if (choice == "e")
    {
        if (moveCheck(EAST, windowPosition))
        {
            windowPosition = windowPosition + 1;
        }
    }


    if (choice == "w")
    {
        if (moveCheck(WEST, windowPosition))
        {
            windowPosition = windowPosition - 1;
        }
    }

    if (choice == "icons")
    {
        showMapIcons();
    }


    if (choice == "q")
    {
        isRunning = false;
    }

    if (choice == "l")
    {
        int tempPos = windowPosition + 14 + mapWidth * 6;
        cout << "You are at a " << overWorldMap[tempPos]->type << " named " << overWorldMap[tempPos]->name << ".\n" << overWorldMap[tempPos]->description << endl;
    }

}

static void titleScreen()
{
    system("clear");
    string x;

    setTextColor(red);
    cout << " ??????   ??????   ??????   ?? ??? ???? ????? ???        ????? ?????? " << endl;
    cout << "???? ??  ????  ??????? ??   ????? ??????? ?????????     ??? ?????   ? " << endl;
    cout << "? ????? ?? ???????? ????? ????? ??????   ???? ??   ???????????????????" << endl;
    cout << "? ?? ?  ?? ?????? ? ?? ?  ?? ?? ??? ??   ?  ? ??   ???? ??   ? ?? ?? ?" << endl;
    cout << "  ?  ?     ? ? ??   ?  ?   ? ?? ???  ?      ?  ?   ?? ?  ?   ?  ? ?  ?" << endl;
    cout << "?        ? ? ? ?  ?        ? ?? ? ?      ?     ?   ?   ? ?   ?    ?   " << endl;
    cout << "? ?          ? ?  ? ?      ?  ?          ?         ?  ?      ?    ?  ?" << endl;
    cout << "?                 ?                                                   " << endl;
    setTextColor(white);
    cout << "Type ok to start...";
    cin >> x;

    system("clear");
}

static void playerCreation()
{
    Entity* player = new Entity;
    string choice;
    player->icon = playerIcon;
    cout << "_______Character Creation________" << endl;
    cout << " Pick your name: ";
    cin.ignore();
    getline(cin, player->name);
    cout << "Ok " << player->name << " what is the name of your ship? ";
    cin.ignore();
    getline(cin, player->shipName);
    cout << "Let me get this right, " << player->name << " is the captain of " << player->shipName << " ?" << endl;
    cout << " Y/N : ";
    cin >> choice;
    system("clear");

}

int main()
{
    // ______Setup_______
    // Randomize the randomizer
    srand(static_cast<unsigned int>(time(nullptr)));
    //srand(static_cast<unsigned int>(seed));

    // load some generated test map tiles.
    loadWorld("test");

    // Load in some specific tiles.
    populatePlaces();

    // Diplay title screen.
    //titleScreen();

    // Player creation.
    //playerCreation();

    //Main loop
    while (isRunning)
    {
        //system("clear");
        drawScreen();
        moveMenu();
    }

    return 0;
}