#include<stdio.h>
#include<stdlib.h>
#include<raylib.h>
#include<math.h>
#define ARRSIZE(ARR) sizeof(ARR)/sizeof(ARR[0])

typedef struct ball
{
   Vector2 positions;
   Vector2 speed;
   Vector2 acceleration;
    int r; //radius
}Ball;

typedef struct Player
{
    Vector2 position;
    Vector2 speed;
    Vector2 acceleration;
    int r;
    int angle;   //otientation
    float strength;   //for runing
    bool direction;     //for kick
}Player;


void DrawField()//Drawing field
{
    int x=GetScreenWidth(),y=GetScreenHeight();

    DrawLine(x/2,100,x/2,y-100,GREEN);
    DrawCircleLines(x/2,y/2,20,GREEN); //circle
    DrawRectangleLines(50,100,x-100,y-200,GREEN);//field
    Vector2 field[]={{50,100},{50,y-200},{x-100,y-200},{x-100,100}};
    DrawRectangle(50,y/2-75,4,150,GREEN);       //GOL LEFT
    DrawRectangleLines(50,y/2-75,50,150,GREEN);
    DrawRectangleLines(x-100,y/2-75,50,150,GREEN);//GOAL RIGHT
    DrawRectangle(x-50-4,y/2-75,4,150,GREEN);
}


//void update()


void DrawBall(Ball *b)
{
        
    //b->positions.x=100;
    //b->positions.y=100;
    //b->speed.x=1;
    //b->speed.y=1;

    //DECRES SPEED
    //b->speed.x+=b->acceleration.x;
    //b->speed.y+=b->acceleration.y;
    b->positions.x+=b->speed.x;
    b->positions.y+=b->speed.y;
    
    if(b->speed.x>0)b->speed.x--;
    if(b->speed.x<0)b->speed.x++;

    if(b->speed.y>0)b->speed.y--;
    if(b->speed.y<0)b->speed.y++;

    DrawCircleV(b->positions,5,GREEN);
}

bool iscoliding=false;
char goal[]="0   -   0";








void BallColision(Ball*b,Rectangle field,Player p1)
{
  iscoliding=false;
     //goalchecking 
    int x=GetScreenWidth(),y=GetScreenHeight();
    Rectangle left={20,y/2-75,34,150},right={x-50-4,y/2-75,20,150};
    
    //DrawRectangle(50,y/2-75,4,150,GREEN);       //GOL LEFT
          //DrawRectangleRec(left,RED);
          //DrawRectangleRec(right,RED);

    if(CheckCollisionCircleRec(b->positions,b->r,left))
        {
            goal[0]+=1;
            b->speed.x=0;
            b->speed.y=0;
            b->positions.x=GetScreenWidth()/2;
            b->positions.y=GetScreenHeight()/2;
            b->r=4;
            
        }

     if(CheckCollisionCircleRec(b->positions,b->r,right))
        {
            goal[8]+=1;
            b->speed.x=0;
            b->speed.y=0;
            b->positions.x=GetScreenWidth()/2;
            b->positions.y=GetScreenHeight()/2;
            b->r=4;
            
        }

    if(goal[0]=='9')
        {
                DrawText("Left win",320,50,40,GREEN);
                //goal[0]='0';
                //goal[8]='0';
        }
    else if (goal[8]=='9')
    {
                DrawText("Right win",320,50,40,GREEN);
                //goal[0]='0';
                //goal[8]='0';
                
    }
    else
        {DrawText(goal,320,50,40,GREEN);}





  if(!CheckCollisionCircleRec(b->positions,1,field)) //checking colison with a terein
    {

        b->speed.x=0;
        b->speed.y=0;
        b->positions.x=GetScreenWidth()/2;
        b->positions.y=GetScreenHeight()/2;
        b->r=4;

    }
    if(CheckCollisionCircles(b->positions,b->r,p1.position,p1.r)) //checking colison with a player
    {iscoliding=true;if(IsKeyPressed(KEY_D))if(p1.direction==1){b->speed.x=30;}else{b->speed.x=-30;};}
  
}


void KickBall(Player *p,Ball *b)
{
        //if(IsKeyPressed(KEY_D)){if()b->speed.x+=30;};
}

Player DrawPlayer(Player *p,Ball *b)
{
for(size_t i ;i<12;i++)
//size_t i = 0;
{
    p[i].r=20;
    if(IsKeyPressed(KEY_W)){p[i].speed.x+=5;}
    if(IsKeyPressed(KEY_UP)){p[i].position.y-=10;if(iscoliding==true)                                    b->speed.y-=7;}
    if(IsKeyPressed(KEY_DOWN)){p[i].position.y+=10;if(iscoliding==true)                                  b->speed.y+=7;}
    if(IsKeyPressed(KEY_LEFT)){p[i].position.x-=(10+p[i].speed.x);p[i].direction=0;if(iscoliding==true)  b->speed.x-=7;}
    if(IsKeyPressed(KEY_RIGHT)){p[i].position.x+=(10+p[i].speed.x);p[i].direction=1;if(iscoliding==true) b->speed.x+=7;}
    if(p[i].speed.x>0){p[i].speed.x--;}
    DrawCircle(p[i].position.x,p[i].position.y,20,RED);
    DrawCircle(p[i].position.x,p[i].position.y,10,WHITE);
   
}
}



void InitPlayers(Player *p)
{
    int x=GetScreenWidth(),y=GetScreenHeight();

    {     //PLAYERS POSITIONS
        p[0].position.x=50;               p[6].position.x=x-50;
        p[0].position.x=y/2;              p[6].position.y=y/2;
        

        p[1].position.x=100;              p[7].position.x=x-100;     
        p[1].position.y=100;              p[7].position.y=100;


        p[2].position.x=100;              p[8].position.x=x-100;        
        p[2].position.y=y/2;              p[8].position.y=y/2;
     

        p[3].position.x=100;              p[9].position.x=x-100;
        p[3].position.y=y-200;            p[9].position.y=y-200;

        p[4].position.x=x/2-100;          p[10].position.x=x/2+100;
        p[4].position.y=y/2-50;           p[10].position.x=y/2-50;

        p[5].position.x=x/2-100;          p[11].position.x=x/2+100;
        p[5].position.y=y/2+50;           p[11].position.y=y/2+50;

        for (size_t i = 0; i < 12; i++)
        {
            p[i].speed.x=0;
            p[i].speed.y=0;
            p[i].direction=0;
            p[i].strength=0;
            p[i].r=20;

            //if(p%2==0){p.c}

        }
        

    }
}

int main(void)
{
    Player p[12];   //first 6 red team last 6 blue team
     Ball b;
    InitWindow(800,600,"ASD");
    InitPlayers(p);
    SetTargetFPS(60);
    printf("Nesto");
    b.positions.x=200;
    b.positions.y=200;
    b.speed.x=4;
    b.speed.y=4;
    b.acceleration.x=0;b.acceleration.y=0;
    Player p1;
    Player p2;
    p1.position.x=100;
    p1.position.y=100;
    p1.acceleration.x=1;
    p1.acceleration.y=1;

    int x=GetScreenWidth(),y=GetScreenHeight();
    Rectangle field={50,100,x-100,y-200};

    while (!WindowShouldClose())
    {
        BeginDrawing();
            DrawField(); 
            DrawBall(&b);
            DrawPlayer(p,&b);
            BallColision(&b,field,p[1]);
            //ClearBackground(BLACK);
        EndDrawing();
    }
    

    return 0;
}