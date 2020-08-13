//Author: Víctor Fernández (LembranT in Github)
#include <ncurses.h>//Contains bolean.h && some functions from stdio.h
#include <stdio.h>  //for r/w highscore file
#include <stdlib.h> //for srand() && abs()
#include <time.h>   //for creating a random seed with time(0)
#include <ctype.h>  //for tolower()
#include <unistd.h> //for usleep()

#define N 40  //Columns (Width)
#define M 20  //Rows (Height)
//Constants for index drawing
#define SNAKE_HEAD 1
#define SNAKE_BODY 2
#define FOOD 3

//Global variables
int Field[M+2][N+2];
int x, y, Gx, head, tail, dir='d', vel=1, score=0, HighScore;
bool food = false, gameOver = false;
FILE *f;

//Auxiliar Functions
void snakeInitialicion(){

f = fopen("highscore.txt", "r");
fscanf(f, "%d", &HighScore);
fclose(f);

  for(int r=0; r<M; r++){
    for(int c=0; c<N; c++){
      Field[r][c]=0;
    }
  }
  y = M/2; x = N/2;
  head = 8; tail = 1; Gx = x;

//Assign the values to the grid where the snake is.
  for (int i=0; i<head; i++){
    Gx++;
    Field[y][Gx-head] = i+1;  //Field[Height][Width]
    //Snake values will always be positives.
  }

}
void draw(){
  //Initialize colors.
  start_color();
  //init_pair(index, c_foreground, c_background)
  init_pair(SNAKE_HEAD, COLOR_GREEN, COLOR_BLACK);
  init_pair(SNAKE_BODY, COLOR_GREEN, COLOR_WHITE);
  init_pair(FOOD, COLOR_YELLOW, -1);  //-1 is the terminal background color.

  //Draw Horizontal Limit
  move(0,0);
  for(int c=0; c<N+2; c++){
    switch(c){
      case 0:
        addch(ACS_ULCORNER); break;
      case N+1:
        addch(ACS_URCORNER); break;
      default:
        addch(ACS_HLINE); break;
    }
  }
  for (int r=1; r<M+2; r++){
    //Draw Vertical Limit
    move(r,0);
    addch(ACS_VLINE);
    for (int c=0; c<N+2; c++){
      if (Field[r][c]==0) printw("%c", 32);
      //Draw Snake Body
      if (Field[r][c]>0 && Field[r][c]!=head){
        attron(COLOR_PAIR(SNAKE_BODY));
        addch(ACS_CKBOARD);
        attroff(COLOR_PAIR(SNAKE_BODY));
      }
      //Draw Snake Head
      if (Field[r][c]==head){
        attron(COLOR_PAIR(SNAKE_HEAD));
        addch(ACS_CKBOARD);
        attroff(COLOR_PAIR(SNAKE_HEAD));
      }
      //Draw Food
      if (Field[r][c]==-1){
        attron(COLOR_PAIR(FOOD));
        addch(ACS_DIAMOND);
        attroff(COLOR_PAIR(FOOD));
      }
      //Draw Vertical Limit
      if (c==N-1)  addch(ACS_VLINE);
    }
  }

  //Draw Horizontal Limit
  move(21,0);
  for(int c=0; c<N+2; c++){
    switch(c){
      case 0:
        addch(ACS_LLCORNER); break;
      case N+1:
        addch(ACS_LRCORNER); break;
      default:
        addch(ACS_HLINE); break;
    }
  }
  //Draw Score
  mvprintw(1,43, "Score: %i", score);
  mvprintw(3,43, "HighScore: %i", HighScore);
}
void Random(){
  srand(time(0));
  int a = 1 + rand() % 18;
  int b = 1 + rand() % 38;

  //Spawn new food if it's not available.
  if (!food && Field[a][b]==0){
    Field[a][b] = -1;
    food = true;
  }
}
int GameOverEvent(){
  sleep(1);
  beep();
  clear();
  mvprintw(10, 20, "GAME OVER !!!!");

  if (score>HighScore){
    attron(A_BOLD);
    mvprintw(14, 18, "NEW HIGHSCORE %d", score);
    attroff(A_BOLD);
    f = fopen("highscore.txt", "w");
    fprintf(f, "%d", score);
    fclose(f);
  } else {
    attron(A_BOLD);
    mvprintw(14, 22, "Score: %d", score);
    attroff(A_BOLD);
  }
  refresh();
  sleep(5);
  gameOver = true;
}
bool key_pressed() {
    int ch = getch();
    if (ch != ERR) {
        ungetch(ch);
        return true;
    } else {
        return false;
    }
}
void Movement(){
  if (key_pressed()){
    int var = getch();
    var = tolower(var);
    /*This will check the snake preserve it's movement.
    Also, the snake will only change direction if it's possible. */
    if ( ((var=='s' || var=='w') || (var=='a' || var=='d'))
    && (abs(dir-var)>5) ){  /*The difference in ascii values of A and D keys is
                            less than 5, same with S and W.*/
      dir = var;
    }
  }
  /*This will update the head position adding one to the head value.
  So if the snake (grid values) are 1 2 3 4 5
                      then will be  1 2 3 4 5 6
                          and then  1 2 3 4 5 6 7 */
  switch (dir){
    case 's':
      y++;  //Update position
      if (Field[y][x]>0) GameOverEvent();
      if (y >= M+1) y = 1; //Check Screen Limit
      if (Field[y][x]==-1) {  //Food controller
        food = false;
        score += 5;
        tail -=3;   //Increase snake size
      }
      head++; //Update head value
      Field[y][x] = head; //Update size
      break;

    case 'w':
      y--;
      if (Field[y][x]>0) GameOverEvent();
      if (y<=0) y = M;
      if (Field[y][x]==-1) {
        food = false;
        score += 5;
        tail -=3;
      }
      head++;
      Field[y][x] = head;
      break;

    case 'a':
      x--;
      if (Field[y][x]>0) GameOverEvent();
      if(x<0) x = N-1;
      if (Field[y][x]==-1) {
        food = false;
        score += 5;
        tail -=3;
      }
      head++;
      Field[y][x] = head;
      break;

    case 'd':
      x++;
      if (Field[y][x]>0) GameOverEvent();
      if (x>=N) x=0;
      if (Field[y][x]==-1) {
        food = false;
        score += 5;
        tail -=3;
      }
      head++;
      Field[y][x] = head;
      break;
  }
}
void UpdTail(){
/*When snake is moving, the head value is increased, so to keep the same snake
  length the tail has to be reduced his size by one.
  So if the snake (grid values) are 1 2 3 4 5
                      then will be  2 3 4 5 6
                          and then  3 4 5 6 7 */
  for(int r=0; r<=M; r++){
    for(int c=0; c<=N; c++){
      if (Field[r][c] == tail){
        Field[r][c] = 0;
      }
    }
  }
  tail++;
}
void timer(){
  int tick = 130000;
  usleep(tick);
}

//Main
void main(){
  initscr();   //Initialize screen
  curs_set(0); //This will make invisible the cursor.
  noecho();   //This won't allow print inputs in screen.
  nodelay(stdscr, TRUE);  //getch() won't wait until key pressed.
  use_default_colors(); //Use the terminal background colors.

  snakeInitialicion();
  while(!gameOver){
    Random();     //Randomize food location.
    Movement();   //Get inputs, update head.
    UpdTail();    //Update snake tail
    timer();      //Determines the snake speed
    draw();       //Draw event
  }
  endwin(); //Close ncurses window.
}
