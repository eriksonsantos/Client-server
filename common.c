#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <string.h>

void logexit(const char *msg)
{

    perror(msg); //Escreve o erro da funcao
    exit(EXIT_FAILURE);
}

int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage)
{

    if (addrstr == NULL || portstr == NULL)
        return -1;

    uint16_t port = (uint16_t)atoi(portstr); //unsignet short

    if (port == 0)
    {
        return -1;
    }

    port = htons(port); //host to network short

    struct in_addr inaddr4; //32 bits IP Address

    //Fazer o parse do IPV4 do addrstr to inaddr4
    if (inet_pton(AF_INET, addrstr, &inaddr4))
    {

        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;

        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;

        return 0;
    }
    struct in6_addr inaddr6; //128 bits IP Address

    //Fazer o parse do IPV6 do addrstr to inaddr4
    if (inet_pton(AF_INET6, addrstr, &inaddr6))
    {

        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;

        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;

        //addr6->sin6_addr = inaddr6;
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));

        return 0;
    }

    return -1;
}

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize)
{

    int version;
    uint16_t port;
    char addrstr[INET6_ADDRSTRLEN + 1] = "";

    if (addr->sa_family == AF_INET)
    {
        version = 4;
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;

        //Faz contrario do inet_pton
        //Verifica se dá erro
        if (!inet_ntop(AF_INET, &(addr4->sin_addr),
         addrstr, INET6_ADDRSTRLEN + 1))
        {
            logexit("ntop");
        }
        port = ntohs(addr4->sin_port); //Networkto host short
    }
    else if (addr->sa_family == AF_INET6)
    {

        version = 6;
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;

        //Faz contrario do inet_pton
        //Verifica se dá erro
        if (!inet_ntop(AF_INET6, &(addr6->sin6_addr), addrstr, INET6_ADDRSTRLEN + 1))
        {
            logexit("ntop");
        }
        port = ntohs(addr6->sin6_port); //Networkto host short
    }
    else
    {
        logexit("unkonw protocol family");
    }

    if (str)
    {
        //Printar na variavel

        snprintf(str, strsize, "IPV%d %s %hu", version, addrstr, port);
    }
}

int server_sockaddr_init(const char *proto, const char *portstr, struct sockaddr_storage *storage)
{

    uint16_t port = (uint16_t)atoi(portstr); //unsignet short

    if (port == 0)
    {
        return -1;
    }

    port = htons(port); //host to network short

    memset(storage, 0, sizeof(*storage));

    if (0 == strcmp(proto, "v4"))
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr.s_addr = INADDR_ANY;

        return 0;
    }
    else if (0 == strcmp(proto, "v6"))
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        addr6->sin6_addr = in6addr_any;
        return 0;
    }
    else
    {
        return -1;
    }
}