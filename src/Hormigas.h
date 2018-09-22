#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"


// number of objects
#define Nob 1500
#define ScreenX 1024
#define ScreenY 768









class Hormigas : public ofBaseApp {
	public:

		void setup();
		void update();
		void draw();
		void exit();
        void Reset();

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		

        ofVideoGrabber 		vidGrabber;
 		ofxCvColorImage		colorImg;
		ofxCvGrayscaleImage grayImage;			// grayscale depth image
    cv::Mat DepthMap;
        ofVec2f TheObjects[Nob];
        ofVec2f TheVelocities[Nob];    
        int TargInd[Nob];
        float Ktar,Kdam;

        vector<ofVec2f> TheTargets;
        vector<int> TargetHits;
        float ThDis;
        ofImage LasHormigas[6];
        ofImage Fondo;
        float RandAngle[Nob];
        int AntState[Nob];
    
        // Midi controller variables
        float G_Quality;
        int G_minDistance;
	
        float G_Slider5;
        float G_Slider6;
        float G_Slider7;
        float G_Slider8;    
        int G_FondoIndex;
    int G_FPS;
    bool G_capture;
    bool G_viewImage;
    // to save the inputs
    
    ofImage Colorsave;
    ofImage HormigSave;
        
};




