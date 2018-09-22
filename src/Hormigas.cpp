#include "Hormigas.h"

#define Nw 800              
#define Nh 600

// histogram equalization

int EquaHist = 0;


//--------------------------------------------------------------
void Hormigas::setup() {

    
    ofBackground(255, 255, 255);
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(Nw,Nh);
    
	colorImg.allocate(Nw, Nh);
	grayImage.allocate(Nw, Nh);
	ofHideCursor();
    G_FPS = 30;
    
	ofSetFrameRate(G_FPS);

    DepthMap = cvCreateImage( cvSize(Nw,Nh),IPL_DEPTH_8U,1);
 
    //  loading the ants
    for (int k=1; k<7; k++) {
        LasHormigas[k-1].loadImage("AntN" + ofToString(k)+".png");
    }
    
    // Images for backround
    
    Fondo.loadImage("Suelo1024.jpg");

    
    G_FondoIndex =0;
    
    for(int i =0;i< Nob;i++){
        TheObjects[i].set(ofRandom(Nw),ofRandom(Nh));
        TargInd[i] =-1;
        TheVelocities[i].set(0.0,0.0);
        RandAngle[i] = ofRandom(360);
        AntState[i] = (int) ofRandom(6);
    }
    
    ThDis =13.4;
    Ktar = 13.55;//20.0;
    Kdam = 0.553307;//1.05;
    G_Quality=0.0001;
    G_minDistance=3;
    G_Slider5 =0.5;//0.566929;  // Hormidark
    G_Slider6 =0.79252; // Background previous 0.84252
    G_Slider7 =0.740157; // HormiOpacity
    G_Slider8 =0.61;  // Hormisize  previous .51
    
    G_capture = false;
    G_viewImage = false;
    
}

//--------------------------------------------------------------
void Hormigas::update() {

    float dt = 0.25;
    bool bNewFrame = false;
    vidGrabber.update();
    bNewFrame = vidGrabber.isFrameNew();
    if (bNewFrame){
		ofPixels ThePixels = vidGrabber.getPixels();
      colorImg.setFromPixels(ThePixels);
      grayImage = colorImg;          
	
      // update the cv images
      grayImage.flagImageChanged();        
      DepthMap = grayImage.getCvImage();
      // histogram equalization
      if (EquaHist == 1){
        cv::equalizeHist(DepthMap,DepthMap);
      }
      else if (EquaHist == 2){
        int clim = 4;
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
        clahe->setClipLimit(clim);
        cv::Mat dst;
        clahe->apply(DepthMap, DepthMap);
      }
      
      
      
      
      cv::flip(DepthMap,DepthMap,1);
 
      TheTargets.clear();
      TargetHits.clear();
          
                
      // Aux images for the corner detector
      
      vector<cv::Point2f> cornersA;
      
      int corner_count = Nob;
     
      cv::goodFeaturesToTrack(DepthMap,cornersA,
                              corner_count,
                              G_Quality,
                              G_minDistance,
                              cv::Mat(),
                              3,
                              false,0.04);
          
      // filling the targets first

      for (int k=0;k<corner_count;k++){
              TheTargets.push_back(ofVec2f(cornersA[k].x,cornersA[k].y));
              TargetHits.push_back(0.0);
      }
          
      // reseting
        
      if (!TheTargets.empty()){     
         for(int i =0;i< Nob;i++){
                TargInd[i]=-1;
          }
         for (int kt=0; kt<TheTargets.size(); kt++) {
           float MinDis =10000;
           int MinIndi =0;
           TargetHits[kt]=0;
           for (int ko=0; ko<Nob; ko++) {
              ofVec2f ErrorVec;
              ErrorVec = TheTargets[kt]-TheObjects[ko];
              float dis = ErrorVec.length();
                if ((dis < MinDis)&&(TargInd[ko]==-1)){
                   MinIndi = ko;  
                   MinDis = dis;
                   }
            }
            TargInd[MinIndi] = kt;
            TargetHits[kt]++;
        }
          
        for (int ko=0; ko<Nob; ko++) {
           float MinHits = 10000;
           ofVec2f UpdateVec;
           float MinDis =10000;
           int MinIndi =0;
           if (TargInd[ko]==-1) {
              MinDis =10000;
              for (int kt=0; kt<TheTargets.size(); kt++) {
                  ofVec2f ErrorVec;
                  ErrorVec = TheTargets[kt]-TheObjects[ko];
                  float dis = ErrorVec.length();
                  if (dis < MinDis){
                      MinDis = dis;  
                      MinIndi = kt;
                   }
               }

               TargetHits[MinIndi]++;          
               TargInd[ko] = MinIndi;
            }
            UpdateVec = TheTargets[TargInd[ko]]-TheObjects[ko];
            float newDis = UpdateVec.length();
            UpdateVec.normalize();
            ofVec2f acc;
            if (newDis < ThDis){
                acc = (newDis/10.0)*(Ktar*UpdateVec) - Kdam*TheVelocities[ko];
             }
             else{
                acc = (Ktar*UpdateVec) - Kdam*TheVelocities[ko];
             }
             TheVelocities[ko] = TheVelocities[ko] - (-dt)*acc;
             TheObjects[ko]  =  TheObjects[ko] - (-dt)*TheVelocities[ko];
            }
        
        }// closing the  if from target non empty
            
	}
    
    // saving image Code:
    
    if (G_capture) {
		ofPixels ThePixels;
		ThePixels = colorImg.getPixels();
        Colorsave.setFromPixels(ThePixels);
        Colorsave.saveImage("Color"  + ofToString(ofGetHours()) +"."
                            + ofToString(ofGetMinutes()) + "."
                            + ofToString(ofGetSeconds()) +".png");
        HormigSave.grabScreen(0, 0, ScreenX, ScreenY);
        HormigSave.saveImage("Hormigas"  + ofToString(ofGetHours()) +"."
                             + ofToString(ofGetMinutes()) + "."
                             + ofToString(ofGetSeconds()) +".png");
        G_capture=false;
        
    }
    
}

//--------------------------------------------------------------
void Hormigas::draw() {
    
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ofClear(255,255,255,255);
    ofVec2f CurrentCorner(320,10);
    
    // coherent
    ofSetColor(255,255,255,255.0*(G_Slider6));
    Fondo.draw(0,0,ScreenX,ScreenY);

    glPushMatrix();
     ofScale((float)ScreenX/(float)Nw, (float)ScreenY/(float)Nh);
     ofSetColor(255*G_Slider5,255*G_Slider5,255*G_Slider5,255*G_Slider7);
     for (int k=0; k < Nob;k++){
        CurrentCorner = TheObjects[k];
        glPushMatrix();  
          ofTranslate(CurrentCorner.x, CurrentCorner.y);
          if (TheVelocities[k].lengthSquared()>20) {
            ofRotate(180-180*atan2(TheVelocities[k].x, TheVelocities[k].y)/(CV_PI));
            AntState[k]++;
            if (AntState[k]>5) {
                AntState[k]=0;
            }
          }
          else{
            ofRotate(RandAngle[k]);
          }
          ofScale(.15*G_Slider8,.15*G_Slider8);   
          LasHormigas[AntState[k]].draw(-LasHormigas[AntState[k]].getWidth()/2.0,-LasHormigas[AntState[k]].getHeight()/2.0);
        glPopMatrix();  
       }
    glPopMatrix(); 
    
    //showing the image
    
    if(G_viewImage){
        ofSetColor(255,255,255,85);
      glPushMatrix(); 
        glTranslated(ScreenX, 0, 0);  
        glScalef(-1, 1, 0); 
        colorImg.draw(0, 0,ScreenX,ScreenY);
     glPopMatrix(); 
    }
    

    
    
    
    
   
}


//--------------------------------------------------------------
void Hormigas::exit() {

}

//--------------------------------------------------------------
void Hormigas::keyPressed (int key) {
	switch (key) {

      //   G_Quality parameter
        case OF_KEY_UP:
            G_Quality+=0.00005;
            if (G_Quality>0.001) {
                G_Quality = 0.001;
            }
            cout<<"G_Quality:"<<G_Quality<<"\n";
        break;
            
		case OF_KEY_DOWN:
            G_Quality-=0.00005;
            if (G_Quality<0.0001) {
                G_Quality = 0.0001;
            }
            cout<<"G_Quality:"<<G_Quality<<"\n";
        break;
    
        case 's':
            Kdam+=.05;
            cout<<"Kdam:"<<Kdam<<"\n";
        break;
        
        case 'x':
            Kdam-=.05;
            if (Kdam<0.01){Kdam=0.01;}
            cout<<"Kdam:"<<Kdam<<"\n";
        break; 
        
        case 'c':
            Ktar+=.05;
            cout<<"Ktar:"<<Ktar<<"\n";
        break;
            
        case 'd':
            Ktar-=.05;
            if (Ktar<0.01){Ktar=0.01;}
            cout<<"Ktar:"<<Ktar<<"\n";
        break;
            
        case 'v':
            ThDis+=.05;
            cout<<"ThDis:"<<ThDis<<"\n";
        break;
            
        case 'f':
            ThDis-=.05;
            if (ThDis<0.001){ThDis=0.001;}
            cout<<"ThDis:"<<ThDis<<"\n";
        break;
            
		case 'm':
            G_minDistance++;
            if (G_minDistance>15){G_minDistance=15;}
            cout<<"G_minDistance:"<<G_minDistance<<"\n";
        break;

		case 'n':
            G_minDistance--;
            if (G_minDistance<3){G_minDistance=3;}
            cout<<"G_minDistance:"<<G_minDistance<<"\n";
        break;

        case 'g':
            G_Slider6+=.05;
            if (G_Slider6>1.0){G_Slider6=1.0;}
            cout<<"G_Slider6_BG:"<<G_Slider6<<"\n";
        break;
            
        case 'b':
            G_Slider6-=.05;
            if (G_Slider6<0.0){G_Slider6=0.0;}
            cout<<"G_Slider6_BG:"<<G_Slider6<<"\n";
        break;
            
        case 'o':
            G_Slider8+=.01;
            if (G_Slider8>1.0){G_Slider8=1.0;}
            cout<<"G_Slider8_Size:"<<G_Slider8<<"\n";
            break;
            
        case 'p':
            G_Slider8-=.01;
            if (G_Slider8<0.0){G_Slider8=0.0;}
            cout<<"G_Slider8_Size:"<<G_Slider8<<"\n";
            break;        
		case 'i':
            G_FPS++;
            if (G_FPS>60){G_FPS=60;}
             ofSetFrameRate(G_FPS);
            cout<<"G_FPS:"<<G_FPS<<"\n";
            break;
            
		case 'u':
            G_FPS--;
            if (G_FPS<5){G_FPS=5;}
             ofSetFrameRate(G_FPS);
            cout<<"G_FPS:"<<G_FPS<<"\n";
            break;
   		case 'w':
            G_Slider5+=.01;
            if (G_Slider5>1.0){G_Slider5=1.0;}
            cout<<"G_Slider5_Dark:"<<G_Slider5<<"\n";

            break;
            
		case 'q':
            
            G_Slider5-=.01;
            if (G_Slider5<0.0){G_Slider5=0.0;}
            cout<<"G_Slider5_Dark:"<<G_Slider5<<"\n";
            break;         
        case 'y':
            G_Slider7+=.01;
            if (G_Slider7>1.0){G_Slider7=1.0;}
            cout<<"G_Slider7_opacityAnt:"<<G_Slider7<<"\n";
            
            break;
            
		case 't':
            
            G_Slider7-=.01;
            if (G_Slider7<0.0){G_Slider7=0.0;}
            cout<<"G_Slider7_opacityAnt:"<<G_Slider7<<"\n";
            break;          
            
        case 'r':
            Reset();
        break;        
            
        case '6':
            G_viewImage=!G_viewImage;
        break;
      
        case 'h':
        {
        
        EquaHist ++;
        if (EquaHist>2){EquaHist = 0;}
          
        }
      break;
      
        // saving images
        case  OF_KEY_RETURN:
            G_capture = true;
        break;
            
            
	}
}


// Reset Function
//--------------------------------------------------------------
void Hormigas::Reset() {
    G_FPS =20;
    ofSetFrameRate(G_FPS);
    for(int i =0;i< Nob;i++){
        TheObjects[i].set(ofRandom(Nw),ofRandom(Nh));
        TargInd[i] =-1;
        TheVelocities[i].set(0.0,0.0);
        RandAngle[i] = ofRandom(360);
        AntState[i] = (int) ofRandom(6);
    }
    
    ThDis =13.4;
    Ktar = 13.55;//20.0;
    Kdam = 0.553307;//1.05;
    G_Quality=0.0001;
    G_minDistance=3;
    G_Slider5 =0.5;//0.566929;  // Hormidark
    G_Slider6 =0.79252; // Background previous .84252
    G_Slider7 =0.740157; // HormiOpacity
    G_Slider8 =0.61;  // Hormisize  previous .51
    G_Slider6 =0.79252; // Background previous 0.84252
}




//--------------------------------------------------------------
void Hormigas::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void Hormigas::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void Hormigas::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void Hormigas::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void Hormigas::windowResized(int w, int h)
{}

