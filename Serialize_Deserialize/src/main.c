#include <stdio.h>
#include <stdlib.h>

#include "const.h"
#include "debug.h"
#include "helperc.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

int main(int argc, char **argv)
{
    int ret = 1;


    //serialize();
    //int number = 1;%
    //int x = new_strlen("abcddf");
    //printf("%d\n", PATH_MAX);
    //path_init("abcdddddf");
    //path_push("12345");
    //path_pop();
    //for(ret = 0; ret < 20; ret++)
    //printf("%c\n", path_buf[ret]);
    //printf("%d\n", path_length);
    //printf("1 = %d\n", global_options);
    if(validargs(argc, argv))
        USAGE(*argv, EXIT_FAILURE);

    debug("path_buf is %s", path_buf);

    debug("validargs success");
    debug("global_options set %d", global_options);
    switch(global_options){
        case 2:
            //for(int i = 0; i<= 20; i++)
            //debug("%c",path_buf[i]);
            if(serialize())
                return EXIT_FAILURE;
            break;
        case 4:
            debug("deserialize called");
            if(deserialize())
                return EXIT_FAILURE;
            break;
        case 12:
            debug("deserialize with clobber called");
            if(deserialize())
                return EXIT_FAILURE;
            break;


    }
    //printf("2 = %d\n", global_options);
    //printf("%d\n", global_options & 1)

    debug("Options: 0x%x", global_options);
    if(global_options & 1)
        USAGE(*argv, EXIT_SUCCESS);

    return EXIT_SUCCESS;
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */
