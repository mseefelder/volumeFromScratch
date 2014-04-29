#include <iostream>
#include <fstream>

#define WIDTH 10
#define HEIGHT 10

using namespace std;

int pixels = WIDTH*HEIGHT;

int main(){
	unsigned int tfArray[pixels*2];
	for (int i = 0; i < pixels*2; i+=2){
		tfArray[i] = (unsigned char)(255&0xFF);
		tfArray[i+1] = (unsigned char)(255&0xFF);
		tfArray[i+2] = (unsigned char)(255&0xFF);
		tfArray[i+3] = (unsigned char)(255&0xFF);//(max(((i-8000)/400),0)&0xFF);
	}

	ofstream tfTexture;
	tfTexture.open("transfer.raw");

	for (int i; i<pixels*4; i++){
		tfTexture<<tfArray[i];
	}
	tfTexture<<'\n';

	tfTexture.close();
	return 0;
}