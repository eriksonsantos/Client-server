#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <errno.h>


#define BUFSZ 500

void usage(int argc, char *argv[])
{

    printf("usage: My server IP in: PORT %s \n", argv[0]);
    printf("example: 192.168.0.2 %s \n", argv[1]);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    //Endereço que recebeu -- porto --ponteiro
    if (0 != addrparse(argv[1], argv[2], &storage))
        usage(argc, argv);

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1)
    { //Deu erro no socket
        logexit("socket");
    }
    //s é o sockete

    struct sockaddr *addr = (struct sockaddr *)(&storage);

    //0 deu certo o connect => qualquer outro deu erro
    if (0 != connect(s, addr, sizeof(storage)))
    {
        logexit("connect");
    }

    char addrstr[BUFSZ];

    //Imprimir o enderenço addr
    addrtostr(addr, addrstr, BUFSZ);
   

    //Nesse momento irá fazer a comunicação do cliente com o servidor
    while (1)
    {
        char buf[BUFSZ];

        //Iniciar o buf com 0
        memset(buf, 0, BUFSZ);
        //Lendo os dados do teclado
        fgets(buf, BUFSZ - 1, stdin);

        //Enviar o dado atraves da função send e o tamanho do buf pelo strlen
        size_t count = send(s, buf, strlen(buf), 0);

        //O count remete quantos dados foram transmitidos na rede
        if (count != strlen(buf))
        {
            logexit("send");
        }

        //Verificar quantos \n tem na mensagem
        int begin;
        int ascii2 = buf[0];
        int ascii3 = buf[0];

        int qtdBarraN = 0;
        for (begin = 0; begin < (int)count + 1; begin++)
        {
            // Verificar se há o caractere \n no buf
            ascii2 = buf[begin];
            ascii3 = buf[begin + 1];

            if (ascii2 == 92 && ascii3 == 110)
            {
                qtdBarraN++;
            }
        }

        int ascii5;
        int i1;
        int invalido = 0;
        for (i1 = 0; i1 < strlen(buf); i1++)
        {
            //Saber o ascii de cada caractere
            ascii3 = buf[i1];
            //Verificar se são caracteres validos ou não
            if (((ascii3 >= 33 && ascii3 <= 47) || (ascii3 >= 58 && ascii3 <= 64) ||
                 (ascii3 >= 93 && ascii3 <= 96) || ascii3 >= 123 || ascii3 < 0 || ascii3 == 91))
            {

                invalido = 1;
            }
        }
        if (invalido == 1)
        {
            close(s);
            break;
        }

        //Iniciar o buf com 0
        memset(buf, 0, BUFSZ);

        //Receber dado do servidor

        begin = 0;

        //Irá receber do servidor a resposta para as
        //múltiplas mensagens em uma única chamada ao [recv] ou em varias recv
        while (begin <= qtdBarraN)
        {
            //Receber a resposta do servidor
            //O dado será recebido pelo socket s e colocarar no buf
            count = recv(s, buf, BUFSZ, 0);

            //IF count é igual a zero quer dizer que nao recebeu nada de bytes
            // 1 recebeu \n

            if (count != 0 && count != 1)
            {
                printf("%s", buf);

                //Se tiver recebido resposta do servidor e a quantidade de \n já foram
                //Alcancada, remetendo que cada \n é uma mensagem completa. Irá sair desse loop
                if (begin == qtdBarraN - 1)
                    break;
            }
            if(count ==1){
                break;
            }

            if (count == 0)
            {
                exit(EXIT_SUCCESS);
            }
            begin++;
        }
        qtdBarraN = 0;
    }
    close(s);

    exit(EXIT_SUCCESS);
}