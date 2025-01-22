#include <iostream>
#include <cstdlib>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

// Initializing Dimensions.
// resolutionX and resolutionY determine the rendering resolution.
// Don't edit unless required. Use functions on lines 43, 44, 45 for resizing the game window.
const int resolutionX = 960;
const int resolutionY = 960;
const int boxPixelsX = 32;
const int boxPixelsY = 32;
const int gameRows = resolutionX / boxPixelsX;	  // Total rows on grid = 30
const int gameColumns = resolutionY / boxPixelsY; // Total columns on grid = 30

// 1 gamerow = 32 resolutions
// nth gamerow = position / boxPixels

// n gamerows = (32 * n) resolutions
// n resloutions = (n * 1.0 / 32) rows

// Initializing GameGrid.
int gameGrid[gameRows][gameColumns] = {};

/*
Game Grid Existence Numbers:
0: Empty space
1: Half Mushroom
2: Full Mushroom
3: Player
4: Bullet
5: Centipede Head
6: Centipede Body
*/

// The following exist purely for readability.
const int x = 0;
const int y = 1;
const int exists = 2;
const int poison = 3;		// Whether a mushroom is poisonous or not

// Number of centipede segments
int centipede_length = 12;

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// Write your functions declarations here. Some have been written for you. //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

void cleararrays(float player[], float bullet[], int mushrooms[][4], int& minmushrooms, const int indexnum, const int numvariable, float**& centipede, bool domushroom = 0, bool docentipede = 1, bool doplayer = 0);
void initializePCM(float player[], float bullet[], int mushrooms[][4], int& minmushrooms, int& nummushrooms, int& totalmushrooms, const int indexnum, const int numvariable, const int head, float** centipede, Sprite& mushroomSprite1, Sprite& mushroomSprite2, Sprite& poisonousmushroomSprite1, Sprite& poisonousmushroomSprite2, bool domushroom = 0, bool docentipede = 1, bool doplayer = 0);
void drawPlayer(sf::RenderWindow &window, float player[], sf::Sprite &playerSprite);	//Spawns player sprite at current location 
void moveBullet(float bullet[], float player[y], sf::Clock& bulletClock, float bullet_speed);	//Moves bullet along the y-axis (Decreases y-axis by a specific value with a specififc time)
void drawBullet(sf::RenderWindow &window, float bullet[], sf::Sprite &bulletSprite);

//---'							'---//
//-----------My functions-----------//
//									//
//									//
//									//
//----------------------------------//
void moveplayer(float player[], int mushrooms[][4], int totalmushrooms, float playerspeed, bool& deductlife);	// This function will move player inside the player area
void shootbullet(float bullet[], float player[], Sound &fire_sound);	// This function updates the position where bullet is to be shooten and shoots the bullets on pressing the key
void drawMushroom2(RenderWindow &window, int mushrooms[][4], int n, Sprite &mushroomSprite2); // Spawns a full mushroom (This will set sprite position and draw it)
void drawMushroom1(RenderWindow &window, int mushrooms[][4], int n, Sprite &mushroomSprite1); // Spawns a half mushroom	(This will set sprite position and draw it)
void drawPoisonousMushroom2(RenderWindow &window, int mushrooms[][4], int n, Sprite &poisonousmushroomSprite2); // Spawns a full poisonous mushroom (This will set sprite position and draw it)
void drawPoisonousMushroom1(RenderWindow &window, int mushrooms[][4], int n, Sprite &poisonousmushroomSprite1); // Spawns a half poisonous mushroom	(This will set sprite position and draw it)
void destroyMushroom(float bullet[], int mushrooms[][4], int& nummushrooms, int& totalmushrooms, int& score, Sound& destroy_sound);	//Destroys a mushroom by halfing it and then finishing it (This will detect mushroom detection with bullet and then update mushroom existence condition accordingly)
void drawCentipede(RenderWindow &window, float** centipede, const int head, Sprite &centipedeleftSprite, Sprite& centipederightSprite, Sprite &centipedeheadleftSprite, Sprite & centipedeheadrightSprite, int*& Up, int*& Down, int*& Left, int*& Right);	//Spawns centipede head and body (Checks whether centipede segment is head or body and prints the sprite accordingly)
void movecentipede(RenderWindow& window, Sprite& centipedeleftSprite, Sprite& centipederightSprite, Sprite& centipedeheadleftSprite, Sprite& centipedeheadrightSprite, float** centipede, float player[], int mushrooms[][4], const int head, int totalmushrooms, bool playercollide, float& centipede_speed, int*& Up, int*& Down, int*& Left, int*& Right, bool& deductlife);	//Moves centipede (Moves centipede while checking wall, mushroom, player collision)
void destroycentipede(float bullet[], float** centipede, const int head,  int mushrooms[][4], int &nummushrooms, int &totalmushrooms, int &score, Sound& kill_sound);	//Destroys a centipede segment and spawns a mushroom (This will detect mushroom-bullet detection, destroys centipede part, spawn mushroom and increases score)
void lives_draw(RenderWindow& window, int lives, Text lives_text, Sprite& heartSprite);

int main()
{
	srand(time(0));
	string name;	// Max Length = 16
	int maxnamelen = 16,
		score = 0,	// Player score
		lives = 3,	// Player lives
		level = 1,	// Player level
		minmushrooms = 20,	// Minimum initial mushrooms
		windows = 1,	//For switching between different windows
		extrawindow = 270,		//Increasing it will increase the black space
		extrasetwindow = 177,	//Increasing it will increase the size of whole window
		menu_pt = 1,
		setting_pt = 1,
		pause_pt = 1,
		sound_vol = 30,	//Sound volume
		music_vol = 25,	//Music volume
		recgap,
		selectcol = 0x2b,	//Colors
		defcol = 0xe0;
	float centipede_speed = 0.4, playerspeed = 1, bullet_speed = 4;	//Speed of centipede (+- x-axis)
	bool playercollide = 0,
		game_over = 0,
		game_won = 0,
		sound_bool = 1,
		music_bool = 1,
		isMainmenu = 1,
		deductlife = 0,
		exit_pt = 0;

	int* Up = new int[centipede_length], * Down = new int[centipede_length], * Left = new int[centipede_length], * Right = new int[centipede_length];

	/*
	Windows:

	Window 1: Home screen
	Window 2: Menu
	Window 3: Settings
	Window 4: Instructions
	Window 5: Controls
	Window 6: High scores
	Window 7: Game
	Window 8: Pause screen
	Window 9: End screen
	Window 10: Exit Ask screen
	*/

	// Declaring RenderWindow.
	sf::RenderWindow window(sf::VideoMode(resolutionX + extrawindow, resolutionY), "Centipede", sf::Style::Close | sf::Style::Titlebar);

	// Used to resize your window if it's too big or too small. Use according to your needs.
	window.setSize(sf::Vector2u(640 + extrasetwindow, 640)); // Recommended for 1366x768 (768p) displays.
	// window.setSize(sf::Vector2u(1280, 1280)); // Recommended for 2560x1440 (1440p) displays.
	// window.setSize(sf::Vector2u(1920, 1920)); // Recommended for 3840x2160 (4k) displays.

	// Used to position your window on every launch. Use according to your needs.
	window.setPosition(sf::Vector2i(70, 0));

	//------------------Initializing TEXTS------------------//
	//														//
	//														//

	//Initializing Text Font.
	Font font;
	font.loadFromFile("Text/Early_GameBoy.ttf");

	//Initializing small text.
	Text small_text;
	small_text.setFont(font);
	small_text.setCharacterSize(22);
	small_text.setFillColor(Color(0xe0, 0xe0, 0xe0));

	//Initailizing score text.
	Text score_text = small_text;
	score_text.setPosition(980, 80);

	//Initializing lives text.
	Text lives_text = small_text;
	lives_text.setString("Lives: ");
	lives_text.setPosition(980, score_text.getPosition().y + 40);

	//Initializing levels text.
	Text levels_text = small_text;
	levels_text.setPosition(980, lives_text.getPosition().y + 40);

	//Initializing text lines.
	Text line1 = small_text;
	line1.setPosition(980, levels_text.getPosition().y + 90);
	line1.setString("Press \'Q\'");

	Text line2 = small_text;
	line2.setPosition(980, line1.getPosition().y + 30);
	line2.setString("or \'Esc\'");
	
	Text line3 = small_text;
	line3.setPosition(980, line2.getPosition().y + 30);
	line3.setString("to Exit.");

	Text line4 = small_text;
	line4.setPosition(980, line3.getPosition().y + 60);
	line4.setString("Press \'P\' to");
	
	Text line5 = small_text;
	line5.setPosition(980, line4.getPosition().y + 30);
	line5.setString("Pause.");

	//Text line6 = small_text;
	//line6.setPosition(980, line5.getPosition().y + 30);
	//line6.setString("\'Esc\' to ");



	//Initializing start game text lines.
	int st_posx = 143,
		st_posy = 610;
	Text entername_text = small_text;
	entername_text.setPosition(st_posx, st_posy);
	entername_text.setCharacterSize(30);
	entername_text.setString("Enter your name: ");

	// Box for name input
	int recspace = 25;
	recgap = 3;
	RectangleShape strectangle1(Vector2f(990, entername_text.getCharacterSize() + recspace * 2));
	strectangle1.setFillColor(Color(0, 0, 0, 255 * 0.100));
	strectangle1.setOutlineColor(Color::Black);
	strectangle1.setOutlineThickness(10.0f);
	strectangle1.setPosition(st_posx - recspace, st_posy - recspace);

	RectangleShape strectangle2(Vector2f(strectangle1.getSize().x - recgap * 2, strectangle1.getSize().y - recgap * 2));
	strectangle2.setFillColor(Color(0, 0, 0, 255 * 0.100));
	strectangle2.setOutlineColor(Color(defcol + 16, defcol + 16, defcol + 16));
	strectangle2.setOutlineThickness(10.0f - 5);
	strectangle2.setPosition(strectangle1.getPosition().x + recgap, strectangle1.getPosition().y + recgap);

	Text stnamestr = entername_text;
	stnamestr.setPosition(entername_text.getPosition().x + (entername_text.getCharacterSize() * 15), entername_text.getPosition().y);
	stnamestr.setString("");

	

	//Initializing BIG text.
	Text big_text;
	big_text.setFont(font);
	big_text.setCharacterSize(100);
	big_text.setFillColor(Color::White);

	int text_pos = 6;
	//Initializing start game text 1.
	Text startgame_text_1 = big_text;
	startgame_text_1.setString("CENTIPEDE");
	startgame_text_1.setCharacterSize(125);
	startgame_text_1.setFillColor(Color(0xe0, 0xe0, 0xe0));
	startgame_text_1.setPosition(65 + text_pos, 370 + text_pos);

	//Initializing start game text 2.
	Text startgame_text_2 = big_text;
	startgame_text_2.setString("CENTIPEDE");
	startgame_text_2.setCharacterSize(125);
	startgame_text_2.setFillColor(Color(0x00, 0x00, 0x00));
	startgame_text_2.setPosition(65, 370);

	//Initializing game over text.
	Text gameover_text = big_text;
	gameover_text.setString("GAME OVER!");
	gameover_text.setPosition(159, 50);
	gameover_text.setOutlineColor(Color::Black);
	gameover_text.setOutlineThickness(5.7f);

	//Initializing game win text.
	Text gamewon_text = gameover_text;
	gamewon_text.setString("YOU WON!");
	gamewon_text.setPosition(259, 50);

	//											//
	//--------------- Menu Texts ---------------//
	//											//
	//											//
	//////////////////////////////////////////////

	Text menu_text;
	menu_text.setFont(font);
	menu_text.setCharacterSize(50);
	menu_text.setFillColor(Color(defcol, defcol, defcol));

	Text menu_text1 = menu_text;
	//menu_text1.setFillColor(Color(defcol, defcol, defcol));

	Text menu_text2 = menu_text;
	menu_text2.setFillColor(Color(selectcol, selectcol, selectcol));

	int m_posy = 300,
		gap = 115,
		m_text_pos = 3;

	//----- Menu -----
	int menu_posx = 462,
		menu_posy = 95,
		heading_pos = 4;
	Text menu1 = menu_text1;
	menu1.setString("Menu");
	menu1.setCharacterSize(80);
	menu1.setPosition(menu_posx + heading_pos, menu_posy + heading_pos);		//Heading_pos is for the 3D style of text
	Text menu2 = menu_text2;
	menu2.setString("Menu");
	menu2.setFillColor(Color(0x00, 0x00, 0x00));
	menu2.setCharacterSize(80);
	menu2.setPosition(menu_posx, menu_posy);

	//----- Start -----
	int start_posx = 432,		//494,	//for "Start"
		start_posy = m_posy + gap * 0;
	Text start = menu_text;
	start.setString("New Game");
	start.setPosition(start_posx, start_posy);
	Text start1 = menu_text1, start2 = menu_text2;
	start1.setString("New Game");
	start2.setString("New Game");
	start1.setPosition(start_posx + m_text_pos, start_posy + m_text_pos);
	start2.setPosition(start_posx, start_posy);

	//----- Settings -----
	int set_posx = 419,
		set_posy = m_posy + gap * 1;
	Text settings = menu_text;
	settings.setString("Settings");
	settings.setPosition(set_posx, set_posy);
	Text settings1 = menu_text1, settings2 = menu_text2;
	settings1.setString("Settings");
	settings2.setString("Settings");
	settings1.setPosition(set_posx + m_text_pos, set_posy + m_text_pos);
	settings2.setPosition(set_posx, set_posy);

	//----- Instructions -----
	int instruc_posx = 319,
		instruc_posy = m_posy + gap * 2;
	Text instructions = menu_text;
	instructions.setString("Instructions");
	instructions.setPosition(instruc_posx, instruc_posy);
	Text instructions1 = menu_text1;
	Text instructions2 = menu_text2;
	instructions1.setString("Instructions");
	instructions2.setString("Instructions");
	instructions1.setPosition(instruc_posx + m_text_pos, instruc_posy + m_text_pos);
	instructions2.setPosition(instruc_posx, instruc_posy);

	//----- Controls -----
	int ctrls_posx = 419,
		ctrls_posy = m_posy + gap * 3;
	Text controls = menu_text;
	controls.setString("Controls");
	controls.setPosition(ctrls_posx, ctrls_posy);
	Text controls1 = menu_text1;
	Text controls2 = menu_text2;
	controls1.setString("Controls");
	controls2.setString("Controls");
	controls1.setPosition(ctrls_posx + m_text_pos, ctrls_posy + m_text_pos);
	controls2.setPosition(ctrls_posx, ctrls_posy);

	//----- High scores -----
	int hsc_posx = 358,
		hsc_posy = m_posy + gap * 4;
	Text high_scores = menu_text;
	high_scores.setString("High Scores");
	high_scores.setPosition(hsc_posx, hsc_posy);
	Text high_scores1 = menu_text1;
	Text high_scores2 = menu_text2;
	high_scores1.setString("High Scores");
	high_scores2.setString("High Scores");
	high_scores1.setPosition(hsc_posx + m_text_pos, hsc_posy + m_text_pos);
	high_scores2.setPosition(hsc_posx, hsc_posy);



	//												 //
	//---------------- Settings Texts ---------------//
	//												 //
	//												 //
	///////////////////////////////////////////////////

	int settings_head_posx = 302,
		settings_head_posy = 50;
	Text Settings1 = menu1;
	Text Settings2 = menu2;
	Settings1.setString("Settings");
	Settings2.setString("Settings");
	Settings1.setPosition(settings_head_posx + heading_pos, settings_head_posy + heading_pos);
	Settings2.setPosition(settings_head_posx, settings_head_posy);

	Text settings_text = menu_text, settings_text1 = menu_text1, settings_text2 = menu_text2;
	settings_text.setCharacterSize(35);
	settings_text1.setCharacterSize(35);
	settings_text2.setCharacterSize(35);
	Text statetxt = settings_text;
	statetxt.setString("State: ");
	Text volumetxt = settings_text;
	volumetxt.setString("Volume: ");

	int offcolor = 0x70,
		offcolor2 = 0x90;
	Text ontxt = settings_text;
	ontxt.setString("ON");
	ontxt.setOutlineColor(Color::Green);
	ontxt.setOutlineThickness(1.0f);
	Text offtxt = settings_text;
	offtxt.setString("OFF");
	offtxt.setOutlineColor(Color(offcolor, offcolor, offcolor));
	offtxt.setOutlineThickness(1.0f);
	offtxt.setFillColor(Color(offcolor, offcolor, offcolor));

	int settposx = 250,
		settposy = 290,
		settgap = 25,
		ongap = 220,
		offgap = 110;
	float sett3Dgap = 2;
	
	Text backtxt = settings_text;
	backtxt.setString("Back");
	backtxt.setPosition(60, 860);
	Text backtxt1 = settings_text1;
	backtxt1.setString("Back");
	backtxt1.setPosition(60 + sett3Dgap, 860 + sett3Dgap);
	Text backtxt2 = settings_text2;
	backtxt2.setString("Back");
	backtxt2.setPosition(60, 860);

	Text pressentertxt = settings_text;
	pressentertxt.setString("Press Enter to continue...");
	pressentertxt.setPosition(60, 860);
	Text pressentertxt1 = settings_text1;
	pressentertxt1.setString("Press Enter to continue...");
	pressentertxt1.setPosition(60 + sett3Dgap, 860 + sett3Dgap);
	Text pressentertxt2 = settings_text2;
	pressentertxt2.setString("Press Enter to continue...");
	pressentertxt2.setPosition(60, 860);

	// Sounds texts
	Text settings_soundstxt = menu_text;
	settings_soundstxt.setString("Sound");
	settings_soundstxt.setPosition(settposx, settposy);

	Text statetxt1=statetxt, statetxt1_1 = statetxt1, statetxt1_2 = settings_text2;
	statetxt1_2.setString("State: ");
	statetxt1.setPosition(settposx + 50, settposy + menu_text.getCharacterSize() + settgap);
	statetxt1_1.setPosition(settposx + 50 + sett3Dgap, settposy + menu_text.getCharacterSize() + settgap + sett3Dgap);
	statetxt1_2.setPosition(settposx + 50, settposy + menu_text.getCharacterSize() + settgap);

	Text volumetxt1 = volumetxt, volumetxt1_1 = volumetxt1, volumetxt1_2 = settings_text2, volumenum1 = settings_text;
	volumetxt1_2.setString("Volume: ");
	volumetxt1.setPosition(settposx + 50, statetxt1.getPosition().y + settings_text.getCharacterSize() + settgap);
	volumetxt1_1.setPosition(settposx + 50 + sett3Dgap, statetxt1.getPosition().y + settings_text.getCharacterSize() + settgap + sett3Dgap);
	volumetxt1_2.setPosition(settposx + 50, statetxt1.getPosition().y + settings_text.getCharacterSize() + settgap);
	volumenum1.setString(to_string(sound_vol));
	volumenum1.setPosition(volumetxt1.getPosition().x + 300, volumetxt1.getPosition().y);

	Text ontxt1_1 = ontxt;
	ontxt1_1.setPosition(settposx + 50 + ongap, statetxt1.getPosition().y);
	Text offtxt1_1 = offtxt;
	offtxt1_1.setPosition(ontxt1_1.getPosition().x + offgap, statetxt1.getPosition().y);
	
	// Music texts
	settposy = volumetxt1.getPosition().y + 120;
	Text settings_musictxt = menu_text;
	settings_musictxt.setString("Music");
	settings_musictxt.setPosition(settposx, settposy);
	
	Text statetxt2 = statetxt, statetxt2_1 = statetxt1, statetxt2_2 = settings_text2;
	statetxt2_2.setString("State: ");
	statetxt2.setPosition(settposx + 50, settposy + menu_text.getCharacterSize() + settgap);
	statetxt2_1.setPosition(settposx + 50 + sett3Dgap, settposy + menu_text.getCharacterSize() + settgap + sett3Dgap);
	statetxt2_2.setPosition(settposx + 50, settposy + menu_text.getCharacterSize() + settgap);

	Text volumetxt2 = volumetxt, volumetxt2_1 = volumetxt2, volumetxt2_2 = settings_text2, volumenum2 = settings_text;
	volumetxt2_2.setString("Volume: ");
	volumetxt2.setPosition(settposx + 50, statetxt2.getPosition().y + settings_text.getCharacterSize() + settgap);
	volumetxt2_1.setPosition(settposx + 50 + sett3Dgap, statetxt2.getPosition().y + settings_text.getCharacterSize() + settgap + sett3Dgap);
	volumetxt2_2.setPosition(settposx + 50, statetxt2.getPosition().y + settings_text.getCharacterSize() + settgap);
	volumenum2.setString(to_string(music_vol));
	volumenum2.setPosition(volumetxt2.getPosition().x + 300, volumetxt2.getPosition().y);

	Text ontxt2_1 = ontxt;
	ontxt2_1.setPosition(settposx + 50 + ongap, statetxt2.getPosition().y);
	Text offtxt2_1 = offtxt;
	offtxt2_1.setPosition(ontxt2_1.getPosition().x + offgap, statetxt2.getPosition().y);

	//													//
	//---------------- Instruction Texts ---------------//
	//													//
	//													//
	//////////////////////////////////////////////////////

	int Instruc_posx = 141,
		Instruc_posy = 50;
	Text Instructions1 = menu1;
	Text Instructions2 = menu2;
	Instructions1.setString("Instructions");
	Instructions2.setString("Instructions");
	Instructions1.setPosition(Instruc_posx + heading_pos, Instruc_posy + heading_pos);
	Instructions2.setPosition(Instruc_posx, Instruc_posy);

	int i_posx = 45,
		i_posy = 200,
		i_gap = 55,
		i_size = 35;
	Text i_para = menu_text;
	i_para.setCharacterSize(i_size);

	Text i_l1 = i_para;
	i_l1.setString("Centipede is a classic arcade game");
	i_l1.setPosition(i_posx, i_posy + i_gap * 0);
	Text i_l2 = i_para;
	i_l2.setString("where your mission is to destroy the");
	i_l2.setPosition(i_posx, i_posy + i_gap * 1);
	Text i_l3 = i_para;
	i_l3.setString("descending centipede. Use the");
	i_l3.setPosition(i_posx, i_posy + i_gap * 2);
	Text i_l4 = i_para;
	i_l4.setString("keyboard to navigate and shoot to");
	i_l4.setPosition(i_posx, i_posy + i_gap * 3);
	Text i_l5 = i_para;
	i_l5.setString("break it into smaller segments.");
	i_l5.setPosition(i_posx, i_posy + i_gap * 4);
	Text i_l6 = i_para;
	i_l6.setString("Watch out for spiders, fleas, and");
	i_l6.setPosition(i_posx, i_posy + i_gap * 5);
	Text i_l7 = i_para;
	i_l7.setString("scorpions. The game gets");
	i_l7.setPosition(i_posx, i_posy + i_gap * 6);
	Text i_l8 = i_para;
	i_l8.setString("progressively challenging with new");
	i_l8.setPosition(i_posx, i_posy + i_gap * 7);
	Text i_l9 = i_para;
	i_l9.setString("enemies. Master precision shooting");
	i_l9.setPosition(i_posx, i_posy + i_gap * 8);
	Text i_l10 = i_para;
	i_l10.setString("and agile dodging for the highest");
	i_l10.setPosition(i_posx, i_posy + i_gap * 9);
	Text i_l11 = i_para;
	i_l11.setString("score in this timeless classic.");
	i_l11.setPosition(i_posx, i_posy + i_gap * 10);
	float aa = 1.3;
	Text i_l12 = i_para;
	i_l12.setString("Press Enter to continue...");
	i_l12.setPosition(i_posx - aa, i_posy + i_gap * 12 + 5 - aa);
	Text i_l13 = i_para;
	i_l13.setString("Press Enter to continue...");
	i_l13.setPosition(i_posx, i_posy + i_gap * 12 + 5);
	//i_l13.setOutlineColor(Color::Black);
	//i_l13.setOutlineThickness(2.0f);

	//													//
	//----------------- Controls Texts -----------------//
	//													//
	//													//
	//////////////////////////////////////////////////////

	int control_posx = 302,
		control_posy = 50;
	//Initializing Control window texts
	Text Controls1 = menu1;
	Text Controls2 = menu2;
	Controls1.setString("Controls");
	Controls2.setString("Controls");
	Controls1.setPosition(control_posx + heading_pos, control_posy + heading_pos);
	Controls2.setPosition(control_posx, control_posy);

	//Initializing controls image
	sf::Texture keyboard_texture1;
	sf::Sprite keyboard_sprite1;
	keyboard_texture1.loadFromFile("Textures/keyboard.png");
	keyboard_sprite1.setTexture(keyboard_texture1);
	keyboard_sprite1.setColor(Color(255, 255, 255, 255 * 0.75));
	keyboard_sprite1.setPosition(30, 250);
	keyboard_sprite1.setScale(0.63, 0.63);

	sf::Texture keyboard_texture2;
	sf::Sprite keyboard_sprite2;
	keyboard_texture2.loadFromFile("Textures/keyboard1.png");
	keyboard_sprite2.setTexture(keyboard_texture2);
	keyboard_sprite2.setColor(Color(255, 255, 255, 255 * 0.98));
	keyboard_sprite2.setPosition(30, 250);
	keyboard_sprite2.setScale(0.63, 0.63);

	Text controlsdef = i_l12;
	controlsdef.setCharacterSize(30);
	controlsdef.setPosition(45, 885);

	//													  //
	//----------------- Highscores Texts -----------------//
	//													  //
	//													  //
	////////////////////////////////////////////////////////

	int hscore_posx = 200,
		hscore_posy = 50;
	//Initializing Control window texts
	Text hscore1 = menu1;
	Text hscore2 = menu2;
	hscore1.setString("High Scores");
	hscore2.setString("High Scores");
	hscore1.setPosition(hscore_posx + heading_pos, hscore_posy + heading_pos);
	hscore2.setPosition(hscore_posx, hscore_posy);




	//												 //
	//----------------- Pause Texts -----------------//
	//												 //
	//												 //
	///////////////////////////////////////////////////

	int pause_posx = 200,
		pause_posy = 50;
	//Initializing Control window texts
	Text pause1 = menu1;
	Text pause2 = menu2;
	pause1.setString("Game Paused");
	pause2.setString("Game Paused");
	pause1.setPosition(pause_posx + heading_pos, pause_posy + heading_pos);
	pause2.setPosition(pause_posx, pause_posy);

	int p_posy = 200;

	//----- Resume -----
	int resume_posx = 468,
		resume_posy = p_posy + gap * 0;
	Text resume = menu_text;
	resume.setString("Resume");		//"Start"
	resume.setPosition(resume_posx, resume_posy);
	Text resume1 = menu_text1, resume2 = menu_text2;
	resume1.setString("Resume");
	resume2.setString("Resume");
	resume1.setPosition(resume_posx + m_text_pos, resume_posy + m_text_pos);
	resume2.setPosition(resume_posx, resume_posy);

	//----- P Settings -----
	set_posx = 419;
	set_posy = p_posy + gap * 1;
	Text psettings = settings,
		psettings1 = settings1, 
		psettings2 = settings2;
	psettings.setPosition(set_posx, set_posy);
	psettings1.setPosition(set_posx + m_text_pos, set_posy + m_text_pos);
	psettings2.setPosition(set_posx, set_posy);

	//----- P Controls -----
	ctrls_posx = 419;
	ctrls_posy = p_posy + gap * 2;
	Text pcontrols = controls,
		pcontrols1 = controls1,
		pcontrols2 = controls2;
	pcontrols.setPosition(ctrls_posx, ctrls_posy);
	pcontrols1.setPosition(ctrls_posx + m_text_pos, ctrls_posy + m_text_pos);
	pcontrols2.setPosition(ctrls_posx, ctrls_posy);

	//----- P High scores -----
	hsc_posx = 358;
	hsc_posy = p_posy + gap * 3;
	Text phigh_scores = high_scores,
		phigh_scores1 = high_scores1,
		phigh_scores2 = high_scores2;
	phigh_scores.setPosition(hsc_posx, hsc_posy);
	phigh_scores1.setPosition(hsc_posx + m_text_pos, hsc_posy + m_text_pos);
	phigh_scores2.setPosition(hsc_posx, hsc_posy);

	//----- Go to Main Menu -----
	int gotomm_posx = 286,
		gotomm_posy = p_posy + gap * 4;
	Text tomainmenu = menu_text;
	tomainmenu.setString("Go to Main Menu");
	tomainmenu.setPosition(gotomm_posx, gotomm_posy);
	Text tomainmenu1 = menu_text1;
	Text tomainmenu2 = menu_text2;
	tomainmenu1.setString("Go to Main Menu");
	tomainmenu2.setString("Go to Main Menu");
	tomainmenu1.setPosition(gotomm_posx + m_text_pos, gotomm_posy + m_text_pos);
	tomainmenu2.setPosition(gotomm_posx, gotomm_posy);

	//----- Exit Game -----
	int exitg_posx = 410,
		exitg_posy = p_posy + gap * 5;
	Text exit_game = menu_text;
	exit_game.setString("Exit Game");
	exit_game.setPosition(exitg_posx, exitg_posy);
	Text exit_game1 = menu_text1;
	Text exit_game2 = menu_text2;
	exit_game1.setString("Exit Game");
	exit_game2.setString("Exit Game");
	exit_game1.setPosition(exitg_posx + m_text_pos, exitg_posy + m_text_pos);
	exit_game2.setPosition(exitg_posx, exitg_posy);


	////////////////////////////////////////////////////////
	//													  //
	//------------      End Screen Texts      ------------//
	//													  //
	//													  //
	////////////////////////////////////////////////////////

	int pstats_posx = 240,
		pstats_posy = 280;

	//PStats Heading
	Text pstats1 = menu1;
	Text pstats2 = menu2;
	pstats1.setCharacterSize(65);
	pstats2.setCharacterSize(65);
	pstats1.setString("Player Stats");
	pstats2.setString("Player Stats");
	pstats1.setPosition(pstats_posx + heading_pos, pstats_posy + heading_pos);
	pstats2.setPosition(pstats_posx, pstats_posy);

	//PStats Sub texts
	int pstat_posx = 150,
		pstat_posy = pstats_posy + 120,
		psgap = 70;
	
	Text ps_text = menu_text;
	ps_text.setCharacterSize(33);
	Text psname = ps_text;
	psname.setString("Player name: ");
	psname.setPosition(pstat_posx, pstat_posy + psgap * 0);

	Text psscore = ps_text;
	psscore.setString("Score: ");
	psscore.setPosition(pstat_posx, pstat_posy + psgap * 1);

	Text pslevel = ps_text;
	pslevel.setString("Level no: ");
	pslevel.setPosition(pstat_posx, pstat_posy + psgap * 2);

	Text pslives = ps_text;
	pslives.setString("Lives left: ");
	pslives.setPosition(pstat_posx, pstat_posy + psgap * 3);

	Text namestr = ps_text;
	namestr.setPosition(psname.getPosition().x + (psname.getCharacterSize() * 12), psname.getPosition().y);

	Text levelstr = ps_text;
	levelstr.setPosition(pslevel.getPosition().x + (pslevel.getCharacterSize() * 9), pslevel.getPosition().y);

	recgap = 3;
	RectangleShape psrectangle1(Vector2f(1000, 450));
	psrectangle1.setFillColor(Color(200, 200, 200, 255 * 0.100));
	psrectangle1.setOutlineColor(Color::Black);
	psrectangle1.setOutlineThickness(10.0f);
	psrectangle1.setPosition(pstat_posx - 40, pstats_posy - 40);
	
	RectangleShape psrectangle2(Vector2f(psrectangle1.getSize().x - recgap * 2, psrectangle1.getSize().y - recgap * 2));
	psrectangle2.setFillColor(Color(200, 200, 200, 255 * 0.100));
	psrectangle2.setOutlineColor(Color(defcol + 16, defcol + 16, defcol + 16));
	psrectangle2.setOutlineThickness(10.0f - 5);
	psrectangle2.setPosition(psrectangle1.getPosition().x + recgap, psrectangle1.getPosition().y + recgap);


	/////////////////////////////////////////////////////////
	//													   //
	//------------      Exit Screen Texts      ------------//
	//													   //
	//													   //
	/////////////////////////////////////////////////////////

	int exit_posx = 76,
		exit_posy = 340;

	//Exit Heading
	Text exit1 = menu1;
	Text exit2 = menu2;
	exit1.setCharacterSize(41);
	exit2.setCharacterSize(41);
	exit1.setString("Do you want to exit the game?");
	exit2.setString("Do you want to exit the game?");
	exit1.setPosition(exit_posx + heading_pos, exit_posy + heading_pos);
	exit2.setPosition(exit_posx, exit_posy);

	//Exit Sub texts
	int exitgap = 445,
		exit_posygap = 95;

	Text exit_text = menu_text;
	exit_text.setCharacterSize(33);
	exit_text.setOutlineThickness(1.0f);
	Text yestxt = exit_text;
	yestxt.setString("Yes");
	yestxt.setOutlineColor(Color(offcolor2, offcolor2, offcolor2));
	yestxt.setFillColor(Color(offcolor2, offcolor2, offcolor2));
	yestxt.setPosition(exit_posx + 280, exit_posy + exit_posygap);

	Text notxt = exit_text;
	notxt.setString("No");
	notxt.setOutlineColor(Color::Red);
	notxt.setPosition(yestxt.getPosition().x + exitgap, exit_posy + exit_posygap);

	recgap = 3;
	recspace = 25;
	RectangleShape exitrectangle1(Vector2f(1120, 195));
	exitrectangle1.setFillColor(Color(100, 100, 100, 255 * 0.10));
	exitrectangle1.setOutlineColor(Color::Black);
	exitrectangle1.setOutlineThickness(10.0f);
	exitrectangle1.setPosition(exit_posx - recspace, exit_posy - recspace);

	RectangleShape exitrectangle2(Vector2f(exitrectangle1.getSize().x - recgap * 2, exitrectangle1.getSize().y - recgap * 2));
	exitrectangle2.setFillColor(Color(200, 200, 200, 255 * 0.100));
	exitrectangle2.setOutlineColor(Color(defcol + 16, defcol + 16, defcol + 16));
	exitrectangle2.setOutlineThickness(10.0f - 5);
	exitrectangle2.setPosition(exitrectangle1.getPosition().x + recgap, exitrectangle1.getPosition().y + recgap);

	


	///////////////////////////////////////////////////////////////
	//															 //
	//------------------- Initializing MUSICs -------------------//
	//															 //
	//															 //
	///////////////////////////////////////////////////////////////

	// Initializing Background Music 1.
	sf::Music bgMusic1;
	bgMusic1.openFromFile("Music/Head_Empty.ogg");
	bgMusic1.setLoop(true);
	bgMusic1.play();
	bgMusic1.setVolume(music_vol);

	// Initializing Background Music 2.
	sf::Music bgMusic2;
	bgMusic2.openFromFile("Music/Retro_Post_Punk.ogg");
	bgMusic2.setLoop(true);
	bgMusic2.setVolume(music_vol);



	///////////////////////////////////////////////////////////////
	//															 //
	//------------------- Initializing SOUNDs -------------------//
	//															 //
	//															 //
	///////////////////////////////////////////////////////////////

	//Initializing player dying sound.
	SoundBuffer die_buffer;
	die_buffer.loadFromFile("Sound_Effects/death2.wav");
	Sound playerdie_sound;
	playerdie_sound.setBuffer(die_buffer);
	playerdie_sound.setVolume(70);

	//Initializing killing sound.
	SoundBuffer kill_buffer;
	kill_buffer.loadFromFile("Sound_Effects/kill2.wav");
	Sound kill_sound;
	kill_sound.setBuffer(kill_buffer);
	kill_sound.setVolume(70);

	//Initializing destroying sound.
	SoundBuffer destroy_buffer;
	destroy_buffer.loadFromFile("Sound_Effects/destroy2.wav");
	Sound destroy_sound;
	destroy_sound.setBuffer(destroy_buffer);
	destroy_sound.setVolume(70);

	//Initializing celebration sound.
	SoundBuffer celebrate_buffer;
	celebrate_buffer.loadFromFile("Sound_Effects/celebrate4.wav");
	Sound celebrate_sound;
	celebrate_sound.setBuffer(celebrate_buffer);
	celebrate_sound.setVolume(sound_vol);

	// Initializing Fire Sound.
	SoundBuffer fire_buffer;
	fire_buffer.loadFromFile("Sound_Effects/fire3.wav");
	Sound fire_sound;
	fire_sound.setBuffer(fire_buffer);
	fire_sound.setVolume(sound_vol);

	// Button click sound
	SoundBuffer click_buffer;
	click_buffer.loadFromFile("Sound_Effects/click_sound.wav");
	Sound click_sound;
	click_sound.setBuffer(click_buffer);
	click_sound.setVolume(sound_vol);

	// Button hover sound
	SoundBuffer hover_buffer;
	hover_buffer.loadFromFile("Sound_Effects/button_hover.wav");
	Sound hover_sound;
	hover_sound.setBuffer(hover_buffer);
	hover_sound.setVolume(sound_vol);


	////////////////////////////////////////////////////////////////////
	//																  //
	//------------------- Initializing BACKGROUNDs -------------------//
	//																  //
	//																  //
	////////////////////////////////////////////////////////////////////

	// Initializing Background Home Screen.
	sf::Texture background_homeTexture;
	sf::Sprite background_homeSprite;
	background_homeTexture.loadFromFile("Textures/grass.png");
	background_homeSprite.setTexture(background_homeTexture);
	background_homeSprite.setColor(sf::Color(255, 255, 255, 255 * 0.55)); // Reduces Opacity to 55%

	// Initializing Background Game Screen level 1.
	Texture background1_lvl1Texture;
	Sprite background1_lvl1;
	//background1_lvl1Texture.loadFromFile("Textures/bg2_1level1.png");
	background1_lvl1Texture.loadFromFile("Textures/bg1level1.png");
	background1_lvl1.setTexture(background1_lvl1Texture);
	background1_lvl1.setColor(sf::Color(255, 255, 255, 255 * 0.35)); // Reduces Opacity to 30%

	// Initializing Background Game Screen level 1.
	Texture background2_lvl1Texture;
	Sprite background2_lvl1;
	//background2_lvl1Texture.loadFromFile("Textures/bg2_2level1.png");
	background2_lvl1.setTexture(background2_lvl1Texture);
	background2_lvl1.setColor(sf::Color(255, 255, 255, 255 * 0.50)); // Reduces Opacity to 30%

	// Initializing Background Game Screen level 1.
	Texture background3_lvl1Texture;
	Sprite background3_lvl1;
	//background3_lvl1Texture.loadFromFile("Textures/bg2_3level1.png");
	background3_lvl1.setTexture(background3_lvl1Texture);
	background3_lvl1.setColor(sf::Color(255, 255, 255, 255 * 0.80)); // Reduces Opacity to 30%

	// Initializing Background Game Screen extended portion.
	sf::Texture background_extTexture;
	sf::Sprite background_ext;
	background_extTexture.loadFromFile("Textures/bg_ext.png");
	background_ext.setTexture(background_extTexture);
	background_ext.setColor(sf::Color(255, 255, 255, 255 * 0.80)); // Reduces Opacity to 90%

	// Initializing ...
	sf::Texture background_hsTexture;
	sf::Sprite background_hs;
	background_hsTexture.loadFromFile("Textures/highscores.png");
	background_hs.setTexture(background_hsTexture);
	//background_hs.setColor(sf::Color(255, 255, 255, 0.100)); // Reduces Opacity to 90%


	////////////////////////////////////////////////////////////////
	//															  //
	//------------------- Initializing SPRITEs -------------------//
	//															  //
	//															  //
	////////////////////////////////////////////////////////////////


	// Initializing Heart Sprite.
	int heartscale = 0.8;
	Texture heartTexture;
	Sprite heartSprite;
	heartTexture.loadFromFile("Textures/heart.png");
	heartSprite.setTexture(heartTexture);

	// Initializing Player Sprites.
	sf::Texture playerTexture;
	sf::Sprite playerSprite;
	playerTexture.loadFromFile("Textures/player1.png");
	playerSprite.setTexture(playerTexture);
	playerSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));

	// Initializing Bullet Sprites.
	sf::Clock bulletClock;
	sf::Texture bulletTexture;
	sf::Sprite bulletSprite;
	bulletTexture.loadFromFile("Textures/bullet2.png");
	bulletSprite.setTexture(bulletTexture);
	bulletSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));

	// Initializing Mushroom Sprite:
	Texture mushroomTexture;
	mushroomTexture.loadFromFile("Textures/obstacle1.png");
	Sprite mushroomSprite2;
	mushroomSprite2.setTexture(mushroomTexture);
	Sprite mushroomSprite1;
	mushroomSprite1.setTexture(mushroomTexture);
	Sprite poisonousmushroomSprite2;
	poisonousmushroomSprite2.setTexture(mushroomTexture);
	Sprite poisonousmushroomSprite1;
	poisonousmushroomSprite1.setTexture(mushroomTexture);
	mushroomSprite2.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
	mushroomSprite1.setTextureRect(IntRect(64, 0, boxPixelsX, boxPixelsY));
	poisonousmushroomSprite2.setTextureRect(IntRect(0, 32, boxPixelsX, boxPixelsY));
	poisonousmushroomSprite1.setTextureRect(IntRect(64, 32, boxPixelsX, boxPixelsY));

	// Initializing Centipede Sprites:
	//Centipede head sprite(left):
	Texture centipedeheadleftTexture;
	centipedeheadleftTexture.loadFromFile("Textures/c_head_left_walk.png");
	Sprite centipedeheadleftSprite;
	centipedeheadleftSprite.setTexture(centipedeheadleftTexture);
	centipedeheadleftSprite.setTextureRect(IntRect(0, 0, boxPixelsX - 4, boxPixelsY));

	//Centipede body sprite(left):
	Texture centipedeleftTexture;
	centipedeleftTexture.loadFromFile("Textures/c_body_left_walk.png");
	Sprite centipedeleftSprite;
	centipedeleftSprite.setTexture(centipedeleftTexture);
	centipedeleftSprite.setTextureRect(IntRect(0, 0, boxPixelsX - 4, boxPixelsY));

	//Centipede head sprite(right):
	Texture centipedeheadrightTexture;
	centipedeheadrightTexture.loadFromFile("Textures/c_head_right_walk.png");
	Sprite centipedeheadrightSprite;
	centipedeheadrightSprite.setTexture(centipedeheadrightTexture);
	centipedeheadrightSprite.setTextureRect(IntRect(0, 0, boxPixelsX - 4, boxPixelsY));

	//Centipede body sprite(right):
	Texture centipederightTexture;
	centipederightTexture.loadFromFile("Textures/c_body_right_walk.png");
	Sprite centipederightSprite;
	centipederightSprite.setTexture(centipederightTexture);
	centipederightSprite.setTextureRect(IntRect(0, 0, boxPixelsX - 4, boxPixelsY));


	/////////////////////////////////////////////////////////////////////////////////////
	//																				   //
	//-----------------   Initializing Player, Centipede, Mushrooms   -----------------//
	//																				   //
	//																				   //
	/////////////////////////////////////////////////////////////////////////////////////
	

	// Initializing Player, Bullet, Mushroom
	float* player = new float[3];
	float* bullet = new float[3];
	int mushrooms[resolutionX][4];

	//Mushrooms total and current quantity variables:
	int nummushrooms = 0, totalmushrooms = nummushrooms;

	// Initializing Centipede:
	int centipedesegments = centipede_length,
		indexnum = 150,
		centipedesize = 4 + (50 * 4);
		//numvariable = 4 + (centipedesegments * 4);
	const int head = 3;

	//Centipede array:
	//float centipede[indexnum][numvariable];	// C[centipede segment number][x, y, existence, head, ...]
	float** centipede = new float* [indexnum];
	for (int i = 0; i < indexnum; ++i)
		centipede[i] = new float[centipedesize];
	
	initializePCM(player, bullet, mushrooms, minmushrooms, nummushrooms, totalmushrooms, indexnum, centipedesize, head, centipede, mushroomSprite1, mushroomSprite2, poisonousmushroomSprite1, poisonousmushroomSprite2, 1, 1, 1);
	
	// Setting texts
	levels_text.setString("Level: " + to_string(level));

	while (window.isOpen())
	{
		static int player_died = 0, centipede_gone = 0;
		int centipede_killed;

		///////////////////////////////////////////////////////////////
		//                                                           //
		//						Events Loop Calling					 //
		//                                                           //
		///////////////////////////////////////////////////////////////

		Event e;
		while (window.pollEvent(e))
		{
			// To close the window:
			if (e.type == sf::Event::Closed)
				windows=11;       //return 0;
			if (e.type == Event::KeyPressed)
			{
				if (windows >= 4 && windows <= 5 && e.key.code == Keyboard::Enter)
				{
					if (isMainmenu)
						windows = 2;
					else
						windows = 8;
					click_sound.play();
				}
				else if (windows == 1)		//Starting window
				{
					//Name input
					char ch;
					if (Keyboard::isKeyPressed(Keyboard::BackSpace) && name.length() > 0)
						name.pop_back();
					if (name.length() < maxnamelen)
					{
						if (e.key.code == Keyboard::Space)
							name += ' ';
						else if (e.key.code == Keyboard::Hyphen && e.key.shift)
							name += '_';
						else if (e.key.code == Keyboard::Hyphen)
							name += '-';
						else if (e.key.code == Keyboard::Period)
							name += '.';
						else if (e.key.code >= Keyboard::A && e.key.code <= Keyboard::Z)
						{
							if (e.key.shift)
								ch = e.text.unicode + 'A';
							else
								ch = e.text.unicode + 'a';
							name += ch;
						}
					}
					if (e.key.code == Keyboard::Enter && name.length() > 0)
					{
						windows = 2;
						click_sound.play();
					}
				}
				else if (windows == 2)
				{
					if (e.key.code == Keyboard::Down)
					{
						if (menu_pt < 5)
							++menu_pt;
						else
							menu_pt = 1;
						hover_sound.play();
					}

					if (e.key.code == Keyboard::Up)
					{
						if (menu_pt > 1)
							--menu_pt;
						else
							menu_pt = 5;
						hover_sound.play();
					}

					if (e.key.code == Keyboard::Enter)
					{
						if (menu_pt == 1)
						{
							bgMusic1.pause();
							bgMusic2.play();
							windows = 7;
						}
						if (menu_pt == 2)
							windows = 3;
						if (menu_pt == 3)
							windows = 4;
						if (menu_pt == 4)
							windows = 5;
						if (menu_pt == 5)
							windows = 6;
						click_sound.play();
					}
				}
				else if (windows == 3)		//Settings window
				{
					if (e.key.code == Keyboard::Up)
					{
						if (setting_pt > 1)
							--setting_pt;
						else
							setting_pt = 5;
						hover_sound.play();
					}
					else if (e.key.code == Keyboard::Down)
					{
						if (setting_pt < 5)
							++setting_pt;
						else
							setting_pt = 1;
						hover_sound.play();
					}
					else if (e.key.code == Keyboard::Left)
					{
						if (setting_pt == 1)
						{
							sound_bool = 1;
							click_sound.setVolume(sound_vol);
							hover_sound.setVolume(sound_vol);
							fire_sound.setVolume(sound_vol);
							playerdie_sound.setVolume(sound_vol);
							celebrate_sound.setVolume(sound_vol);
							kill_sound.setVolume(sound_vol);
							destroy_sound.setVolume(sound_vol);
							ontxt1_1.setFillColor(Color(0xe0, 0xe0, 0xe0));
							ontxt1_1.setOutlineColor(Color::Green);
							offtxt1_1.setFillColor(Color(offcolor, offcolor, offcolor));
							offtxt1_1.setOutlineColor(Color(offcolor, offcolor, offcolor));
							click_sound.play();
						}
						else if (setting_pt == 2)
						{
							if (sound_vol > 0)
							{
								--sound_vol;
								if (sound_bool)
								{
									click_sound.setVolume(sound_vol);
									hover_sound.setVolume(sound_vol);
									fire_sound.setVolume(sound_vol);
									playerdie_sound.setVolume(sound_vol);
									celebrate_sound.setVolume(sound_vol);
									kill_sound.setVolume(sound_vol);
									destroy_sound.setVolume(sound_vol);
								}
								volumenum1.setString(to_string(sound_vol));
								hover_sound.play();
							}
						}
						else if (setting_pt == 3)
						{
							music_bool = 1;
							bgMusic1.setVolume(music_vol);
							bgMusic2.setVolume(music_vol);
							ontxt2_1.setFillColor(Color(0xe0, 0xe0, 0xe0));
							ontxt2_1.setOutlineColor(Color::Green);
							offtxt2_1.setFillColor(Color(offcolor, offcolor, offcolor));
							offtxt2_1.setOutlineColor(Color(offcolor, offcolor, offcolor));
							click_sound.play();
						}
						else if (setting_pt == 4)
						{
							if (music_vol > 0)
							{
								--music_vol;
								if (music_bool)
								{
									bgMusic1.setVolume(music_vol);
									bgMusic2.setVolume(music_vol);
								}
								volumenum2.setString(to_string(music_vol));
								hover_sound.play();
							}
						}
					}
					else if (e.key.code == Keyboard::Right)
					{
						if (setting_pt == 1)
						{
							sound_bool = 0;
							click_sound.setVolume(0);
							hover_sound.setVolume(0);
							fire_sound.setVolume(0);
							playerdie_sound.setVolume(0);
							celebrate_sound.setVolume(0);
							kill_sound.setVolume(0);
							destroy_sound.setVolume(0);
							ontxt1_1.setFillColor(Color(offcolor, offcolor, offcolor));
							ontxt1_1.setOutlineColor(Color(offcolor, offcolor, offcolor));
							offtxt1_1.setFillColor(Color(0xe0, 0xe0, 0xe0));
							offtxt1_1.setOutlineColor(Color::Red);
							click_sound.play();
						}
						else if (setting_pt == 2)
						{
							if (sound_vol < 100)
							{
								++sound_vol;
								if (sound_bool)
								{
									click_sound.setVolume(sound_vol);
									hover_sound.setVolume(sound_vol);
									fire_sound.setVolume(sound_vol);
									playerdie_sound.setVolume(sound_vol);
									celebrate_sound.setVolume(sound_vol);
									kill_sound.setVolume(sound_vol);
									destroy_sound.setVolume(sound_vol);
								}
								volumenum1.setString(to_string(sound_vol));
								hover_sound.play();
							}
						}
						else if (setting_pt == 3)
						{
							music_bool = 0;
							bgMusic1.setVolume(0);
							bgMusic2.setVolume(0);
							ontxt2_1.setFillColor(Color(offcolor, offcolor, offcolor));
							ontxt2_1.setOutlineColor(Color(offcolor, offcolor, offcolor));
							offtxt2_1.setFillColor(Color(0xe0, 0xe0, 0xe0));
							offtxt2_1.setOutlineColor(Color::Red);
							click_sound.play();
						}
						else if (setting_pt == 4)
						{
							if (music_vol < 100)
							{
								++music_vol;
								if (music_bool)
								{
									bgMusic1.setVolume(music_vol);
									bgMusic2.setVolume(music_vol);
								}
								volumenum2.setString(to_string(music_vol));
								hover_sound.play();
							}
						}
					}
					else if (e.key.code == Keyboard::Enter)
					{
						click_sound.play();
						if (setting_pt == 1)
						{
							sound_bool = sound_bool ^ 1;
							if (sound_bool)
							{
								click_sound.setVolume(sound_vol);
								hover_sound.setVolume(sound_vol);
								fire_sound.setVolume(sound_vol);
								playerdie_sound.setVolume(sound_vol);
								celebrate_sound.setVolume(sound_vol);
								kill_sound.setVolume(sound_vol);
								destroy_sound.setVolume(sound_vol);
								ontxt1_1.setFillColor(Color(0xe0, 0xe0, 0xe0));
								ontxt1_1.setOutlineColor(Color::Green);
								offtxt1_1.setFillColor(Color(offcolor, offcolor, offcolor));
								offtxt1_1.setOutlineColor(Color(offcolor, offcolor, offcolor));
							}
							else
							{
								click_sound.setVolume(0);
								hover_sound.setVolume(0);
								fire_sound.setVolume(0);
								playerdie_sound.setVolume(0);
								celebrate_sound.setVolume(0);
								kill_sound.setVolume(0);
								destroy_sound.setVolume(0);
								ontxt1_1.setFillColor(Color(offcolor, offcolor, offcolor));
								ontxt1_1.setOutlineColor(Color(offcolor, offcolor, offcolor));
								offtxt1_1.setFillColor(Color(0xe0, 0xe0, 0xe0));
								offtxt1_1.setOutlineColor(Color::Red);
							}
						}
						else if (setting_pt == 3)
						{
							music_bool = music_bool ^ 1;
							if (music_bool)
							{
								bgMusic1.setVolume(music_vol);
								bgMusic2.setVolume(music_vol);
								ontxt2_1.setFillColor(Color(0xe0, 0xe0, 0xe0));
								ontxt2_1.setOutlineColor(Color::Green);
								offtxt2_1.setFillColor(Color(offcolor, offcolor, offcolor));
								offtxt2_1.setOutlineColor(Color(offcolor, offcolor, offcolor));
							}
							else
							{
								bgMusic1.setVolume(0);
								bgMusic2.setVolume(0);
								ontxt2_1.setFillColor(Color(offcolor, offcolor, offcolor));
								ontxt2_1.setOutlineColor(Color(offcolor, offcolor, offcolor));
								offtxt2_1.setFillColor(Color(0xe0, 0xe0, 0xe0));
								offtxt2_1.setOutlineColor(Color::Red);
							}
						}
						else if (setting_pt == 5)
						{
							if (isMainmenu)
								windows = 2;
							else
								windows = 8;
						}
					}
				}
				else if (windows == 4)
				{

				}
				else if (windows == 5)
				{

				}
				else if (windows == 6)	//Highscore screen
				{
					if (e.key.code == Keyboard::Enter)
					{
						if (isMainmenu)
							windows = 2;
						else if (game_over || game_won || exit_pt)
							windows=11;       //return 0;
						else
							windows = 8;
						click_sound.play();
					}
				}
				else if (windows == 7)	//Game window
				{
					if (e.key.code == Keyboard::P)
					{
						bgMusic2.pause();
						bgMusic1.play();
						windows = 8;
					}
					else if (e.key.code == Keyboard::Escape || e.key.code == Keyboard::Q)
					{
						bgMusic2.pause();
						bgMusic1.play();
						windows = 10;
					}
				}
				else if (windows == 8)	//Pause window
				{
					if (e.key.code == Keyboard::Down)
					{
						if (pause_pt < 6)
							++pause_pt;
						else
							pause_pt = 1;
						hover_sound.play();
					}

					if (e.key.code == Keyboard::Up)
					{
						if (pause_pt > 1)
							--pause_pt;
						else
							pause_pt = 6;
						hover_sound.play();
					}

					if (e.key.code == Keyboard::Enter)
					{
						click_sound.play();
						if (pause_pt == 1)
						{
							bgMusic1.pause();
							bgMusic2.play();
							windows = 7;
						}
						if (pause_pt == 2)
							windows = 3;
						if (pause_pt == 3)
							windows = 5;
						if (pause_pt == 4)
							windows = 6;
						if (pause_pt == 5)
							windows = 2;
						if (pause_pt == 6)
							windows=11;       //return 0;
					}
				}
				else if (windows == 9)	//End game screen
				{
					if (e.key.code == Keyboard::Enter)
					{
						windows = 6;
						click_sound.play();
					}
				}
				else if (windows == 10)	//Exit Ask screen
				{
					if (e.key.code == Keyboard::Left)
					{
						exit_pt = 1;
						yestxt.setFillColor(Color(defcol, defcol, defcol));
						yestxt.setOutlineColor(Color::Green);
						notxt.setFillColor(Color(offcolor2, offcolor2, offcolor2));
						notxt.setOutlineColor(Color(offcolor2, offcolor2, offcolor2));
						hover_sound.play();
					}
					else if (e.key.code == Keyboard::Right)
					{
						exit_pt = 0;
						notxt.setFillColor(Color(defcol, defcol, defcol));
						notxt.setOutlineColor(Color::Red);
						yestxt.setFillColor(Color(offcolor2, offcolor2, offcolor2));
						yestxt.setOutlineColor(Color(offcolor2, offcolor2, offcolor2));
						hover_sound.play();
					}
					else if (e.key.code == Keyboard::Enter)
					{
						click_sound.play();
						if (exit_pt)
							windows = 9;
						else
						{
							bgMusic1.pause();
							bgMusic2.play();
							windows = 7;
						}
					}
				}
			}
		}
		//////////////////////////////////////
		//									//
		//				Windows				//
		//									//
		//////////////////////////////////////
		if (windows != 7)
			window.draw(background_homeSprite);

		////////////////  Starting window  ////////////////
		if (windows == 1)
		{
			stnamestr.setString(name);
			
			window.draw(startgame_text_1);
			window.draw(startgame_text_2);
			window.draw(strectangle1);
			window.draw(strectangle2);
			window.draw(entername_text);
			window.draw(stnamestr);
		}
		////////////////  Main Menu  ////////////////
		else if (windows == 2)	
		{
			window.draw(menu1);
			window.draw(menu2);
			if (menu_pt == 1)
			{
				window.draw(start1);
				window.draw(start2);
			}
			else
				window.draw(start);

			if (menu_pt == 2)
			{
				window.draw(settings1);
				window.draw(settings2);
			}
			else
				window.draw(settings);

			if (menu_pt == 3)
			{
				window.draw(instructions1);
				window.draw(instructions2);
			}
			else
				window.draw(instructions);

			if (menu_pt == 4)
			{
				window.draw(controls1);
				window.draw(controls2);
			}
			else
				window.draw(controls);

			if (menu_pt == 5)
			{
				window.draw(high_scores1);
				window.draw(high_scores2);
			}
			else
				window.draw(high_scores);
		}
		////////////////  Settings window  ////////////////
		else if (windows == 3)
		{
			window.draw(Settings1);
			window.draw(Settings2);

			window.draw(settings_soundstxt);
			if (setting_pt == 1)
			{
				window.draw(statetxt1_1);
				window.draw(statetxt1_2);
			}
			else
				window.draw(statetxt1);
			window.draw(ontxt1_1);
			window.draw(offtxt1_1);
			if (setting_pt == 2)
			{
				window.draw(volumetxt1_1);
				window.draw(volumetxt1_2);
			}
			else
				window.draw(volumetxt1);
			window.draw(volumenum1);


			window.draw(settings_musictxt);
			if (setting_pt == 3)
			{
				window.draw(statetxt2_1);
				window.draw(statetxt2_2);
			}
			else
				window.draw(statetxt2);
			window.draw(ontxt2_1);
			window.draw(offtxt2_1);
			
			if (setting_pt == 4)
			{
				window.draw(volumetxt2_1);
				window.draw(volumetxt2_2);
			}
			else
				window.draw(volumetxt2);
			window.draw(volumenum2);

			if (setting_pt == 5)
			{
				window.draw(backtxt1);
				window.draw(backtxt2);
			}
			else
				window.draw(backtxt);
		}

		////////////////  Instructions window  ////////////////
		else if (windows == 4)
		{
			window.draw(Instructions1);
			window.draw(Instructions2);
			window.draw(i_l1);
			window.draw(i_l2);
			window.draw(i_l3);
			window.draw(i_l4);
			window.draw(i_l5);
			window.draw(i_l6);
			window.draw(i_l7);
			window.draw(i_l8);
			window.draw(i_l9);
			window.draw(i_l10);
			window.draw(i_l11);
			//window.draw(i_l12);
			//window.draw(i_l13);
			window.draw(backtxt1);
			window.draw(backtxt2);
		}
		////////////////  Controls window  ////////////////
		else if (windows == 5)
		{
			window.draw(Controls1);
			window.draw(Controls2);

			window.draw(keyboard_sprite1);
			window.draw(keyboard_sprite2);

			//window.draw(controlsdef);
			window.draw(backtxt1);
			window.draw(backtxt2);
		}
		////////////////  Highscores window  ////////////////
		else if (windows == 6)
		{
			window.draw(hscore1);
			window.draw(hscore2);

			window.draw(background_hs);
			/////////////////////////////// #note: scores to be printed

			if (game_over || game_won)
			{
				window.draw(pressentertxt);
			}
			else
			{
				window.draw(backtxt1);
				window.draw(backtxt2);
			}
		}
		////////////////  Pause window  ////////////////
		else if (windows == 8)
		{
			window.draw(pause1);
			window.draw(pause2);

			if (pause_pt == 1)
			{
				window.draw(resume1);
				window.draw(resume2);
			}
			else
				window.draw(resume);

			if (pause_pt == 2)
			{
				window.draw(psettings1);
				window.draw(psettings2);
			}
			else
				window.draw(psettings);

			if (pause_pt == 3)
			{
				window.draw(pcontrols1);
				window.draw(pcontrols2);
			}
			else
				window.draw(pcontrols);
			
			if (pause_pt == 4)
			{
				window.draw(phigh_scores1);
				window.draw(phigh_scores2);
			}
			else
				window.draw(phigh_scores);
			
			if (pause_pt == 5)
			{
				window.draw(tomainmenu1);
				window.draw(tomainmenu2);
			}
			else
				window.draw(tomainmenu);

			if (pause_pt == 6)
			{
				window.draw(exit_game1);
				window.draw(exit_game2);
			}
			else
				window.draw(exit_game);
		}
		////////////////  End game window  ////////////////
		else if (windows == 9)
		{
			score_text.setCharacterSize(psscore.getCharacterSize());
			score_text.setPosition(psscore.getPosition().x + (psscore.getCharacterSize() * 7) - 10, psscore.getPosition().y);

			lives_text.setCharacterSize(pslives.getCharacterSize());
			lives_text.setPosition(pslives.getPosition().x + (pslives.getCharacterSize() * 11), pslives.getPosition().y);

			if (game_won)
				window.draw(gamewon_text);
			else if(game_over)
				window.draw(gameover_text);

			window.draw(psrectangle1);
			window.draw(psrectangle2);

			window.draw(pstats1);
			window.draw(pstats2);

			window.draw(psname);
			namestr.setString(name);
			window.draw(namestr);

			window.draw(psscore);
			score_text.setString(to_string(score));
			window.draw(score_text);

			window.draw(pslevel);
			levelstr.setString(to_string(level));
			window.draw(levelstr);

			window.draw(pslives);
			lives_text.setString(to_string(lives));
			window.draw(lives_text);

			window.draw(pressentertxt);
		}
		////////////////  Exit Ask window  ////////////////
		else if (windows == 10)
		{
			window.draw(exitrectangle1);
			window.draw(exitrectangle2);

			window.draw(exit1);
			window.draw(exit2);

			window.draw(yestxt);
			window.draw(notxt);
		}
		////////////////  Heap Clearing window  ////////////////
		else if (windows == 11)
		{
			///////////////////////////////// clear heap
			/*delete[] Up;
			delete[] Down;
			delete[] Left;
			delete[] Right;
			delete[] player;
			delete[] bullet;*/
			
			for (int i = 0; i < indexnum; ++i)
				delete[] centipede[i];
			delete[] centipede;

			return 0;
		}
		////////////////  GAME window  ////////////////
		else if (windows == 7)
		{
			isMainmenu = 0;
			window.draw(background1_lvl1);
			window.draw(background2_lvl1);
			window.draw(background3_lvl1);
			window.draw(background_ext);

			if (deductlife)
			{
				--lives;		//Player lives decreases by one
				playerdie_sound.play();
				deductlife = 0;
				if (lives > 0)
				{
					initializePCM(player, bullet, mushrooms, minmushrooms, nummushrooms, totalmushrooms, indexnum, centipedesize, head, centipede, mushroomSprite1, mushroomSprite2, poisonousmushroomSprite1, poisonousmushroomSprite2);
				}
			}
			

			if (lives <= 0)
				player[exists] = 0;

			//Player dying consequences:
			if (!player[exists] && !player_died)
			{
				cout << "Player died  😔" << endl;
				player_died = 1;
				game_over = 1;
			}

			//Centipede killed condition checking:
			centipede_killed = 1;
			for (int i = 0; i < centipede_length; ++i)
			{
				if (centipede[i][exists] > 0)
				{
					centipede_killed = 0;
				}
			}

			//Centipede killed consequences:
			if (player[exists] && centipede_killed)
			{
				++centipede_gone;
				//game_won = 1;
			}
			if (centipede_gone == 1)
			{
				cout << "Centipede killed  🥳" << endl;
				celebrate_sound.play();

				centipede_gone = 0;
				++level;
				levels_text.setString("Level: " + to_string(level));
				centipede_length += 2;
				minmushrooms += 5;
				centipede_speed += 0.2;
				initializePCM(player, bullet, mushrooms, minmushrooms, nummushrooms, totalmushrooms, indexnum, centipedesize, head, centipede, mushroomSprite1, mushroomSprite2, poisonousmushroomSprite1, poisonousmushroomSprite2);
			}

			if (player[exists])
			{
				drawPlayer(window, player, playerSprite);
			}

			// Spawning mushrooms:
			for (int n = 0; n < totalmushrooms; ++n)
			{
				//Normal mushrooms:
				if (mushrooms[n][exists] == 2 && !mushrooms[n][poison])
					drawMushroom2(window, mushrooms, n, mushroomSprite2);
				if (mushrooms[n][exists] == 1 && !mushrooms[n][poison])
					drawMushroom1(window, mushrooms, n, mushroomSprite1);

				//Poisonous mushrooms:
				if (mushrooms[n][exists] == 2 && mushrooms[n][poison])
					drawPoisonousMushroom2(window, mushrooms, n, poisonousmushroomSprite2);
				if (mushrooms[n][exists] == 1 && mushrooms[n][poison])
					drawPoisonousMushroom1(window, mushrooms, n, poisonousmushroomSprite1);
			}
			//cout << "Num = " << nummushrooms << endl << "Total = " << totalmushrooms << endl;

			//Drawing centipede head and body
			if (player[exists])
			{
				movecentipede(window, centipedeleftSprite, centipederightSprite, centipedeheadleftSprite, centipedeheadrightSprite, centipede, player, mushrooms, head, totalmushrooms, playercollide, centipede_speed, Up, Down, Left, Right, deductlife);
				drawCentipede(window, centipede, head, centipedeleftSprite, centipederightSprite, centipedeheadleftSprite, centipedeheadrightSprite, Up, Down, Left, Right);
				destroycentipede(bullet, centipede, head, mushrooms, nummushrooms, totalmushrooms, score, kill_sound);
			}

			if (player[exists])
			{
				moveplayer(player, mushrooms, totalmushrooms, playerspeed, deductlife);	// Move player func
				shootbullet(bullet, player, fire_sound); // Shoot bullet
			}

			if (bullet[exists] == true)
			{
				moveBullet(bullet, player, bulletClock, bullet_speed);
				drawBullet(window, bullet, bulletSprite);
			}
			destroyMushroom(bullet, mushrooms, nummushrooms, totalmushrooms, score, destroy_sound);


			//Printing game grid...
			//system("cls");
			/*
			cout<<" ";
			for(int i=0; i<60; ++i)
				cout<<"_";
			cout<<endl;
			for(int i=0; i<gameRows; ++i)
			{
				cout<<"|";
				for(int j=0; j<gameColumns; ++j)
				if(gameGrid[j][i] == 0)
					cout<<"  ";
				else
					cout<<" "<<gameGrid[j][i];
				cout<<"|"<<endl;
			}
			cout<<" ";
			for(int i=0; i<60; ++i)
				cout<<"_";
			cout<<endl;
			//...
			*/

			//Updating score, lives & text:
			score_text.setString("Score: " + to_string(score));

			//Printing text:
			window.draw(score_text);
			window.draw(lives_text);
			lives_draw(window, lives, lives_text, heartSprite);
			window.draw(levels_text);

			window.draw(line1);
			window.draw(line2);
			window.draw(line3);
			window.draw(line4);
			window.draw(line5);

			//If player dies OR If centipede is killed
			if (game_over || game_won)
			{
				score += 150 * (lives - 1);		// Bonous score for remaining lives
				windows = 9;
				bgMusic2.pause();
				bgMusic1.play();
			}
		}
		window.display();
		window.clear();
	}
}

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// Write your functions definitions here. Some have been written for you. //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

void cleararrays(float player[], float bullet[], int mushrooms[][4], int& minmushrooms, const int indexnum, const int centipedesize, float**& centipede, bool domushroom, bool docentipede, bool doplayer)
{
	//Emptying arrays
	if (doplayer)
		for (int i = 0; i < 3; ++i)
		{
			player[i] = 0;
			bullet[i] = 0;
		}
	if (domushroom)
	{
		for (int i = 0; i < resolutionX; ++i)
		{
			for (int j = 0; j < 4; ++j)
				mushrooms[i][j] = 0;
		}
	}
	else
	{
		for (int i = 0; i < resolutionX; ++i)
			mushrooms[i][poison] = 0;
	}
	if (docentipede)
	{
		for (int i = 0; i < indexnum; ++i)
		{
			for (int j = 0; j < centipedesize; ++j)
				centipede[i][j] = 0;
		}
		//int centipedesegments = centipede_length,
		//	indexnum = 150,
		//	numvariable = 4 + (centipedesegments * 4);

		//for (int i = 0; i < indexnum; ++i)
		//	delete[] centipede[i];
		//delete[] centipede;

		//centipede = new float* [indexnum];		// C[centipede segment number][x, y, existence]
		//for (int i = 0; i < indexnum; ++i)
		//	centipede[i] = new float[numvariable];
	}
	if (domushroom)
		for (int i = 0; i < gameRows; ++i)
		{
			for (int j = 0; j < gameColumns; ++j)
				gameGrid[i][j] = 0;
		}
}

void initializePCM(float player[], float bullet[], int mushrooms[][4], int& minmushrooms, int& nummushrooms, int& totalmushrooms, const int indexnum, const int centipedesize, const int head, float** centipede, Sprite& mushroomSprite1, Sprite& mushroomSprite2, Sprite& poisonousmushroomSprite1, Sprite& poisonousmushroomSprite2, bool domushroom, bool docentipede, bool doplayer)
{
	cleararrays(player, bullet, mushrooms, minmushrooms, indexnum, centipedesize, centipede, domushroom, docentipede, doplayer);

	if (doplayer)
	{
		// Initializing Player
		player[x] = (gameColumns / 2) * boxPixelsX; // player[x] = 480 (from left)
		player[y] = resolutionY - (boxPixelsY * 5); // player[y] = 800 (from above)
		player[exists] = 1;
		gameGrid[(int)player[x] / boxPixelsX][(int)player[y] / boxPixelsY] = 3;

		// Initializing Bullet
		bullet[x] = player[x];				// bullet[x] = 480 (from left)
		bullet[y] = player[y] - boxPixelsY; // bullet[y] = 768 (from above) to start from playes's top
		bullet[exists] = false;
	}

	//Mushrooms total and current quantity variables:
	int numtemp = (rand() % 11) + minmushrooms;		// Min mushrooms = 20, Max mushrooms = Min + 11
	totalmushrooms = numtemp;
	numtemp = totalmushrooms - nummushrooms;
	nummushrooms = totalmushrooms;
	cout << "\nNumber of Mushrooms: " << nummushrooms << endl << endl;

	int tempposx, tempposy, n = 0, count = 0;
	for (; count < numtemp; ++count, ++n)
	{
		tempposx = ((rand() % 28) + 2);
		tempposy = ((rand() % 28) + 1);

		//Checking if a mushroom already exists there or not:
		if (gameGrid[tempposx][tempposy] != 0)
		{
			--count;
			--n;
			continue;
		}
		while (mushrooms[n][exists] == 1 || mushrooms[n][exists] == 2)
		{
			++n;
		}

		mushrooms[n][x] = tempposx * 32; // Row position of mushroom
		mushrooms[n][y] = tempposy * 32; // Column position of mushroom

		//Setting gamegrid position of mushrooms:
		/*for (int i = 0; i < gameRows; ++i)
		{
			for (int j = 0; j < gameColumns; ++j)
			{
				if (gameGrid[i][j] == 0)
				{
					gameGrid[tempposx][tempposy] = 2;
				}
			}
		}*/
		gameGrid[tempposx][tempposy] = 2;
		mushrooms[n][exists] = 2;	//Mushroom existence setting
		mushrooms[n][poison] = 0;	//Mushroom poison setting

		cout << mushrooms[n][x] / 32 << " " << mushrooms[n][y] / 32 << endl;	//Prinitng mushroom locations on terminal
	}
	cout << endl;

	// Initializing Centipede:

	//.....Setting centipede position.....
	//Setting centipede first head as default:
	centipede[0][head] = 1;
	for (int i = 1; i < centipede_length; ++i)
	{
		centipede[i][head] = 0;
	}

	//Setting existence
	for (int i = 0; i < centipede_length; ++i)
	{
		centipede[i][exists] = 1;
	}

	centipede[0][x] = ((rand() % 19) + 1) * 32;	//Centipede head position on columns
	centipede[0][y] = 0 * 32;	//Centipede head position on first row

	for (int i = 1; i < centipede_length; ++i)		//Setting centipede body positions
	{
		centipede[i][x] = centipede[i - 1][x] + 28;
		centipede[i][y] = centipede[0][y];
	}
	//.....
}

void drawPlayer(sf::RenderWindow &window, float player[], sf::Sprite &playerSprite)
{
	playerSprite.setPosition(player[x], player[y]);
	window.draw(playerSprite);
}

void moveplayer(float player[], int mushrooms[][4], int totalmushrooms, float playerspeed, bool& deductlife)
{
	bool up = 0, down = 0, left = 0, right = 0, collide = 0, check = 0;
	int tempx, tempy, prevx, prevy;

	if (Keyboard::isKeyPressed(Keyboard::Up) && player[y] > resolutionY - (boxPixelsY * 5))
		up = 1;
	if (Keyboard::isKeyPressed(Keyboard::Down) && player[y] < resolutionX - boxPixelsY)
		down = 1;
	if (Keyboard::isKeyPressed(Keyboard::Left) && player[x] > 0)
		left = 1;
	if (Keyboard::isKeyPressed(Keyboard::Right) && player[x] < resolutionY - boxPixelsX)
		right = 1;

	tempx = player[x];
	tempy = player[y];

	if (up)
		tempy -= 3;
	if (down)
		tempy += 3;
	if (left)
		tempx -= 3;
	if (right)
		tempx += 3;

	//Mushroom collision detection:
	for (int i = 0; i < totalmushrooms; ++i)
	{
		if (mushrooms[i][exists] > 0)
			if (tempx > (mushrooms[i][x] - 32) && tempx < (mushrooms[i][x] + 32) && tempy<(mushrooms[i][y] + 32) && tempy>(mushrooms[i][y] - 32))
			{
				collide = 1;
				if (collide && mushrooms[i][poison])
				{
					deductlife = 1;
				}
				break;
			}
	}

	//For setting zero in game grid on previuos position of player:
	prevx = (int)player[x] / boxPixelsX;
	prevy = (int)player[y] / boxPixelsY;

	//Player movement if no collsion:
	if (!collide)
	{
		if (up)
		{
			player[y] -= playerspeed;
			check = 1;
		}
		if (down)
		{
			player[y] += playerspeed;
			check = 1;
		}
		if (left)
		{
			player[x] -= playerspeed;
			check = 1;
		}
		if (right)
		{
			player[x] += playerspeed;
			check = 1;
		}
	}
	//Setting player position value on gamegrid:
	gameGrid[(int)player[x] / boxPixelsX][(int)player[y] / boxPixelsY] = 3;
	if (check == 1)
	{
		gameGrid[prevx][prevy] = 0;
		check = 0;
	}
}

void moveBullet(float bullet[], float player[], sf::Clock &bulletClock, float bullet_speed)
{
	int prevx, prevy;
	if (bulletClock.getElapsedTime().asMilliseconds() < 0.01f)
		return;

	//bullet_speed = 15;	//Setting speed
	bulletClock.restart();
	bullet[y] -= bullet_speed;	//Bullet going up
	
	//Setting game grid value of bullet
	if(gameGrid[(int)bullet[x]/boxPixelsX][(int)bullet[y]/boxPixelsY] == 0 && gameGrid[(int)bullet[x]/boxPixelsX][(int)bullet[y]/boxPixelsY] != player[x])
		gameGrid[(int)bullet[x]/boxPixelsX][(int)bullet[y]/boxPixelsY] = 4;
	
	//Makes the previous value of bullet on game grid to zro as default
	prevx = (int)bullet[x]/boxPixelsX;
	prevy = ((int)bullet[y]/boxPixelsY)+1;
	gameGrid[prevx][prevy] = 0;
	
	//Checks when the bullet is out of boundary of screen from top
	if (bullet[y] < -32)
		bullet[exists] = false;
}
void drawBullet(sf::RenderWindow &window, float bullet[], sf::Sprite &bulletSprite)
{
	bulletSprite.setPosition(bullet[x], bullet[y]);
	window.draw(bulletSprite);
}

void shootbullet(float bullet[], float player[], Sound &fire_sound)
{
	bool shoot = 0;
	if (bullet[exists] == false)
	{
		if (Keyboard::isKeyPressed(Keyboard::X))
			shoot=1;
	}
	if (shoot)
	{
		fire_sound.play();

		bullet[exists] = true;
		bullet[x] = player[x];
		bullet[y] = player[y];
	}
}

void drawMushroom2(RenderWindow &window, int mushrooms[][4], int n, Sprite &mushroomSprite2)
{
	mushroomSprite2.setPosition(mushrooms[n][x], mushrooms[n][y]);
	window.draw(mushroomSprite2);
}
void drawMushroom1(RenderWindow &window, int mushrooms[][4], int n, Sprite &mushroomSprite1)
{
	mushroomSprite1.setPosition(mushrooms[n][x], mushrooms[n][y]);
	window.draw(mushroomSprite1);
}

void drawPoisonousMushroom2(RenderWindow &window, int mushrooms[][4], int n, Sprite &poisonousmushroomSprite2)
{
	poisonousmushroomSprite2.setPosition(mushrooms[n][x], mushrooms[n][y]);
	window.draw(poisonousmushroomSprite2);
}
void drawPoisonousMushroom1(RenderWindow &window, int mushrooms[][4], int n, Sprite &poisonousmushroomSprite1)
{
	poisonousmushroomSprite1.setPosition(mushrooms[n][x], mushrooms[n][y]);
	window.draw(poisonousmushroomSprite1);
}

void destroyMushroom(float bullet[], int mushrooms[][4], int& nummushrooms, int& totalmushrooms, int& score, Sound& destroy_sound)
{
	bool hit = 0;
	int i = 0;
	for (; i < totalmushrooms; ++i)
	{
		if (mushrooms[i][exists] > 0)
			if (bullet[x] > (mushrooms[i][x] - 32) && bullet[x] < (mushrooms[i][x] + 32) && bullet[y] < (mushrooms[i][y] + 32) && bullet[y] > (mushrooms[i][y] - 32))
			{
				hit = 1;
				break;
			}
	}
	if (hit)
	{
		--mushrooms[i][exists];
		//if (gameGrid[mushrooms[i][x] / boxPixelsX] [mushrooms[i][y] / boxPixelsY] == 1)
		if (mushrooms[i][exists] == 0)	//Setting number of mushrooms:
		{
			--nummushrooms;
			--mushrooms[i][exists];
			++score;
			destroy_sound.play();
		}
		--gameGrid[mushrooms[i][x] / boxPixelsX] [mushrooms[i][y] / boxPixelsY];

		bullet[exists] = false;
		bullet[y] = -1000;
		hit = 0;
	}
}

void drawCentipede(RenderWindow& window, float** centipede, const int head, Sprite& centipedeleftSprite, Sprite& centipederightSprite, Sprite& centipedeheadleftSprite, Sprite& centipedeheadrightSprite, int*& Up, int*& Down, int*& Left, int*& Right)
{
	for (int i = 0; i < centipede_length; ++i)
	{
		//Centipede Head spawning:
		if (centipede[i][head] == 1 && centipede[i][exists] == 1)
		{
			if (centipede[i][Left[i]] == 1)
			{
				centipedeheadleftSprite.setPosition(centipede[i][x], centipede[i][y]);
				window.draw(centipedeheadleftSprite);
			}
			else
			{
				centipedeheadrightSprite.setPosition(centipede[i][x], centipede[i][y]);
				window.draw(centipedeheadrightSprite);
			}
		}
		//Not draw killed centipedes:
		if (centipede[i][exists] <= 0)
		{
			continue;
		}
		//Centipede body spawning:
		if (centipede[i][head] == 0 && centipede[i][exists] == 1)
		{
			if (centipede[i][Left[i]] == 1)
			{
				centipedeleftSprite.setPosition(centipede[i][x], centipede[i][y]);
				window.draw(centipedeleftSprite);
			}
			else
			{
				centipederightSprite.setPosition(centipede[i][x], centipede[i][y]);
				window.draw(centipederightSprite);
			}
		}
	}
}

void movecentipede(RenderWindow& window, Sprite& centipedeleftSprite, Sprite& centipederightSprite, Sprite& centipedeheadleftSprite, Sprite& centipedeheadrightSprite, float** centipede, float player[], int mushrooms[][4], const int head, int totalmushrooms, bool playercollide, float& centipede_speed, int*& Up, int*& Down, int*& Left, int*& Right, bool& deductlife)
{
	static int* godown = new int[centipede_length], check = 0,
			  * collide = new int[centipede_length];	//Left right collision

	for (int i = 0; i < centipede_length; ++i)	//Setting collision to zero
	{
		collide[i] = 0;
	}
	
	float* tempx = new float[centipede_length], * tempy = new float[centipede_length], prevx, prevy;
	//Left[centipede_length], Right[centipede_length], Down[centipede_length], Up[centipede_length];	//For each centipede segment

	//Initializing indexes:
	for (int i = 4, j = 16, k = 28, l = 40, m = 0; m < centipede_length; ++i, ++j, ++k, ++l, ++m)
	{
		Left[m] = i;
		Right[m] = j;
		Up[m] = k;
		Down[m] = l;
	}

	for (int t = 0; t < centipede_length; ++t)	//For each(centipede_length) centipede segment
	{
		if (centipede[t][y] == 0)		//Default left movement
			centipede[t][Left[t]] = 1;

		//Temp x and y for collision detections:
		tempx[t] = centipede[t][x];
		tempy[t] = centipede[t][y];
		
		if (centipede[t][y] < (resolutionY - (boxPixelsY * 5)))	//(Above 5th row)		centipede[t][y] < 800 or 5rows
		{
			godown[t] = 1;
		}
		if (centipede[t][y] > (resolutionY - (boxPixelsY * 5)) && centipede[t][y] < resolutionY)	//(Below 5th row)		centipede[t][y] > 800 (5th row) && centipede[t][y] > 960	
		{
			if (tempy[t] > resolutionY)	//960
			{
				godown[t] = 0;
			}
			if (tempy[t] < (resolutionY - (boxPixelsY * 5)))	//800(5th row)
			{
				godown[t] = 1;
			}
		}

		//Mushroom collision:
		for (int i = 0; i < totalmushrooms; ++i)
		{
			if (mushrooms[i][exists] > 0)
			{
				if (tempx[t] > (mushrooms[i][x] - 32) && tempx[t] < (mushrooms[i][x] + 32) && tempy[t]<(mushrooms[i][y] + 32) && tempy[t]>(mushrooms[i][y] - 32) && centipede[t][exists] > 0)
				{
					collide[t] = 1;
					break;
				}
			}
		}
		
		//Wall collision:	(left wall, right wall)
		if (tempx[t] < 0 || tempx[t] > (resolutionX - boxPixelsX))
		{
			collide[t] = 1;
		}
		
		//Player collision:
		if (tempx[t] > (player[x] - 32) && tempx[t] < (player[x] + 32) && tempy[t]<(player[y] + 32) && tempy[t]>(player[y] - 32) && centipede[t][exists] > 0)
		{
			playercollide = 1;
		}
		
		if (playercollide == 1)
		{
			for (int i = 0; i < centipede_length; ++i)
			{
				centipede[t][Left[t]] = 0, centipede[t][Right[t]] = 0, centipede[t][Up[t]] = 0, centipede[t][Down[t]] = 0;
			}
			deductlife = 1;
		}

		//Setting centipede movement in player area(5 rows):...
		if (centipede[t][y] < (resolutionY - (boxPixelsY * 5)))	//centipede[t][y] < 800 (Above 5rows)
		{
			godown[t] = 1;
		}
		if (centipede[t][y] >= (resolutionY - (boxPixelsY * 5)) && centipede[t][y] < resolutionY)	//Below 5th row		c[t][y]>800 && c[t][y]<960
		{
			if (centipede[t][y] == (resolutionY - (boxPixelsY * 5)))	//On 5th row
			{
				godown[t] = 1;
			}
			if (centipede[t][y] == resolutionY - 32)
			{
				godown[t] = 0;
			}
		}
		
		if (godown[t])
		{
			if (collide[t])
			{
				centipede[t][Down[t]] = 1;
			}
		}
		if (!godown[t])
		{
			if (collide[t])
			{
				centipede[t][Up[t]] = 1;
			}
		}
		//...


		if (centipede[t][Down[t]])
		{
			tempy[t] += 32;
		}
		if (centipede[t][Up[t]])
		{
			tempy[t] -= 32;
		}
		if (centipede[t][Left[t]])
		{
			tempx[t] -= centipede_speed;
		}
		if (centipede[t][Right[t]])
		{
			tempx[t] += centipede_speed;
		}
		
		//To check the up, down, left, right movement of centipede:
		//cout << "right= " << centipede[t][Right[t]] << endl << "left= " << centipede[t][Left[t]] << endl << "down= " << centipede[t][Down[t]] << endl << "up= " << centipede[t][Up[t]] << endl;
		
		//Down movement:
		if (centipede[t][Down[t]])
		{
			centipede[t][y] += 32;
			centipede[t][Down[t]] = 0;
			if (centipede[t][Left[t]])
			{
				centipede[t][x] += centipede_speed;
				{
					centipede[t][Left[t]] = 0;
					centipede[t][Right[t]] = 1;
				}
			}
			else if (centipede[t][Right[t]])
			{
				centipede[t][x] -= centipede_speed;
				{
					centipede[t][Right[t]] = 0;
					centipede[t][Left[t]] = 1;
				}
			}
		}

		//Up movement:
		if (centipede[t][Up[t]])
		{
			centipede[t][y] -= 32;
			centipede[t][Up[t]] = 0;
			if (centipede[t][Left[t]])
			{
				centipede[t][x] += centipede_speed;
				{
					centipede[t][Left[t]] = 0;
					centipede[t][Right[t]] = 1;
				}
			}
			else if (centipede[t][Right[t]])
			{
				centipede[t][x] -= centipede_speed;
				{
					centipede[t][Right[t]] = 0;
					centipede[t][Left[t]] = 1;
				}
			}
		}
		
		for (int i = 0; i < centipede_length; ++i)
		{
			collide[i] = 0;
		}
		
		if (!collide[t])
		{
			if (centipede[t][Left[t]])
			{
				centipede[t][x] -= centipede_speed;
				check = 1;
			}
			if (centipede[t][Right[t]])
			{
				centipede[t][x] += centipede_speed;
				check = 1;
			}
		}
	}
}

void destroycentipede(float bullet[], float** centipede, const int head, int mushrooms[][4], int& nummushrooms, int& totalmushrooms, int& score, Sound& kill_sound)
{
	bool hit = 0;
	int i = 0, j = 0;

	//Centipede-bullet collsion detection:
	for (; i < centipede_length; ++i)
	{
		if (centipede[i][exists] > 0)
			if (bullet[x] > (centipede[i][x] - 32) && bullet[x] < (centipede[i][x] + 32) && bullet[y]<(centipede[i][y] + 32) && bullet[y]>(centipede[i][y] - 32))
			{
				hit = 1;
				break;
			}
	}

	//Mushroom spawning and centipede part destroying:
	if (hit)
	{
		centipede[i][exists] = 0;
		centipede[i + 1][head] = 1;
		kill_sound.play();

		for (; j < totalmushrooms + 40; ++j)
		{
			if (mushrooms[j][exists] <= 0)
				break;
		}
		mushrooms[j][x] = centipede[i][x];
		mushrooms[j][y] = centipede[i][y];
		mushrooms[j][exists] = 2;
		mushrooms[j][poison] = 0;
		gameGrid[mushrooms[j][x] / boxPixelsX] [mushrooms[j][y] / boxPixelsY] = 2;

		//Making the mushroom poisonous in the player area:
		if (centipede[i][y] >= (resolutionY - (boxPixelsY * 5)) && centipede[i][y] < resolutionY)	//Below 5th row		c[t][y]>800 && c[t][y]<960
		{
			mushrooms[j][poison] = 1;
		}

		++nummushrooms;
		++totalmushrooms;

		bullet[exists] = false;
		bullet[y] = -1000;
		hit = 0;
	}

	//Scoring setting:
	for (int k = 0; k < centipede_length; ++k)
	{
		if (centipede[k][exists] == 0)
		{
			if (centipede[k][head] == 1)
				score += 20;
			else
				score += 10;
			--centipede[k][exists];
		}
	}
}

void lives_draw(RenderWindow& window, int lives, Text lives_text, Sprite& heartSprite)
{
	int x = lives_text.getPosition().x + (lives_text.getCharacterSize() * 7) - 23, y = lives_text.getPosition().y;
	for (int i = 0; i < lives; ++i)
	{
		heartSprite.setPosition(x, y);
		window.draw(heartSprite);
		x += 34;
	}
}