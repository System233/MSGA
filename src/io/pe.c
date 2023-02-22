/**
 * Copyright (c) 2023 System233
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "io/pe.h"
#include "msga_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



IMAGE_FILE*image_load(const char*file){
    FILE*fp=fopen(file,"rb");
    if(fp==NULL){
        return NULL;
    }
#define _FREAD(X,NUM) if((NUM)&&fread(X,sizeof(*(X)),(NUM),fp)!=(NUM)){fprintf(stderr,"read %s fail\n",#X);goto err;}

    IMAGE_FILE*image=calloc(1,sizeof(IMAGE_FILE));
    _FREAD(&image->dosHeader,1);

    int dosDataSize=image->dosHeader.e_lfanew-sizeof(image->dosHeader);
    image->dosData=calloc(1,dosDataSize);
    _FREAD(image->dosData,dosDataSize);
    
    _FREAD(&image->ntHeader,1);
    _FREAD((BYTE*)&image->optionalHeader64,image->ntHeader.FileHeader.SizeOfOptionalHeader);

    IMAGE_SECTION*s=&image->sections;
    for(int i=0;i<image->ntHeader.FileHeader.NumberOfSections;++i){
        IMAGE_SECTION*section=calloc(1,sizeof(IMAGE_SECTION));
        s->next=section;
        section->prev=s;
        s=section;
        _FREAD(&section->header,1);
        // fread(&section->header,sizeof(section->header),1,fp);
    }
    image->sections.prev=s;
    s=image->sections.next;
    if(s){
        s->prev=NULL;
    }
    while (s)
    {
        s->data=calloc(1,s->header.SizeOfRawData);
        fseek(fp,s->header.PointerToRawData,SEEK_SET);
        _FREAD(s->data,s->header.SizeOfRawData);
        s=s->next;
    }
    if(image->ntHeader.FileHeader.PointerToSymbolTable){
        image->symbolTable=calloc(image->ntHeader.FileHeader.NumberOfSymbols,sizeof(IMAGE_SYMBOL));
        fseek(fp,image->ntHeader.FileHeader.PointerToSymbolTable,SEEK_SET);
        _FREAD(image->symbolTable,image->ntHeader.FileHeader.NumberOfSymbols);
        _FREAD(&image->symbolSize,1);
        image->symbolData=calloc(image->symbolSize-sizeof(image->symbolSize),1);
        _FREAD(image->symbolData,image->symbolSize-sizeof(image->symbolSize));
    }

    fclose(fp);
    return image;
err:
    image_free(image);
    fclose(fp);
    return NULL;

#undef _FREAD
}

int image_dump(IMAGE_FILE*image,const char*file){
    FILE*fp=fopen(file,"wb");
#define _FWRITE(X,NUM) if((NUM)&&fwrite(X,sizeof(*(X)),(NUM),fp)!=(NUM)){fprintf(stderr,"write %s fail\n",#X);goto err;}
    if(fp==NULL){
        return 0;
    }

    IMAGE_SECTION*s=image->sections.next;
    int numSection=0;
    while (s)
    {
        numSection++;
        s=s->next;
    }
    int headerSize=image->dosHeader.e_lfanew+sizeof(image->ntHeader)+image->ntHeader.FileHeader.SizeOfOptionalHeader;
    int sectionBase=headerSize+numSection*IMAGE_SIZEOF_SECTION_HEADER;
    int fileAlignment=0;
    if(image->optionalHeader64.Magic==IMAGE_NT_OPTIONAL_HDR64_MAGIC){
        fileAlignment=image->optionalHeader64.FileAlignment;
        sectionBase=MSGA_ALIGN(sectionBase,fileAlignment);
        image->optionalHeader64.SizeOfHeaders=sectionBase;
    }else if(image->optionalHeader32.Magic==IMAGE_NT_OPTIONAL_HDR32_MAGIC){
        fileAlignment=image->optionalHeader32.FileAlignment;
        sectionBase=MSGA_ALIGN(sectionBase,fileAlignment);
        image->optionalHeader64.SizeOfHeaders=sectionBase;
    }else{
        goto err;
    }

    s=image->sections.next;
    int sectionIndex=0;
    int sectionOffset=0;
    while (s)
    {
        s->header.PointerToRawData=sectionBase+sectionOffset;
        fseek(fp,headerSize+sectionIndex*IMAGE_SIZEOF_SECTION_HEADER,SEEK_SET);
        _FWRITE(&s->header,1);

        fseek(fp,s->header.PointerToRawData,SEEK_SET);
        _FWRITE(s->data,s->header.SizeOfRawData);
        sectionOffset=MSGA_ALIGN(sectionOffset+s->header.SizeOfRawData,fileAlignment);
        s=s->next;
    }

    if(image->symbolTable){
        image->ntHeader.FileHeader.PointerToSymbolTable=sectionBase+sectionOffset;
        fseek(fp,image->ntHeader.FileHeader.PointerToSymbolTable,SEEK_SET);
        _FWRITE(image->symbolTable,image->ntHeader.FileHeader.NumberOfSymbols);
        _FWRITE(&image->symbolSize,1);
        _FWRITE(image->symbolData,image->symbolSize-sizeof(image->symbolSize));
    }
    int len=ftell(fp);
    fseek(fp,0,SEEK_SET);
    _FWRITE(&image->dosHeader,1);
    _FWRITE(image->dosData,image->dosHeader.e_lfanew-sizeof(image->dosHeader));
    _FWRITE(&image->ntHeader,1);
    _FWRITE((BYTE*)&image->optionalHeader64,image->ntHeader.FileHeader.SizeOfOptionalHeader);

    fclose(fp);
    return len;
err:
    fclose(fp);
    return 0;
#undef _FWRITE
}
IMAGE_DATA_DIRECTORY*image_get_dir(IMAGE_FILE*image,int entry){
    if(image->optionalHeader64.Magic==IMAGE_NT_OPTIONAL_HDR64_MAGIC){
        return &image->optionalHeader64.DataDirectory[entry];
    }
    if(image->optionalHeader32.Magic==IMAGE_NT_OPTIONAL_HDR32_MAGIC){
        return &image->optionalHeader32.DataDirectory[entry];
    }
    return NULL;
}
int image_set_reloc(IMAGE_FILE*image,msga_addr_t*va_list,int num){
    IMAGE_DATA_DIRECTORY*reloc=image_get_dir(image,IMAGE_DIRECTORY_ENTRY_BASERELOC);
    if(!reloc){
        goto err;
    }
    // image->optionalHeader64.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
    
err:
    return 0;
}
void image_clear_reloc(IMAGE_FILE*image,msga_addr_t*va_list,int num){
    
}
void image_free(IMAGE_FILE*image){
    free(image->dosData);
    free(image->symbolTable);
    free(image->symbolData);
    IMAGE_SECTION*s=image->sections.next;
    while (s)
    {
        IMAGE_SECTION*p=s->next;
        free(s->data);
        free(s);
        s=p;
    }
    free(image);
}
void image_read(IMAGE_FILE*image){
    
}
void image_write(IMAGE_FILE*image){
    
}
void image_mmap(IMAGE_FILE*image){
    
}
void image_munmap(IMAGE_FILE*image){
    
}