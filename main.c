#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <Windows.h>

#include "inicio.c"

int test =1;

int main(){
    int sair = 0;
    int tecla = 0;
    int matrizVazia = 1;
    int fim, constante;
    srand(time(NULL));

    //Iniciando o programa
    if(!inicializar()){ //iniciando o Allegro caso falhe
        return -1;
    }
    
    preencheMatrizPosicao();
    preencheMatrizBlocos();
    iniciaImagens();

    al_draw_bitmap(fundo, 0, 0, NULL);
    novoBloco();
    novoBloco();
    imprimeBlocos();
    constante = 0;

    while (!sair){

//Movimentos randomicos
/*
        if(!verificaJogoAcabou()) {
                tecla = 1+ constante%4;
                constante++;
            }
        while(!al_is_event_queue_empty(fila_eventos)) {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);
            if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch(evento.keyboard.keycode) {
                    case ALLEGRO_KEY_ENTER:
                        reiniciar();
                        break;
                    case ALLEGRO_KEY_ESCAPE:
                        sair = true;
                        break;
                }
            } else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                sair = true;
            }
        }
     */

        while(!al_is_event_queue_empty(fila_eventos)) { //enquanto tiver evento executa
            ALLEGRO_EVENT evento;

            al_wait_for_event(fila_eventos, &evento); //Se a posição da fila NÃO for null o primeiro sera copiado e removido.

            if (evento.type == ALLEGRO_EVENT_KEY_DOWN) { //teclado
                if(!verificaJogoAcabou()) {
                    switch(evento.keyboard.keycode) {
                        case ALLEGRO_KEY_UP: 
                            tecla = 1;
                            break;
                        case ALLEGRO_KEY_DOWN:
                            tecla = 2;
                            break;
                        case ALLEGRO_KEY_LEFT:
                           tecla = 3;
                            break;
                        case ALLEGRO_KEY_RIGHT:
                            tecla = 4;
                            break;
                        case ALLEGRO_KEY_ENTER:
                            reiniciar();
                            break;
                        case ALLEGRO_KEY_ESCAPE:
                            sair = true;
                            break;
                    }
                }

                switch(evento.keyboard.keycode) {
                    case ALLEGRO_KEY_ENTER:
                        reiniciar();
                        break;
                    case ALLEGRO_KEY_ESCAPE:
                        sair = true;
                        break;
                }
            } else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            sair = true;
            }
        }

    if (tecla) {
        int validaMovimento;
        switch (tecla) {
            case 1:
                validaMovimento = movimentoCima();
                break;
            case 2:
                validaMovimento = movimentoBaixo();
                break;
            case 3:
                validaMovimento = movimentoEsquerda();
                break;
            case 4:
                validaMovimento = movimentoDireita();
                break;
        }

        if(validaMovimento) {
                desbloqueiaBlocos();
                al_draw_bitmap(fundo, 0, 0, NULL);
                novoBloco();
                imprimeBlocos();
            }
            tecla = 0;

            fim = verificaJogoAcabou();
            

            if(fim) {
                if(fim == 1) {
                    al_draw_bitmap(vetImagens[2], 0, 0, 0), LARGURA_TELA / 2, ALTURA_TELA *0.2, ALLEGRO_ALIGN_CENTRE;
                    al_draw_bitmap(vetImagens[1], 0, 0, 0), LARGURA_TELA / 2, ALTURA_TELA *0.6, ALLEGRO_ALIGN_CENTRE;

                    al_play_sample(sample_Sons[1],0.75,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
                    
                }else {
                    al_draw_bitmap(vetImagens[0], 0, 0, 0), LARGURA_TELA / 2, ALTURA_TELA *0.2, ALLEGRO_ALIGN_CENTRE; 
                    al_draw_bitmap(vetImagens[1], 0, 0, 0), LARGURA_TELA / 2, ALTURA_TELA *0.6, ALLEGRO_ALIGN_CENTRE;

                    al_play_sample(sample_Sons[0],0.75,0,1,ALLEGRO_PLAYMODE_ONCE,NULL);
                    
                }
            }
        }

        
        al_flip_display();
    }

    return 0;
}    