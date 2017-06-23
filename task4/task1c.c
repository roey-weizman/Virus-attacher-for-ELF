#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void PrintHex(char buffer[],int length){
    int i=0;
    for(i=0;i<length;i++){
        printf("%02X ",buffer[i]& 0xFF); /*TODO:for some reason this works.*/
    }
    printf("\n");
}

typedef struct virus virus;

struct virus {
    unsigned short length;
    char name[16];
    char signature[];
};
typedef struct link link; /*this helps to remove the need to write struct every time.*/

struct link {
    virus *v;
    link *next;
};

/*Recursive*/
void list_print(link *virus_list){
    printf("Name: %s\n",virus_list->v->name);
    printf("Length: %d\n",virus_list->v->length);
    printf("Signature: \n");
    PrintHex(virus_list->v->signature,virus_list->v->length);
    printf("\n");

    if(virus_list->next!=0){
        list_print(virus_list->next);
    }
}

/* Add a new link with the given data to the list
   (either at the end or the beginning, depending on what your TA tells you),
   and return a pointer to the list (i.e., the first link in the list).
   If the list is null - create a new entry and return a pointer to the entry. */

link* list_append(link* virus_list, virus* data){
    /*implemented at the begining, more efficent*/
    link* newNode=(link*)malloc(sizeof(link)); /*check this*/
    newNode->v=data;
    if(virus_list==0){
        newNode->next=0; /*reset pointer*/
        return newNode;
    }
    else{
        newNode->next=virus_list;
        return newNode;
    }
}
/* Free the memory allocated by the list. */
void list_free(link *virus_list){
    link* node=virus_list;
    while(node!=0){
        link* temp=node;
        node=node->next;
        //printf("deleting: %s\n",temp->v->name);
        free(temp->v); /*first clean the virus*/
        free(temp);/* now clear the node*/
    }
}
void detect_virus(char *buffer, link *virus_list, unsigned int size){
    /*thats going to be |viruslist|*n^2 complexity*/
    unsigned int i,j,k,virusSignatureLength,countVirus=0;
    /*  for each byte in the buffer we will do:
     *      1.take a virus from the list
     *      2.compare the signature bytes one by one until we have a match
     *      3.if in any time there is no match,we jump back and test next virus
     *      4. if no viruses were a match, continue to next byte.
     *
     *
     * */
    for(i=0;i<size;i++){
        link* vtemp=virus_list;
        while(vtemp!=0){

            if((size-i)>vtemp->v->length){
                /*enough bytes to compare | for some reason this works*/
                int compare=memcmp(&buffer[i],vtemp->v->signature,vtemp->v->length);
                if(compare==0){
                    printf("@@\n");
                    printf("Found A virus! at line %X :\n",i);
                    printf("@@@@@@\n");
                    printf("Name: %s\n",vtemp->v->name);
                    printf("Length: %d\n",vtemp->v->length);
                    printf("Signature: \n");
                    PrintHex(vtemp->v->signature,vtemp->v->length);
                    countVirus++;
                    break; /*continue to next virus*/
                }
            }
            vtemp=vtemp->next;/*continue to next virus*/

        }
    }
    if(countVirus>0){
        printf("\n\n@@@ TotalViruses found: %d\n\n\n",countVirus);
    }
    else
        printf("\n\n@@@ no VIRUS: \n\n\n");
}

int main(int argc, char **argv) {
    int fileLength=0;
    int i,j,k;
    char isBigEndian;
    char isUnknownNameSize;
    link* virusList=0;
    FILE *fp= 0;
    fp=fopen(argv[1], "rb");/*open for Read in Binary mode*/
    if(fp==NULL){
        perror("could not load viruses file");
        exit(1);
    }
    fseek(fp,0,SEEK_END); /*gets info of the size of the file*/
    fileLength=ftell(fp);
    /*fread(void *ptr, size_t size, size_t nmemb, FILE *stream)*/
    /*                  1 here loads one byte into the buffer array */
    rewind(fp); /*rewind pointer back to start*/
    unsigned char* buffer=(unsigned char*)malloc(sizeof(unsigned char)*fileLength); /*allocate space for file.*/

    fread(buffer,1,fileLength,fp); /*TODO: how does it really read byte? since each 'word' is like "01"*/
    //printf("%d :: is length size\n",fileLength);
//    for(i=0;i<fileLength;i++){
//        printf("%02X ",buffer[i]& 0xFF);
//    }
//    printf("\n");
//    for(i=0;i<fileLength;i++){
//        printf("%c ",buffer[i]);
//    }
    printf("\n");
    /*virus code:*/
    if(buffer[0]=='\0'){
        isBigEndian='0';
    }
    else{
        isBigEndian='1';
    }

    /*pay anntatntion to the offset.*/
    unsigned short signatureLength;
    i=1;
    while(i<fileLength){/*got over the buffer*/
        if(isBigEndian='1'){
            /*increment after use*/
            signatureLength=(buffer[i++]*16)+buffer[i++]-16-2;
        }
        else{/*litte endian*/
            signatureLength=buffer[i++]+(buffer[i++]*16)-16-2;
        }   
        virus* temp=(virus*)malloc(signatureLength+16+2);/* size: 2 length, 16 name, signature is dynamic with structlength*/
        virus v1;
        //v1.length=structLength;
       temp->length=signatureLength;
        for(j=0;j<16;j++){
             temp->name[j]=buffer[i++];
            }

            for(j=0;j<signatureLength;j++){
                temp->signature[j]=buffer[i++];
            }

            virusList=list_append(virusList,temp);
           printf("Name: %s\n",temp->name);
           printf("Length: %d\n",temp->length);
           printf("Signature: \n");
           PrintHex(temp->signature,temp->length);
            //printf("\n");
            //free(temp);

    }

    //list_print(virusList);


    /*TODO: task 2 starts here*/
    /*close buffer and file and load new infected binary.*/
    free(buffer);
    fclose(fp);

    fp= 0;
    fp=fopen(argv[2], "rb");/*open for Read in Binary mode*/
    if(fp==NULL){
        perror("could not load infected  file");
        exit(1);
    }
    fseek(fp,0,SEEK_END); /*gets info of the size of the file*/
    fileLength=ftell(fp);
    printf("Infected file size:%d\n",fileLength);
    rewind(fp);
    buffer=(unsigned char*)malloc(sizeof(unsigned char)*fileLength); /*allocate space for file.*/
    fread(buffer,1,fileLength,fp);
    detect_virus(buffer,virusList,fileLength);

    /*free Virus*/
    list_free(virusList);



/*
    PrintHex(buffer,length);
*/
    free(buffer);
    fclose(fp);

    return 0;
}


