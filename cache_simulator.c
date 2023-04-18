#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <limits.h>
typedef struct
{
    uint32_t tag, value;
    bool valid; // conteudo de cada indice
    int countLru;
    int countFifo;
} CacheLine;

typedef struct
{
    CacheLine *block;
} CacheSet; // block == associatividade

typedef struct
{
    int countAcess; // contador para o universal para lru e fifo
    char *subst;
    int nsets;
    int bsize;
    int assoc;
    int block_count;
    CacheSet *sets; // indices
} Cache;

uint32_t ReverseBytes(uint32_t bytes); // função retirada da internet para reverter os
                                       // bits, assim ficando no formato 0x12345678
int tam_arquivo(char *arquivoEntrada);
void ler_Arquivo(char *arquivoEntrada, int numadreesss, uint32_t *adreess);
Cache *createCache(int nsets, int bsize, int assoc, char *subst);
void destroyCache(Cache *cache);
void access_cache(Cache *cache, uint32_t end, float *hits, float *misses,
                  float *miss_compulsorio, float *miss_capacidade,
                  float *miss_conflito);
void printFlagOut(Cache *cache, float *hits, float *misses,
                  float *miss_compulsorio, float *miss_capacidade,
                  float *miss_conflito, int numadreesss);
int main(int argc, char *argv[])
{
    SetConsoleOutputCP(65001);
    if (argc != 7)
    {
        printf("Número de argumentos incorreto. Utilize:\n");
        printf("./cache_simulator <nsets> <bsize> <assoc> <substituição> "
               "<flag_saida> arquivo_de_entrada\n");
        exit(EXIT_FAILURE);
    }
    int nsets = atoi(argv[1]);
    int bsize = atoi(argv[2]);
    int assoc = atoi(argv[3]);
    char *subst = argv[4];
    int flagOut = atoi(argv[5]);
    char *arquivoEntrada = argv[6];
    int numadreesss = tam_arquivo(arquivoEntrada);              // vefirica o tamanho o arquivo (quantidade de endereços)
    uint32_t *adreess = malloc(numadreesss * sizeof(uint32_t)); // aloca espaço para os endereços

    ler_Arquivo(arquivoEntrada, numadreesss, adreess);

    for (int i = 0; i < numadreesss; i++)
    {
        adreess[i] = ReverseBytes(adreess[i]); // inverte o endereço, passando os bits mais
                                               // significativos a direita.
    }

    Cache *cache = createCache(nsets, bsize, assoc, subst);
    float hits = 0, misses = 0, miss_compulsorio = 0, miss_capacidade = 0,
          miss_conflito = 0;

    // Simulação de acesso à cache
    for (int i = 0; i < numadreesss; i++)
    {
        access_cache(cache, adreess[i], &hits, &misses, &miss_compulsorio,
                     &miss_capacidade, &miss_conflito);
    }

    if (flagOut == 1)
    {
        printf("%d %.4f %.4f %.2f %.2f %.2f", numadreesss, hits / numadreesss,
               misses / numadreesss, miss_compulsorio / misses,
               miss_capacidade / misses, miss_conflito / misses);
    }
    else
    {
        printFlagOut(cache, &hits, &misses, &miss_compulsorio, &miss_capacidade,
                     &miss_conflito, numadreesss);
    }

    destroyCache(cache);

    free(adreess);
    return 0;
}

/*
=====
reverses bytes order cause they're backwards for whatever reason
-----
extracted shamelessly from
https://stackoverflow.com/questions/32786493/reversing-byte-order-in-c
=====
*/
uint32_t ReverseBytes(uint32_t bytes)
{
    uint32_t aux = 0;
    uint8_t byte;

    for (int i = 0; i < 32; i += 8)
    {
        byte = (bytes >> i) & 0xff;
        aux |= byte << (32 - 8 - i);
    }

    return (aux);
}
int tam_arquivo(char *arquivoEntrada)
{
    FILE *inputFile = fopen(arquivoEntrada, "rb");
    if (inputFile == NULL)
    {
        printf("Erro ao abrir arquivo de entrada.\n");
        exit(EXIT_FAILURE);
    }
    fseek(inputFile, 0, SEEK_END); // bota o poteiro na primeira posição do arquivo
    int tam = ftell(inputFile);    // retorna o tamanho do arquivo em  32 bits
    tam = tam / 4;
    fclose(inputFile);
    return tam;
}

void ler_Arquivo(char *arquivoEntrada, int numadreesss, uint32_t *adreess)
{
    FILE *fp;
    fp = fopen(arquivoEntrada, "rb");
    if (fp == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        exit(EXIT_FAILURE);
    }
    int bytesRead = fread(adreess, sizeof(uint32_t), numadreesss, fp); // retorna a quantidade de bytes lidos, armazena os dados na primeira
    if (bytesRead != numadreesss)                                      // variavel passada como parametro
    {
        printf("Erro ao ler o arquivo.\n");
        exit(EXIT_FAILURE);
    }
    fclose(fp);
}

Cache *createCache(int nsets, int bsize, int assoc, char *subst)
{
    Cache *cache = malloc(sizeof(Cache));
    cache->nsets = nsets;
    cache->bsize = bsize;
    cache->assoc = assoc;
    cache->subst = subst;
    cache->block_count = 0;
    cache->countAcess = 0;

    cache->sets = malloc(nsets * sizeof(CacheSet));

    for (int i = 0; i < nsets; i++)
    {
        cache->sets[i].block = malloc(assoc * sizeof(CacheLine)); // imaginar como uma matriz
        for (int j = 0; j < assoc; j++)
        {
            cache->sets[i].block[j].value = -1;
            cache->sets[i].block[j].valid = false;
            cache->sets[i].block[j].countLru = -1;
            cache->sets[i].block[j].countFifo = -1;
        }
    }

    return cache;
}

void access_cache(Cache *cache, uint32_t end, float *hits, float *misses,
                  float *miss_compulsorio, float *miss_capacidade,
                  float *miss_conflito)
{
    int bitsOffset = (int)log2(cache->bsize);
    int bitsIndex = (int)log2(cache->nsets);
    uint32_t tag, index;

    tag = end >> (bitsOffset + bitsIndex);
    index = (end >> bitsOffset) % cache->nsets;

    cache->countAcess++; // contador de acessos para politica lru.

    bool hit = false;
    for (int i = 0; i < cache->assoc; i++)
    {
        if (cache->sets[index].block[i].valid &&
            cache->sets[index].block[i].tag == tag) // se a validade for true e a tag bater == hit
        {
            hit = true;
            (*hits)++;
            cache->sets[index].block[i].countLru = cache->countAcess; // politica lru. se houve hit, atualiza o acesso do bloco.
            break;
        }
    }

    if (!hit)
    {
        int block_to_replace = -1;
        (*misses)++;
        int empty_block = -1;
        for (int i = 0; i < cache->assoc; i++)
        {
            if (!cache->sets[index].block[i].valid) // encontra algum bloco vazio
            {
                empty_block = i; // posição bloco vázio
                break;
            }
        }

        if (empty_block != -1)
        {
            cache->sets[index].block[empty_block].valid = true;
            cache->sets[index].block[empty_block].tag = tag;
            (*miss_compulsorio)++; // se encontrar atualiza os paramentros do bloco e
                                   // soma 1 miss compulsorio e interrompe a função
            cache->sets[index].block[empty_block].value = end;
            cache->block_count++; // contador para avaliar se á bloco vazio na cache
                                  // para identificar miss conflito ou capacidade,
                                  // caso não for miss compulssório

            cache->sets[index].block[empty_block].countLru = cache->countAcess; // preenche o bloco com o valor de acesso novo para lru.
            cache->sets[index].block[empty_block].countFifo = cache->countAcess; // preenche o bloco com o valor de acesso novo para fifo.
            return;
        }

        else // se não encontrou bloco vázio
        {
            if (strcmp(cache->subst, "r") == 0 ||
                strcmp(cache->subst, "R") == 0)
            {
                block_to_replace = rand() % cache->assoc; // escolhe aleatóriamente um bloco, entre a
                                                          // quantidade de blocos.
            }
            else if (strcmp(cache->subst, "f") == 0 ||
                     strcmp(cache->subst, "F") == 0)
            {
                int menor = cache->sets[index].block[0].countFifo;
                for (int i = 1; i < cache->assoc; i++)
                {
                    if (cache->sets[index].block[i].countFifo < menor)
                    {
                        menor = cache->sets[index].block[i].countFifo;
                    }

                } // obtemos o contador menor.

                for (int i = 0; i < cache->assoc; i++)
                {
                    if (menor == cache->sets[index].block[i].countFifo)
                    { // encontra o bloco que bate os valores.
                        block_to_replace = i;
                    }
                }
            }
            else if (strcmp(cache->subst, "l") == 0 ||
                     strcmp(cache->subst, "L") == 0)
            {

                int menor = cache->sets[index].block[0].countLru;
                for (int i = 1; i < cache->assoc; i++)
                {
                    if (cache->sets[index].block[i].countLru < menor)
                    {
                        menor = cache->sets[index].block[i].countLru;
                    }

                } // obtemos o contador menor. // semelhante ao fifo, porém o contador de lru é atualizado quando ocorre hit tambem.

                for (int i = 0; i < cache->assoc; i++)
                {
                    if (menor == cache->sets[index].block[i].countLru)
                    { // encontra o bloco que bate os valores.
                        block_to_replace = i;
                    }
                }
            }

            else
            {
                printf("Invalid substitution policy: %s\n", cache->subst);
                exit(1);
            }
            cache->sets[index].block[block_to_replace].tag = tag;
            cache->sets[index].block[block_to_replace].valid = true; // atualiza os parametros de acordo com o bloco definido pela
                                                                     // politica
            cache->sets[index].block[block_to_replace].value = end;
            cache->sets[index].block[block_to_replace].countLru = cache->countAcess; // apos aplicar a politica, atualiza o valor novo
                                                                                     // do contado para a politica lru.
            cache->sets[index].block[block_to_replace].countFifo = cache->countAcess;// apos aplicar a politica atualiza o valor da nova
        }                                                                            //posição do bloco que foi substituido

        if (cache->block_count == cache->nsets * cache->assoc)
        {
            (*miss_capacidade)++;
        } // aqui checa se tem mais blocos vazios, ou foi conflito.
        else
        {
            (*miss_conflito)++;
        }
    }
}

void destroyCache(Cache *cache)
{
    // Libera o espaço alocado para cada linha em cada conjunto
    for (int i = 0; i < cache->nsets; i++)
    {
        free(cache->sets[i].block);
    }

    // Libera o espaço alocado para cada conjunto
    free(cache->sets);

    // Libera o espaço alocado para a cache
    free(cache);
}
void printFlagOut(Cache *cache, float *hits, float *misses,
                  float *miss_compulsorio, float *miss_capacidade,
                  float *miss_conflito, int numadreesss)
{
    printf("\n\n");
    printf("\t\t\t\t\t\t\t\t\t\tIMPRIMINDO DADOS DA CACHE\n\n\n");
    for (int i = 0; i < cache->nsets; i++)
    {
        printf("\nÍndice %d", i);
        for (int j = 0; j < cache->assoc; j++)
        {
            printf("\t Conjunto %d value: %d", j, cache->sets[i].block[j].value);
        }
    }
    printf("\n\n");
    printf("Número de Conjuntos: %d\t", cache->nsets);
    printf("Tamanho do Bloco: %d\t", cache->bsize);
    printf("Associatividade: %d\n", cache->assoc);
    if (cache->assoc == 1)
    {                                                             // Aqui imprime a cache detalhadamente a cache
        printf("Tipo de Mapeamento da cache: Mapeamento direto"); // caso o
                                                                  // flagout for 0
    }
    else if (cache->nsets == 1)
    {
        printf("Tipo de Mapeamento da cache: Totalmente Associativa");
    }
    else
    {
        printf("Tipo de Mapeamento da cache: Associativa por conjuntos");
    }
    printf("\nTipo de Substiuição: ");
    if (strcmp(cache->subst, "r") == 0 || strcmp(cache->subst, "R") == 0)
    {
        printf("RANDOM \n");
    }
    else if (strcmp(cache->subst, "f") == 0 || strcmp(cache->subst, "F") == 0)
    {
        printf("FIFO \n");
    }
    else if (strcmp(cache->subst, "l") == 0 || strcmp(cache->subst, "L") == 0)
    {
        printf("LRU \n");
    }
    printf("---------------------------------------------- \n");
    printf("Acessos\t| %d\nhits\t| %.0f (%.2f%%)\n", numadreesss, *hits,
           (*hits / numadreesss) * 100);
    printf("Misses\t| %.0f (%.2f%%)\n", *misses, (*misses / numadreesss) * 100);
    printf("---------------------------------------------- \n");
    printf("Misses Compulssórios\t| %.0f (%.2f%%) \nMisses de Capacidade\t| %.0f "
           "(%.2f%%) \nMisses de Conflito\t| %.0f (%.2f%%) \n,",
           *miss_compulsorio, (*miss_compulsorio / *misses) * 100,
           *miss_capacidade, (*miss_capacidade / *misses) * 100, *miss_conflito,
           (*miss_conflito / *misses) * 100);
    printf("============================================== \n");
}