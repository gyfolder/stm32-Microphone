#define _USE_MATH_DEFINES
#include "headerjuttf.h"
#include "MyMath.h"
//#define M_PI 3.14159265358979323846
SDL_Event event;
#undef main
int main() {
	const Uint8 *keystate;
	int quit = 1;
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("", 100, 100, 1500, 600, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);
	const int q = 4096; int Hz = 600;
	const double sampletime = 0.00001;
	while(quit==1){
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 127);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 127);
		for (unsigned long long i = 0; i < 15; i++)SDL_RenderDrawLine(renderer, i*100, 0, i*100, 600);
		vecRef input = vecAlloc(q);
		for (unsigned long long i = 0; i < q; i++)
		{
			Real f = 0.0;
			f += sin((((double)i / q)) * (2.0 * M_PI)*(double)100);
			//f += sin((((double)i / q)) * (2.0 * M_PI)*(double)200);
			//f += sin((((double)i / q)) * (2.0 * M_PI)*(double)333);
			//f += sin((((double)i / q)) * (2.0 * M_PI)*(double)890);
			//f += (((sin((((double)i / q)) * (2.0 * M_PI)*(double)203))>0.5)?1:-1);
			//f += 1.0 * sin((1.0 * M_PI * i) / q);
			//f += 4.0 * cos((2.0 * M_PI * i) / q);
			//f -= 2.0 * cos((8.0 * M_PI * i) / q);
			input->arr[i].re = f;
		}
		vecRef output = vecAlloc(q);// = NULL;
		fft(input, &output);
		for (unsigned long long i = 0; i < q; i++) {
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 127);
			SDL_RenderDrawLine(renderer, i, 200 - (input->arr[i].re * 20.0), i + 1, 200 - (input->arr[i + 1].re * 20.0));
			//SDL_RenderDrawPoint(renderer, i, 200-(input->arr[i].re));
		}
		/*
		for (long long j = (6 * 16) - 1; j >= 0; j--) {
			for (unsigned long long i = 0; i < q; i++) {
				if((input->arr[i].re) < ((j - 3 * 16) / 8.0))SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
				else SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
				SDL_RenderDrawPoint(renderer,i,j);
				//putchar((input->arr[i].re) < ((j - 3 * 16) / 8.0) ? ' ' : '#');
				//printf_s("");
			}
			//putchar('\n');
		}*/
		//putchar('\n');
		//putchar('\n');
		//putchar('\n');
		/*
		for (long long j = (6 * 16) - 1; j >= 0; j--) {
			for (unsigned long long i = 0; i < q; i++) {
				if ((input->arr[i].re) < ((j - 3 * 16) / 8.0))SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
				else SDL_SetRenderDrawColor(renderer, 0, 0, 255, 0);
				SDL_RenderDrawPoint(renderer, i, j);
				//putchar((sqrt(pow(output->arr[i].re, 2)+pow(output->arr[i].im,2))) < ((j - 3 * 16) / 8.0) ? ' ' : '#');
				//putchar((output->arr[i].im) < ((j - 3 * 16) / 8.0) ? ' ' : '#');
				//printf_s("");
			}
			//putchar('\n');
		}*/
		for (unsigned long long i = 0; i < q/2; i++) {
			SDL_SetRenderDrawColor(renderer, 0, 255, 255, 127);
			SDL_RenderDrawLine(renderer, i, 500 - sqrt(pow(output->arr[i].re, 2) + pow(output->arr[i].im, 2)), i + 1, 500 - sqrt(pow(output->arr[i+1].re, 2) + pow(output->arr[i + 1].im, 2)));
			//SDL_RenderDrawPoint(renderer, i, 500-sqrt(pow(output->arr[i].re, 2) + pow(output->arr[i].im, 2)));
		}

		SDL_RenderPresent(renderer);
		keystate = SDL_GetKeyboardState(NULL);
		//if (keystate[SDL_SCANCODE_A]) Hz++;
		//if (keystate[SDL_SCANCODE_D]) Hz--;
		vecRelease(&input);
		vecRelease(&output);
		//system("pause");
		//for (unsigned long long i = 0; i < 50; i++) {
		//	putchar('\n');
		//}
	}
	SDL_Quit();
	FreeConsole();
	return 0;
}