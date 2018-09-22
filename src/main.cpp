//#include "testApp.h"
#include "Hormigas.h"

#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	//ofSetupOpenGL(&window, 1280, 1024, OF_FULLSCREEN);
    ofSetupOpenGL(&window, 1024, 768, OF_FULLSCREEN);
    // parece que 1280 x 1024
    // revisar # de objetos y FPS (si se pueden poner por teclado)
    // default de horigas mas pequenio
    
//	ofSetupOpenGL(&window, 800, 600, OF_WINDOW);
    ofRunApp(new Hormigas());

}
