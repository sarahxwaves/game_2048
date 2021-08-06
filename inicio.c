#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// 16 blocos
struct bloco {
    float x;
    float y;
    int valor;
    int imagem;
    int bloqueado;
};

//Declaraçoes
ALLEGRO_DISPLAY *disp = NULL;  //janela
ALLEGRO_BITMAP *fundo = NULL;  //fundo
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_BITMAP **imagens = NULL;
ALLEGRO_FONT *fontes[2];
ALLEGRO_FONT *fonte1 = NULL;
ALLEGRO_FONT *fonte2 = NULL;
ALLEGRO_SAMPLE *sample_Sons[4];
ALLEGRO_BITMAP *vetImagens[3];


//Tela, teclado, imagem e sons
const float VELOCIDADE = 4;
const int LARGURA_TELA = 700;
const int ALTURA_TELA = 700;

//Ponteiros
float **matrizPosicoesX;
float **matrizPosicoesY;
struct bloco **matrizBlocos;

void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

//Iniciando Jogo... Tela, Teclado e Imagem
int inicializar(){
    
    if(!al_init()){
        fprintf(stderr,"%s", "Falha ao iniciar o Allegro! \n"); //em caso de erro de inicialização do allegro
        return 0;
    }

    //al_set_new_display_flags(ALLEGRO_OPENGL);//função para uso de OpenGL ao inves de Directx
    al_init_font_addon(); //Addon de fonte sendo inicializado

    if(!al_init_image_addon()){ //inicializa allegro_imagem, caso haja erro
        fprintf(stderr,"%s", "Falha na inicialização do allegro_image! \n");
    }

    if (!al_install_keyboard()) { //inicializa o teclado, caso haja erro
        fprintf(stderr,"%s", "Falha ao inicializar o teclado.\n"); 
        return 1;
    }

    if (!al_init_ttf_addon()) { // caso haja falha no allegro_ttf
        fprintf(stderr,"%s", "Falha ao inicializar add-on allegro_ttf.\n");
        return 0;
    }


    //audio
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(16);
    

    //Vitoria
    sample_Sons[0]= al_load_sample("Sons/Vitoria2.wav"); 
    must_init(sample_Sons[0], "Vitoria2");
    // Derrota
    sample_Sons[1]= al_load_sample("Sons/DerrotaTrombeta.wav"); 
    must_init(sample_Sons[1], "DerrotaTrombeta");
    //Movimento
    sample_Sons[2]= al_load_sample("Sons/Movimento.wav");
    must_init(sample_Sons[2], "Movimento");
    //junta Bloco
    sample_Sons[3]= al_load_sample("Sons/BlocoAumenta.wav");
    must_init(sample_Sons[3], "BlocoAumenta");
    

    //Criação da janela
    disp = al_create_display(LARGURA_TELA, ALTURA_TELA);  // inicializando display
    if (!disp)  {
        fprintf(stderr,"%s", "Falha ao criar a janela de Display.\n");
        return 1;
    }

    fontes[0] = al_load_font("fontes/babyblocks.ttf", 72, 0);
    fontes[1] = al_load_font("fontes/8-bitArcadeOut.ttf", 72, 0);
    if (!fontes) {
        fprintf(stderr,"%s", "Falha ao carregar a fonte\n");
        al_destroy_display(disp);
        return 0;
    }

    fonte1 = al_load_font("fontes/babyblocks.ttf", 72, 0);
    if (!fonte1) {
        fprintf(stderr,"%s", "Falha ao carregar a fonte\n");
        al_destroy_display(disp);
        return 0;
    }

    fonte2 = al_load_font("fontes/8-bitArcadeOut.ttf", 72, 0);
    if (!fonte2) {
        fprintf(stderr,"%s", "Falha ao carregar a fonte\n");
        al_destroy_display(disp);
        return 0;
    }

    al_set_window_title(disp, "2048"); //nome da janela

    fila_eventos = al_create_event_queue(); //cria uma nova fila vazia, devolvendo um ponteiro para o objeto recem criado
    if (!fila_eventos)  {
        fprintf(stderr,"%s", "Falha ao criar fila de eventos.\n");
        al_destroy_display(disp);
        return 1;
    }

    fundo = al_load_bitmap("imagens/fundo.png"); //imagem de fundo
    if (!fundo)  {
        fprintf(stderr,"%s", "Falha ao carregar imagem de fundo.\n");
        al_destroy_display(disp);
        al_destroy_event_queue(fila_eventos);
        return 1;
    }

    //imagens
    vetImagens[0] = al_load_bitmap("imagens/Venceu.png"); 
    vetImagens[1] = al_load_bitmap("imagens/Continuar.png"); 
    vetImagens[2] = al_load_bitmap("imagens/Perdeu.png"); 

    if (!vetImagens){
        fprintf(stderr,"%s", "Falha ao carregar imagem de fundo.\n");
        al_destroy_display(disp);
        al_destroy_event_queue(fila_eventos);
        return 1;
    }

    al_register_event_source(fila_eventos, al_get_keyboard_event_source()); //Registro de eventos.
    al_register_event_source(fila_eventos, al_get_display_event_source(disp));

    return 1;
}

//separa cada bloco;
void preencheMatrizPosicao(){ 
    int i;

    matrizPosicoesX = malloc(sizeof(int*)*4);
    matrizPosicoesY = malloc(sizeof(int*)*4);
    for(i = 0; i < 4; i++) {
        *(matrizPosicoesX+i) = malloc(sizeof(int)*4);
        *(matrizPosicoesY+i) = malloc(sizeof(int)*4);
    }

    //1
    *(*(matrizPosicoesX+0)+0) = 14;
    *(*(matrizPosicoesY+0)+0) = 14;

    //2
    *(*(matrizPosicoesX+1)+0) = 14;
    *(*(matrizPosicoesY+1)+0) = 185;

    //3
    *(*(matrizPosicoesX+2)+0) = 14;
    *(*(matrizPosicoesY+2)+0) = 357;

    //4
    *(*(matrizPosicoesX+3)+0) = 14;
    *(*(matrizPosicoesY+3)+0) = 529;

    //5
    *(*(matrizPosicoesX+0)+1) = 185;
    *(*(matrizPosicoesY+0)+1) = 14;

    //6
    *(*(matrizPosicoesX+1)+1) = 185;
    *(*(matrizPosicoesY+1)+1) = 185;

    //7
    *(*(matrizPosicoesX+2)+1) = 185;
    *(*(matrizPosicoesY+2)+1) = 357;

    //8
    *(*(matrizPosicoesX+3)+1) = 185;
    *(*(matrizPosicoesY+3)+1) = 529;

    //9
    *(*(matrizPosicoesX+0)+2) = 357;
    *(*(matrizPosicoesY+0)+2) = 14;

    //10
    *(*(matrizPosicoesX+1)+2) = 357;
    *(*(matrizPosicoesY+1)+2) = 185;

    //11
    *(*(matrizPosicoesX+2)+2) = 357;
    *(*(matrizPosicoesY+2)+2) = 357;

    //12
    *(*(matrizPosicoesX+3)+2) = 357;
    *(*(matrizPosicoesY+3)+2) = 529;

    //13
    *(*(matrizPosicoesX+0)+3) = 529;
    *(*(matrizPosicoesY+0)+3) = 14;

    //14
    *(*(matrizPosicoesX+1)+3) = 529;
    *(*(matrizPosicoesY+1)+3) = 185;

    //15
    *(*(matrizPosicoesX+2)+3) = 529;
    *(*(matrizPosicoesY+2)+3) = 357;

    //16
    *(*(matrizPosicoesX+3)+3) = 529;
    *(*(matrizPosicoesY+3)+3) = 529;
}

//Preenche os blocos
void preencheMatrizBlocos(){ //matriz 4x4
    matrizBlocos = malloc(sizeof(struct bloco*)*4);
    int i,j;
    for(i = 0; i < 4; i++) {
        *(matrizBlocos+i) = malloc(sizeof(struct bloco)*4);
        for(j = 0; j < 4; j++) {
            (*(*(matrizBlocos+i)+j)).x = -1;
            (*(*(matrizBlocos+i)+j)).y = -1;
            (*(*(matrizBlocos+i)+j)).valor = 0;
            (*(*(matrizBlocos+i)+j)).imagem = NULL;
            (*(*(matrizBlocos+i)+j)).bloqueado = 0;
        }
    }
}

//vetor com as imagens blocos
void iniciaImagens(){ 
    imagens = malloc(sizeof(ALLEGRO_BITMAP*)*11);
    *imagens = al_load_bitmap("imagens/img_2.png");
    *(imagens+1) = al_load_bitmap("imagens/img_4.png");
    *(imagens+2) = al_load_bitmap("imagens/img_8.png");
    *(imagens+3) = al_load_bitmap("imagens/img_16.png");
    *(imagens+4) = al_load_bitmap("imagens/img_32.png");
    *(imagens+5) = al_load_bitmap("imagens/img_64.png");
    *(imagens+6) = al_load_bitmap("imagens/img_128.png");
    *(imagens+7) = al_load_bitmap("imagens/img_256.png");
    *(imagens+8) = al_load_bitmap("imagens/img_512.png");
    *(imagens+9) = al_load_bitmap("imagens/img_1024.png");
    *(imagens+10) = al_load_bitmap("imagens/img_2048.png");
}

//FUNÇÕES:

//FUNÇOES MOVIMENTAÇÃO

//funcao de troca de posição dos blocos
void trocaPosicao(int destX,int destY, int operandoX,int operandoY,int op){

    if(op == 1) { //movimento pra cima
        (*(*(matrizBlocos+operandoX)+operandoY)).y -= VELOCIDADE;
        if( (*(*(matrizBlocos+operandoX)+operandoY)).y <= (*(*(matrizPosicoesY+destX)+destY)) ) {

            if((*(*(matrizBlocos+destX)+destY)).valor == 0) { //movimenta bloco
                (*(*(matrizBlocos+destX)+destY)).imagem = (*(*(matrizBlocos+operandoX)+operandoY)).imagem;
                (*(*(matrizBlocos+destX)+destY)).valor = (*(*(matrizBlocos+operandoX)+operandoY)).valor;

            }else { //junta bloco
                (*(*(matrizBlocos+destX)+destY)).imagem += 1;
                (*(*(matrizBlocos+destX)+destY)).valor *= 2;
                (*(*(matrizBlocos+destX)+destY)).bloqueado = 1;
            }

            (*(*(matrizBlocos+destX)+destY)).x = (*(*(matrizPosicoesX+destX)+destY));
            (*(*(matrizBlocos+destX)+destY)).y = (*(*(matrizPosicoesY+destX)+destY));

            blocoEsvazia(operandoX,operandoY);
        }

    }else if(op == 2) { //movimento pra baixo
        (*(*(matrizBlocos+operandoX)+operandoY)).y += VELOCIDADE;
        if((*(*(matrizBlocos+operandoX)+operandoY)).y >= (*(*(matrizPosicoesY+destX)+destY))) {

            if((*(*(matrizBlocos+destX)+destY)).valor == 0) { //movimenta bloco
                (*(*(matrizBlocos+destX)+destY)).imagem = (*(*(matrizBlocos+operandoX)+operandoY)).imagem;
                (*(*(matrizBlocos+destX)+destY)).valor = (*(*(matrizBlocos+operandoX)+operandoY)).valor;
            }else { //junta bloco
                (*(*(matrizBlocos+destX)+destY)).imagem += 1;
                (*(*(matrizBlocos+destX)+destY)).valor *= 2;
                (*(*(matrizBlocos+destX)+destY)).bloqueado = 1;
            }

            (*(*(matrizBlocos+destX)+destY)).x = (*(*(matrizPosicoesX+destX)+destY));
            (*(*(matrizBlocos+destX)+destY)).y = (*(*(matrizPosicoesY+destX)+destY));

            blocoEsvazia(operandoX,operandoY);
        }

    }else if(op == 3) { //movimento pra esquerda
        (*(*(matrizBlocos+operandoX)+operandoY)).x -= VELOCIDADE;

        if((*(*(matrizBlocos+operandoX)+operandoY)).x <= (*(*(matrizPosicoesX+destX)+destY))) {

            if((*(*(matrizBlocos+destX)+destY)).valor == 0) { //movimenta bloco
                (*(*(matrizBlocos+destX)+destY)).imagem = (*(*(matrizBlocos+operandoX)+operandoY)).imagem;
                (*(*(matrizBlocos+destX)+destY)).valor = (*(*(matrizBlocos+operandoX)+operandoY)).valor;

            }else { //junta bloco
                (*(*(matrizBlocos+destX)+destY)).imagem += 1;
                (*(*(matrizBlocos+destX)+destY)).valor *= 2;
                (*(*(matrizBlocos+destX)+destY)).bloqueado = 1;
            }

            (*(*(matrizBlocos+destX)+destY)).x = (*(*(matrizPosicoesX+destX)+destY));
            (*(*(matrizBlocos+destX)+destY)).y = (*(*(matrizPosicoesY+destX)+destY));

            blocoEsvazia(operandoX,operandoY);
        }
    }else if (op == 4){ //movimento pra direita
        (*(*(matrizBlocos+operandoX)+operandoY)).x += VELOCIDADE;

        if((*(*(matrizBlocos+operandoX)+operandoY)).x >= (*(*(matrizPosicoesX+destX)+destY))) {

            if((*(*(matrizBlocos+destX)+destY)).valor == 0) { //movimenta bloco
                (*(*(matrizBlocos+destX)+destY)).imagem = (*(*(matrizBlocos+operandoX)+operandoY)).imagem;
                (*(*(matrizBlocos+destX)+destY)).valor = (*(*(matrizBlocos+operandoX)+operandoY)).valor;

            }else { //junta bloco
               (*(*(matrizBlocos+destX)+destY)).imagem += 1;
               (*(*(matrizBlocos+destX)+destY)).valor *= 2;
               (*(*(matrizBlocos+destX)+destY)).bloqueado = 1;
            }

            (*(*(matrizBlocos+destX)+destY)).x = (*(*(matrizPosicoesX+destX)+destY));
            (*(*(matrizBlocos+destX)+destY)).y = (*(*(matrizPosicoesY+destX)+destY));

            blocoEsvazia(operandoX,operandoY);
        }
    }
}

//posição fica vazia
void blocoEsvazia(int i,int j) {
    (*(*(matrizBlocos+i)+j)).imagem = -1;
    (*(*(matrizBlocos+i)+j)).valor = 0;
    (*(*(matrizBlocos+i)+j)).x = -1;
    (*(*(matrizBlocos+i)+j)).y = -1;
    (*(*(matrizBlocos+i)+j)).bloqueado = 0;
}

////precisa validar se pode mover
//função de movimentação para cima
int movimentoCima() {
    int i,j,k,aux,loop,validaMovimento;
    validaMovimento = 0;
    loop = 1;
    struct bloco atual;
    struct bloco posicaoLivre;
    while(loop) {
        loop = 0;
        for(i = 1; i < 4; i++) {
            aux = 0;
            for(j = 0; j < 4; j++) {
                atual = (*(*(matrizBlocos + i) + j));
                if(atual.valor != 0) {
                    for(k = 0; k < (i+1); k++) {
                        posicaoLivre = (*(*(matrizBlocos + k) + j));
                        if(posicaoLivre.valor == 0) { //movimento de bloco
                            validaMovimento = 1;
                            loop = 1;
                            trocaPosicao(k,j,i,j,1);
                            break;
                        }
                        if((*(*(matrizBlocos+i-1)+j)).valor == atual.valor && atual.bloqueado == 0 &&
                           (*(*(matrizBlocos+i-1)+j)).bloqueado == 0) { // junção de bloco
                            loop = 1;
                            validaMovimento = 1;
                            trocaPosicao(i-1,j,i,j,1);
                            break;
                        }
                    }
                }
            }
        }
        al_draw_bitmap(fundo, 0, 0, NULL);
        imprimeBlocos();
        al_flip_display();
    }
        al_play_sample(sample_Sons[2],0.2,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
    return validaMovimento;
}

//função movimentação baixo
int movimentoBaixo() {
    int i,j,k,aux,loop,validaMovimento;
    validaMovimento = 0;
    loop = 1;
    struct bloco atual;
    struct bloco posicaoLivre;
    while(loop) {
        loop = 0;
        for(i = 3; i >= 0; i--) {
            aux = 0;
            for(j = 0; j < 4; j++) {
                atual = (*(*(matrizBlocos + i) + j));
                if(atual.valor != 0) {
                    for(k = 3; k > i; k--) {
                        posicaoLivre = (*(*(matrizBlocos + k) + j));
                        if(posicaoLivre.valor == 0) { //movimento de bloco
                            validaMovimento = 1;
                            loop = 1;
                            trocaPosicao(k,j,i,j,2);
                            break;
                        }
                        if((*(*(matrizBlocos+i+1)+j)).valor == atual.valor && atual.bloqueado == 0 &&
                           (*(*(matrizBlocos+i+1)+j)).bloqueado == 0) { // junção de bloco
                            loop = 1;
                            validaMovimento = 1;
                            trocaPosicao(i+1,j,i,j,2);
                            break;
                        }
                    }
                }
            }
        }
        al_draw_bitmap(fundo, 0, 0, NULL);
        imprimeBlocos();
        al_flip_display();
    }
        al_play_sample(sample_Sons[2],0.2,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
    return validaMovimento;
}

//função movimentação esquerda
int movimentoEsquerda() {
    int i,j,k,aux,loop,validaMovimento;
    validaMovimento = 0;
    loop = 1;
    struct bloco atual;
    struct bloco posicaoLivre;
    while(loop) {
        loop = 0;
        for(i = 0; i < 4; i++) {
            aux = 0;
            for(j = 1; j < 4; j++) {
                atual = (*(*(matrizBlocos + i) + j));
                if(atual.valor != 0) {
                    for(k = 0; k < (j+1); k++) {
                        posicaoLivre = (*(*(matrizBlocos + i) + k));
                        if(posicaoLivre.valor == 0) { //movimento de bloco
                            validaMovimento = 1;
                            loop = 1;
                            trocaPosicao(i,k,i,j,3);
                            break;
                        }
                        if((*(*(matrizBlocos+i)+j-1)).valor == atual.valor && atual.bloqueado == 0 &&
                           (*(*(matrizBlocos+i)+j-1)).bloqueado == 0) { // junção de bloco
                            loop = 1;
                            validaMovimento = 1;
                            trocaPosicao(i,j-1,i,j,3);
                            break;
                        }
                    }
                }
            }
        }
        al_draw_bitmap(fundo, 0, 0, NULL);
        imprimeBlocos();
        al_flip_display();
    }
        al_play_sample(sample_Sons[2],0.2,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
    return validaMovimento;
}

//função movimentação direita
int movimentoDireita() {
    int i,j,k,aux,loop,validaMovimento;
    validaMovimento = 0;
    loop = 1;
    struct bloco atual;
    struct bloco posicaoLivre;
    while(loop) {
        loop = 0;
        for(i = 0; i < 4; i++) {
            aux = 0;
            for(j = 3; j >= 0; j--) {
                atual = (*(*(matrizBlocos + i) + j));
                if(atual.valor != 0) {
                    for(k = 3; k > j; k--) {
                        posicaoLivre = (*(*(matrizBlocos + i) + k));
                        if(posicaoLivre.valor == 0) { //movimento de bloco
                            loop = 1;
                            validaMovimento = 1;
                            trocaPosicao(i,k,i,j,4);
                            break;
                        }
                        if((*(*(matrizBlocos+i)+j+1)).valor == atual.valor && atual.bloqueado == 0 &&
                           (*(*(matrizBlocos+i)+j+1)).bloqueado == 0) { // junção de bloco
                            loop = 1;
                            validaMovimento = 1;
                            trocaPosicao(i,j+1,i,j,4);
                            break;
                        }
                    }
                }
            }
        }

        al_draw_bitmap(fundo, 0, 0, NULL);
        imprimeBlocos();
        al_flip_display();
    }
        al_play_sample(sample_Sons[2],0.2,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
    return validaMovimento; 
}

//Reiniciar Jogo
void reiniciar() {
    preencheMatrizPosicao();
    preencheMatrizBlocos();
    iniciaImagens();
    al_draw_bitmap(fundo, 0, 0, NULL);
    novoBloco();
    novoBloco();
    imprimeBlocos();
}

//bloco fica livre para movimentos nele
void desbloqueiaBlocos() {
    int i,j;
    for(i = 0; i < 4; i++) {
        for(j = 0; j < 4; j++) {
            (*(*(matrizBlocos+i)+j)).bloqueado = 0;
        }
    }
}

//Como proprio nome da função diz, verifica se chegou a 2048 OU sem movimentos
int verificaJogoAcabou() {
    int i,j,resp;
    resp = 1;
    struct bloco atual,atualEsquerda,atualDireita,atualCima,atualBaixo;
    for(i = 0; i < 4; i++) {
        for(j = 0; j < 4; j++) {
            atual = (*(*(matrizBlocos+i)+j));
            if(atual.valor == 2048) {
                return 2;   //Ganhou jogo
            }else {
                if(i != 0) {
                    atualCima = (*(*(matrizBlocos+i-1)+j));
                }
                if(i != 3) {
                    atualBaixo = (*(*(matrizBlocos+i+1)+j));
                }
                if(j != 0) {
                    atualEsquerda = (*(*(matrizBlocos+i)+j-1));
                }
                if(j != 3) {
                    atualDireita = (*(*(matrizBlocos+i)+j+1));
                }
                if (atual.valor == 0 || (i != 0 && atual.valor == atualCima.valor)
                    || (i != 3 && atual.valor == atualBaixo.valor)
                    || (j != 0 && atual.valor == atualEsquerda.valor)
                    || (j != 3 && atual.valor == atualDireita.valor)) {
                    resp = 0; //Jogo continua
                }
            }
        }
    }
    return resp; //Perdeu jogo
}

//aparece na tela
void imprimeBlocos() {
    int i,j;
    struct bloco aux;
    for(i = 0; i < 4; i++) {
        for(j = 0; j < 4; j++) {
            aux = (*(*(matrizBlocos+i)+j));
            if (aux.valor != 0) {
                al_draw_bitmap(*(imagens + aux.imagem),aux.x,aux.y,NULL);
            }
        }
    }
}

//Bloco randomico
void novoBloco() {
    int x,y,valor;
    struct bloco novo;
    while(1) {
        x = rand()%4;
        y = rand()%4;

        if( (*(*(matrizBlocos+x)+y)).valor == 0 ) {
            novo.x = (*(*(matrizPosicoesX+x)+y));
            novo.y = (*(*(matrizPosicoesY+x)+y));
            break;
        }
    }
    valor = rand()%4;
    if(valor == 0) {
        novo.valor = 4;
        novo.imagem = 1;
    }else {
        novo.valor = 2;
        novo.imagem = 0;
    }
    novo.bloqueado = 0;
    (*(*(matrizBlocos+x)+y)) = novo;
    return novo;
};


void loopReiniciar(){
    int k =0;
    while (k != 100){
        sleep(3);
        al_draw_bitmap(vetImagens[1], 0, 0, 0), LARGURA_TELA / 2, ALTURA_TELA *0.6, ALLEGRO_ALIGN_CENTRE;
        k++;
    }
}