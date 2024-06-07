#include "utils.h"





i32 mapHeight  = 16;
i32 mapWidth   = 64;
const i32 tileWidth  = 40;
const i32 tileHeight = 40;
const i32 visibleTilesX = WIDTH / tileWidth;
const i32 visibleTilesY = HEIGHT / tileHeight;
f64 cameraX = 0.0f;
f64 cameraY = 0.0f;

f64 ofsetX;
f64 ofsetY;

f64 playerPosX = 0 ;
f64 playerPosY = 0 ;

f64 playerVelX = 0.0f;
f64 playerVelY = 0.0f;
u64 frameStartTime;

static inline void calculateOfset() {
	ofsetX = cameraX -  (f64)visibleTilesX / 2.0f ;
	ofsetY = cameraY -  (f64)visibleTilesY / 2.0f;
	//CLAMP OFSET
	}



char map[] =  "................................................................"
              "................................................................"
              "................................................................"
              "................................................................"
              "................................................................"
              ".................#..#..........................................."
              "...............###..###........................................."
              "...............###..###..............########################..."
              "..............####..####.............########################..."
              "###################################............................."
              "###################################...........................##"
              ".................................##########..........###########"
              "..........................................##........######......"
              "...........................................#....#...#####......."
              "..........................................###...#..######......."
              "..........................................#########............."
              ;



static inline char getTile(i32 x, i32 y) {


	if(x >=0 && x < mapWidth && y >=0 && y < mapHeight)
		return map[y*mapWidth + x];
	else {
		//ERROR_BREAK("Jebiga");
		return ' ';
		}


	}


static inline void setTile(i32 x, i32 y, char c) {


	if(x >=0 && x < mapWidth && y >=0 && y < mapHeight)
		map[y*mapWidth + x] = c;
	else {
		ERROR_BREAK("Jebiga");
		//return 0;
		}


	}




void initGame() {
	if(SDL_Init(SDL_INIT_EVERYTHING)) {
		ERROR_BREAK("Init Error!!!\n");
		}

	SDL_Window *window =  SDL_CreateWindow(
	                        "asd",
	                        SDL_WINDOWPOS_CENTERED,
	                        SDL_WINDOWPOS_CENTERED,
	                        WIDTH,
	                        HEIGHT,
	                        SDL_WINDOW_OPENGL
	                      );
	if(window == NULL) {
		ERROR_BREAK("Window not inited!!!\n");
		}

	renderer = SDL_CreateRenderer(window, -1, 0);

	if(renderer == NULL) {
		ERROR_BREAK("Renderer not init!!!\n");
		}
	isClose = FALSE;


	}

static inline void tileRenderer() {

	SDL_Rect r;
	r.h = tileHeight;
	r.w = tileWidth;
	cameraX = playerPosX;
	cameraY = playerPosY;
	calculateOfset();

	f64 displasmentX = (ofsetX - (i32)ofsetX) * tileWidth;
	f64 displasmentY = (ofsetY - (i32)ofsetY) * tileHeight;
	displasmentX = 0;
	displasmentY = 0;
	for(i64 y = 0; y < mapHeight; y++) {
		for(i64 x = 0; x < mapWidth; x++) {

			char tile = getTile(x + ofsetX, y + ofsetY);
			switch(tile) {
				case '#': {
						r.x = (i32)(x)*tileWidth  - displasmentX;
						r.y = (i32)(y)*tileHeight - displasmentY;
						SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
						SDL_RenderFillRect(renderer, &r);
						break;
						}

				case '.': {
						r.x = x*tileWidth - displasmentX ;
						r.y = y*tileHeight - displasmentY;
						SDL_SetRenderDrawColor(renderer, 0, 0, 255, 0);
						SDL_RenderFillRect(renderer, &r);

						break;
						}
				default:
					break;
				}


			}

		SDL_SetRenderDrawColor(renderer, 0,255,0,0);
		r.x = playerPosX*tileWidth - ofsetX*tileWidth;
		r.y = playerPosY*tileHeight - ofsetY*tileHeight;
		SDL_RenderFillRect(renderer, &r);
		}


	}


static inline void inputHandling() {
	const u8* stateOfKeyBoard = SDL_GetKeyboardState(NULL);

	f64 displasmentX = (ofsetX - (i32)ofsetX) * tileWidth;
	f64 displasmentY = (ofsetY - (i32)ofsetY) * tileHeight;

	playerVelX = 0.0f;
	playerVelY = 0.0f;
	const f64 a = 0.1 + rand() / RAND_MAX*0.1f;
	u8 onGround = TRUE;

	if(stateOfKeyBoard[SDL_SCANCODE_W]) {
		playerVelY = -2.0f * a ;
		}
	if(stateOfKeyBoard[SDL_SCANCODE_S]) {
		playerVelY = a;
		}
	if(stateOfKeyBoard[SDL_SCANCODE_A]) {
		playerVelX = -a;
		}
	if(stateOfKeyBoard[SDL_SCANCODE_D]) {
		playerVelX = a;
		}


	f64 changeX = playerVelX * (f64)(SDL_GetTicks64() - frameStartTime);
	f64 changeY = playerVelY * (f64)(SDL_GetTicks64() - frameStartTime);
	f64 x = changeX + playerPosX;
	f64 y = changeY + playerPosY;
	//LOG("x = %f y = %f ax = %f ay = %f\n", playerPosX, playerPosY, playerVelX, playerVelY);


	if(playerVelY > 0.0f) {
		char tile = getTile(x, y + 0.9f) & getTile(x + 1.0f, y + 0.9f);

		if(tile != '.') {
			changeY = 0;
			}
	

		}
	else {
		char tile = getTile(x, y) & getTile(x + 0.9f, y);

		if(tile != '.') {
			changeY = 0;
			}
	
		}

	if(playerVelX <= 0.0f) {
		char tile = getTile(x, y) & getTile(x, y + 0.7f);

		if(tile != '.') {
			changeX = 0;
			}



		}
	else {
		char tile = getTile(x + 1.1f, y) & getTile(x + 1.1f, y + 0.7f);

		if(tile != '.') {
			changeX = 0;
			}



		}

	playerPosY  += changeY;
	playerPosX  += changeX;
	char tile = getTile(x, y + 0.9f);
	if(tile == '.') {
		playerPosY += a;
		}




	}


static inline void eventHandling(void) {
	//STA GOD DAL STATIC ILI NE
	static SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {

			case SDL_QUIT: {
					LOG("Window Closed!!!\n");
					isClose = TRUE;
					break;
					}
			}
		}
	}


#define FPS (u64)60
#define TARGETTIME (u64)(1000 / FPS)

u64 passedTime;


void loop() {
	while(!isClose) {
		frameStartTime = SDL_GetTicks64();

		SDL_RenderClear(renderer);
		tileRenderer();
		SDL_SetRenderDrawColor(renderer, 0,0,255, 0);
		SDL_RenderPresent(renderer);
		inputHandling();
		eventHandling();
		//system("pause");

		passedTime = SDL_GetTicks64() - frameStartTime;
		if(passedTime < TARGETTIME) {
			SDL_Delay((u32)(TARGETTIME - passedTime));
			//LOG("passed time %u\n", ((u32)(TARGETTIME - passedTime)));
			}
		//system("pause");



		}



	}



int main() {
	//LOG("%", map);
	//exit(-1);
	initGame();
	system("Pause");
	loop();
	printf("Nesto");
	return 0;
	}