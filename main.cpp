#include<iostream>
#include<map>
#include<cstdlib>
#include<string>
#include<vector>
#include<memory>
#include<thread>
#include<chrono>

using namespace std;

// Define OS-specific command for clearing the screen.
#ifdef _WIN32
const char OS_CLEAR[] = "cls";
#else
const char OS_CLEAR[] = "clear";
#endif

int textPrintSpeed = 125;
int globalturns = 0;
unsigned int randomSeed = 0;
unsigned int seed = 69;

bool skipTime = false;
bool isRunning = true;
bool printDebugData = true;

// Recorded stats
int moveCount = 0;
string worldName = "";

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

//Forward declare structures.
struct Tile;
struct Entity;
struct Stats;


struct vec2d
{
    int x = 0;
    int y = 0;
    int z = 0;
};


// the position of the window on the world map.
vec2d windowPosition = {0,0,0};

// where the player starts in the world map.
vec2d playerStartPosition = {0,0,0};

// the position of the player on the window.
vec2d playerWindowPosition = {14,6,0};

//the players position on the world map
vec2d playerPosition = {0,0,0};

//these define the sizes of the world and view window.
constexpr vec2d windowSize = {32,16,0};
constexpr vec2d mapSize    = {256,256,0};

static void printWithDelay(const string& text)
{
    for (char c : text)
    {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(textPrintSpeed));
    }
    cout << endl;
}

static int getAbsolutePosition(const vec2d pos)
{
    int answer = pos.x + pos.y * mapSize.x;
    return answer;
}

vec2d addVec2d(vec2d first, vec2d second)
{
    vec2d answer;
    answer.x = first.x + second.x;
    answer.y = first.y + second.y;
    answer.z = 0;
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
    std::string firstPart  = "";
    std::string secondPart = "";
    std::string fullName   = "";

    std::vector<std::string> firstHalf =
    {
        "ever","chill","night","dark","deep",
        "front","grime","dirt","hallow","ember",
        "never","under","gloom","mist","shadow",
        "terra","spirit","ether","wind","water",
        "west","east","south","north","wither",
        "frost","sun","rain","snow","hail","sullon",
        "sayda","bore","over","ara","far","chill",
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
        "cairn","fal","depth","hill"
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

enum ActionState
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

char world[mapSize.x * mapSize.y];
char window[windowSize.x * windowSize.y]; 

struct Stats
{
    
};

struct Entity
{
    string firstName = "none";
    string lastName = "none";
    string type = "none";
    char icon = 0;
    bool isAlive = true;
    ActionState actionState = idle;
    vec2d mapPosition;
    Tile** map = nullptr;
};

struct Tile
{
    vec2d mapPosition;
    int absolutePosiiton = 0;
    string name = "none";
    string type = "none";
    string groupName = "none";
    string continentName = "none";
    string description = "No description";
    char graphic = 65;
    bool isPassible = true;
    Color color;
    Tile** map = nullptr; //map that the tile occupies.
    vector<Entity*>entities;
};

// This will hold a pointer to the current map being used.

Tile* overWorldMap[mapSize.x * mapSize.y];
Tile* iceWorldMap[mapSize.x  * mapSize.y];
Tile** currentMap = nullptr;

static void makeTestEntities(int quantity, int x, int y)
{
    int spot = getAbsolutePosition({x,y,0});
    for (int x = 0; x < quantity; x++)
    {
        Entity* entity = new Entity;
        entity->firstName = generateNpcName();
        currentMap[spot]->entities.push_back(entity);
    }
}

char overWorldMapData[mapSize.x * mapSize.y];

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

static bool moveWindowCheck(Direction _direction, const vec2d _windowPosition)
{
    
    if (_direction == NORTH)
    {
        if (_windowPosition.y - 1 < 0)
        {
            return false;
        }
    }
    
    if (_direction == SOUTH)
    {
        if (_windowPosition.y + 1 > mapSize.y - windowSize.y)
        {
            return false;
        }
    }
    
    if (_direction == WEST)
    {
        if (_windowPosition.x - 1 < 0)
        {
            return false;
        }
    }
    
    if (_direction == EAST)
    {
        if (_windowPosition.x + 1 > mapSize.x - windowSize.x )
        {
            return false;
        }
    }
    
    return true;

}

static bool movePlayerCheck(Direction _direction, const vec2d _playerPosition)
{

    int checkPosition = 0;
    
    if (_direction == SOUTH)
    {
        checkPosition = getAbsolutePosition(_playerPosition) + mapSize.x;
        if (currentMap[checkPosition]->isPassible == false)
        {
            return false;
        }
        
    }
    
    if (_direction == NORTH)
    {
        checkPosition = getAbsolutePosition(_playerPosition) - mapSize.x;
        if (currentMap[checkPosition]->isPassible == false)
        {
            return false;
        }
        
    }
    
    if (_direction == EAST)
    {
        checkPosition = getAbsolutePosition(_playerPosition)+1;
        if (currentMap[checkPosition]->isPassible == false)
        {
            return false;
        }
        
    } 
    
    if (_direction == WEST)
    {
        checkPosition = getAbsolutePosition(_playerPosition)-1;
        if (currentMap[checkPosition]->isPassible == false)
        {
            return false;
        }
        
    } 
    
    return true;
}

static void populateOverWorldPlaces()
{
    int randomTownCount = 100;

    // this is test data...
    // Create 20 random towns and place them on the map within an x and y of 100
    // Dont generate if its in the border area..

    for (int x = 0; x < randomTownCount; x++)
    {

        int ran_x = 15 + rand() % 100;
        int ran_y = 6  + rand() % 100;

        vec2d spot = { ran_x, ran_y, 0 };
        Tile* place = new Tile;
        place->name = generatePlaceName();
        place->type = "town";
        place->color = yellow;
        place->continentName = worldName;
        place->description = "A cozy small town, inhabitted by a small group of close residents named " + place->name;
        place->mapPosition = spot;
        place->graphic = town;
        place->absolutePosiiton = getAbsolutePosition(place->mapPosition);
        place->map = currentMap;
        delete overWorldMap[place->absolutePosiiton];

        overWorldMap[place->absolutePosiiton] = place;
        //add some entities to towns.
        makeTestEntities(5,spot.x,spot.y);

    }
}

static void drawScreen()
{
    
    bool isPlayer = false;


    for (int y = 0; y < windowSize.y; y++)
    {
        for (int x = 0; x < windowSize.x; x++)
        {

            int position = windowPosition.x + x + ((windowPosition.y + y) * mapSize.x);
            
            // If draw is center of window draw the player.
            if (y == playerWindowPosition.y && x == playerWindowPosition.x)
            {
                setTextColor(yellow);
                cout << playerIcon2;
                isPlayer = true;
            }

            if (!isPlayer)
            {
                // Draw the current char tile.
                setTextColor(currentMap[position]->color);
                cout << currentMap[position]->graphic;

                // Reset text to white.
                setTextColor(white);
            }
            isPlayer = false;

        }
        cout << endl;

    }
    
    if (printDebugData)
    {
        vec2d temp = addVec2d(windowPosition,playerWindowPosition);
        cout << "Player xy: " << playerWindowPosition.x + windowPosition.x<< ", " << playerWindowPosition.y + windowPosition.y << endl;
        cout << "Window xy: " << windowPosition.x << ", " << windowPosition.y << endl;
        cout << "playerABS: " << getAbsolutePosition(temp) << endl;
        cout << "windowABS: " << getAbsolutePosition(windowPosition) << endl;
    }
    
}

static void loadIceWorld()
{
     // fill the map with tiles.
    for (int y = 0; y < mapSize.y; y++)
    {
        for (int x = 0; x < mapSize.x; x++)
        {
            if (x < 14 || x > mapSize.x - windowSize.x || y < 6 || y > mapSize.y - windowSize.y)
            {
                
                Tile* tile = new Tile;
                tile->mapPosition = { x,y,0 };
                tile->type = "Ice";
                tile->graphic = '#';
                tile->groupName = generateNpcName() + " Ice wall.";
                tile->continentName = worldName;
                tile->description = "a megalithisly large ice wall.";
                tile->name = tile->groupName;
                tile->isPassible = false;
                tile->color = blue;
                tile->absolutePosiiton = getAbsolutePosition(tile->mapPosition);
                
                iceWorldMap[tile->absolutePosiiton] = tile;
            }
            
            else
            {
                vec2d pos = { x,y,0 };
                Tile* tile = new Tile;

                tile->mapPosition = { x,y,0 };
                tile->type = "tundra";
                tile->name = "unnamed";
                tile->groupName = "snow";
                tile->continentName = worldName;
                tile->description = "a snowy field";
                tile->isPassible = true;
                tile->color = white;
                tile->continentName = worldName;
                
                // Make grass 1 of 3 different graphics.
                char grassIcon = 0;
                int randomChoice = 0;
                randomChoice = rand() % 2;
                
                switch (randomChoice)
                {
                case 0:
                    grassIcon = 39;
                    break;
                case 1:
                    grassIcon = 44;

                case 2:
                    grassIcon = 46;
                    break;
                }

                tile->graphic = grassIcon;

                tile->absolutePosiiton = getAbsolutePosition(tile->mapPosition);
                iceWorldMap[tile->absolutePosiiton] = tile;
            }
        }
    }
}

static void loadWorld(string type)
{
    // fill the map with tiles.
    string oceanName = generateNpcName();
    for (int y = 0; y < mapSize.y; y++)
    {
        for (int x = 0; x < mapSize.x; x++)
        {
            if (x < 14 || x > mapSize.x - windowSize.x || y < 6 || y > mapSize.y - windowSize.y)
            {
                
                Tile* tile = new Tile;
                tile->mapPosition = { x,y,0 };
                tile->type = "sea";
                tile->graphic = water;
                tile->groupName = oceanName + " Sea";
                tile->continentName = worldName;
                tile->description = "a blue green ocean";
                tile->name = tile->groupName;
                tile->isPassible = false;
                tile->color = blue;
                tile->absolutePosiiton = getAbsolutePosition(tile->mapPosition);
                tile->map = currentMap;
                overWorldMap[tile->absolutePosiiton] = tile;
            }

            else
            {
                vec2d pos = { x,y,0 };
                Tile* tile = new Tile;

                tile->mapPosition = { x,y,0 };
                tile->type = "grassland";
                tile->name = "unnamed";
                tile->groupName = "grassland";
                tile->continentName = worldName;
                tile->description = "a grass area";
                tile->isPassible = true;
                tile->color = green;
                tile->continentName = worldName;
                tile->map = currentMap;
                
                // Make grass 1 of 3 different graphics.
                char grassIcon = 0;
                int randomChoice = 0;
                randomChoice = rand() % 2;
                
                switch (randomChoice)
                {
                case 0:
                    grassIcon = 39;
                    break;
                case 1:
                    grassIcon = 44;

                case 2:
                    grassIcon = 46;
                    break;
                }

                tile->graphic = grassIcon;

                tile->absolutePosiiton = getAbsolutePosition(tile->mapPosition);
                overWorldMap[tile->absolutePosiiton] = tile;
            }
        }
        
    }
    vec2d pos = { 20,10,0 };
    Tile* tile = new Tile;
    tile->continentName = worldName;
    tile->mapPosition = pos;
    tile->name = "none";
    tile->groupName = generateNpcName();
    tile->type = "lava";
    tile->graphic = 35;
    tile->description = "boiling lava pool";
    tile->isPassible = false;
    tile->color = red;
    tile->absolutePosiiton = getAbsolutePosition(tile->mapPosition);
    overWorldMap[tile->absolutePosiiton] = tile; 
    tile->map = currentMap;
    
    // Put in all the special places on the overworld.
    populateOverWorldPlaces();
}

static void createWorld()
{
    // Create tiles from world data 
    // then put them in world tile array.
    for (int y = 0; y < mapSize.y; y++)
    {
        for (int x = 0; x < mapSize.x; x++)
        {
            Tile* tile = new Tile;
            vec2d location = {x,y,0};
            int absolutePosition = getAbsolutePosition(location);
            // take data and call create tile for it, this will fill
            // the tile data in for that type tile.
            // this will keep the size of this function down and focussed, 
            // also making it easier to code each tile type
            
            // if dataArray[absolutePosition] == grassIcon
            //      createGrassTile(Tile* tile) // for grass
            
            
        }  
    }
    
}

static void moveMenu()
{
    playerPosition = addVec2d(windowPosition, playerWindowPosition);
    skipTime = false;
    string choice = "none";
    cout << "______Action_____" << endl;
    cout << " Move  : N E S W " << endl;
    cout << " Peer  : 1 2 3 4 " << endl;
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
        skipTime = true;
    }

    // move for north, all moves are checked the same way.
    if (choice == "n")
    {
        // First check if window can be moved.
        if(moveWindowCheck(NORTH , windowPosition))
        {
            // second see the next space if blocked
            if (movePlayerCheck(NORTH , playerPosition))
            {
                // if move is possible, move the screen and player. 
                windowPosition.y -= 1;
                
            }
        }
        
    }
    
    if (choice == "s")
    {
        
        if(moveWindowCheck(SOUTH , windowPosition))
        {
            if (movePlayerCheck(SOUTH , playerPosition))
            {
                //adjust the actual positions of both
                windowPosition.y += 1;
                
            }
        }
        
    }
    
    if (choice == "e")
    {
        
        if(moveWindowCheck(EAST , windowPosition))
        {
            if (movePlayerCheck(EAST , playerPosition))
            {
                //adjust the actual positions of both
                windowPosition.x += 1;
                
            }
        }
        
    }
    
    if (choice == "w")
    {
        
        if(moveWindowCheck(WEST , windowPosition))
        {
            if (movePlayerCheck(WEST , playerPosition))
            {
                //adjust the actual positions of both
                windowPosition.x -= 1;
                
            }
        }
        
    }

    if (choice == "icons")
    {
        skipTime = true;
    }

    if (choice == "q")
    {
        isRunning = false;
    }

    if (choice == "l")
    {
        //skipTime = true;
        int tempPlayerPos = getAbsolutePosition(playerPosition);
        cout << "Continent: " << currentMap[tempPlayerPos]->continentName << endl;
        cout << "Type: "      << currentMap[tempPlayerPos]->type << endl;
        cout << "Group Name: "<< currentMap[tempPlayerPos]->groupName << endl;
        cout << "Name: "      << currentMap[tempPlayerPos]->name << endl;
        cout << "Desc: "      << currentMap[tempPlayerPos]->description << endl;
        cout << "Turn#: "     << globalturns << endl;
        cout << "Entities on tile:" << endl;
                
        if (currentMap[tempPlayerPos]->entities.empty() == false)
        {
            for (Entity* entity: currentMap[tempPlayerPos]->entities)
            {
                std::cout << "Name: " << entity->firstName << " of " 
                << currentMap[tempPlayerPos]->name << " " << currentMap[tempPlayerPos]->type 
                << std::endl;
            }
        }
       else
         cout << "None." << endl;
        
        
    }
    
    if (choice == "1")
    {
        int tempPos = getAbsolutePosition(playerPosition)-mapSize.x;
        cout << "You look to the north and see..." << endl;
        cout << currentMap[tempPos]->description << " named " << currentMap[tempPos]->name << endl;
    }
    
    if (choice == "2")
    {
        int tempPos = getAbsolutePosition(playerPosition)+1;
        cout << "You look to the east and see..." << endl;
        cout << currentMap[tempPos]->description << " named " << currentMap[tempPos]->name << endl;
    }
    
    if (choice == "3")
    {
        int tempPos = getAbsolutePosition(playerPosition) + mapSize.x;
        cout << "You look to the south and see..." << endl;
        cout << currentMap[tempPos]->description << " named " << currentMap[tempPos]->name << endl;
    }
    
    if (choice == "4")
    {
        int tempPos = getAbsolutePosition(playerPosition) - 1;
        cout << "You look to the west and see..." << endl;
        cout << currentMap[tempPos]->description <<  " named " << currentMap[tempPos]->name << endl;
    }  
    
    if (choice == "0")
    {
        printDebugData = !printDebugData;
    }
    
    if (choice == "8")
    {
        currentMap = overWorldMap;
    }
    
    if (choice == "9")
    {
        currentMap = iceWorldMap;
    }
    
}

static void titleScreen()
{
    system(OS_CLEAR);
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
    cout << " Pick your first name: ";
    //cin.ignore();
    getline(cin, player->firstName);
    cout << "Ok, so your name is " << player->firstName << endl;
    cout << "What is your last name?" << endl;
    //cin.ignore();
    getline(cin, player->lastName);
    cout << "Your full name is " << player->firstName << " " << player->lastName << endl;
    cout << " Y/N : ";
    cin >> choice;
    system(OS_CLEAR);

}

static void cleanUp()
{
    for (auto Tile : overWorldMap)
    {
       //delete Tile;
       //delete [] overWorldMap;
    }
}

int main()
{
    currentMap = overWorldMap;
    
    // Randomize the randomizer
    randomSeed = static_cast<unsigned int>(time(nullptr));
    srand(seed);
    
    // Generate importanat names.
    worldName = generateNpcName();
    
    // load some generated test map tiles.
    loadWorld("test");
    
    //ice world test map, for testing map switch.
    loadIceWorld();
    
    //Point to the overworld to start
    
    makeTestEntities(5,16,16);
    
    // Load in some specific tiles.
    //populatePlaces();

    // Diplay title screen.
    //titleScreen();

    // Player creation.
    //playerCreation();

    while (isRunning)
    {
        //system(OS_CLEAR);
        drawScreen();
        moveMenu();
        
        // increment the global turn counter.
        if (skipTime == false){ globalturns++;}
        
    }

    cleanUp();

    return 0;
}