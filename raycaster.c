//WADS to move player.

#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

//-----------------------------MAP----------------------------------------------
#define mapX  8      //map width
#define maplayerayY  8      //map height
#define mapS 64      //map cube size
int map[]=           //the map array. Edit to change level but keep the outer walls
{
 1,1,1,1,1,1,1,1,
 1,0,1,0,0,0,0,1,
 1,0,1,0,0,0,0,1,
 1,0,1,0,0,0,0,1,
 1,0,0,0,0,0,0,1,
 1,0,0,0,0,1,0,1,
 1,0,0,0,0,0,0,1,
 1,1,1,1,1,1,1,1,	
};

void drawMap2D()
{
 int x,y,xo,yo;
 for(y=0;y<maplayerayY;y++)
 {
  for(x=0;x<mapX;x++)
  {
   if(map[y*mapX+x]==1){ glColor3f(1,1,1);} else{ glColor3f(0,0,0);}
   xo=x*mapS; yo=y*mapS;
   glBegin(GL_QUADS); 
   glVertex2i( 0   +xo+1, 0   +yo+1); 
   glVertex2i( 0   +xo+1, mapS+yo-1); 
   glVertex2i( mapS+xo-1, mapS+yo-1);  
   glVertex2i( mapS+xo-1, 0   +yo+1); 
   glEnd();
  } 
 } 
}//-----------------------------------------------------------------------------


//------------------------PLAYER------------------------------------------------
#define M_PI 3.1415
float degToRad(int a) { return a*M_PI/180.0;}
int FixAng(int a){ if(a>359){ a-=360;} if(a<0){ a+=360;} return a;}

float playerayX,playerayY,playerDeltaX,playerDeltaY,playerAngle;

void drawPlayer2D()
{
 glColor3f(1,1,0);   glPointSize(8);    glLineWidth(4);
 glBegin(GL_POINTS); glVertex2i(playerayX,playerayY); glEnd();
 glBegin(GL_LINES);  glVertex2i(playerayX,playerayY); glVertex2i(playerayX+playerDeltaX*20,playerayY+playerDeltaY*20); glEnd();
}

void Buttons(unsigned char key,int x,int y)
{
 if(key=='a'){ playerAngle+=5; playerAngle=FixAng(playerAngle); playerDeltaX=cos(degToRad(playerAngle)); playerDeltaY=-sin(degToRad(playerAngle));} 	
 if(key=='d'){ playerAngle-=5; playerAngle=FixAng(playerAngle); playerDeltaX=cos(degToRad(playerAngle)); playerDeltaY=-sin(degToRad(playerAngle));} 
 if(key=='w'){ playerayX+=playerDeltaX*5; playerayY+=playerDeltaY*5;}
 if(key=='s'){ playerayX-=playerDeltaX*5; playerayY-=playerDeltaY*5;}
 glutPostRedisplay();
}//-----------------------------------------------------------------------------


//---------------------------Draw Rays and Walls--------------------------------
float distance(int ax,int ay,int bx,int by,int ang){ return cos(degToRad(ang))*(bx-ax)-sin(degToRad(ang))*(by-ay);}

void drawRays2D()
{
 glColor3f(0,1,1); glBegin(GL_QUADS); glVertex2i(526,  0); glVertex2i(1006,  0); glVertex2i(1006,160); glVertex2i(526,160); glEnd();	
 glColor3f(0,0,1); glBegin(GL_QUADS); glVertex2i(526,160); glVertex2i(1006,160); glVertex2i(1006,320); glVertex2i(526,320); glEnd();	 	
	
 int r,mx,my,mp,distanceOfView,side; float vx,vy,rayX,rayY,ra,xo,yo,distanceV,distanceH; 
 
 ra=FixAng(playerAngle+30);                                                              //ray set back 30 degrees
 
 for(r=0;r<60;r++)
 {
  //---Vertical--- 
  distanceOfView=0; side=0; distanceV=100000;
  float Tan=tan(degToRad(ra));
       if(cos(degToRad(ra))> 0.001){ rayX=(((int)playerayX>>6)<<6)+64;      rayY=(playerayX-rayX)*Tan+playerayY; xo= 64; yo=-xo*Tan;}//looking left
  else if(cos(degToRad(ra))<-0.001){ rayX=(((int)playerayX>>6)<<6) -0.0001; rayY=(playerayX-rayX)*Tan+playerayY; xo=-64; yo=-xo*Tan;}//looking right
  else { rayX=playerayX; rayY=playerayY; distanceOfView=8;}                                                  //looking up or down. no hit  

  while(distanceOfView<8) 
  { 
   mx=(int)(rayX)>>6; my=(int)(rayY)>>6; mp=my*mapX+mx;                     
   if(mp>0 && mp<mapX*maplayerayY && map[mp]==1){ distanceOfView=8; distanceV=cos(degToRad(ra))*(rayX-playerayX)-sin(degToRad(ra))*(rayY-playerayY);}//hit         
   else{ rayX+=xo; rayY+=yo; distanceOfView+=1;}                                               //check next horizontal
  } 
  vx=rayX; vy=rayY;

  //---Horizontal---
  distanceOfView=0; distanceH=100000;
  Tan=1.0/Tan; 
       if(sin(degToRad(ra))> 0.001){ rayY=(((int)playerayY>>6)<<6) -0.0001; rayX=(playerayY-rayY)*Tan+playerayX; yo=-64; xo=-yo*Tan;}//looking up 
  else if(sin(degToRad(ra))<-0.001){ rayY=(((int)playerayY>>6)<<6)+64;      rayX=(playerayY-rayY)*Tan+playerayX; yo= 64; xo=-yo*Tan;}//looking down
  else{ rayX=playerayX; rayY=playerayY; distanceOfView=8;}                                                   //looking straight left or right
 
  while(distanceOfView<8) 
  { 
   mx=(int)(rayX)>>6; my=(int)(rayY)>>6; mp=my*mapX+mx;                          
   if(mp>0 && mp<mapX*maplayerayY && map[mp]==1){ distanceOfView=8; distanceH=cos(degToRad(ra))*(rayX-playerayX)-sin(degToRad(ra))*(rayY-playerayY);}//hit         
   else{ rayX+=xo; rayY+=yo; distanceOfView+=1;}                                               //check next horizontal
  } 
  
  glColor3f(0,0.8,0);
  if(distanceV<distanceH){ rayX=vx; rayY=vy; distanceH=distanceV; glColor3f(0,0.6,0);}                  //horizontal hit first
  glLineWidth(2); glBegin(GL_LINES); glVertex2i(playerayX,playerayY); glVertex2i(rayX,rayY); glEnd();//draw 2D ray
    
  int ca=FixAng(playerAngle-ra); distanceH=distanceH*cos(degToRad(ca));                            //fix fisheye 
  int lineH = (mapS*320)/(distanceH); if(lineH>320){ lineH=320;}                     //line height and limit
  int lineOff = 160 - (lineH>>1);                                               //line offset
  
  glLineWidth(8);glBegin(GL_LINES);glVertex2i(r*8+530,lineOff);glVertex2i(r*8+530,lineOff+lineH);glEnd();//draw vertical wall  

  ra=FixAng(ra-1);                                                              //go to next ray
 }
}//-----------------------------------------------------------------------------


void init()
{
 glClearColor(0.3,0.3,0.3,0);
 gluOrtho2D(0,1024,510,0);
 playerayX=150; playerayY=400; playerAngle=90;
 playerDeltaX=cos(degToRad(playerAngle)); playerDeltaY=-sin(degToRad(playerAngle)); 
}

void display()
{   
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
 drawMap2D();
 drawPlayer2D();
 drawRays2D();
 glutSwapBuffers();  
}

int main(int argc, char* argv[])
{ 
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
 glutInitWindowSize(1024,510);
 glutCreateWindow("Nesto");
 init();
 glutDisplayFunc(display);
 glutKeyboardFunc(Buttons);
 glutMainLoop();
}
