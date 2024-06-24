#include <iostream>
#include <raylib.h>
using namespace std;

float screenWidth = 1200;
float screenHeight = 800;
int score1 = 0;
int score2 = 0;
Color scoreColor = {255,255,255,255};
Color backgroundColor = {178, 61, 217,255};
// Color lightGreen = {23, 212, 99, 255};
Color circle = {255,255,255, 60};


class sound{
    private:
        Sound ballHit;
        Sound pointAdd;
        Sound gameWon;
        Sound gameStarted;
    public:
        sound(){
            InitAudioDevice();
            ballHit = LoadSound("Sounds/ballHit.wav");
            pointAdd = LoadSound("Sounds/pointAdd.wav");
            gameWon = LoadSound("Sounds/gameWon.wav");
            gameStarted = LoadSound("Sounds/gameStarted.wav");
        }
        ~sound(){
            UnloadSound(ballHit);
            UnloadSound(pointAdd);
            UnloadSound(gameWon);
            UnloadSound(gameStarted);
            CloseAudioDevice();
        }
        void playBallHit(){
            PlaySound(ballHit);
        }
        void playPointAdd(){
            PlaySound(pointAdd);
        }
        void playGameWon(){
            PlaySound(gameWon);
        }
        void playGameStarted(){
            PlaySound(gameStarted);
        }
};
class shape{
    protected:
        float x;
        float y;
    public:
        float getX(){
            return x;
        }
        float getY(){
            return y;
        }
};
class ball:public shape{
    private:
        float radius = 20;
        float speedX;
        float speedY;
        Color ballColor = {243,213,91,255};
        sound soundEffects;
    public:
        ball(){
            soundEffects.playGameStarted();
            x = screenWidth/2;
            y = screenHeight/2;
            speedX = 7;
            speedY = 7;
        }
        void draw(){
            DrawCircle(x,y,radius,ballColor);
            checkCollisionWithEdges();
            updatePosition();
        }
        void updatePosition(){
            x+=speedX;
            y+=speedY;
        }
        void checkCollisionWithEdges(){
            if(y+20>=screenHeight){
                speedY = -(speedY);
            }
            else if(y-20<=0){
                speedY = -(speedY);
            }
            else if(x-20>=screenWidth){
                resetBall();
                score1+=1;
                if(score1==10){
                    soundEffects.playGameWon();
                }
            }
            else if(x+20<=0){
                resetBall();
                score2+=1;
                if(score2==10){
                    soundEffects.playGameWon();
                }
            }
        }
        void resetBall(){
            soundEffects.playPointAdd();
            x = screenWidth/2;
            y = screenHeight/2;
            speedX = -(speedX);
            speedY = -(speedY);
        }
        void collisionWithPaddle(){
            soundEffects.playBallHit();
            speedX = -(speedX);
        }
        float getRadius(){
            return radius;
        }
};
class paddle:public shape{
    protected:
        float width;
        float height;
        float speed = 6;
        Rectangle p;
        Color paddleColor = {255,255,255,255};
    public:
        paddle(float x,float y,float width,float height){
            this->x = x;
            this->y = y;
            this->width = width;
            this->height = height;
        }
        void checkCollisionWithEdges(){
            if(y<=2){
                y = 2;
            }
            else if(y+height>=screenHeight){
                y = screenHeight - height;
            }
        }
        void checkCollisionWithPaddle(ball *ptr){
            if(CheckCollisionCircleRec(Vector2{ptr->getX(),ptr->getY()},ptr->getRadius(),Rectangle{x,y,width,height})){
                ptr->collisionWithPaddle();
            }
        }
        float getWidth(){
            return width;
        }
        float getHeight(){
            return height;
        }
};
class manualPaddle1:public paddle{
    private:

    public:
        manualPaddle1(float x,float y,float width,float height):paddle(x,y,width,height){};
        void move(){
            if(IsKeyDown(KEY_S)){
                y+=speed;
            }
            else if(IsKeyDown(KEY_W)){
                y-=speed;
            }
        }
        void draw(){
            DrawRectangleRounded(p,0.8,0,paddleColor);
            move();
            checkCollisionWithEdges();
            p = {x,y,width,height};
        }
};
class manualPaddle2:public paddle{
    private:

    public:
        manualPaddle2(float x,float y,float width,float height):paddle(x,y,width,height){};
        void move(){
            if(IsKeyDown(KEY_DOWN)){
                y+=speed;
            }
            else if(IsKeyDown(KEY_UP)){
                y-=speed;
            }
        }
        void draw(){
            DrawRectangleRounded(p,0.8,0,paddleColor);
            move();
            checkCollisionWithEdges();
            p = {x,y,width,height};
        }
};
class botPaddle:public paddle{
    private:

    public:
        botPaddle(float x,float y,float width,float height):paddle(x,y,width,height){};
        void move(float ballY){
            if(y+(height/2)>ballY){
                y-=speed;
            }
            else if(y+(height/2)<ballY){
                y+=speed;
            }
        }
        void draw(ball *ptr){
            DrawRectangleRounded(p,0.8,0,paddleColor);
            move(ptr->getY());
            checkCollisionWithEdges();
            p = {x,y,width,height};
        }
    
};
class line:public shape{
    private:
        float endPosX;
        float endPosY;
        Color centreLineColor = {255,255,255,255};
    public:
        line(float x,float y,float endPosX,float endPosY){
            this->x=x;
            this->y=y;
            this->endPosX = endPosX;
            this->endPosY=endPosY;
        }
        void draw(){
            DrawLine(x,y,endPosX,endPosY,centreLineColor);
        }
};
//implement class game for more cleaner look (at the end)
//can make colors the members of the respective classes (to make cleaner and do more oop)
class image{
    private:
        Image mainMenuImg;
        Texture2D mainMenuTex;
    public:
        image(){
            mainMenuImg = LoadImage("Images/mainmenu.png");
            mainMenuTex = LoadTextureFromImage(mainMenuImg);
            UnloadImage(mainMenuImg);
        }
        Texture2D getTexture(){
            return mainMenuTex;
        }
        ~image(){
            UnloadTexture(mainMenuTex);
        }
};
class game{
    private:
        ball *ballObj;
        manualPaddle1 *player1;
        manualPaddle2 *player2;
        botPaddle *player2C;
        line *centreLine;
    public:
        game(){
            ballObj = new ball();
            player1 = new manualPaddle1(10,screenHeight/2-60,25,120);
            player2 = new manualPaddle2(screenWidth-35,screenHeight/2-60,25,120);
            player2C = new botPaddle(screenWidth-35,screenHeight/2-60,25,120);
            centreLine = new line(screenWidth/2,0,screenWidth/2,screenHeight);
        }
        void draw2P(){
            ballObj->draw();
            player1->draw(); 
            player2->draw();
            player1->checkCollisionWithPaddle(ballObj);
            player2->checkCollisionWithPaddle(ballObj);
            centreLine->draw();
            drawScore();
        }
        void draw1P(){
            ballObj->draw();
            player1->draw();
            player2C->draw(ballObj); 
            player1->checkCollisionWithPaddle(ballObj);
            player2C->checkCollisionWithPaddle(ballObj);
            centreLine->draw();
            drawScore();
        }
        void drawScore(){
            DrawText(TextFormat("%i",score1),(screenWidth/2)/2-60/2,20,60,scoreColor);
            DrawText(TextFormat("%i",score2),+screenWidth/2+(screenWidth/2)/2-60/2,20,60,scoreColor);
        }
        void drawResult(){
            if(score1>score2){
                DrawText("Player 1 won",screenWidth/2-(screenWidth/2)/2-20,screenHeight/2-100/2,100,scoreColor);
            }
            else if(score2>score1){
                DrawText("Player 2 won",screenWidth/2-(screenWidth/2)/2-20,screenHeight/2-100/2,100,scoreColor);
            }
        }
        ~game(){
            delete ballObj;
            delete player1;
            delete player2;
            delete player2C;
            delete centreLine;
        }
};
class menu{
    private:
        image imageObj;
    public:
        int mode;
        bool pressed = false;
        void draw(){
            DrawTexture(imageObj.getTexture(),0,0,WHITE);
        }
};
int main() {
    InitWindow(screenWidth,screenHeight,"Ping-Pong");
    SetTargetFPS(60);
    game gameObj;
    menu menuObj;

    while(WindowShouldClose()==0){
        ClearBackground(backgroundColor);
        DrawCircle(screenWidth/2,screenHeight/2,180,circle);
        BeginDrawing();
        if(IsKeyPressed(KEY_ONE) && menuObj.pressed==false){
            menuObj.pressed = true;
            menuObj.mode = 1;
        }
        else if(IsKeyPressed(KEY_TWO) && menuObj.pressed==false){
            menuObj.pressed = true;
            menuObj.mode = 2;
        }
        if(menuObj.pressed == false){
            menuObj.draw();
        }
        else if(score1<10 && score2<10 && menuObj.mode==1){
            gameObj.draw1P();
        }
        else if(score1<10 && score2<10 && menuObj.mode==2){
            gameObj.draw2P();
        }
        else{
            gameObj.drawResult();
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}