/**
 * Copyright (c) 2023 System233
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "io/pe.h"
#include <stdio.h>
int main(int argc, char const *argv[])
{
    IMAGE_FILE*image=image_load(argv[0]);
    if(!image){
        printf("load %s fail\n",argv[0]);
        return -1;
    }
    printf("loaded %s\n",argv[0]);

    int len=image_dump(image,"dumped.exe");
    printf("dumped %d\n",len);

    image_free(image);
    printf("test end");
    return 0;
}
