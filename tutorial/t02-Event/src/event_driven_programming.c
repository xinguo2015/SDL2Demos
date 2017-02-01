//Using SDL and standard IO

#include <SDL.h>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

typedef int BOOL;
#define FALSE 0  
#define TRUE  1  
 
//Starts up SDL and creates window
BOOL init();
//Loads media
BOOL loadMedia();
//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;
//The image we will load and show on the screen
SDL_Surface* gHelloWorld = NULL;

BOOL init()
{
	//Initialization flag
	BOOL success = TRUE;
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		success = FALSE;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			success = FALSE;
		}
		else
		{
			//Get window surface
			gScreenSurface = SDL_GetWindowSurface( gWindow );
		}
	} 
	return success;
}

BOOL loadMedia()
{
	//Loading success flag
	BOOL success = TRUE; 
	//Load splash image
	gHelloWorld = SDL_LoadBMP( "media/hello_world.bmp" );
	if( !gHelloWorld ) gHelloWorld = SDL_LoadBMP( "../media/hello_world.bmp" );
	if( !gHelloWorld ) gHelloWorld = SDL_LoadBMP( "../../media/hello_world.bmp" );
	if( !gHelloWorld ) gHelloWorld = SDL_LoadBMP( "./hello_world.bmp" );
	if( !gHelloWorld ) gHelloWorld = SDL_LoadBMP( "../hello_world.bmp" );
	if( gHelloWorld == NULL )
	{
		printf( "Unable to load image %s! SDL Error: %s\n", "02_event_driven_programming/hello_world.bmp", SDL_GetError() );
		success = FALSE;
	} 
	return success;
}

void close()
{
	//Deallocate surface
	if( gHelloWorld )
	SDL_FreeSurface( gHelloWorld );
	gHelloWorld = NULL;

	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	
	//Quit SDL subsystems
	SDL_Quit();
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
				//Apply the image
				SDL_BlitSurface( gHelloWorld, NULL, gScreenSurface, NULL ); 
				//Update the surface
				SDL_UpdateWindowSurface( gWindow );
			}
		}
	} 
	//Free resources and close SDL
	close(); 

	return 0;
}
