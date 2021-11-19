// SDL_Shape.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include <SDL.h>
#include <SDL_shape.h>

#include <Windows.h>

#pragma comment (lib,"sdl")

#define WIDTH 512
#define HEIGHT 512
#define TITLE "SDL shaped window test"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

void Render(){
	SDL_RenderClear(renderer);

	SDL_Rect src,dst;
	src.x = src.y = dst.x = dst.y = 0;
	SDL_QueryTexture(texture,NULL,NULL,&src.w,&src.h);
	dst.w = WIDTH;
	dst.h = HEIGHT;
	
	// 창에 그림을 그려넣는다.
	SDL_RenderCopy(renderer,texture,&src,&dst);

	SDL_RenderPresent(renderer);
}

#undef main
int _tmain(int argc, _TCHAR* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	// 모양 있는 SDL 창을 만든다.
	window = SDL_CreateShapedWindow(
		TITLE,
		0,0,
		WIDTH,HEIGHT,
		SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(
		window,
		-1,0);

	SDL_Surface *shape;
	SDL_Color colorKey = {255,255,255,255};	// 그림에서 칼라키로 쓰일 색을 설정한다.
	SDL_WindowShapeMode mode;

	shape = SDL_LoadBMP("alice.bmp");	// 창의 모양에 쓰일 그림을 불러온다.
	texture = SDL_CreateTextureFromSurface(renderer,shape);

	// 서피스가 알파 마스크를 가지고 있을경우는
	if(shape->format->Amask != 0){
		// 알파값에 의해 투명 부분을 지정하도록
		mode.mode = ShapeModeBinarizeAlpha;
		// 투명 임계치
		mode.parameters.binarizationCutoff = 255;
	}
	// 그 이외에는
	else{
		// 칼라키 방식응로 투명 부분을 지정
		mode.mode = ShapeModeColorKey;
		// 칼라키를 지정
		mode.parameters.colorKey = colorKey;
	}

	// 윈도우의 모양을 설정한다
	SDL_SetWindowShape(
		window,	// 대상 윈도우
		shape,	// 모양이 들어있는 서피스
		&mode);	// 어떤 방식으로 설정할건지 모드


	HWND shell;
	HWND me;
	RECT rect;
	
	// 작업 표시줄(젤밑에있는 시작버튼잇는창)을 찾는다
	shell = FindWindowA("Shell_TrayWnd",NULL);
	// 자기 자신의 창을 찾는다
	me = FindWindowA(NULL,TITLE);


	// 작업 표시줄 창의 영역을 구해낸다
	GetWindowRect(shell,&rect);

	// SDL창의 위치가 화면 오른쪽 아래에 가서 박히도록 설정한다.
	SDL_SetWindowPosition(
		window,
		rect.right - WIDTH,
		rect.top - HEIGHT);

	// SDL창에 layered윈도우 스타일을 준다.
	SetWindowLong(
		me,
		GWL_EXSTYLE,
		WS_EX_LAYERED);

	bool quit = false;	// SDL 어플리케이션의 종료 여부를 체크
	SDL_Event event;	// SDL 어플리케이션이 받는 이벤트를 저장하기 위해서
	bool hot = false;	// SDL 창에 마우스가 올려져 있는 상태인지 저장용 (true시 마우스 올려져잇는거)
	bool moving = false;	
	POINT pt,cpt;			// 마우스 커서 저장용/

	// 프로그램 시작시에 커서가 창 위에 있는지 구한다.
	GetCursorPos(&pt);
	if(WindowFromPoint(pt) == me)
		hot = true;

	while(!quit){
		while(SDL_PollEvent(&event)){
			switch(event.type){
			case SDL_QUIT:
				quit = true;
				break;
			// 마우스 모션 이벤트를 받았다는건, 마우스가 SDL창 위에 있는것이므로
			case SDL_MOUSEMOTION:
				hot = true;		// 핫 상태로
				SetLayeredWindowAttributes( // 창의 투명도를
					me,
					0,128,	// 128로
					LWA_ALPHA);
				break;

			case SDL_MOUSEBUTTONDOWN:
				{
					int x = event.button.x;
					int y = event.button.y;

					moving = true;
					cpt.x = x;
					cpt.y = y;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				{
					int x = event.button.x;
					int y = event.button.y;

					moving = false;
				}
				break;
			}
		}
		Render();

		// 커서 위치를 얻어옴
		GetCursorPos(&pt);
		// hot 상태일경우 커서 바로 아래의 창이 SDL 창인지 조사, 근데 아니면
		if(hot &&
			WindowFromPoint(pt) != me){
			// hot을 false로
			hot = false;
			SetLayeredWindowAttributes( // 창의 투명도를
				me,
				0,255,	// 255로
				LWA_ALPHA);
		}

		if(moving){
			GetCursorPos(&pt);
			
			SDL_SetWindowPosition(window,
				pt.x-cpt.x,pt.y-cpt.y);
		}

		SDL_Delay(1);
	}


	SDL_Quit();
	return 0;
}
