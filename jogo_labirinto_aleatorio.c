#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "glut.h"

/*
Disciplina: Computação Gráfica
Professor: Jorge Luis Cavalcanti Ramos
Alunos: Rodrigo Santos Gonçalves, Murilo da Silva Ferreira
*/

#define TEMPO_DE_JOGO 1200
#define TEMPO_DE_TELAFINAL 10
#define MAX 91
#define ESTADO_MENU_RAIZ 1
#define ESTADO_MENU_DIFICULDADE 2
#define ESTADO_JOGANDO 3
#define ESTADO_VENCEU 4
#define ESTADO_PERDEU 5
#define BOTAO_JOGAR 1
#define BOTAO_DIFICULDADE 2
#define BOTAO_SAIR 3
#define BOTAO_FACIL 1
#define BOTAO_MEDIO 2
#define BOTAO_DIFICIL 3
#define KEY_ENTER 13
#define false 0
#define true 1

typedef struct lab{
    char estado[MAX][MAX];
    int posicaox_jogador;
    int posicaoy_jogador;
    int xf_labirinto;
    int yf_labirinto;
}LAB;

typedef LAB *LABIRINTO;

typedef struct nodo{
    int px;
    int py;
    struct nodo *next;
}NODO;

typedef NODO *PILHA_ENC;


void cria_pilha(PILHA_ENC *pl);
int eh_vazia(PILHA_ENC pl);
void push(PILHA_ENC *pl, int x, int y);
void top_pop(PILHA_ENC *pl, int *x, int *y);
long int rdtsc(void);
int aleatorio(int a, int b);
void inicializar_labirinto(LABIRINTO *t);
void resetar_labirinto(LABIRINTO t);
void imprimir_estado(LABIRINTO t);
void construir_labirinto(LABIRINTO l);
int gerar_posicoes(int *x, int *y, LABIRINTO l);
void gerar_sequencia(int s[], int iteracoes);
void Desenhar_Labirinto(void);
void Criar_Retangulo(int cx, int cy, int lx, int ly);
void Logotipo_Jogo(int cx, int cy, int lx, int ly);
void controle_teclado(int key, int x, int y);
void controle_teclado_ascii(unsigned char key, int x, int y);
void mover_jogador(char direcao);
void myReshapeFunc(int w, int h);
void Menu_Raiz(void);
void Menu_Dificuldade(void);
void Fluxo_do_Jogo(void);
void Timer(int valor);
void Timer_Final(int valor);
void layout_superior_jogo(void);
void DesenhaTexto(void *font, char *string);

LABIRINTO mapa;
int TAMANHO_PIXEL_LABIRINTO = 18;
int PASSO_LABIRINTO = 18;
int TRANSLACAOX_LABIRINTO = 74;
int TRANSLACAOY_LABIRINTO = 18;
int dimensao = 31;
int variavel_de_estado = ESTADO_MENU_RAIZ;
int variavel_menu_raiz = BOTAO_JOGAR;
int variavel_menu_dificuldade = BOTAO_FACIL;
int posicaox_retangulo = 339, posicaoy_retangulo = 359, largura_x = 100, largura_y = 19;
int ppx = 107, ppy = 658, ttx = 103, tty = 39;
int contador = 0, pontos = 0, saida_liberada = 0, reinicio = 0;
char mensagem_tempo[15] = "TEMPO: ";
char mensagem_pontuacao[15] = "PONTUACAO: ";
char contagem[4],  pontuacao[4];

int main(){
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH  |GLUT_RGB);
    glutInitWindowSize(700,700);
    glutInitWindowPosition(350,0);
    inicializar_labirinto(&mapa);
    construir_labirinto(mapa);
    glutCreateWindow("Jogo: Labirinto Randomico");
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glLoadIdentity();
    gluOrtho2D(0.0,700.0,0.0,700.0);
    glMatrixMode(GL_MODELVIEW);
    glutDisplayFunc(Fluxo_do_Jogo);
    glutKeyboardFunc(controle_teclado_ascii);
    glutSpecialFunc(controle_teclado);
    glutReshapeFunc(myReshapeFunc);
    PlaySound(TEXT("tetris.wav"),NULL,SND_ASYNC | SND_LOOP);
    glutMainLoop();
    return 0;
}

void Timer_Final(int valor){
    reinicio++;
    if(reinicio < TEMPO_DE_TELAFINAL){
        glutTimerFunc(1000,Timer_Final,0);
    }
    else{
        contador = 0;
        pontos = 0;
        saida_liberada = 0;
        resetar_labirinto(mapa);
        construir_labirinto(mapa);
        variavel_de_estado = ESTADO_MENU_RAIZ;
        variavel_menu_raiz = BOTAO_JOGAR;
        posicaoy_retangulo = 359;
        PlaySound(TEXT("tetris.wav"),NULL,SND_ASYNC | SND_LOOP);
    }
    glutPostRedisplay();
}


void Timer(int valor){
    contador--;
    if(contador > 0 && variavel_de_estado != ESTADO_VENCEU){
        glutTimerFunc(100,Timer,0);
    }
    else if(reinicio != TEMPO_DE_TELAFINAL && variavel_de_estado != ESTADO_VENCEU){
        variavel_de_estado = ESTADO_PERDEU;
        glutTimerFunc(1000,Timer_Final,0);
        PlaySound(TEXT("starwarsimperio.wav"),NULL,SND_ASYNC | SND_LOOP);
    }
    glutPostRedisplay();
}

void layout_superior_jogo(void){
        glColor3f(0.196100f,0.800000f,0.196100f);
        Logotipo_Jogo(117,657,113,39);
        itoa((int)contador/10,contagem,10);
        strcat(contagem,"s");
        strcat(mensagem_tempo,contagem);
        glColor3f(0.0f,0.0f,0.0f);
        glRasterPos2f(6,672);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,mensagem_tempo);
        strcpy(mensagem_tempo,"TEMPO: ");
        itoa(pontos,pontuacao,10);
        strcat(pontuacao,"%");
        strcat(mensagem_pontuacao,pontuacao);
        glRasterPos2f(6,647);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,mensagem_pontuacao);
        strcpy(mensagem_pontuacao,"PONTUACAO: ");
        glRasterPos2f(6,623);
        if(variavel_menu_dificuldade == BOTAO_FACIL)
            DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"NIVEL: FACIL");
        else if(variavel_menu_dificuldade == BOTAO_MEDIO)
            DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"NIVEL: MEDIO");
        else
            DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"NIVEL: DIFICIL");
}

void Tela_Final(void){
    glColor3f(0.196100f,0.800000f,0.196100f);
    Logotipo_Jogo(328,348,169,38);
    glColor3f(0,0,0);
    Logotipo_Jogo(328,348,162,30);
    glColor3f(0.196100f,0.800000f,0.196100f);
    glRasterPos2f(230,338);
    if(variavel_de_estado == ESTADO_VENCEU){
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"VOCE VENCEU!");
    }
    else if(variavel_de_estado == ESTADO_PERDEU){
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"VOCE PERDEU..");
    }
}


void Fluxo_do_Jogo(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    if(variavel_de_estado == ESTADO_MENU_RAIZ){
        Menu_Raiz();
    }
    else if(variavel_de_estado == ESTADO_JOGANDO){
        layout_superior_jogo();
        Desenhar_Labirinto();
    }
    else if(variavel_de_estado == ESTADO_MENU_DIFICULDADE){
        Menu_Dificuldade();
    }
    else if(variavel_de_estado == ESTADO_VENCEU || variavel_de_estado == ESTADO_PERDEU){
        Tela_Final();
    }
    glutSwapBuffers();
}

void myReshapeFunc(int w, int h){
	glutReshapeWindow(w,h);
	glViewport(0,0,w,h);
	glutPostRedisplay();
}

void mover_jogador(char direcao){
    char bloco = '@';
    bloco = bloco + 155;
    if(direcao == 'c' && mapa->estado[mapa->posicaox_jogador][mapa->posicaoy_jogador - 1] != bloco){
            if(mapa->estado[mapa->posicaox_jogador][mapa->posicaoy_jogador - 1] == '#')
                pontos = pontos + 20;
            mapa->estado[mapa->posicaox_jogador][mapa->posicaoy_jogador] = ' ';
            mapa->estado[mapa->posicaox_jogador][mapa->posicaoy_jogador - 1] = '1';
            mapa->posicaoy_jogador = mapa->posicaoy_jogador - 1;
    }
    else if(direcao == 'b' && mapa->estado[mapa->posicaox_jogador][mapa->posicaoy_jogador + 1] != bloco){
            if(mapa->estado[mapa->posicaox_jogador][mapa->posicaoy_jogador + 1] == '#')
                pontos = pontos + 20;
            mapa->estado[mapa->posicaox_jogador][mapa->posicaoy_jogador] = ' ';
            mapa->estado[mapa->posicaox_jogador][mapa->posicaoy_jogador + 1] = '1';
            mapa->posicaoy_jogador = mapa->posicaoy_jogador + 1;
    }
    else if(direcao == 'd' && mapa->estado[mapa->posicaox_jogador + 1][mapa->posicaoy_jogador] != bloco && mapa->estado[mapa->posicaox_jogador + 1][mapa->posicaoy_jogador] != 'r'){
            if(mapa->estado[mapa->posicaox_jogador + 1][mapa->posicaoy_jogador] == '#')
                pontos = pontos + 20;
            mapa->estado[mapa->posicaox_jogador][mapa->posicaoy_jogador] = ' ';
            mapa->estado[mapa->posicaox_jogador + 1][mapa->posicaoy_jogador] = '1';
            mapa->posicaox_jogador = mapa->posicaox_jogador + 1;
    }
    else if(direcao == 'e' && mapa->estado[mapa->posicaox_jogador - 1][mapa->posicaoy_jogador] != bloco && mapa->posicaox_jogador - 1 > -1){

            if(mapa->estado[mapa->posicaox_jogador - 1][mapa->posicaoy_jogador] == '#')
                pontos = pontos + 20;
            mapa->estado[mapa->posicaox_jogador][mapa->posicaoy_jogador] = ' ';
            mapa->estado[mapa->posicaox_jogador - 1][mapa->posicaoy_jogador] = '1';
            mapa->posicaox_jogador = mapa->posicaox_jogador - 1;
    }
    if(pontos == 100 && !saida_liberada){
        mapa->estado[mapa->xf_labirinto][mapa->yf_labirinto] = ' ';
        saida_liberada = 1;
    }
    if(saida_liberada && contador > 0 && mapa->estado[mapa->posicaox_jogador][mapa->posicaoy_jogador] == mapa->estado[mapa->xf_labirinto][mapa->yf_labirinto]){
        variavel_de_estado = ESTADO_VENCEU;
        reinicio = 0;
        glutTimerFunc(1000,Timer_Final,0);
        PlaySound(TEXT("starwars.wav"),NULL,SND_ASYNC | SND_LOOP);
    }
    printf("\a");
}

void controle_teclado_ascii(unsigned char key, int x, int y){
    if(variavel_de_estado == ESTADO_MENU_RAIZ && variavel_menu_raiz == BOTAO_JOGAR && key == KEY_ENTER){
        PlaySound(TEXT("topgear.wav"),NULL,SND_ASYNC | SND_LOOP);
        variavel_de_estado = ESTADO_JOGANDO;
        contador = TEMPO_DE_JOGO;
        reinicio = 0;
        glutTimerFunc(1000,Timer,0);
    }
    else if(variavel_de_estado == ESTADO_MENU_RAIZ && variavel_menu_raiz == BOTAO_DIFICULDADE && key == KEY_ENTER){
        variavel_de_estado = ESTADO_MENU_DIFICULDADE;
        variavel_menu_dificuldade = BOTAO_FACIL;
        posicaoy_retangulo = 359;
    }
    else if(variavel_de_estado == ESTADO_MENU_RAIZ && variavel_menu_raiz == BOTAO_SAIR && key == KEY_ENTER){
        exit(0);
    }
    else if(variavel_de_estado == ESTADO_MENU_DIFICULDADE && key == KEY_ENTER){
        if(variavel_menu_dificuldade == BOTAO_FACIL){
            dimensao = 31;
            TRANSLACAOX_LABIRINTO = 74;
            TRANSLACAOY_LABIRINTO = 18;
            TAMANHO_PIXEL_LABIRINTO = 18;
            PASSO_LABIRINTO = 18;
        }
        else if(variavel_menu_dificuldade == BOTAO_MEDIO){
            dimensao = 41;
            TRANSLACAOX_LABIRINTO = 50;
            TRANSLACAOY_LABIRINTO = -7;
            TAMANHO_PIXEL_LABIRINTO = 15;
            PASSO_LABIRINTO = 15;
        }
        else if(variavel_menu_dificuldade == BOTAO_DIFICIL){
            dimensao = 51;
            TRANSLACAOX_LABIRINTO = 50;
            TRANSLACAOY_LABIRINTO = -4;
            TAMANHO_PIXEL_LABIRINTO = 12;
            PASSO_LABIRINTO = 12;
        }
        resetar_labirinto(mapa);
        construir_labirinto(mapa);
        variavel_de_estado = ESTADO_MENU_RAIZ;
        variavel_menu_raiz = BOTAO_JOGAR;
        posicaoy_retangulo = 359;
    }
    glutPostRedisplay();
}

void controle_Menus(int key){
    if(variavel_de_estado == ESTADO_MENU_RAIZ){
        if(key == GLUT_KEY_UP && variavel_menu_raiz > BOTAO_JOGAR){
            variavel_menu_raiz--;
            posicaoy_retangulo = posicaoy_retangulo + 50;
        }
        else if(key == GLUT_KEY_DOWN && variavel_menu_raiz < BOTAO_SAIR){
            variavel_menu_raiz++;
            posicaoy_retangulo = posicaoy_retangulo - 50;
        }
    }
    else if(variavel_de_estado == ESTADO_MENU_DIFICULDADE){
            if(key == GLUT_KEY_UP && variavel_menu_dificuldade > BOTAO_FACIL){
                variavel_menu_dificuldade--;
                posicaoy_retangulo = posicaoy_retangulo + 50;
            }
            else if(key == GLUT_KEY_DOWN && variavel_menu_dificuldade < BOTAO_DIFICIL){
                variavel_menu_dificuldade++;
                posicaoy_retangulo = posicaoy_retangulo - 50;
            }
    }
}

void controle_jogo(int key){
    if(key == GLUT_KEY_UP){
        mover_jogador('c');
    }
    else if(key == GLUT_KEY_DOWN){
        mover_jogador('b');
    }
    else if(key == GLUT_KEY_RIGHT){
        mover_jogador('d');
    }
    else if(key == GLUT_KEY_LEFT){
        mover_jogador('e');
    }
}

void controle_teclado(int key, int x, int y){
    if(variavel_de_estado == ESTADO_MENU_RAIZ || variavel_de_estado == ESTADO_MENU_DIFICULDADE)
        controle_Menus(key);
    else if(variavel_de_estado == ESTADO_JOGANDO)
        controle_jogo(key);
    glutPostRedisplay();
}

void Desenhar_Labirinto(void){
    glPointSize(TAMANHO_PIXEL_LABIRINTO);
    glBegin(GL_POINTS);
    int contador_local = 0;
    for(int i = 0; i < dimensao; i++){
        for(int j = 0; j < dimensao; j++){
            if(mapa->estado[i][j] != ' ' && mapa->estado[i][j] != '1' && mapa->estado[i][j] != '#' && mapa->estado[i][j] != 'r'){
                glColor3f(0.0,0.0,1.0);
                glVertex2i(PASSO_LABIRINTO*i+TRANSLACAOX_LABIRINTO,PASSO_LABIRINTO*(dimensao-j)+TRANSLACAOY_LABIRINTO);
            }
            else if(mapa->estado[i][j] == '1'){
                if(contador%2==0)
                    glColor3f(0.0,1.0,0.0);
                else
                    glColor3f(0.0,0.0,0.0);
                glVertex2i(PASSO_LABIRINTO*i+TRANSLACAOX_LABIRINTO,PASSO_LABIRINTO*(dimensao-j)+TRANSLACAOY_LABIRINTO);
            }
            else if(mapa->estado[i][j] == '#'){
                glColor3f(1.0,1.0,0.0);
                glVertex2i(PASSO_LABIRINTO*i+TRANSLACAOX_LABIRINTO,PASSO_LABIRINTO*(dimensao-j)+TRANSLACAOY_LABIRINTO);
                contador_local++;
            }
            else if(mapa->estado[i][j] == 'r'){
                glColor3f(1.0,0.0,0.0);
                glVertex2i(PASSO_LABIRINTO*i+TRANSLACAOX_LABIRINTO,PASSO_LABIRINTO*(dimensao-j)+TRANSLACAOY_LABIRINTO);
            }
            else{
                glColor3f(0.0,0.0,0.0);
                glVertex2i(PASSO_LABIRINTO*i+TRANSLACAOX_LABIRINTO,PASSO_LABIRINTO*(dimensao-j)+TRANSLACAOY_LABIRINTO);
            }
        }
    }
    glEnd();
}

void DesenhaTexto(void *font, char *string){
	while(*string)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,*string++);
}

void Criar_Retangulo(int cx, int cy, int lx, int ly){
    glColor3f(0.196100f,0.800000f,0.196100f);
    glBegin(GL_QUADS);
    glVertex2i(cx - lx,cy + ly);
    glVertex2i(cx + lx,cy + ly);
    glVertex2i(cx + lx,cy - ly);
    glVertex2i(cx - lx,cy - ly);
    glEnd();
}

void Logotipo_Jogo(int cx, int cy, int lx, int ly){
    glBegin(GL_QUADS);
    glVertex2i(cx - lx,cy + ly);
    glVertex2i(cx + lx,cy + ly);
    glVertex2i(cx + lx,cy - ly);
    glVertex2i(cx - lx,cy - ly);
    glEnd();
}

void Menu_Dificuldade(void){
	glClear(GL_COLOR_BUFFER_BIT);
	if(variavel_menu_dificuldade == BOTAO_FACIL){
        Criar_Retangulo(posicaox_retangulo,posicaoy_retangulo,largura_x,largura_y);
        glColor3f(0,0,0);
        glRasterPos2f(250,350);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"FACIL");
        glColor3f(0.196100f,0.800000f,0.196100f);
        glRasterPos2f(250,300);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"MEDIO");
        glRasterPos2f(250,250);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"DIFICIL");
	}
    else if(variavel_menu_dificuldade == BOTAO_MEDIO){
        Criar_Retangulo(posicaox_retangulo,posicaoy_retangulo,largura_x,largura_y);
        glColor3f(0,0,0);
        glRasterPos2f(250,300);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"MEDIO");
        glColor3f(0.196100f,0.800000f,0.196100f);
        glRasterPos2f(250,350);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"FACIL");
        glRasterPos2f(250,250);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"DIFICIL");
    }
    else if(variavel_menu_dificuldade == BOTAO_DIFICIL){
        Criar_Retangulo(posicaox_retangulo,posicaoy_retangulo,largura_x,largura_y);
        glColor3f(0,0,0);
        glRasterPos2f(250,250);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"DIFICIL");
        glColor3f(0.196100f,0.800000f,0.196100f);
        glRasterPos2f(250,350);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"FACIL");
        glRasterPos2f(250,300);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"MEDIO");
    }
}

void Menu_Raiz(void){
	glClear(GL_COLOR_BUFFER_BIT);
	if(variavel_menu_raiz == BOTAO_JOGAR){
        Criar_Retangulo(posicaox_retangulo,posicaoy_retangulo,largura_x,largura_y);
        glColor3f(0,0,0);
        glRasterPos2f(250,350);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"JOGAR");
        glColor3f(0.196100f,0.800000f,0.196100f);
        glRasterPos2f(250,300);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"DIFICULDADE");
        glRasterPos2f(250,250);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"SAIR");
	}
    else if(variavel_menu_raiz == BOTAO_DIFICULDADE){
        Criar_Retangulo(posicaox_retangulo,posicaoy_retangulo,largura_x,largura_y);
        glColor3f(0,0,0);
        glRasterPos2f(250,300);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"DIFICULDADE");
        glColor3f(0.196100f,0.800000f,0.196100f);
        glRasterPos2f(250,350);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"JOGAR");
        glRasterPos2f(250,250);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"SAIR");
    }
    else if(variavel_menu_raiz == BOTAO_SAIR){
        Criar_Retangulo(posicaox_retangulo,posicaoy_retangulo,largura_x,largura_y);
        glColor3f(0,0,0);
        glRasterPos2f(250,250);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"SAIR");
        glColor3f(0.196100f,0.800000f,0.196100f);
        glRasterPos2f(250,350);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"JOGAR");
        glRasterPos2f(250,300);
        DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"DIFICULDADE");
    }
    glColor3f(0.196100f,0.800000f,0.196100f);
    Logotipo_Jogo(336,566,169,38);
    glColor3f(0,0,0);
    Logotipo_Jogo(336,566,162,30);
    glColor3f(0.196100f,0.800000f,0.196100f);
    glRasterPos2f(184,559);
    DesenhaTexto(GLUT_BITMAP_TIMES_ROMAN_24,"O LABIRINTO ALEATORIO");
}

long int rdtsc(void){
  unsigned long int x;
  __asm__ volatile ("rdtsc" : "=A" (x));
  return x*x;
}

int aleatorio(int a, int b){
      unsigned long int x;
    __asm__ volatile ("rdtsc" : "=A" (x));
    srand(x*x);
    return (a + rand() % (b - a));
}

void inicializar_labirinto(LABIRINTO *t){
    int i, j;
    *t = (LABIRINTO) malloc(sizeof(LAB));
    if(!(*t)){
        printf("Erro! Nao existe memoria disponivel!");
        exit(1);
    }
    else{
        char caractere = '@';
        for(i=0 ;i < MAX; i++){
            for(j=0;j < MAX;j++){
                    (*t)->estado[i][j]=caractere+155;
            }
        }
    }
}

void resetar_labirinto(LABIRINTO t){
    int i, j;
      char caractere = '@';
        for(i=0 ;i < MAX; i++){
            for(j=0;j < MAX;j++){
                    t->estado[i][j]=caractere+155;
            }
        }
}

void imprimir_estado(LABIRINTO t){
    int i, j;
    printf("\n");
    for(i=0;i<dimensao;i++){
        for(j=0;j<dimensao;j++){
            printf("%c",t->estado[j][i]);
        }
        printf("\n");
    }
}

void construir_labirinto(LABIRINTO l){
    int x, y, cont=0;
    PILHA_ENC visitados;
    cria_pilha(&visitados);
    do{
        srand(rdtsc());
        x = (rand() % (dimensao-2)) + 1;
        y = (rand() % (dimensao-2)) + 1;
    }while(x%2!=0 && y%2!=0);
    l->estado[x][y] = ' ';
    push(&visitados,x,y);
    while(!eh_vazia(visitados)){
        while(gerar_posicoes(&x,&y,l)){
            push(&visitados,x,y);
            l->estado[x][y] = ' ';
        }
        top_pop(&visitados,&x,&y);
    }
    int larg_superior = 0, larg_inferior = 0;
    for(int i = 0; i < 3; i++){
        if(l->estado[2][i] != ' ')
            larg_superior++;
        if(l->estado[2][dimensao - i] != ' ')
            larg_inferior++;
    }
    for(int i = 0, flag1 = 0, flag2 = 0; flag1 != 1 || flag2 != 1; i++){
        if(l->estado[i][dimensao - larg_inferior] != ' ')
            l->estado[i][dimensao - larg_inferior] = ' ';
        else
            flag1 = 1;
        if(l->estado[dimensao - i][larg_superior] != ' ')
            l->estado[dimensao - i][larg_superior] = ' ';
        else
            flag2 = 1;
    }
    l->estado[0][dimensao - larg_inferior] = '1';
    l->estado[dimensao - 1][larg_superior] = 'r';
    mapa->posicaox_jogador = 0;
    mapa->posicaoy_jogador = dimensao - larg_inferior;
    mapa->xf_labirinto = dimensao - 1;
    mapa->yf_labirinto = larg_superior;
    int i = 0;
    while(i < 5){
        srand(rdtsc());
        x = (rand() % (dimensao-2)) + 1;
        y = (rand() % (dimensao-2)) + 1;
        if(l->estado[x][y] == ' ' && l->estado[x][y] != '#'){
            l->estado[x][y] = '#';
            i++;
        }
    }
}

int gerar_posicoes(int *x, int *y, LABIRINTO l){
    int i, novo_x, novo_y, dado[4];
    char acoes[4] = {'c', 'b', 'd', 'e'};
    gerar_sequencia(dado,3);
    for(i = 0; i < 4; i++){
        if(acoes[dado[i]] == 'c'){
            novo_x = *x;
            novo_y = (*y) - 2;
            if(l->estado[novo_x][novo_y] != ' ' && novo_x > 0 && novo_x < dimensao - 1 && novo_y > 0 && novo_y < dimensao - 1){
                l->estado[novo_x][novo_y+1] = ' ';
                break;
            }
        }
        else if(acoes[dado[i]] == 'b'){
            novo_x = *x;
            novo_y = (*y) + 2;
            if(l->estado[novo_x][novo_y] != ' ' && novo_x > 0 && novo_x < dimensao - 1 && novo_y > 0 && novo_y < dimensao - 1){
                l->estado[novo_x][novo_y-1] = ' ';
                break;
            }

        }
        else if(acoes[dado[i]] == 'd'){
            novo_x = (*x) + 2;
            novo_y = *y;
            if(l->estado[novo_x][novo_y] != ' ' && novo_x > 0 && novo_x < dimensao - 1 && novo_y > 0 && novo_y < dimensao - 1){
                l->estado[novo_x-1][novo_y] = ' ';
                break;
            }
        }
        else{
            novo_x = (*x) - 2;
            novo_y = *y;
            if(l->estado[novo_x][novo_y] != ' ' && novo_x > 0 && novo_x < dimensao - 1 && novo_y > 0 && novo_y < dimensao - 1){
                 l->estado[novo_x+1][novo_y] = ' ';
                 break;
            }

        }
    }
    if(i < 4){
        *x = novo_x;
        *y = novo_y;
        return 1;
    }
    else{
        return 0;
    }
}

void gerar_sequencia(int s[], int iteracoes){
	int k, k1, condicao;
	condicao=false;
	k=0;
	while(k<=iteracoes)
	{
		srand(rdtsc());
		s[k] = rand() % (iteracoes+1);
		for(k1=k-1;k1>=0;k1--)
		{
			condicao=((s[k]==s[k1]) || false);
			if(condicao)
				break;
		}
		if(!condicao)
			k++;
	}
}

void cria_pilha(PILHA_ENC *pl){
    *pl = NULL;
}

int eh_vazia(PILHA_ENC pl){
    return (pl==NULL);
}

void push(PILHA_ENC *pl, int x, int y){
    NODO *novo;
    novo = (NODO *) malloc(sizeof(NODO));
    if(!novo){
        printf("Erro! Nao foi possivel alocar memoria.");
        exit(1);
    }
    novo->px = x;
    novo->py = y;
    if(eh_vazia(*pl)){
        novo->next = NULL;
    }
    else{
        novo->next = *pl;
    }
    *pl = novo;
}

void top_pop(PILHA_ENC *pl, int *x, int *y){
    NODO *aux;
    if(eh_vazia(*pl)){
        printf("\nErro! A pilha esta vazia.");
        exit(4);
    }
    aux = *pl;
    *pl = (*pl)->next;
    *x = aux->px;
    *y = aux->py;
    free(aux);
}




