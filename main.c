#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <mmsystem.h>
/* ���İO�o�n�[�J�o�� */
#include "audio.h"
/* �t�~�٦����U���]�w */
/* For sound effect: In [Project Options]->[Parameters]->[Linker] add the parameter -lwinmm */


#include "console_draw3.h"
#include "kb_input2.h"
#include "font.h"
#include "image.h"
#include "framework.h"

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


int timer(void);

int my_game_one(void);
int my_game_two(void);
int ending(void);
int play_about(void);

void setMainmenu(Menu *m);

void assign_initial(int cells[][21]);                                           //�N��l����T��l�Ƭ�0
void print_blanks(void);                                                        //�L�X�C����l����l
void open_cell(int *op_x, int *op_y);                                           //�ШϥΪ̿�J�n���}����m
void put_mine(int cells[][21], int op_x, int op_y);                             //��m�a�p
void calculate_num_of_cells(int cells[][21]);                                   //�p��D�a�p�Ϫ��񦳴X���a�p
void assign_inf_after_open(int cells[][21], int op_x, int op_y);                //���ܨϥΪ̥��}����m����T
void open_surrounding(int cells[][21], int p_x, int op_y);                      //���}���񪺰ϰ�]���l����L�a�p�ɡ^
void check_no_mine(int cells[][21], int op_x, int op_y);                        //�ˬd���񪺰ϰ즳�S���a�p
int  check_over(int cells[][21], int op_x, int op_y);                           //�ˬd�C���O�_����(���Ĺ)
void print_cells(int cells[][21], int op_x, int op_y, int over);                //�L�X�ثe�����p
void print_condition_playing(int cells[][21]);                                  //�L�X�|�����������p
void print_condition_over(int cells[][21], int over);                           //�L�X�C�����������p(�C�Ӯ�l����T)
char play_again(void);
int about(void);
int cur_tick;                                                //�߰ݬO�_�A���@��


int main(void)
{
    int IsEnding = 0;
    int k;
    char str[40] = {'\0'};
    int key_down[NUM_KEYS] = {0};
    int key_val[NUM_KEYS] = {VK_UP, VK_DOWN, VK_ESCAPE, VK_RETURN, VK_SPACE};
    int cur_tick, last_tick;
    Menu mainmenu;
    Image* background;
    background = read_image("256.pixel", "256.color");
    Audio audio;
    openAudioFile("metalplate.wav", &audio);

    /* �Ұ���L���� ��ӵ{�����u�n���@���N��F*/
    initializeKeyInput();
    cur_tick = last_tick = timer();

    setMainmenu(&mainmenu);

    show_image(background, 0, 0);
    saveScreen();

    /* �L�a�j��  �̭��]�t�F�o�ӵ{�����D�n�u�@
    �ӥB�C���j��|���_��̷s���e����ܨ�ù��W
    ���O���ʵe�@�� �C���j���s�@���e�� */
    while (!IsEnding) {
        /* �C�g�L REFRESH_RATE �� ticks �~�|��s�@���e�� */
        cur_tick = timer(); /* �C�� tick 0.01 �� */
        sprintf(str, "%10d", cur_tick/1000);

        if (cur_tick-last_tick > REFRESH_RATE) {

            last_tick = cur_tick;


           clearScreen();
           restoreScreen();

            putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y-1, str, 14);

            /* ����e�X�� */
            showMenu(&mainmenu);

            /* ���F�n���@�s�� drawChar() ���ʧ@���ͮĪG
               �����n�I�s�@�� drawCmdWindow() �⤧�e�e���������e�@����ܨ�ù��W */
            drawCmdWindow();

        } /* end of if (cur_tick % REFRESH_RATE == 0 ... */

        for (k=0; k<NUM_KEYS; k++) {
            /* ����q�쥻�Q���U�����A �ܦ���}�����A  �o�O���F�B�z���ۤ��񪺱��p */
            if(KEY_UP(key_val[k]) && key_down[k]) {
                key_down[k] = 0;
            }
        }

        /* ��L���� �B�z���ۤ��񪺪��p */
        for (k=0; k<NUM_KEYS; k++) {
            /* ����q�쥻�Q���U�����A �ܦ���}�����A  �o�O���F�B�z���ۤ��񪺱��p */
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
                        if (IsItemSelected(&mainmenu, 0)){
                            playAudio(&audio);
                            my_game_one();
                            ending();
                        }
                    } else if (IsOnItem(&mainmenu, 1)) {
                        IsEnding = 1;


                    } else if (IsOnItem(&mainmenu, 2)) {
                        if (IsItemSelected(&mainmenu, 2)){
                            about();
                        }
                    }
                    break;
                case VK_SPACE:
                    /* �i�H�ոլݩ��U�t�@�ؿ��Φ�
                        �P�ɯ঳�h�Ӷ��إi�H�Q���
                        toggleMenu(&mainmenu);
                    */
                    radioMenu(&mainmenu);
                    break;
                }
            }
        }
    } /* while (IsEnding) */
    ending();
    return 0;
}
int timer(void)
{
    return (clock()/(0.001*CLOCKS_PER_SEC));
}
void setMainmenu(Menu *m)
{
    int i;

    m->large_font = read_font("font.txt");
    m->x = 2;
    m->y = 2;
    m->fgcolor = 4;
    m->bgcolor = 10;
    m->num_options = 3;
    m->cursor = 0;
    for (i=0; i<m->num_options; i++) {
        m->state[i] = 0;
    }
    m->state[m->cursor] = m->state[m->cursor] | 1;  /* �ثe��ܪ����� */

    strcpy(m->text[0], "START");
    strcpy(m->text[1], "EXIT");
    strcpy(m->text[2], "ABOUT");
    strcpy(m->alt_text[0], "START *");
    strcpy(m->alt_text[1], "EXIT *");
    strcpy(m->alt_text[2], "ABOUT *");
}

int play_about(void)
{

    int IsEnding = 0;

    int k;

    int key_down[NUM_KEYS] = {0};
    int key_val[NUM_KEYS] = {VK_UP, VK_DOWN, VK_ESCAPE, VK_RETURN};

    /* �Ұ���L���� ��ӵ{�����u�n���@���N��F*/
    initializeKeyInput();
/*

     ** �{���]�p�����G
     1. �N��l�ŧi��int cells[20][20]���}�C�A�s���T����
            0  �N��w�����a�ϡA�B����S�a�p
         1~ 19  �N��w�����a�ϡA�����񦳦a�p�A�Ʀr���a�p��
           -1  �N��|���Q��쪺�a�p��
        20~39  �N��w�Q������w���a��
           -2  �N��w�Q��쪺�a�p�ϡA�C������

     2. �b�ù��W�L�X��l
     3. �ШϥΪ̿�J�Ĥ@�ӭn���}����m(x, y)
     2. �Q�ζüƩ�m�a�p�Q���A�b�����ƪ���m�B���O�ϥΪ̿�J����}
     4. �b�w���a�ϭp����T(����X���a�p)
     5. �N��Q����쪺�w���a�ϭȥ[10(�N��w���)
     6. �M���ù�
     7. ���cells�}�C���A��>10����T�]>10�N��w�g����^
     8. �ШϥΪ̿�J���U�ӭn���}����m(x2, y2)
     9. �p��(x2, y2)���� (�w��-> cells[x2][y2] += 10�F�a�p-> cells[x2][y2] = -2�A�C������)
    10. �p�Gcells[x2, y2]���Ȭ�10�A�h���}���񪺮�l
    10. �Y�������h����step 6 ~ 9�A����Ĺ(������>10 or ==-1 )�N���X
    11. �߰ݬO�_�A���@���A�Y�A���@���h����2~10
*/


    while (!IsEnding) {
        /* �C�g�L REFRESH_RATE �� ticks �~�|��s�@���e�� */


        for (k=0; k<NUM_KEYS; k++) {
            if(KEY_DOWN(key_val[k]) && !key_down[k]) {
                key_down[k] = 1;
                switch (key_val[k]) {
                case VK_ESCAPE:
                    IsEnding = 1;
                    break;
                }
            }
            if(KEY_UP(key_val[k]) && key_down[k]) {
                key_down[k] = 0;
            }
        }

    }
      /*while (IsEnding)*/
    return 0;
}
/*--------------------------------------------------------------*/
int my_game_one(void)
{
    int  cells[21][21],
         open_x, open_y,
         over;
    char again;

    int IsEnding = 0;

    int k;
    Audio uccu;
    openAudioFile("999.wav", &uccu);

    int key_down[NUM_KEYS] = {0};
    int key_val[NUM_KEYS] = {VK_UP, VK_DOWN, VK_ESCAPE, VK_RETURN};


    initializeKeyInput();


    do {
       playAudio(&uccu);
       system("cls");
       assign_initial(cells);                                                   //�N��l����T��l�Ƭ�0
       print_blanks();                                                          //�L�X�C����l����l
       open_cell(&open_x, &open_y);                                             //�ШϥΪ̿�J�n���}����m
       put_mine(cells, open_x, open_y);                                         //��m�a�p
       calculate_num_of_cells(cells);                                           //�p��D�a�p�Ϫ��񦳴X�u�a�p
       assign_inf_after_open(cells, open_x, open_y);                            //���ܨϥΪ̥��}����m����T
       check_no_mine(cells, open_x, open_y);                                    //�ˬd���񪺰ϰ즳�S���a�p�A�ñN�L�a�p���ϰ쥴�}
       over = check_over(cells, open_x, open_y);                                //�ˬd�C���O�_����(���Ĺ)
       print_cells(cells, open_x, open_y, over);                                //�L�X�ثe�����p
       while (over == 0) {
             open_cell(&open_x, &open_y);
             assign_inf_after_open(cells, open_x, open_y);                      //�ШϥΪ̿�J�n���}����m
             check_no_mine(cells, open_x, open_y);                              //�ˬd���񪺰ϰ즳�S���a�p�A�ñN�L�a�p���ϰ쥴�}
             over = check_over(cells, open_x, open_y);                          //�ˬd�C���O�_����(���Ĺ)
             print_cells(cells, open_x, open_y, over);                          //�L�X�ثe�����p
       }
        pauseAudio(&uccu);
        again = play_again();                                          //�߰ݬO�_�A���@��
    } while (again=='y'||again=='Y');




 while (!IsEnding) {
        //�e����s

        //��L����
        for (k=0; k<NUM_KEYS; k++) {
            if(KEY_DOWN(key_val[k]) && !key_down[k]) {
                key_down[k] = 1;
                switch (key_val[k]) {
                case VK_RETURN:
                    IsEnding = 1;
                    break;
                }
            }
            if(KEY_UP(key_val[k]) && key_down[k]) {
                key_down[k] = 0;
            }
        }
    }


    return(0);
}

void assign_initial(int cells[][21]) {                                          //�N��l����T��l�Ƭ�0
     int i, j;
     for (i = 0; i < 21; i++)
         for (j = 0; j < 21; j++)
             cells[i][j] = 0;
}

void print_blanks(void) {                                                       //�L�X�C����l����l
     int i, j;
      printf("\n\n\n                        �C���]�p�����G\n");
     printf("             ---------------------------------------\n");
     printf("             1. ��a�p��19*19����l�A�@40���a�p\n");
     printf("             2. ��ܼƦr�����@����񪺤K�Ӧa�Ϧ@���X���a�p)\n");
     printf("             3. ����ܼƦr�h��ܪ��񪺤K�Ӧa�ϨS�a�p\n");
     printf("             ---------------------------------------\n\n\n\n\n");

    printf("                                  ");
     for (i = 1; i <= 19; i++)
         printf("%2d", i);
     printf("\n");
     for (i = 1; i <= 19; i++) {
         for (j = 0; j <= 19; j++) {
             if (j == 0)
                printf("                              %4d", i);
             else
                 printf("�f");
         }
         printf("\n");
     }
}

void open_cell(int *op_x, int *op_y) {                                          //�ШϥΪ̿�J�n���}����m
        printf("\n\n                              ");
     printf("Please input which site you want >> ");
     scanf("%d%d", op_x, op_y);
}

void put_mine(int cells[][21], int op_x, int op_y) {                            //��m�a�p
     srand(time(NULL));          //  �üƺؤl
     int  n = 0,
          rand_num,
          cell_x,
          cell_y;

     do {
         rand_num = rand() % 361;     // ( rand() % (�̤j��-�̤p��+1)) + �̤p��
         cell_x = (rand_num % 19) + 1;
         cell_y = ((rand_num - cell_x) / 19) + 1;
         if (cells[cell_x][cell_y] != -1 && !(cell_x == op_x && cell_y == op_y)) {
             cells[cell_x][cell_y] = -1;
             n++;
         }
     } while (n < 40);
}

void calculate_num_of_cells(int cells[][21]) {                                  //�p��D�a�p�Ϫ��񦳴X�u�a�p
     int i, j, u, v;
     for (i = 1; i <= 19; i++)
         for (j = 1; j <= 19; j++)
             if (cells[i][j] == 0)
                for (u = i - 1; u <= i + 1; u++)
                    for (v = j - 1; v <= j + 1; v++)
                        if (cells[u][v] == -1)
                           cells[i][j]++;
}

void assign_inf_after_open(int cells[][21], int op_x, int op_y) {               //���ܨϥΪ̥��}����m����T
     if (cells[op_x][op_y] == -1)
        cells[op_x][op_y] = -2;
     if (cells[op_x][op_y] >= 0 && cells[op_x][op_y] <= 18)
        cells[op_x][op_y] += 20;
}

void open_surrounding(int cells[][21], int op_x, int op_y) {                    //���}���񪺰ϰ�]���l����L�a�p�ɡ^
     int i, j;
     for (i = op_x - 1; i <= op_x + 1; i++) {
         for (j = op_y - 1; j <= op_y + 1; j++) {
             if (cells[i][j] >= 0 && cells[i][j] <=19 && i % 20 != 0 && j %20 != 0) {
                cells[i][j] += 20;
                check_no_mine(cells, i, j);
             }
         }
     }
}

void check_no_mine(int cells[][21], int op_x, int op_y) {                       //�ˬd���񪺰ϰ즳�S���a�p
     if (cells[op_x][op_y] == 20)        //����S�a�p
        open_surrounding(cells, op_x, op_y);
}

void print_condition_playing(int cells[][21]) {                                 //�L�X�|�����������p
     int i, j;
     printf("\n\n\n\n\n\n\n\n\n\n");
     printf("                                    ");
     for (i = 1; i <= 19; i++)
         printf("%2d", i);
     printf("\n                                  �z�ССССССССССССССССССТ{\n");
     for (i = 1; i <= 19; i++) {
             for (j = 0; j <= 19; j++) {
                 if (j == 0){
                    printf("                              ");
                    printf("%4d�U", i);
                 }
                 else if (cells[i][j] < 20)
                      printf("�f");
                 else if (cells[i][j] == 20)
                      printf(" 0");
                 else if (cells[i][j] > 20 && cells[i][j] <= 39)
                      printf(" %d", cells[i][j] - 20);
                 if (j == 19)
                    printf("�U%d", i);
         }
         printf("\n");
     }
     printf("                                  �|�ССССССССССССССССССТ}\n");
     printf("                                    ");
     for (i = 1; i <= 19; i++){

         printf("%2d", i);
     }
     printf("\n");
}


void print_condition_over(int cells[][21], int over) {                          //�L�X�C�����������p(�C�Ӯ�l����T)
     int i, j;
     printf("\n\n\n\n\n\n                                �z�ССССССССССССССССССТ{\n");
     for (i = 1; i <= 19; i++) {
         for (j = 0; j <= 20; j++) {
             if (j  == 0)
                printf("                                �U");
             else if (j == 20)
                  printf("�U");
             else if(cells[i][j] % 20 == 0)
                  printf("�D");
             else if (cells[i][j] == -1)
                  printf(" *");
             else if (cells[i][j] == -2)
                  printf("��");
             else
                 printf("%2d", cells[i][j] % 20);
         }
         printf("\n");
     }
     printf("                                �|�ССССССССССССССССССТ}\n");
     if (over == 1)
        printf("                               Congratulations~~~~~\n\n");
     else
        printf("                               Boooooooom!\n\n");
     printf("                               the figure represent how many mines are around eight sites\n");
     printf("                                * represent mine\n");
     printf("                               �D represent there didn't have mine around\n");
     printf("                               �� represent the mine which exploded\n");
     printf("                                  thanks for your playing.\n");
}

void print_cells(int cells[][21], int op_x, int op_y, int over) {               //�L�X�ثe�����p
     system("cls");
     printf("\n");
     if (over == 0) {
        print_condition_playing(cells);
     } else
           print_condition_over(cells, over);
}

int  check_over(int cells[][21], int op_x, int op_y) {                          //�ˬd�C���O�_����(���Ĺ)
    //result == 0 ->�|�������Aresult == 1 ->Ĺ�Aresult == -1 ->��
    int i, j, result = 1;
    if (cells[op_x][op_y] == -2) {
       result = -1;
    } else {
           for (i = 1; i <= 20 && result == 1; i++)
               for (j = 1; j <= 20 && result == 1; j++)
                   if ((cells[i][j] >= 0 && cells[i][j] <= 19))
                      result = 0;
    }
    return(result);
}

char play_again(void) {                                                         //�߰ݬO�_�A���@��
     char again;
     printf("\n                               Do you want to play again?");
     do {
        printf("\n                               If you want to play again please input Y\n");
        printf("                               or input N to close the game. >>");
        scanf(" %c", &again);
     } while (again != 'Y' && again != 'y' && again != 'N' && again != 'n');
     return(again);
    return 0;
}


int my_game_two(void)
{
    int IsEnding = 0;
    char logo[] = "MY BRILLIANT (OR FUNNY) GAME";
    int i, j, k;
    char str[40] = {'\0'};
    int key_down[NUM_KEYS] = {0};
    int key_val[NUM_KEYS] = {VK_UP, VK_DOWN, VK_ESCAPE, VK_RETURN};
    int cur_tick, last_tick;

    /* �Ұ���L���� ��ӵ{�����u�n���@���N��F*/
    initializeKeyInput();
    cur_tick = last_tick = timer();

    while (!IsEnding) {
        /* �C�g�L REFRESH_RATE �� ticks �~�|��s�@���e�� */
        cur_tick = timer();
        if (cur_tick-last_tick > REFRESH_RATE) {
            last_tick = timer();
            clearScreen();
            i = rand()%15;
            j = rand()%15;
            drawChar(0xA1, j*2 + OFFSET_X, i + OFFSET_Y,  0, 8);
            drawChar(0xBD, j*2+1 + OFFSET_X, i + OFFSET_Y,  0, 8);
            drawString(logo,OFFSET_X, OFFSET_Y-2,3,14);
            sprintf(str, "%10d", timer()/100);
            drawString(str,OFFSET_X, OFFSET_Y-1,1,14);
            drawCmdWindow();
        } /* end of if (cur_tick % REFRESH_RATE == 0 ... */

        for (k=0; k<NUM_KEYS; k++) {
            if(KEY_DOWN(key_val[k]) && !key_down[k]) {
                key_down[k] = 1;
                switch (key_val[k]) {
                case VK_ESCAPE:
                    IsEnding = 1;
                    break;
                }
            }
            if(KEY_UP(key_val[k]) && key_down[k]) {
                key_down[k] = 0;
            }
        }
    }  /*while (IsEnding)*/

    return 0;
}
int ending(){
    int IsEnding = 0;
    int key_down[NUM_KEYS] = {0};
    int key_val[NUM_KEYS] = {VK_UP, VK_DOWN, VK_ESCAPE, VK_RETURN};
    int cur_tick, last_tick,k,count=0;

    Image* background;
    background = read_image("789.pixel", "789.color");
    show_image(background,0,0);
    saveScreen();
    cur_tick = last_tick = timer();
    while (!IsEnding) {
        //�e����s
        cur_tick = timer();
        if(cur_tick-last_tick > 100){
            last_tick = cur_tick;
            clearScreen();
            restoreScreen();

            show_image(background,0,0);

            count ++;
            drawCmdWindow();
        }
        clearScreen();
            restoreScreen();
             drawCmdWindow();

        //��L����
        for (k=0; k<NUM_KEYS; k++) {
            if(KEY_DOWN(key_val[k]) && !key_down[k]) {
                key_down[k] = 1;
                switch (key_val[k]) {
                case VK_RETURN:
                    IsEnding = 1;
                    break;
                }
            }
            if(KEY_UP(key_val[k]) && key_down[k]) {
                key_down[k] = 0;
            }
        }
    }
    return 0;
}

int about(void){
    int IsEnding = 0;
    int key_down[NUM_KEYS] = {0};
    int key_val[NUM_KEYS] = {VK_UP, VK_DOWN, VK_ESCAPE, VK_RETURN};
    int cur_tick, last_tick,k,count=0;

    Image* background;
    background = read_image("555.pixel", "555.color");
    show_image(background,0,0);
    saveScreen();
    cur_tick = last_tick = timer();
    while (!IsEnding) {
        //�e����s
        cur_tick = timer();
        if(cur_tick-last_tick > 100){
            last_tick = cur_tick;
            clearScreen();
            restoreScreen();

            show_image(background,0,0);

            count ++;
            drawCmdWindow();
        }
        clearScreen();
            restoreScreen();
             drawCmdWindow();

        //��L����
        for (k=0; k<NUM_KEYS; k++) {
            if(KEY_DOWN(key_val[k]) && !key_down[k]) {
                key_down[k] = 1;
                switch (key_val[k]) {
                case VK_RETURN:
                    IsEnding = 1;
                    break;
                }
            }
            if(KEY_UP(key_val[k]) && key_down[k]) {
                key_down[k] = 0;
            }
        }
    }
    return 0;



}
