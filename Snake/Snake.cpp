#include <iostream>
#include <fstream>
#include <vector>
#include <ncurses.h>
#include <ctime>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

bool  Game_Over = false;

struct movement
{
  int x; int y;
};

class SNAKE{
  
  vector<movement> snake;    // posiciones del snake
  int direccion = KEY_LEFT;  // dirección actual del snake
  int distx,disty;           // dimenciones de la pantalla
  movement Fruta;            // posición de la fruta
  bool eliminar = true;      // se utiliza para saber cuando se elimina la última posición del snake
  int puntos = 0;            // puntos actuales en la partida
  int mejor_puntuacion = 0;  
  char cabeza = '8';         // carácter de la cabeza del snake
  char cuerpo = '0';         // carácter de la cuerpo del snake
  int tiempo = 100;          // velocidad del juego
    

  public:
    SNAKE ();
    void mostrar_snake(); 
    void mover_snake();
    void generar_fruta();
    bool colisiones();
    void mostrar_objetos();
    void record();

};
// Genera las 3 primeras posiciones del Snake y la primera posición de la fruta
SNAKE::SNAKE(){
    getmaxyx(stdscr,disty, distx);
    for(int i = 1; i <= 3; i++){
        snake.push_back({(distx / 2), (disty / 2 ) + i});
    }
    generar_fruta();
}

void SNAKE::mostrar_snake(){
    //Se llama a la función mostra_objetos y luego se imprime la Snake según las posiciones almacenadas en el vector (snake)
    mostrar_objetos();
    // Detecta si se ha chocado con algun límite de la pantalla 
     if(snake[0].x == 0 || snake[0].y == 0 || snake[0].x == distx-1|| snake[0].y == disty-1) { Game_Over = true; return;}

    for(int i = 0; i < (int)snake.size(); i++){
        int x1 = snake[i].x; int y1 = snake[i].y;
        if(i == 0)
            mvprintw(y1,x1,"%c",cabeza); // Si es la primera posición se imprime la cabeza 
        else mvprintw(y1,x1,"%c",cuerpo); // Si no lo es, se imprime el cuerpo
    }
    refresh();
}
void SNAKE::mover_snake(){
    clear();
    box(stdscr,0,0);
    mostrar_snake();
    
    if(colisiones()) Game_Over = true;
    
    movement cabeza_snake(snake.front());

    int key = getch();

    // Pausar el juego
    if(key == 32) {
        mvprintw(10,10,"Pulse BACKSPACE para continuar: ");
        refresh();
        int salir = -1;
        while(salir == -1) salir = getch();
    }
 
    // Detectar las teclas presionadas
    if(key == KEY_RIGHT && direccion != KEY_LEFT )
      direccion = KEY_RIGHT;
    else if(key == KEY_LEFT && direccion != KEY_RIGHT)
      direccion = KEY_LEFT;
    else if(key == KEY_DOWN && direccion != KEY_UP)
      direccion = KEY_DOWN;
    else if(key == KEY_UP && direccion != KEY_DOWN)
      direccion = KEY_UP;
    

    // Ajustar la dirección del SNAKE
    if(direccion == KEY_RIGHT)
        cabeza_snake.x++;
    else if(direccion == KEY_LEFT)
        cabeza_snake.x--;
    else if(direccion == KEY_DOWN)
        cabeza_snake.y++;
    else if(direccion == KEY_UP)
        cabeza_snake.y--;

    //Insertar la nueva cabeza del SNAKE
    snake.insert(snake.begin(), cabeza_snake);
    //Elimiar la última posición del SNAKE
    if(eliminar) snake.pop_back();
    else eliminar = true; 
    napms(tiempo);

} 

void SNAKE::generar_fruta(){
  srand(time(NULL));
  bool it = true;
  int x1,y1;
    while(true){
         x1 = (rand() % (distx - 2)) + 1; 
         y1 = (rand() % (disty - 2)) + 1;
        for(int i = 0; i < (int)snake.size(); i++){
            if(snake[i].x == x1 && snake[i].y == y1) it = false; // Si la posición de la fruta coincide con alguna del Snake
        }
        if(it) break;
        it = true;
    }
    Fruta.x = x1;
    Fruta.y = y1;
}
void SNAKE::mostrar_objetos(){
    mvprintw(0,10,"SCORE : %d",puntos);
    mvprintw(Fruta.y,Fruta.x,"*");
    refresh();
}
//Retorna true si se detecta que la Snake ha chocado y false en caso contrario
bool SNAKE::colisiones(){
    if(snake[0].x == Fruta.x && snake[0].y == Fruta.y) { // Si se ha alcanzado la posición de la fruta
      puntos += 5;  // Se aumentan los puntos 
      generar_fruta();  // Se genera una nueva fruta
      eliminar = false; // Permite que no se  elimine el último carácter

        if(puntos % 10 == 0) tiempo -= 2; // Cada de 10 puntos se aumenta la velocidad del juego

    }
      // Colisiones 
     for(int i = 1; i < (int)snake.size(); i++){
        if(snake[0].x == snake[i].x && snake[0].y == snake[i].y) return true;
     }
     return false;
}
// Carga la mejor puntuación y actualiza si es necesario 
void SNAKE::record(){
    ifstream archivo("Puntuación.txt");
    if(archivo.is_open()){
        archivo >> mejor_puntuacion;
        archivo.close();
    }

    int x1 = distx/2, y1 = disty/2;
    if(puntos > mejor_puntuacion){

      mvprintw(disty/2,distx/2-7,"Nuevo Récord: %d",puntos);

      ofstream archivo("Puntuación.txt");
      if(archivo.is_open()){
        archivo << puntos;
        archivo.close();
      }
     
   }
    else{
      mvprintw(disty/2,distx/2-3,"Score: %d",puntos);
    }
    
}


int main(){
    initscr();
	  keypad(stdscr, TRUE);
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);
   
    bool Juego = true;
    while(Juego){
           SNAKE s;
          Game_Over = false;

          while(!Game_Over){
              s.mover_snake();
          }
          
          mvprintw(1,1,"Salir del Juego ?");
          mvprintw(2,1," y / n :");
          s.record();
          refresh();

            int pulsar = -1;
            while(true){
                pulsar = getch();
                if(pulsar == 121 || pulsar == 110) break;
            }
            if(pulsar == 121) Juego = false;
            
      
    }
     
  

    getch();
    endwin();

    return 0;
}
