#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <mmsystem.h>
#include <conio.h>
/* 音效記得要加入這個 */
#include "audio.h"
/* 另外還有底下的設定 */
/* For sound effect: In [Project Options]->[Parameters]->[Linker] add the parameter -lwinmm */


#include "console_draw3.h"
#include "kb_input2.h"
#include "font.h"
#include "image.h"
#include "framework.h"
#define N 30
#define M 60

char map[N][M];
int Field[N][M], i, j, x, y, Speed, Gy, Tail, Head, Game, Frogs, score, a, b, Highscore;
char pos, Var;
char score_arr[10000];
FILE *f;
Audio audio, audio2, audio3, audio4;

int mine = 0;
int speed_mod_exist = 0;

/*
drawChar()  最後兩個參數分別是字元背景和前景的顏色
可以參考下面的顏色值對照表來設定你想要的顏色
   0: 黑     1: 暗藍   2: 暗綠   3: 暗青
   4: 暗紅   5: 暗紫   6: 暗黃   7: 淺灰
   8: 暗灰   9: 亮藍  10: 亮綠  11: 亮青
  12: 亮紅  13: 亮紫  14: 亮黃  15: 白
*/

#define WIDTH     30
#define HEIGHT    20
#define OFFSET_X  5
#define OFFSET_Y  5

#define NUM_KEYS 7
#define REFRESH_RATE 10

char about[4][12];
int timer(void);
int difficulty = 0;
int IsEnding = 0;

int my_game_one(void);
void setMainmenu(Menu *m);

int getch_noblock() // get char from keyboard;
{
    if (_kbhit())
        return _getch();
    else
        return -1;
}

void SnakeInitialization()
{
    for(i = 0 ; i < N ; i++)
    {
        for(j = 0 ; j < M ; j++)
        {
            Field[i][j] = 0;// fill field matrix with 0;
        }
    }
    x=N/2; y=M/2; Speed=200; Head=5; Tail=1; score=0; Frogs=0; Gy=y; pos = 'd'; Game=0; mine = 0; speed_mod_exist = 0;
    for(i = 0 ; i < Head ; i++)
    {
        Gy++;
        Field[x][Gy-Head] = i+1;
    }
}

int game_level(void)
{
    clearScreen();
    drawString("EASY-------PRESS　0",35,8,0,10);
    drawString("MEDIUM-----PRESS　1",35,16,0,10);
    drawString("HARD-------PRESS　2",35,24,0,10);
    drawCmdWindow();


    while(1)
    {
        Var = getch_noblock();
        //choose the difficulty-0, 1, 2
        if(Var == '0')
        {
            difficulty = 0;
            break;
        }
        else if(Var == '1')
        {
            difficulty = 1;
            break;
        }
        else if(Var == '2')
        {
            difficulty = 2;
            break;
        }
        else if(Var == 27)//keyboard ESC
        {
            return 1;
        }
    }
    return 0;
    //clearScreen();
}

// to change int type to char type numbers;
void int_to_char (int input , char temp[])
{
    if (input == 0)
    {
        temp[0] = '0';
        temp[1] = '\0';
        return;
    }
    char arr[10000];
    int i=0;
    while (input!=0)
    {
        arr[i]= input%10+'0';
        input = input/10;
        i++;
    }
    arr[i] = '\0';
    int j = strlen(arr)-1;
    i=0;
    for (;j>=0;j--)
    {
        temp[i] = arr[j];
        i++;
    }
    temp[i] = '\0';
    return;
}

void GameOver()
{
    Sleep(500);//delay

    if (difficulty == 0)
    {
        f=fopen("high.txt", "r");
        fscanf(f,"%d",&Highscore);//read high score
        fclose(f);
    }
    else if (difficulty == 1)
    {
        f=fopen("normalhigh.txt", "r");
        fscanf(f,"%d",&Highscore);//read high score
        fclose(f);
    }
    else if (difficulty == 2)
    {
        f=fopen("hardhigh.txt", "r");
        fscanf(f,"%d",&Highscore);//read high score
        fclose(f);
    }

    if (score>Highscore)// update high score;
    {
          if (difficulty == 0 ) f= fopen("high.txt","w");
          if (difficulty == 1 ) f = fopen("normalhigh.txt","w");
          if (difficulty == 2 ) f = fopen("hardhigh.txt","w");
          fprintf(f,"%d   ",score);
          fclose(f);
          Highscore = score;
    }
    pauseAudio(& audio3);
    openAudioFile("20-game-over.wav", &audio4);
    // picture
    clearScreen();
    Image *gameover;
    gameover = read_image("gameover.pixel" ,"gameover.color");
    show_image(gameover,1,0);
    drawCmdWindow();
    destroy_image(gameover);
    Sleep(1000);

    playAudio(& audio4);
    Sleep(1000);
    clearScreen();
    drawString("GAME OVER",10,10,0,10);
    drawString("YOUR SCORE",10,11,0,10);
    drawString( score_arr , 22,11,0,14);
    drawString("HIGH SCORE",10,12,0,10);
    int_to_char(Highscore, score_arr);
    drawString( score_arr , 22,12,0,14);
    drawString("PRESS ENTER TO PLAY AGAIN",10,13,0,10);
    drawString("PRESS ESC TO RETURN TO MAIN MENU",10,14,0,10);
    drawCmdWindow();

    while((Var = getch_noblock()))
    {
        if(Var == 13)// press enter
        {
            pauseAudio(& audio4);
            restartAudio(&audio3);
            Game = 0;
            clearScreen();
            SnakeInitialization();
            return ;
        //break;
        }
        else if(Var == 27)// press esc
        {
            Game = 1;
            return;
        }
    }
}

void Pozition()
    {
    Var = getch_noblock();
    if (Var == 27)// if esc is pressed
    {
        Game = 1;
        return ;
    }
    if (Var == 'r')// reset the game
    {
        Game = 0;
        clearScreen();
        SnakeInitialization();
        return ;
    }
    if (Var == 'q')//pause the game
    {
        while (1)
        {
            Var = getch_noblock();
            if (Var == 'r')//reset the game
                {
                    Game = 0;
                    clearScreen();
                    SnakeInitialization();
                    return ;
                }
            if (Var == 27)// quit the game if esc
                {
                    Game = 1;
                    return ;
                }
            if(Var== 'q')// if q , unpause.
                    break;
        }
    }
    if(((Var == 'd' || Var=='a') || (Var == 'w' || Var == 's')) && (abs(pos - Var) > 5))
        // abs(pos - Var) > 5 to determine if var and pos are a,d or w,s. if it is not those 2 pairs, update pos.
        pos = Var;
    if(pos == 'd')
    {
        y++;
        if(Field[x][y] != 0 && Field[x][y] != -1 && Field[x][y] != -3) GameOver();// collide with tail.
        else if(y == M-1)// collide with boundary.
        {
            if (difficulty>0) GameOver();// gameover if normal
            else y=1;// if easy, continue.
        }
            if(Field[x][y]==-1)// if collide with food
            {
                score = score + 5;
                if(Speed - score/2 >0 && difficulty >1)// if hard, speed up.
                    Speed = Speed - score/2;
                Tail--;
                Tail--;
                Frogs=0;
            }
            if(Field[x][y] == -3)// if collide with speedmod
            {
                Speed = abs(Speed + 75 * ((rand()%3)-2));
                speed_mod_exist=0;
            }
            Head++;
            Field[x][y] = Head;
    }

        if(pos == 'a')
        {
            y--;
            if(y == 0)
            {
                if(difficulty>0) GameOver();
                else y = M-2;
            }
            else if(Field[x][y] != 0 && Field[x][y] != -1 && Field[x][y] != -3) GameOver();
            if(Field[x][y]==-1)
            {
                score =score + 5;
                if(Speed - score/2 >0 && difficulty >1)
                Speed = Speed - score/2;
                Tail--;
                Tail--;
                Frogs=0;
            }
            if(Field[x][y] == -3)// if collide with speedmod
            {
                Speed = abs(Speed + 75 * ((rand()%3)-2));
                speed_mod_exist=0;
            }
            Head++;
            Field[x][y] = Head;
        }

        if(pos == 'w')
        {
            x--;
            if(x == 0)
            {
                if (difficulty>0) GameOver();
                else x=N-2;
            }
            else if(Field[x][y] != 0 && Field[x][y] != -1 && Field[x][y] != -3) GameOver();
            if(Field[x][y]==-1)
            {
                score = score + 5;
                if(Speed - score/2 >0 && difficulty >1)
                Speed = Speed - score/2;
                Tail--;
                Tail--;
                Frogs = 0;
            }
            if(Field[x][y] == -3)// if collide with speedmod
            {
                Speed = abs(Speed + 75 * ((rand()%3)-2));
                speed_mod_exist=0;
            }
            Head++;
            Field[x][y] = Head;
        }

        if(pos == 's')
        {
            x++;
            if(x == N-1)
            {
                //x = 0;
                if(difficulty>0) GameOver();
                else x=1;

            }
            else if(Field[x][y] != 0 && Field[x][y] != -1 && Field[x][y] != -3) GameOver();
            if(Field[x][y]==-1)
            {
                score = score + 5;
                if(Speed - score/2 >0 && difficulty >1)
                    Speed = Speed - score/2;
                Tail--;
                Tail--;
                Frogs=0;
            }
            if(Field[x][y] == -3)// if collide with speedmod
            {
                Speed = abs(Speed + 75 * ((rand()%3)-2));
                speed_mod_exist=0;
            }
            Head++;
            Field[x][y] = Head;
        }
    }
void read_map(void)
{
    int i, j;
    FILE *fp;
    fp = fopen("map.txt", "r");
        for (i = 0; i < N; i++)
        {
            for (j = 0; j < M; j++)
            {
                fscanf(fp, "%c", &map[i][j]);
            }
            fscanf(fp, "%*c");  // to eat '\n' from each sentence's end. "%*c" means read 1 word from buffer and clean it from buffer.
        }
        fclose(fp);

}

void show_map(void)
{
    int i, j;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            drawChar(map[i][j] , j, i, 0 , a);// draw the boundaries
        }
    }
    for(i = 0 ; i < N ; i++)
    {
        for(j = 0 ; j < M ; j++)
        {
            if(Field[i][j] > 0 && Field[i][j] != Head)
                drawChar( 'O' ,j,i,0,11);// if it is not zero and head, print body.
            if(Field[i][j] == Head)
                drawChar( 'O' , j, i, 0,12);//head
            if(Field[i][j] == -1)
                drawChar( 'O' ,j ,i,0,15);//food
            if(Field[i][j] == -2)
                drawChar( '*' ,j ,i,0,10);//mine;
            if(Field[i][j] == -3)
                drawChar( '?' ,j , i, 0, 14);//speed modifier.
        }
    }
    // show score;
    int_to_char(score,score_arr);
    drawString("Score : " , 25 , N+2 , 0 ,10 );
    drawString( score_arr , 33 , N+2 , 0 , 15);
}



void TailRemove()
{
     for(i = 0 ; i < N ; i++)
    {
        for(j = 0 ; j < M ; j++)
        {
            if(Field[i][j] == Tail)
            {
                Field[i][j]=0;
            }
        }
    }
    Tail++;
}



void Random()//generate food.
{
    a = 1 + rand()%28;
    b = 1 + rand()%58;
    int i;
    if(Frogs == 0 && Field[a][b] == 0)//no food and nothing at that point
    {
        Field[a][b] = -1;
        Frogs = 1;
        if ( mine == 0){//generate mine;
        for (i=0;i<(difficulty+1)*10 ; i++)
        {
            a = 1 + rand()%28;
            b = 1 + rand()%58;
            Field[a][b] = -2;
        }
        mine =1;
        }
    }
    if(speed_mod_exist == 0 && Field[a][b] == 0 )// speedmod
    {
        a = 1 + rand()%28;
        b = 1 + rand()%58;
        Field[a][b] = -3;// speedmod
        speed_mod_exist = 1;
    }
}

int main(void)
{
    int k;
    char str[40] = {'\0'};
    int key_down[NUM_KEYS] = {0};
    int key_val[NUM_KEYS] = {VK_UP, VK_DOWN, VK_ESCAPE, VK_RETURN, VK_SPACE};
    int cur_tick, last_tick;
    Menu mainmenu;
    Image* background;
    background = read_image("background66.pixel", "background66.color");
    openAudioFile("metalplate.wav", &audio);
    openAudioFile("21-pause.wav", &audio2);

    initializeKeyInput();// 啟動鍵盤控制 整個程式中只要做一次就行了
    cur_tick = last_tick = timer();

    setMainmenu(&mainmenu);
    show_image(background, 0, 0);
    saveScreen();

    // 無窮迴圈  裡面包含了這個程式的主要工作
    //而且每次迴圈會不斷把最新的畫面顯示到螢幕上
    //像是播動畫一樣 每次迴圈更新一次畫面
    while (!IsEnding) {
        // 每經過 REFRESH_RATE 個 ticks 才會更新一次畫面
        cur_tick = timer(); // 每個 tick 0.01 秒
        //sprintf(str, "%10d", cur_tick/1000);

        if (cur_tick-last_tick > REFRESH_RATE) {

            last_tick = cur_tick;


           clearScreen();
           restoreScreen();//叫回背景

            putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y-1, str, 14);

            // 把選單畫出來
            showMenu(&mainmenu);

            // 為了要讓一連串 drawChar() 的動作產生效果
               //必須要呼叫一次 drawCmdWindow() 把之前畫的全部內容一次顯示到螢幕上
            drawCmdWindow();

        } // end of if (cur_tick % REFRESH_RATE == 0 ...

        for (k=0; k<NUM_KEYS; k++) {
            // 按鍵從原本被按下的狀態 變成放開的狀態  這是為了處理按著不放的情況
            if(KEY_UP(key_val[k]) && key_down[k]) { //keydown看有無按下
                key_down[k] = 0;
            }
        }
        // 鍵盤控制 處理按著不放的狀況
        for (k=0; k<NUM_KEYS; k++) {
            // 按鍵從原本被按下的狀態 變成放開的狀態  這是為了處理按著不放的情況
            if(KEY_DOWN(key_val[k]) && !key_down[k]) {
                key_down[k] = 1;

                switch (key_val[k]) {
                case VK_UP:
                    scrollMenu(&mainmenu, -1);
                    break;
                case VK_DOWN:
                    scrollMenu(&mainmenu, +1);
                    break;
                case VK_RETURN:
                    if (IsOnItem(&mainmenu, 0)) {

                            playAudio(&audio);
                            Sleep(300);
                            if( game_level() == 1)
                                break;
                            my_game_one();
                            pauseAudio(&audio3);
                            pauseAudio(&audio4);
                            break;
                    }
                    else if(IsOnItem(&mainmenu, 1)){

                            playAudio(&audio);
                            clearScreen();
                            drawString("CONTROL KEYS",40,2,0,12);
                            drawString("UP-----------W",40,4,0,10);
                            drawString("DOWN---------S",40,5,0,10);
                            drawString("LEFT---------A",40,6,0,10);
                            drawString("RIGHT--------D",40,7,0,10);
                            drawString("PAUSE--------Q",40,8,0,10);
                            drawString("RESTART------R",40,9,0,10);
                            drawString("EXIT---------ESC",40,10,0,10);

                            drawString("SPECIAL ICON",40,12,0,12);
                            drawString("O : Food           -> Eat it to increase your scores           ",20,14,0,15);
                            drawString("? : SPEED MODIFIER -> Eat it to lower or upper your speed       ",20,15,0,14);
                            drawString("* : MINE           -> Oh gosh! If you eat it, game over",20,16,0,10);

                            drawString("MODE",44,18,0,12);
                            drawString("EASY MODE  : Game over when hit itself.",20,20,0,11);
                            drawString("NORMAL MODE: Game over when hit itself or wall.",20,21,0,12);
                            drawString("HARD MODE  : The more the snake eats, the faster the snake moves.",20,22,0,13);
                            drawString("             Game over when hit itself or wall.",20,23,0,13);
                            drawCmdWindow();
                            system("pause");
                    }
                    else if (IsOnItem(&mainmenu, 2)){
                        playAudio(&audio2);
                        Sleep(1000);
                        IsEnding = 1;
                    }
                    break;
                }
            }
        }
    }
    return 0;
}

int timer(void)
{
    return (clock()/(0.001*CLOCKS_PER_SEC));
}

void setMainmenu(Menu *m)
{
    int i;

    m->large_font = read_font("font.txt");//m是pointer
    m->x = 1;
    m->y = 1;
    m->fgcolor = 13;
    m->bgcolor = 15;
    m->num_options = 3;
    m->cursor = 0;
    for (i=0; i<m->num_options; i++) {
        m->state[i] = 0;
    }
    m->state[m->cursor] = m->state[m->cursor] | 1;  // 目前選擇的項目


    strcpy(m->text[0], "START");
    strcpy(m->text[2], "EXIT");
    strcpy(m->text[1], "ABOUT");

}

int my_game_one(void)
{

    read_map();

    openAudioFile("01-intro-1.wav", &audio3);

    SnakeInitialization();
    while(Game == 0)
    {
        playAudio(&audio3);
        show_map();
        drawCmdWindow();
        Random();
        Pozition();
        TailRemove();
        Sleep(Speed);//making delay;
        clearScreen();
    }
    clearScreen();
    drawCmdWindow();
    clearScreen();
    drawCmdWindow();
    return 0;
}

