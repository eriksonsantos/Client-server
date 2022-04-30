#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <errno.h>
#include <math.h>

#define BUFSZ 500

typedef struct
{
    int size;
    char posicaoX[50][BUFSZ];
    char posicaoY[50][BUFSZ];
} Localization;

void usage(int argc, char **argv)
{
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s vv4 51511\n", argv[1]);
    exit(EXIT_FAILURE);
}

Localization localizacao;

void query(char *buf, int csock)
{
    //Enviar os dados para o cliente
    char enviarDados[BUFSZ];
    memset(enviarDados, 0, BUFSZ);

    int count = strlen(buf) + 1;

    double distancia[50];
    int i;
    int begin;
    for (begin = 0; begin < 50; begin++)
    {
        distancia[begin] = 0;
    }

    char auxPosicaoX2[BUFSZ];
    char auxPosicaoY2[BUFSZ];

    memset(auxPosicaoX2, 0, BUFSZ);
    memset(auxPosicaoY2, 0, BUFSZ);
    memset(enviarDados, 0, BUFSZ);

    int size1 = (int)count;
    // Posicao que inicia a posicao X
    i = 6;
    //Conta quantos caracteres são da posição X até chegar no espaço
    while (buf[i] != ' ' && i <= size1)
    {
        i++;
    }
    // Posicao que inicia a posicao Y
    int j = i + 1;

    while (buf[j] != '\n' && j < size1 - 1)
    {
        j = j + 1;
    }

    //Copia a posicao X em auxPosicaoX

    int aux = 0;
    for (begin = 6; begin < i; begin++)
    {
        auxPosicaoX2[aux] = buf[begin];
        aux++;
    }

    //Copia a posicao Y em auxPosicaoY
    aux = 0;

    for (begin = i + 1; begin < j; begin++)
    {
        auxPosicaoY2[aux] = buf[begin];
        aux++;
    }

    int x1, y1, x2, y2;
    for (begin = 0; begin < localizacao.size; begin++)
    {
        x1 = atoi(localizacao.posicaoX[begin]);
        y1 = atoi(localizacao.posicaoY[begin]);
        y2 = atoi(auxPosicaoY2);
        x2 = atoi(auxPosicaoX2);

        distancia[begin] = pow(pow(x1 - x2, 2) + pow(y1 - y2, 2), (1.0 / 2));
    }
    double menor = distancia[0];
    int indice = 0;

    for (begin = 0; begin < localizacao.size; begin++)
    {
        if (distancia[begin] < menor)
        {
            menor = distancia[begin];
            indice = begin;
        }
    }
    stpcpy(enviarDados, localizacao.posicaoX[indice]);
    strncat(enviarDados, " ", strlen(" "));
    strncat(enviarDados, localizacao.posicaoY[indice], strlen(localizacao.posicaoY[indice]));
    strncat(enviarDados, "\n", strlen("\n"));

    //Enviar para o cliente
    send(csock, enviarDados, strlen(enviarDados) + 1, 0);
    memset(enviarDados, 0, BUFSZ);
    memset(buf, 0, BUFSZ);
}

void adicionar(char *buf, int csock)
{
    //Enviar os dados para o cliente
    char enviarDados[BUFSZ];
    memset(enviarDados, 0, BUFSZ);
    int count = strlen(buf) + 1;

    char auxPosicaoX[BUFSZ];
    char auxPosicaoY[BUFSZ];
    int i;
    char enviarNulo[BUFSZ];
    memset(auxPosicaoX, 0, BUFSZ);
    memset(auxPosicaoY, 0, BUFSZ);

    int size = (int)count;

    // Posicao que inicia a posicao X
    i = 4;
    //Conta quantos caracteres são da posição X até chegar no espaço
    while (buf[i] != ' ' && i <= size)
    {
        i++;
    }
    // Posicao que inicia a posicao Y
    int j = i + 1;

    while (buf[j] != '\n' && j < size - 1)
    {
        j = j + 1;
    }

    //Copia a posicao X em auxPosicaoX

    int begin;
    int aux = 0;
    for (begin = 4; begin < i; begin++)
    {
        auxPosicaoX[aux] = buf[begin];
        aux++;
    }

    //Copia a posicao Y em auxPosicaoY
    aux = 0;
    for (begin = i + 1; begin < j; begin++)
    {
        auxPosicaoY[aux] = buf[begin];
        aux++;
    }

    //Limites de X e Y
    if ((atoi(auxPosicaoX) >= 0) && (atoi(auxPosicaoX) <= 9999) && (atoi(auxPosicaoY) >= 0) && (atoi(auxPosicaoY) <= 9999))
    {
        //Verificar se há locais iguais
        int igual = 0;
        for (i = 0; i < localizacao.size; i++)
        {

            if (atoi(localizacao.posicaoX[i]) == atoi(auxPosicaoX) && igual == 0)
            {
                if (atoi(localizacao.posicaoY[i]) == atoi(auxPosicaoY))
                {
                    igual = 1;
                }
            }
        }
        if (igual == 0)
        {

            stpcpy(localizacao.posicaoX[localizacao.size], auxPosicaoX);
            stpcpy(localizacao.posicaoY[localizacao.size], auxPosicaoY);

            //Concatenar os dados para poder enviar ao cliente
            strncat(enviarDados, localizacao.posicaoX[localizacao.size],
                    strlen(localizacao.posicaoX[localizacao.size]));

            strncat(enviarDados, " ", strlen(" "));

            strncat(enviarDados, localizacao.posicaoY[localizacao.size],
                    strlen(localizacao.posicaoY[localizacao.size]));

            strncat(enviarDados, " added\n", strlen(" added\n"));
            localizacao.size++;
            //Enviar para o cliente
            send(csock, enviarDados, strlen(enviarDados) + 1, 0);
            
            memset(enviarDados, 0, BUFSZ);
            memset(buf, 0, BUFSZ);
        }
        else
        {
            stpcpy(localizacao.posicaoX[localizacao.size], auxPosicaoX);
            stpcpy(localizacao.posicaoY[localizacao.size], auxPosicaoY);

            //Concatenar os dados para poder enviar ao cliente
            strncat(enviarDados, localizacao.posicaoX[localizacao.size],
                    strlen(localizacao.posicaoX[localizacao.size]));

            strncat(enviarDados, " ", strlen(" "));

            strncat(enviarDados, localizacao.posicaoY[localizacao.size],
                    strlen(localizacao.posicaoY[localizacao.size]));

           
            strncat(enviarDados, " already exists\n", strlen(" already exists\n"));

            //Enviar para o cliente
            send(csock, enviarDados, strlen(enviarDados) + 1, 0);

            memset(enviarDados, 0, BUFSZ);
            memset(buf, 0, BUFSZ);
        }
    }
    else
    {
        //Enviar para o cliente mensagem nula. Para poder assim o cliente enviar
        //sua proxima operação
        send(csock, enviarNulo, strlen(enviarNulo) + 1, 0);
        memset(buf, 0, BUFSZ);
        memset(enviarDados, 0, BUFSZ);
    }
}

void list(char *buf, int csock)
{
    //Enviar os dados para o cliente
    char enviarDados[BUFSZ];
    memset(enviarDados, 0, BUFSZ);
    int count = strlen(buf) + 1;

    if (localizacao.size == 0)
    {
        memset(enviarDados, 0, BUFSZ);
        stpcpy(enviarDados, "none\n");
        //Enviar para o cliente mensagem nula. Para poder assim o cliente enviar
        //sua proxima operação
        count = send(csock, enviarDados, strlen(enviarDados) + 1, 0);
        memset(buf, 0, BUFSZ);
        memset(enviarDados, 0, BUFSZ);
    }
    else
    {

        memset(enviarDados, 0, BUFSZ);

        int begin = 0;
        for (begin = 0; begin < localizacao.size; begin++)
        {
            strncat(enviarDados, localizacao.posicaoX[begin], strlen(localizacao.posicaoX[begin]));
            strncat(enviarDados, " ", strlen(" "));
            strncat(enviarDados, localizacao.posicaoY[begin], strlen(localizacao.posicaoY[begin]));
            strncat(enviarDados, " ", strlen(" "));
        }

        int size1 = strlen(enviarDados);
        enviarDados[size1] = '\n';

        count = send(csock, enviarDados, strlen(enviarDados) + 1, 0);
        memset(enviarDados, 0, BUFSZ);
        memset(buf, 0, BUFSZ);
        memset(enviarDados, 0, BUFSZ);
    }
}

void rm(char *buf, int csock)
{
    //Enviar os dados para o cliente
    char enviarDados[BUFSZ];
    memset(enviarDados, 0, BUFSZ);

    int count = strlen(buf) + 1;

    char auxPosicaoX1[BUFSZ];
    char auxPosicaoY1[BUFSZ];
    char enviarNulo[BUFSZ];
    int i;

    memset(auxPosicaoX1, 0, BUFSZ);
    memset(auxPosicaoY1, 0, BUFSZ);
    memset(enviarDados, 0, BUFSZ);

    int size1 = (int)count;
    // Posicao que inicia a posicao X
    i = 3;
    //Conta quantos caracteres são da posição X até chegar no espaço
    while (buf[i] != ' ' && i <= size1)
    {
        i++;
    }
    // Posicao que inicia a posicao Y
    int j = i + 1;

    while (buf[j] != '\n' && j < size1 - 1)
    {
        j = j + 1;
    }

    //Copia a posicao X em auxPosicaoX

    int begin;
    int aux = 0;
    for (begin = 3; begin < i; begin++)
    {
        auxPosicaoX1[aux] = buf[begin];
        aux++;
    }

    //Copia a posicao Y em auxPosicaoY
    aux = 0;

    for (begin = i + 1; begin < j; begin++)
    {
        auxPosicaoY1[aux] = buf[begin];
        aux++;
    }
    //Vericar se já está adicionado
    int igual2 = 0;
    int posicao;
    for (begin = 0; begin < localizacao.size; begin++)
    {

        if (atoi(localizacao.posicaoX[begin]) == atoi(auxPosicaoX1) &&
            atoi(localizacao.posicaoY[begin]) == atoi(auxPosicaoY1))
        {
            igual2 = 1;
            posicao = begin;
        }
    }
    if (igual2 == 0)
    {
        stpcpy(enviarDados, auxPosicaoX1);
        strncat(enviarDados, " ", strlen(" "));
        strncat(enviarDados, auxPosicaoY1, strlen(auxPosicaoY1));
        strncat(enviarDados, " does not exist\n", strlen(" does not exist\n"));

        //Enviar para o cliente
        send(csock, enviarDados, strlen(enviarDados) + 1, 0);
        
        memset(enviarDados, 0, BUFSZ);
        memset(buf, 0, BUFSZ);
        memset(auxPosicaoX1, 0, BUFSZ);
        memset(auxPosicaoY1, 0, BUFSZ);
    }
    else
    {
        //Deletando uma localizacao
        memset(enviarDados, 0, BUFSZ);
        //Verificando se é a ultima posicao do vetor.
        if (posicao == localizacao.size - 1)
        {
            localizacao.size--;
        }
        else
        {
            for (begin = posicao; begin < localizacao.size; begin++)
            {
                stpcpy(localizacao.posicaoX[begin], localizacao.posicaoX[begin + 1]);
                stpcpy(localizacao.posicaoY[begin], localizacao.posicaoY[begin + 1]);
            }
            localizacao.size--;
        }

        stpcpy(enviarDados, auxPosicaoX1);
        strncat(enviarDados, " ", strlen(" "));
        strncat(enviarDados, auxPosicaoY1, strlen(auxPosicaoY1));
        strncat(enviarDados, " removed\n", strlen(" removed\n"));

        //Enviar para o cliente
        send(csock, enviarDados, strlen(enviarDados) + 1, 0);
        memset(enviarDados, 0, BUFSZ);
        memset(buf, 0, BUFSZ);
        memset(auxPosicaoX1, 0, BUFSZ);
        memset(auxPosicaoY1, 0, BUFSZ);
    }

    send(csock, "", strlen("") + 1, 0);
    memset(enviarDados, 0, BUFSZ);
    memset(buf, 0, BUFSZ);
}

void limitExceed(char *buf, int csock)
{

    char enviarDados[BUFSZ];
    memset(enviarDados, 0, BUFSZ);
    strcpy(enviarDados, "limit exceeded\n");

    //Enviar para o cliente
    send(csock, enviarDados, strlen(enviarDados) + 1, 0);
    //puts(enviarDados);

    memset(buf, 0, BUFSZ);
}

int main(int argc, char *argv[])
{
    localizacao.size = 0;

    if (argc < 3)
    {
        usage(argc, argv);
    }
    struct sockaddr_storage storage;
    //Endereço que recebeu -- porto --ponteiro
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage))
        usage(argc, argv);

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1)
    { //Deu erro no socket
        logexit("socket");
    }
    //Reutilizar a porta depois encerrada e não aguardar 2 min devido ao linux
    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
        logexit("setsockopt");

    struct sockaddr *addr = (struct sockaddr *)(&storage);

    if (0 != bind(s, addr, sizeof(storage)))
    {
        logexit("bind");
    }
    if (0 != listen(s, 10))
    {
        logexit("listen");
    }

    char addrstr[BUFSZ];

    addrtostr(addr, addrstr, BUFSZ);

    while (1)
    {

        //Armazenar o endereço do cliente
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&storage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        //Deu erro
        if (csock == -1)
        {
            logexit("accept");
        }

        char caddrstr[BUFSZ];
        //Escrever o endereço do cliente
        addrtostr(caddr, caddrstr, BUFSZ);

        int sair = 0;
        while (1 && sair == 0)
        {

            //Zerar o buffer
            char buf[BUFSZ];
            memset(buf, 0, BUFSZ);

            //Receber a mensagem do cliente
            size_t count1 = recv(csock, buf, BUFSZ - 1, 0);

            int begin;
            for(begin=0;begin<strlen(buf);begin++){
                printf("%i \n",buf[begin]);
            }

            //Fechar se limite de mensagens forem maiores de 500 bytes
            if (strlen(buf) - 2 >= 500)
            {
                close(csock);
                sair = 1;
            }

            char auxBuf5[BUFSZ];
            memset(auxBuf5, 0, BUFSZ);
            stpcpy(auxBuf5, buf);
            memset(buf, 0, BUFSZ);
            //Impedir que se o cliente fechar a aplicação, o servidor não pare de funcionar
            if (count1 <= 0)
                break;

            //Copiar menos o \n vindo do fgets do client
            strncpy(buf, auxBuf5, strlen(auxBuf5) - 1);

            //Verificar se os ultimos caracteres do buffer é \n
            //Se não forem devem fazer o recv novamente porque a mensagem veio em partes

            //ascii5 verifica se é o caractere '\'
            int ascii5 = buf[strlen(buf) - 2];
            int sair1 = 0;
            while (!(buf[strlen(buf) - 1] == 'n' && ascii5 == 92))
            {

                //Enviar para o cliente mensagem nula, para liberar a proxima mensagem
                send(csock, "", strlen("") + 1, 0);
                char auxBuf[BUFSZ];
                memset(auxBuf, 0, BUFSZ);
                //Receber a proxima mensagem
                size_t count2 = recv(csock, auxBuf, BUFSZ - 1, 0);

                //Impedir que se o cliente fechar a aplicação, o servidor não pare de funcionar
                if (count2 <= 0)
                {
                    sair1 = 1;
                    break;
                }

                char auxBuf6[BUFSZ];
                memset(auxBuf6, 0, BUFSZ);
                stpcpy(auxBuf6, auxBuf);
                memset(auxBuf, 0, BUFSZ);

                //Copiar menos o \n vindo do fgets do client
                strncpy(auxBuf, auxBuf6, strlen(auxBuf6) - 1);

                //Concatenar com a mensagem
                strncat(buf, auxBuf, strlen(auxBuf));

                //ascii5 verifica se é o caractere '\'
                ascii5 = buf[strlen(buf) - 2];
            }

            //Se a mensagem particionada em multiplas partes, e acontecer que o cliente tiver
            //saido abrutamente, a mensagem que já tiver armazenado no buf deve ser ignorada.
            if (sair1 == 1)
                break;

            int comeco;
            int ascii2 = buf[0];
            int ascii3 = buf[0];

            for (comeco = 0; comeco < strlen(buf); comeco++)
            {
                // Verificar se há o caractere \n no buf e substituir \ por .
                ascii2 = buf[comeco];
                ascii3 = buf[comeco + 1];
                if (ascii2 == 92 && ascii3 == 110)
                {
                    buf[comeco] = '.';
                }
            }

            char auxBuf2[BUFSZ];
            stpcpy(auxBuf2, buf);

            char *auxBuf3;
            auxBuf3 = strtok(auxBuf2, ".n");
            int count = (int)count1;

            //Múltiplas mensagens em uma única chamada ao [recv].
            while (auxBuf3 != NULL && strlen(auxBuf3) != 1)
            {

                memset(buf, 0, BUFSZ);
                stpcpy(buf, auxBuf3);
                puts(buf);

                int pularProximaMensagem = 1;

                count = strlen(buf);

                //FInalizando o programa se ouver o comando kill

                if (buf[0] == 'k' && buf[1] == 'i' && buf[2] == 'l' && buf[3] == 'l')
                {
                    close(csock);
                    sair = 1;
                }

                //Testar se os caracteres são validos (numeros e letras(que não estejam acentuadas))
                int i;
                //Tamanho da mensagem
                int size = (int)count;
                int ascii;

                for (i = 0; i < size; i++)
                {
                    //Saber o ascii de cada caractere
                    ascii = buf[i];
                    //Verificar se são caracteres validos ou não
                    if (((ascii >= 33 && ascii <= 47) || (ascii >= 58 && ascii <= 64) ||
                         (ascii >= 93 && ascii <= 96) || ascii >= 123 || ascii < 0 || ascii == 91))
                    {

                        //Enviar para o cliente mensagem nula
                        send(csock, "", strlen("") + 1, 0);

                        memset(buf, 0, BUFSZ);
                        pularProximaMensagem = 0;
                    }
                }

                //Verifica se a mensagem inicia com -> add e espaço
                if (buf[0] == 'a' && buf[1] == 'd' && buf[2] == 'd' && buf[3] == ' ')
                {
                    //Ultrapassou o limite exigido. Inicia no 0 as localização

                    if (localizacao.size >= 50)
                    {
                        limitExceed(buf, csock);
                        memset(buf, 0, BUFSZ);
                        pularProximaMensagem = 0;
                    }
                    else
                    {
                        adicionar(buf, csock);

                        memset(buf, 0, BUFSZ);
                        pularProximaMensagem = 0;
                    }
                }
                if (buf[0] == 'l' && buf[1] == 'i' && buf[2] == 's' && buf[3] == 't')
                {
                    list(buf, csock);

                    memset(buf, 0, BUFSZ);
                    pularProximaMensagem = 0;
                }

                //Remover uma localização
                if (buf[0] == 'r' && buf[1] == 'm' && buf[2] == ' ')
                {
                    rm(buf, csock);
                    memset(buf, 0, BUFSZ);
                    pularProximaMensagem = 0;
                }
                if (buf[0] == 'q' && buf[1] == 'u' && buf[2] == 'e' && buf[3] == 'r' && buf[4] == 'y' && buf[5] == ' ')
                {
                    query(buf, csock);
                    memset(buf, 0, BUFSZ);
                    pularProximaMensagem = 0;
                }
                //Se não for nenhum comando, pular para a próxima mensagem
                if (pularProximaMensagem == 1)
                {
                    //Enviar para o cliente mensagem nula
                    send(csock, "", strlen("") + 1, 0);

                    memset(buf, 0, BUFSZ);
                    pularProximaMensagem = 0;
                }

                //Pular para a proxima mensagem que vem depois do \n
                auxBuf3 = strtok(NULL, ".n");
            }
        }
    }

    exit(EXIT_SUCCESS);
}