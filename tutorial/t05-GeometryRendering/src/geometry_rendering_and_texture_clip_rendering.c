
//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>  
 
typedef int BOOL;
#define FALSE 0  
#define TRUE  1 
#define WALKING_ANIMATION_FRAMES 4

typedef struct { 

	SDL_Texture* mTexture;  
	int mWidth;
	int mHeight;
}LTexture;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480; 
 
//Starts up SDL and creates window
BOOL init();
//Loads media
BOOL loadMedia();
//Frees media and shuts down SDL
void close(); 
//Loads individual image as texture
LTexture loadTexture( char* path ); 
//Free LTexture
void freeLTexture(LTexture ltexture);
//Render texture 
void renderTexture(LTexture ltexture, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip);
//Get width of sprite sheet
int spriteW();
//Get height of sprite sheet
int spriteH();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//The window renderer
SDL_Renderer* gRenderer = NULL;
//Background texture
LTexture gBackgroundTexture;
//Displayed texture
LTexture gSpriteTexture;  

SDL_Rect gSpriteClips[ WALKING_ANIMATION_FRAMES ];
SDL_Rect* currentClip; 
int frame = 0;

BOOL init()
{
	int imgFlags = IMG_INIT_PNG;
	//Initialization flag
	BOOL success = TRUE; 
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = FALSE;
	}
	else
	{ 
		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = FALSE;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED  | SDL_RENDERER_PRESENTVSYNC);
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = FALSE;
			}
			else
			{ 
				//Initialize PNG loading 
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = FALSE;
				}
			}
		}
	} 
	return success;
}

BOOL loadMedia()
{
	//Loading success flag
	BOOL success = TRUE; 
	//Load PNG texture
	gBackgroundTexture = loadTexture( "media/background.png" ); 
	if( gBackgroundTexture.mTexture == NULL )
	{
		printf( "Failed to load background image!\n" );
		success = FALSE;
	}  

	gSpriteTexture = loadTexture( "media/animation.png" ); 
	if( gSpriteTexture.mTexture == NULL )
	{
		printf( "Failed to load foo image!\n" );
		success = FALSE;
	}  
	else
	{ 
		//Set sprite clips
		gSpriteClips[ 0 ].x =   0;
		gSpriteClips[ 0 ].y =   0;
		gSpriteClips[ 0 ].w =  64;
		gSpriteClips[ 0 ].h = 205;

		gSpriteClips[ 1 ].x =  64;
		gSpriteClips[ 1 ].y =   0;
		gSpriteClips[ 1 ].w =  64;
		gSpriteClips[ 1 ].h = 205;

		gSpriteClips[ 2 ].x = 128;
		gSpriteClips[ 2 ].y =   0;
		gSpriteClips[ 2 ].w =  64;
		gSpriteClips[ 2 ].h = 205;

		gSpriteClips[ 3 ].x = 196;
		gSpriteClips[ 3 ].y =   0;
		gSpriteClips[ 3 ].w =  64;
		gSpriteClips[ 3 ].h = 205;
	} 
	return success;
}

void close()
{
	//Free loaded image 
	freeLTexture(gSpriteTexture);
	freeLTexture(gBackgroundTexture);

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}
 
LTexture loadTexture(char* path){
	 
	LTexture ltexture;
	//Load image at specified path
	SDL_Surface* loadedSurface=IMG_Load(path);
	if(loadedSurface==NULL)
	{
		printf("Unable to load image %s!SDL Error: %s\n",path,SDL_GetError());
	} 
	else
	{ 
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );
		//Create texture from surface pixels  
		ltexture.mTexture=SDL_CreateTextureFromSurface(gRenderer,loadedSurface);
		if (ltexture.mTexture==NULL)
		{ 
			printf( "Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError() );
		} 
		else{
			//Get image dimensions
			ltexture.mWidth=loadedSurface->w;
			ltexture.mHeight=loadedSurface->h;
		}
		//Get rid of old surface
		SDL_FreeSurface(loadedSurface);
	} 
	//Return LTexture
	return ltexture;	
}

void freeLTexture(LTexture ltexture){

	if( ltexture.mTexture != NULL ){ 

		SDL_DestroyTexture(ltexture.mTexture);
		ltexture.mWidth=0;
		ltexture.mHeight=0;
		ltexture.mTexture=NULL;
	} 
}
 
void renderTexture(LTexture ltexture, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip){

	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, ltexture.mWidth, ltexture.mHeight };
	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	//Render to screen
	SDL_RenderCopyEx(gRenderer, ltexture.mTexture, clip, &renderQuad, angle, center, flip);
}
 
int spriteW(){

	return gSpriteTexture.mWidth/WALKING_ANIMATION_FRAMES;
}

int spriteH(){

	return gSpriteTexture.mHeight;
}
 
void geometryRendering(){

	int i; 
	SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
	SDL_Rect outlineRect = { SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
	//绘制红色填充区域 
	SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );		
	SDL_RenderFillRect( gRenderer, &fillRect );

	//绘制绿色方形轮廓线 
	SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );		
	SDL_RenderDrawRect( gRenderer, &outlineRect );

	//绘制蓝色水平线
	SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0xFF, 0xFF );		
	SDL_RenderDrawLine( gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2 );

	//绘制黄色点构成的虚线
	SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0x00, 0xFF );
	for( i = 0; i < SCREEN_HEIGHT; i += 4 )
	{
		SDL_RenderDrawPoint( gRenderer, SCREEN_WIDTH / 2, i );
	}

}

void spriteRendering(){
	
	//Render background
	SDL_RenderCopy(gRenderer,gBackgroundTexture.mTexture,NULL,NULL);
	
	//Render sprite
	currentClip = &gSpriteClips[ frame / 8 ];

	renderTexture(gSpriteTexture, (SCREEN_WIDTH - currentClip->w) / 2, (SCREEN_HEIGHT - currentClip->h) / 2, currentClip, 0, NULL, SDL_FLIP_NONE);

	//Go to next frame
	++frame; 
	//Cycle animation
	if( frame / 8 >= WALKING_ANIMATION_FRAMES )
	{
		frame = 0;
	}
}

int main( int argc, char* args[] )
{
	//Main loop flag
	BOOL quit = FALSE; 
	//Event handler
	SDL_Event e;  
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{ 
			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = TRUE;
					}
				} 
				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				//Geometry rendering
				//geometryRendering();
				
				//Sprite rendering
				spriteRendering();

				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	} 
	//Free resources and close SDL
	close(); 
	return 0;
} 