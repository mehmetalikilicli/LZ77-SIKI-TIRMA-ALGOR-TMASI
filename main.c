#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define OFFSETBITS 5
#define LENGTHBITS (8-OFFSETBITS)

#define OFFSETMASK ((1 << (OFFSETBITS)) - 1)
#define LENGTHMASK ((1 << (LENGTHBITS)) - 1)

#define GETOFFSET(x) (x >> LENGTHBITS)
#define GETLENGTH(x) (x & LENGTHMASK)
#define OFFSETLENGTH(x,y) (x << LENGTHBITS | y)

struct token {
    uint8_t offset_len;
    char c;
};


 int on_ek(char *s1, char *s2, int limit)
{
    int len = 0;

    while (*s1++ == *s2++ && len < limit)
        len++;

    return len;
}

 void kopyalama(char *s1, char *s2, int size)
{
    while (size--)
        *s1++ = *s2++;
}

char *decode(struct token *tokens, int numTokens, int *ikili_sikistir)
{
    int cap = 1 << 3;

    *ikili_sikistir = 0;

    char *decoded = malloc(cap);

    int i;
    for (i = 0; i < numTokens; i++)
    {

        int offset = GETOFFSET(tokens[i].offset_len);
        int len = GETLENGTH(tokens[i].offset_len);
        char c = tokens[i].c;

        if (*ikili_sikistir + len + 1 > cap)
        {
            cap = cap << 1;
            decoded = realloc(decoded, cap);
        }


        if (len > 0)
        {
            kopyalama(&decoded[*ikili_sikistir], &decoded[*ikili_sikistir - offset], len);
        }

        *ikili_sikistir += len;

        decoded[*ikili_sikistir] = c;

        *ikili_sikistir = *ikili_sikistir + 1;
    }

    return decoded;
}

struct token *encode(char *text, int limit, int *numTokens)
{
    int cap = 1 << 3;

    int _numTokens = 0;

    struct token t;

    char *lookahead, *search;

    struct token *encoded = malloc(cap * sizeof(struct token));

    for (lookahead = text; lookahead < text + limit; lookahead++)
    {

        search = lookahead - OFFSETMASK;

        if (search < text)
            search = text;


        int max_len = 0;


        char *max_match = lookahead;

        for (; search < lookahead; search++)
        {
            int len = on_ek(search, lookahead, LENGTHMASK);

            if (len > max_len)
            {
                max_len = len;
                max_match = search;
            }
        }


        if (lookahead + max_len >= text + limit)
        {
            max_len = text + limit - lookahead - 1;
        }


        t.offset_len = OFFSETLENGTH(lookahead - max_match, max_len);
        lookahead += max_len;
        t.c = *lookahead;

        if (_numTokens + 1 > cap)
        {
            cap = cap << 1;
            encoded = realloc(encoded, cap * sizeof(struct token));
        }

        encoded[_numTokens++] = t;
    }

    if (numTokens)
        *numTokens = _numTokens;

    return encoded;
}

char *file_read(FILE *f, int *size)
{
    char *content;
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    content = malloc(*size);
    fseek(f, 0, SEEK_SET);
    fread(content, 1, *size, f);
    return content;
}

int main(void)
{

    int n;
    char bolge[3];
    FILE *dosya = fopen("metin.txt","r");


    char* metin_ptr = (char*)malloc(sizeof(char)*100);
    *(metin_ptr)='\0';
    char gecici_satir[100000];
    int satir_sayisi=0;

    char satirlar[satir_sayisi][100000];

    fclose(dosya);
    dosya=fopen("metin.txt","r");
    int i=0,j;
    int satir_sonlari[100];
    satir_sayisi=0;
    while ( fgets(gecici_satir, 100000, dosya) != NULL )
    {
        metin_ptr=realloc(metin_ptr,sizeof(metin_ptr)+sizeof(gecici_satir));
        strcat(metin_ptr,gecici_satir);

    }
    printf("%s",metin_ptr);


    FILE *f;
    ///FILE *kaynak_metin = fopen("metin.txt", "r");
    ///char *decoded_metin = "";
    int metin_boyutu , token_sayisi, decode_boyutu;
    char *kaynak_metin = metin_ptr, *decoded_metin = "";
    metin_boyutu=strlen(kaynak_metin);
    struct token *encoded_metin;


    encoded_metin = encode(kaynak_metin, metin_boyutu, &token_sayisi);

    if (f = fopen("sikistirilmis.txt", "wb"))
    {
        fwrite(encoded_metin, sizeof(struct token), token_sayisi, f);
        fclose(f);
    }

    decoded_metin = decode(encoded_metin, token_sayisi, &decode_boyutu);

    if (f = fopen("cozumlenmis.txt", "wb"))
    {
        fwrite(decoded_metin, 1, decode_boyutu, f);
        fclose(f);
    }

    printf("\n\nGercek Boyut: %d, Sikistirilmis Boyutu: %d, Sikistirilmamis Boyutu: %d\n", metin_boyutu, token_sayisi * sizeof(struct token), decode_boyutu);

    return 0;
}
