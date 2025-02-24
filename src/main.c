#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "../include/dataStructure.h"
#include "../include/minefield.h"
#include "../include/colors.h"


int main() {
    
    int start;
    bool gameWon = false, gameLost = false;

    printf("\033[1;92m");

    // cabeçalho estilizado em ASCII
    printf("  __  __ _                                                   \n");
    printf(" |  \\/  (_)                                                  \n");
    printf(" | \\  / |_ _ __   ___  _____      _____  ___ _ __   ___ _ __ \n");
    printf(" | |\\/| | | '_ \\ / _ \\/ __\\ \\ /\\ / / _ \\/ _ \\ '_ \\ / _ \\ '__|\n");
    printf(" | |  | | | | | |  __/\\__ \\\\ V  V /  __/  __/ |_) |  __/ |   \n");
    printf(" |_|  |_|_|_| |_|\\___||___/ \\_/\\_/ \\___|\\___| .__/ \\___|_|   \n");
    printf("                                            | |               \n");
    printf("                                            |_|               \n");

    // reseta a cor para o padrão do terminal
    printf("\033[0m");

    do {
        printf(BLUE"\nDigite 1 para iniciar o jogo: "RESET);
        scanf("%d", &start);
        if (start != 1) {
            printf(RED"Entrada invalida. Tente novamente.\n"RESET);
        }
    } while (start != 1);

    limparTela();
    printf(GREEN"O jogo comecou!\n"RESET);

    int rows, cols, bombs;
    int flagsLeft, correctlyFlagged = 0;

    // configurar tamanho do tabuleiro
    do {
        printf(CYAN"\nDigite as linhas (5-26): \n"RESET);
        scanf("%d", &rows);
        printf(CYAN"\nDigite as colunas (5-40): \n"RESET);
        scanf("%d", &cols);
    } while (rows < 5 || rows > 26 || cols < 5 || cols > 40);

    // configurar número de bombas
    int maxBombs = (rows*cols)/5;
    
    if (maxBombs < 7) {
        maxBombs = 7;
    }

    do {
        printf(RED"\nDigite o numero de bombas (7-%d): \n"RESET, maxBombs);
        scanf("%d", &bombs);
    } while (bombs < 7 || bombs > maxBombs);

    flagsLeft = bombs;

    // cria e configura o campo minado
    Node* head = createMinefield(rows, cols);
    placeBombs(head, rows, cols, bombs);
    updateBombNumber(head, rows, cols);

    int chosenRow, chosenCol;
    char charChosenRow, op;

    while (true) {
        // solicita input do jogador
        userPrintMinefield(head, rows, cols);
        printf("Escreva tudo junto, sem espacos.\n");
        printf("Digite # para marcar ou desmarcar uma bomba e ! para explorar.");
        printf("\nEscolha uma celula e se deseja explorar 'e' ou marcar 'f': ");
        printf("\n(ex. #A1 para marcar uma bandeira e !B2 para explorar)\n");
        
        // acredita que eu não sabia que scanf retornava um int?
        int input = scanf(" %c%c%d", &op, &charChosenRow, &chosenCol);
        if (input != 3) {
            printf("Entrada invalida. por favor, siga o formato 'opcao-linha-coluna' -- ex. #A1.\n");
            // limpa o buffer de entrada
            while ((getchar()) != '\n');
            continue;
        }

        /* 
        esse decremento aqui é pq a lógica do programa funciona com índice inicial 0, mas o do miguel começa com 1 kkkkkkkkkkkkkkk 
        então, para o usuário, ele escolhe de 1 a 40, o programa entende de 0 a 39
        */
        chosenCol--;

        // isso aqui é a mesma coisa, pq mtas funções precisam saber qual linha o usuário escolheu e manipular um char é péssimo
        chosenRow = getIntChosenRow(charChosenRow);

        if (chosenRow < 0 || chosenRow >= rows || chosenCol < 0 || chosenCol >= cols) {
            printf("Posição invalida, escolha dentro da grade.\n");
            continue;
        }

        Node* selected = getNode(head, chosenRow, chosenCol);

        if (op == '#') {
            // pra ver se já tava marcado
            bool wasFlagged = selected->isFlagged;
            
            toggleFlag(head, chosenRow, chosenCol);
            
            // Atualiza contadores após mudar o estado da flag
            if (!wasFlagged && selected->isBomb) {
                // uma bomba foi marcada corretamente
                flagsLeft--;
                correctlyFlagged++;
            } 
            else if (wasFlagged && selected->isBomb) {
                // uma bomba foi desmarcada
                flagsLeft++;
                correctlyFlagged--;
            }

            
            // verifica condição de vitória e encerra o laço
            if (correctlyFlagged == bombs && flagsLeft == 0) {
                userPrintMinefield(head, rows, cols);
                printf("\nVoce venceu!\n");
                gameWon = true;
                break;
            }
        }

        if (op == '!') {
            exploreNode(head, chosenRow, chosenCol);

            if (selected->isBomb) {
                userPrintMinefield(head, rows, cols);
                printf("\nVoce perdeu!\n");
                revealMinefield(head, rows, cols);
                gameLost = true;
                break;
            }
        }
    }

    if (gameLost || gameWon) {
        while (true) {
            int esc;
            printf("Digite 1 para sair.");
            scanf("%d", &esc);
            if (esc == 1) {
                break;
            }
        }
    }

    // limpa a memória, só recebe o primeiro termo da lista e vai limpando de um por um
    freeMinefield(head);
    return EXIT_SUCCESS;
}
