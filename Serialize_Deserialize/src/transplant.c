#include "const.h"
#include "transplant.h"
#include "debug.h"
#include <stdlib.h>
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

/*s
 * You may modify this file and/or move the functions contained here
 * to other source files (except for main.c) as you wish.
 *
 * IMPORTANT: You MAY NOT use any array brackets (i.e. [ and ]) and
 * you MAY NOT declare any arrays or allocate any storage with malloc().
 * The purpose of this restriction is to force you to use pointers.
 * Variables to hold the pathname of the current file or directory
 * as well as other data have been pre-declared for you in const.h.
 * You must use those variables, rather than declaring your own.
 * IF YOU VIOLATE THIS RESTRICTION, YOU WILL GET A ZERO!
 *
 * IMPORTANT: You MAY NOT use floating point arithmetic or declare
 * any "float" or "double" variables.  IF YOU VIOLATE THIS RESTRICTION,
 * YOU WILL GET A ZERO!
 */

/*
 * A function that returns printable names for the record types, for use in
 * generating debugging printout.
 */
static char *record_type_name(int i) {
    switch(i) {
    case START_OF_TRANSMISSION:
	return "START_OF_TRANSMISSION";
    case END_OF_TRANSMISSION:
	return "END_OF_TRANSMISSION";
    case START_OF_DIRECTORY:
	return "START_OF_DIRECTORY";
    case END_OF_DIRECTORY:
	return "END_OF_DIRECTORY";
    case DIRECTORY_ENTRY:
	return "DIRECTORY_ENTRY";
    case FILE_DATA:
	return "FILE_DATA";
    default:
	return "UNKNOWN";
    }
}

/*
 * @brief  Initialize path_buf to a specified base path.
 * @details  This function copies its null-terminated argument string into
 * path_buf, including its terminating null byte.
 * The function fails if the argument string, including the terminating
 * null byte, is longer than the size of path_buf.  The path_length variable
 * is set to the length of the string in path_buf, not including the terminating
 * null byte.
 *
 * @param  Pathname to be copied into path_buf.
 * @return 0 on success, -1 in case of error
 */
int path_init(char *name) {
    char nameCha = *name;
    char *ptr = path_buf;
    if(new_strlen(name) > PATH_MAX)
        return -1;
    else{
        path_length = new_strlen(name) - 1;
        while(nameCha != '\0'){
            *ptr = nameCha;
            ptr = ptr + 1;
            name = name + 1;
            nameCha = *name;
        }
        *ptr = nameCha;
        return 0;
    }
}

/*
 * @brief  Append an additional component to the end of the pathname in path_buf.
 * @details  This function assumes that path_buf has been initialized to a valid
 * string.  It appends to the existing string the path separator character '/',
 * followed by the string given as argument, including its terminating null byte.
 * The length of the new string, including the terminating null byte, must be
 * no more than the size of path_buf.  The variable path_length is updated to
 * remain consistent with the length of the string in path_buf.
 *
 * @param  The string to be appended to the path in path_buf.  The string must
 * not contain any occurrences of the path separator character '/'.
 * @return 0 in case of success, -1 otherwise.
 */
int path_push(char *name) {
    char nameCha = *name;
    char *ptr = path_buf;
    if((new_strlen(name)+path_length+1) > PATH_MAX)
        return -1;
    else{
        path_length = path_length + new_strlen(name);
        //printf("%d\n", path_length);
        while(*ptr != '\0'){
            ptr = ptr + 1;
        }
        *ptr = '/';
        ptr = ptr + 1;

        while(nameCha != '\0'){
            *ptr = nameCha;
            name = name + 1;
            nameCha = *name;
            ptr = ptr + 1;
        }
        *ptr = nameCha;
        return 0;
    }
}

/*
 * @brief  Remove the last component from the end of the pathname.
 * @details  This function assumes that path_buf contains a non-empty string.
 * It removes the suffix of this string that starts at the last occurrence
 * of the path separator character '/'.  If there is no such occurrence,
 * then the entire string is removed, leaving an empty string in path_buf.
 * The variable path_length is updated to remain consistent with the length
 * of the string in path_buf.  The function fails if path_buf is originally
 * empty, so that there is no path component to be removed.
 *
 * @return 0 in case of success, -1 otherwise.
 */
int path_pop() {
    char *ptr = path_buf;
    char *checkSlash = path_buf;
    int removedStrLen = 0;
    if(path_length <= 0)
        return -1;
    else{
        while(*checkSlash != '\0'){
            if(*checkSlash == '/'){
                ptr = ptr + (path_length-1);
                while(*ptr != '/'){
                    *ptr = '\0';
                    ptr = ptr - 1;
                    removedStrLen = removedStrLen + 1;
                }
                *ptr = '\0';
                removedStrLen = removedStrLen + 1;
                path_length = path_length - removedStrLen;
                return 0;
            }
            checkSlash = checkSlash + 1;
        }
        while(*ptr != '\0'){
            *ptr = '\0';
            ptr = ptr + 1;
        }
        path_length = 0;
        return 0;
    }

}

/*
 * @brief Deserialize directory contents into an existing directory.
 * @details  This function assumes that path_buf contains the name of an existing
 * directory.  It reads (from the standard input) a sequence of DIRECTORY_ENTRY
 * records bracketed by a START_OF_DIRECTORY and END_OF_DIRECTORY record at the
 * same depth and it recreates the entries, leaving the deserialized files and
 * directories within the directory named by path_buf.
 *
 * @param depth  The value of the depth field that is expected to be found in
 * each of the records processed.
 * @return 0 in case of success, -1 in case of an error.  A variety of errors
 * can occur, including depth fields in the records read that do not match the
 * expected value, the records to be processed to not being with START_OF_DIRECTORY
 * or end with END_OF_DIRECTORY, or an I/O error occurs either while reading
 * the records from the standard input or in creating deserialized files and
 * directories.
 */
int deserialize_directory(int depth) {

    int i = 0;
    int getDepth = 0;
    int getSize = 0;
    int getMode = 0;
    int getMSize = 0;
    int nameByte = 0;
    debug("start of DE");
    for(i=3; i>=0; --i){
        getDepth += (getchar()<<(8*i));
    }
    if(depth != getDepth){
        return -1;
    }
    for(i=7; i>=0; --i){
        getSize += (getchar()<<(8*i));
    }
    debug("Size of DE %d", getSize);
    for(i=3; i>=0; --i){
        getMode += (getchar()<<(8*i));
    }
    debug("mode of DE %d", getMode);
    for(i=7; i>=0; --i){
        getMSize += (getchar()<<(8*i));
    }

    debug("mode of DE %d", getMSize);

    nameByte = getSize - 16 - 12;
    debug("name size of DE %d", nameByte);

    int temp = 0;
    while(*(name_buf+temp) != '\0'){
            *(name_buf+temp) = '\0';
            temp++;
        }

    for(i=0; i<nameByte; ++i){
        *(name_buf + i )= getchar();
    }

    debug("name of DE %s", name_buf);

    if(S_ISREG(getMode)){
        debug("DE is file");

        path_push(name_buf);
        debug("DE path_buf %s", path_buf);
        if(deserialize_file(depth))
            return -1;
        path_pop();
        debug("after file pop %s", path_buf);
    }
    else {
        debug("DE is dir");
        path_push(name_buf);
        debug("DE path_buf %s", path_buf);
        if(opendir(path_buf)){
            debug("dir exist");
        }
        else{
            debug("dir not exist");
            mkdir(path_buf, 0700);
        }
        chmod(path_buf, getMode & 0777);
    }

    return 0;
}

/*
 * @brief Deserialize the contents of a single file.
 * @details  This function assumes that path_buf contains the name of a file
 * to be deserialized.  The file must not already exist, unless the ``clobber''
 * bit is set in the global_options variable.  It reads (from the standard input)
 * a single FILE_DATA record containing the file content and it recreates the file
 * from the content.
 *
 * @param depth  The value of the depth field that is expected to be found in
 * the FILE_DATA record.
 * @return 0 in case of success, -1 in case of an error.  A variety of errors
 * can occur, including a depth field in the FILE_DATA record that does not match
 * the expected value, the record read is not a FILE_DATA record, the file to
 * be created already exists, or an I/O error occurs either while reading
 * the FILE_DATA record from the standard input or while re-creating the
 * deserialized file.
 */
int deserialize_file(int depth){
    int i = 0;
    int getDepth = 0;
    int getSize = 0;
    int getConSize = 0;
    getchar();
    getchar();
    getchar();
    char a = getchar();
    debug("FILE_DATA %d", a);
    for(i=3; i>=0; --i){
        getDepth += (getchar()<<(8*i));
    }
    if(depth != getDepth){
        return -1;
    }
    debug("depth of FD %d", getDepth);
    for(i=7; i>=0; --i){
        getSize += (getchar()<<(8*i));
    }
    debug("size of FD %d", getSize);
    getConSize = getSize - 16;
    debug("content size of FD %d", getConSize);
        if(fopen(path_buf, "r") != NULL){
            debug("file found");
            switch(global_options){
                case 4:
                    debug("ERROR! clobber is not set");
                    return -1;
                    break;
                case 12:
                    debug("clobber is set");
                    FILE *f = fopen(path_buf, "w");
                    for(i=0; i<getConSize; i++){
                        fputc(getchar(), f);
                    }
                    fclose(f);
                    break;
            }
        }
        else{
            debug("file not found");
            FILE *f = fopen(path_buf, "w");
            for(i=0; i<getConSize; i++){
                fputc(getchar(), f);
            }

        }

        debug("file return");
    return 0;
}

/*
 * @brief  Serialize the contents of a directory as a sequence of records written
 * to the standard output.
 * @details  This function assumes that path_buf contains the name of an existing
 * directory to be serialized.  It serializes the contents of that directory as a
 * sequence of records that begins with a START_OF_DIRECTORY record, ends with an
 * END_OF_DIRECTORY record, and with the intervening records all of type DIRECTORY_ENTRY.
 *
 * @param depth  The value of the depth field that is expected to occur in the
 * START_OF_DIRECTORY, DIRECTORY_ENTRY, and END_OF_DIRECTORY records processed.
 * Note that this depth pertains only to the "top-level" records in the sequence:
 * DIRECTORY_ENTRY records may be recursively followed by similar sequence of
 * records describing sub-directories at a greater depth.
 * @return 0 in case of success, -1 otherwise.  A variety of errors can occur,
 * including failure to open files, failure to traverse directories, and I/O errors
 * that occur while reading file content and writing to standard output.
 */
int serialize_directory(int depth) {
    debug("start of directory");
    debug("depth is %d", depth);
    int i = 0;
    char magic1 = 0x0C;
    char magic2 = 0x0D;
    char magic3 = 0xED;
    long int size = 16;
    putchar(magic1);
    putchar(magic2);
    putchar(magic3);
    putchar(START_OF_DIRECTORY);
    //debug("%d\n", depth);
    for(i = sizeof(depth)-1; i >= 0; --i)
        putchar((depth)>>(8*i));
    for(i = sizeof(size)-1; i>= 0; --i)
        putchar((size)>>(8*i));

    if(!opendir(path_buf)){
        //debug("hahahah");
        return -1;
    }

    DIR *dir = opendir(path_buf);
    struct dirent *de;
    struct stat stat_buf;

    while((de = readdir(dir)) != NULL){
        //debug("%s", de->d_name);
        //debug("%s", "hahahahahah");
        if(!new_strcmp(de->d_name, "..")){
            //debug("%d", 111);
            continue;
        }

        if(!new_strcmp(de->d_name, ".")){
            //debug("%d", 112);
            continue;
        }

        //printf("%s\n", de->d_name);
        //debug("%s", de->d_name);

        putchar(magic1);
        putchar(magic2);
        putchar(magic3);
        putchar(DIRECTORY_ENTRY);
        for(i = sizeof(depth)-1; i >= 0; --i)
            putchar((depth)>>(8*i));

        long int entrySize = 16 + 12 + (new_strlen(de->d_name)-1);
        //debug("entry size: %ld", entrySize);
        for(i = sizeof(entrySize)-1; i>= 0; --i)
            putchar((entrySize)>>(8*i));

        debug("before push %s", path_buf);
        path_push(de->d_name);
        stat(path_buf, &stat_buf);
        //debug("%s", de->d_name);
        //debug("%d", new_strlen(de->d_name)-1);

        for(i = sizeof(stat_buf.st_mode)-1; i>=0; --i)
            putchar((stat_buf.st_mode)>>(8*i));
        for(i = sizeof(stat_buf.st_size)-1; i>=0; --i)
            putchar((stat_buf.st_size)>>(8*i));

        char *nameStr = de->d_name;
        while(*nameStr != '\0'){
            putchar(*nameStr);
            nameStr = nameStr + 1;
        }

        debug("after push before pop%s", path_buf);

    switch(de->d_type){
        case DT_DIR:
            if(serialize_directory(depth+1)){
                return -1;
            }
            else
                path_pop();
            break;
        case DT_REG:
            if(serialize_file(depth, stat_buf.st_size))
                return -1;
            else
                path_pop();
            break;
    }
    debug("after pop %s", path_buf);
}


        putchar(magic1);
        putchar(magic2);
        putchar(magic3);
        putchar(END_OF_DIRECTORY);
        for(i = sizeof(depth)-1; i >= 0; --i)
            putchar((depth)>>(8*i));
        //debug("%ld", size);
        for(i = sizeof(size)-1; i>= 0; --i)
            putchar((size)>>(8*i));

        closedir(dir);
        return 0;
}

/*
 * @brief  Serialize the contents of a file as a single record written to the
 * standard output.
 * @details  This function assumes that path_buf contains the name of an existing
 * file to be serialized.  It serializes the contents of that file as a single
 * FILE_DATA record emitted to the standard output.
 *
 * @param depth  The value to be used in the depth field of the FILE_DATA record.
 * @param size  The number of bytes of data in the file to be serialized.
 * @return 0 in case of success, -1 otherwise.  A variety of errors can occur,
 * including failure to open the file, too many or not enough data bytes read
 * from the file, and I/O errors reading the file data or writing to standard output.
 */
int serialize_file(int depth, off_t size) {
    debug("serialize file start");
    int i = 0;
    char magic1 = 0x0C;
    char magic2 = 0x0D;
    char magic3 = 0xED;
    putchar(magic1);
    putchar(magic2);
    putchar(magic3);
    putchar(FILE_DATA);

    for(i = sizeof(depth)-1; i >= 0; --i)
            putchar((depth)>>(8*i));
    long int fileSize = 16 + size;
    for(i = sizeof(fileSize)-1; i>= 0; --i)
            putchar((fileSize)>>(8*i));
    FILE *f = fopen(path_buf, "r");
    //if(f){
        char fileCha = fgetc(f);
        while(fileCha != EOF && fileCha != '\0'){
            putchar(fileCha);
            fileCha = fgetc(f);
        }
        fclose(f);
    //}
        return 0;
}

/**
 * @brief Serializes a tree of files and directories, writes
 * serialized data to standard output.
 * @details This function assumes path_buf has been initialized with the pathname
 * of a directory whose contents are to be serialized.  It traverses the tree of
 * files and directories contained in this directory (not including the directory
 * itself) and it emits on the standard output a sequence of bytes from which the
 * tree can be reconstructed.  Options that modify the behavior are obtained from
 * the global_options variable.
 *
 * @return 0 if serialization completes without error, -1 if an error occurs.
 */
int serialize() {
    debug("The starting of transmission");
    //debug("%d", depth);
    int i = 0;
    char magic1 = 0x0C;
    char magic2 = 0x0D;
    char magic3 = 0xED;
    int depth = 0;
    long int size = 16;
    putchar(magic1);
    putchar(magic2);
    putchar(magic3);
    putchar(START_OF_TRANSMISSION);
    for(i = sizeof(depth)-1; i >= 0; --i)
        putchar((depth)>>(8*i));
    for(i = sizeof(size)-1; i>= 0; --i)
        putchar((size)>>(8*i));


    if(serialize_directory(depth+1)){
        return -1;
    }
    //printf("%s\n", "haha");
    //debug("%d\n", depth);
    putchar(magic1);
    putchar(magic2);
    putchar(magic3);
    putchar(END_OF_TRANSMISSION);
    for(i = sizeof(depth)-1; i >= 0; --i)
        putchar((depth)>>(8*i));
    for(i = sizeof(size)-1; i>= 0; --i)
        putchar((size)>>(8*i));

    return 0;
}

/**
 * @brief Reads serialized data from the standard input and reconstructs from it
 * a tree of files and directories.
 * @details  This function assumes path_buf has been initialized with the pathname
 * of a directory into which a tree of files and directories is to be placed.
 * If the directory does not already exist, it is created.  The function then reads
 * from from the standard input a sequence of bytes that represent a serialized tree
 * of files and directories in the format written by serialize() and it reconstructs
 * the tree within the specified directory.  Options that modify the behavior are
 * obtained from the global_options variable.
 *
 * @return 0 if deserialization completes without error, -1 if an error occurs.
 */
int deserialize() {
    int i = 0;
    int getDepth = 0;
    int depth = 0;
    int SODNum = 0;
    //debug("path_buf is aa %s", path_buf);
    //FILE *f = fopen("outfile", "r");
    //if(f){
        char fileCha = getchar();
        while(fileCha != EOF){
            //debug("char from file %d", fileCha);
            if(fileCha == 0x0C){
                debug("beginning of a header");
                fileCha = getchar();
                fileCha = getchar();
                //debug("1 %d", fileCha);
                fileCha = getchar();
                //debug("%d", fileCha);
                switch (fileCha){
                    case START_OF_TRANSMISSION:
                        debug("START_OF_TRANSMISSION");
                        for(i = 0; i<13; i++)
                            fileCha = getchar();
                        break;
                    case START_OF_DIRECTORY:
                        SODNum++;
                        debug("START_OF_DIRECTORY");
                        getDepth = 0;
                        for(i=3; i>=0; --i){
                            fileCha = getchar();
                            getDepth += (fileCha<<(8*i));
                        }
                        debug("depth of SOD is %d", getDepth);
                        for(i = 0; i<8; i++){      //size is always 16. get rid of the size.
                            fileCha = getchar();
                        }
                        //debug("aaaaaaa%d", fileCha);
                            getchar();
                            getchar();
                            getchar();
                            getchar();//start with type

                        if(deserialize_directory(getDepth)){
                            return -1;
                        }
                        fileCha = getchar();
                        debug("exit");
                        //debug("aaa %d", getchar()); // starts at 0D
                        //fileCha = getchar();
                        //debug("finish ");
                        break;
                    case DIRECTORY_ENTRY:
                        debug("DIRECTORY_ENTRY");
                        debug("DE depth %d", getDepth);
                        if(deserialize_directory(getDepth)){
                            return -1;
                        }
                        fileCha = getchar();
                        break;
                    case END_OF_DIRECTORY:
                        debug("END_OF_DIRECTORY");
                        path_pop();
                        debug("# of End of dirrectory need %d", SODNum);
                        SODNum--;
                        for(i = 0; i<13; i++)
                            fileCha = getchar();
                        break;
                    case END_OF_TRANSMISSION:
                        debug("END_OF_TRANSMISSION");
                        //debug("12%d", SODNum);
                        if(SODNum != 0)
                            return -1;
                        for(i = 0; i<13; i++)
                            fileCha = getchar();
                        break;
                }

            }
            //fileCha = getchar();
            //debug("aaaaa%d", fileCha);
        }
        //fclose(f);
    return 0;
}

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the selected program options will be set in the
 * global variable "global_options", where they will be accessible
 * elsewhere in the program.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * Refer to the homework document for the effects of this function on
 * global variables.
 * @modifies global variable "global_options" to contain a bitmap representing
 * the selected options.
 */
int validargs(int argc, char **argv)
{

    int ind;

    argv = argv + 1;
    char *reqFlag = *argv;
    char *stras = *argv;

    //printf("this is argc %d\n", argc);
    if(argc <= 1 || argc > 5){
        return -1;
    }
    if(!new_strcmp(reqFlag, "-h")){
        global_options |= 1;
        //printf("this is -h %d\n", 1);ppppppppppppppppppppppppppppppppppppu
        return 0;

    }
    else if(!new_strcmp(reqFlag, "-s")){

        argv = argv + 1;
        char *optFlag = *argv;
        debug("argument is %s", *argv);

        if(optFlag == NULL){
            path_init(".");
            global_options |= 2;
            return 0;
        }
        else if(!new_strcmp(optFlag, "-p")){
            argv = argv + 1;
            char *dirFlag = *argv;
            if(dirFlag == NULL || is_arg(dirFlag)){
                return -1;
            }
            else{
                path_init(dirFlag);
                global_options |= 2;
                return 0;
            }
        }
        else{
        return -1;
        }
    }
    else if(!new_strcmp(reqFlag, "-d")){
        argv = argv + 1;
        char *optFlag = *argv;
        //printf("this is -d -p fail %s\n", optFlag);
        debug("-d flag given");
        debug("optFlag  is %s", optFlag);
        if(optFlag == NULL){
            path_init(".");
            debug("optFlag is NULL");
            global_options |=4;
            return 0;
        }
        else if(!new_strcmp(optFlag, "-p")){
            debug("-p flag detected");
            argv = argv + 1;
            char *dirFlag = *argv;
            //printf(" haha %s\n", dirFlag);

////////////////////////////////////////////////////////////////
            if(dirFlag == NULL || is_arg(dirFlag)){

                return -1;
            }

            else{
                //debug("i am here hahahahah");
                path_init(dirFlag);
                //printf("this is -d -p fail %d\n", 1);
                argv = argv + 1;
                optFlag = *argv;
                //printf("this is -d -p fail %s\n", optFlag);
                if(optFlag != NULL && !new_strcmp(optFlag, "-c")){
                    //printf("this is -d -p fail %d\n", 100);
                    global_options |= 8;
                    global_options |= 4;
                    return 0;
                }
                else{
                    //printf("this is -d -p fail %d\n", 3);
                    global_options |= 4;
                    return 0;
                }
            }
        }

        else if(optFlag != NULL && !new_strcmp(optFlag, "-c")){
            argv = argv + 1;
            optFlag = *argv;
            if(optFlag == NULL){
                path_init(".");
                debug("optFlag is NULL");
                global_options |= 8;
                return 0;
            }
            if(optFlag != NULL && !new_strcmp(optFlag, "-p")){                          //this check will assume bin/transplant -d will always floow -c and -p
                argv = argv + 1;
                char *dirFlag = *argv;
                if(dirFlag == NULL || is_arg(dirFlag))
                    return -1;

                else{
                    path_init(dirFlag);
                    global_options |= 8;
                    global_options |= 4;
                    return 0;
                }
            }
            else{
                //printf("%d\n", 111);
                global_options |= 4;
                return 0;
            }
        }
        else{

            return -1;
        }
        }
    else
        return -1;
    return EXIT_SUCCESS;
}


