#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <mmsystem.h>
#include <conio.h>
/* ���İO�o�n�[�J�o�� */
#include "audio.h"
/* �t�~�٦����U���]�w */
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
drawChar()  �̫��ӰѼƤ��O�O�r���I���M�e�����C��
�i�H�ѦҤU�����C��ȹ�Ӫ�ӳ]�w�A�Q�n���C��
   0: ��     1: �t��   2: �t��   3: �t�C
   4: �t��   5: �t��   6: �t��   7: �L��
   8: �t��   9: �G��  10: �G��  11: �G�C
  12: �G��  13: �G��  14: �G��  15: ��
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
    drawString("EASY-------PRESS�@0",35,8,0,10);
    drawString("MEDIUM-----PRESS�@1",35,16,0,10);
    drawString("HARD-------PRESS�@2",35,24,0,10);
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

    initializeKeyInput();// �Ұ���L���� ��ӵ{�����u�n���@���N��F
    cur_tick = last_tick = timer();

    setMainmenu(&mainmenu);
    show_image(background, 0, 0);
    saveScreen();

    // �L�a�j��  �̭��]�t�F�o�ӵ{�����D�n�u�@
    //�ӥB�C���j��|���_��̷s���e����ܨ�ù��W
    //���O���ʵe�@�� �C���j���s�@���e��
    while (!IsEnding) {
        // �C�g�L REFRESH_RATE �� ticks �~�|��s�@���e��
        cur_tick = timer(); // �C�� tick 0.01 ��
        //sprintf(str, "%10d", cur_tick/1000);

        if (cur_tick-last_tick > REFRESH_RATE) {

            last_tick = cur_tick;


           clearScreen();
           restoreScreen();//�s�^�I��

            putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y-1, str, 14);

            // ����e�X��
            showMenu(&mainmenu);

            // ���F�n���@�s�� drawChar() ���ʧ@���ͮĪG
               //�����n�I�s�@�� drawCmdWindow() �⤧�e�e���������e�@����ܨ�ù��W
            drawCmdWindow();

        } // end of if (cur_tick % REFRESH_RATE == 0 ...

        for (k=0; k<NUM_KEYS; k++) {
            // ����q�쥻�Q���U�����A �ܦ���}�����A  �o�O���F�B�z���ۤ��񪺱��p
            if(KEY_UP(key_val[k]) && key_down[k]) { //keydown�ݦ��L���U
                key_down[k] = 0;
            }
        }
        // ��L���� �B�z���ۤ��񪺪��p
        for (k=0; k<NUM_KEYS; k++) {
            // ����q�쥻�Q���U�����A �ܦ���}�����A  �o�O���F�B�z���ۤ��񪺱��p
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

    m->large_font = read_font("font.txt");//m�Opointer
    m->x = 1;
    m->y = 1;
    m->fgcolor = 13;
    m->bgcolor = 15;
    m->num_options = 3;
    m->cursor = 0;
    for (i=0; i<m->num_options; i++) {
        m->state[i] = 0;
    }
    m->state[m->cursor] = m->state[m->cursor] | 1;  // �ثe��ܪ�����


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

