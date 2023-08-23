#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

#define MAXWORDS 1024

// LINKED LISTS ///////////////////////////////////////////////////////////////////////////////////////////////

typedef struct n{
    char* value;
    struct n* next;
    bool isFile;
}Node;

typedef struct ll{
    Node* head;
    Node* tail;
    int size;
}LinkedList;

//initializes an empty linked list
LinkedList* createList(){
    LinkedList* ll = (LinkedList*)malloc(sizeof(LinkedList));
    ll->head = NULL;
    ll->size = 0;
    ll->tail = NULL;
    return ll;
}

//appends a node at the end of a linked list
void append(LinkedList* ll, const char* value, bool isFile){
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->next = NULL;
    newNode->value = strdup(value);
    newNode->isFile = isFile;
    ll->size++;
    //if linkedlist is empty, head and tail will be the new node
    if(ll->head == NULL){
        ll->head = newNode;
        ll->tail = newNode;
        return;
    }
    ll->tail->next = newNode;
    ll->tail = newNode;
    return;
}

// MAIN ///////////////////////////////////////////////////////////////////////////////////////////////

LinkedList* allowedExtensions = NULL;
int numberOfLines;
int numberOfFiles;
int numberOfEmptyLines;

//get input from user, given the message
char* input(char* message){
    char* input = NULL;
    size_t buffer_size = 0;
    printf(message);
    getline(&input, &buffer_size, stdin);
    input[strcspn(input, "\n")] = '\0';
    return input;
}

//check if a string ends with a certain substring
bool endsWith(char *str, const char *suffix) {
    int strLen = strlen(str);
    int suffLen = strlen(suffix);
    if (strLen < suffLen) {
        return false;
    }
    char *end_of_str = str + (strLen - suffLen);
    if (strcmp(end_of_str, suffix) == 0) {
        return true;
    }
    return false; 
}

//checks if a file is forbidden
bool isAllowed(const char* filepath, LinkedList* allowedList){
    Node* iter = allowedList->head;
    while(iter){
        if(endsWith(filepath, iter->value) == true){
            return true;
        }   
        iter = iter->next;
    }
    return false;
}

//checks if a string is just whitespace
bool isWhiteSpace(char* string){
    while (*string) {
        if (!isspace((unsigned char)*string)) {
            return false;
        }
        string++;
    }
    return true;
}

//prints out and counts the lines of a file, given its path.
int findLines(char* filePath) {
    printf("OPENING FILE: %s\n", filePath);
    char path[MAXWORDS];
    strcpy(path, filePath);
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return -1;
    }
    char line[256];
    int numLines = 0;
    bool endsWithNewLine = false;
    while (fgets(line, sizeof(line), file)) {
        endsWithNewLine = false;
        numLines++;
        numberOfLines++;
        if (isWhiteSpace(line)){
            numberOfEmptyLines++;
        }
        printf("%d. %s", numberOfLines, line);
        if(endsWith(line, "\n")){
            endsWithNewLine = true;
        }
    }
    if(!endsWithNewLine){
        printf("\n");
    }
    fclose(file);
    return numLines;
}

//walk folderpath 
int walk(char* folderPath){
    DIR* directory;
    struct dirent *entry;
    directory = opendir(folderPath);
    while ((entry = readdir(directory)) != NULL) {
        if(
            strcmp(entry->d_name, "..") == 0 
            || 
            strcmp(entry->d_name, ".") == 0
        ){
            continue;
        }
        if (entry->d_type == DT_REG) {
            if(isAllowed(entry->d_name, allowedExtensions)){
                numberOfFiles++;
                char filePath[MAXWORDS];
                snprintf(filePath, sizeof(filePath), "%s/%s", folderPath, entry->d_name);
                findLines(filePath);
            }
        }else if(entry->d_type == DT_DIR){
            char filePath[MAXWORDS];
            snprintf(filePath, sizeof(filePath), "%s/%s", folderPath, entry->d_name);
            walk(filePath);
        }
    }
}

//one-line format:
//[readlines], [this/PATH], [extension]
int main(int argc, char *argv[]){
    while(1){
        numberOfLines = 0;
        numberOfFiles = 0;
        numberOfEmptyLines = 0;
        char* folderPath;
        char* extensions;
        allowedExtensions = createList();
        //get command line arguments
        if(argc == 2 || argc > 3){
            //invalid number of arguments
            printf("Unknown command.\n");
            return 0;
        }
        if(argc == 1){
            //no argument. 
            folderPath = input("Enter the target folder path.\n");
            extensions = input("Enter the file extensions you'd like to read, seperated by forwardslashes (/) if there are multiple.\n");
        }
        if(argc == 3){
            //arguments present
            folderPath = argv[1];
            extensions = argv[2];
        }
        if(strcmp(folderPath, "this") == 0){
            //get current directory as a string
            char dir[MAXWORDS]; 
            getcwd(folderPath, sizeof(dir));
        }
        char* token = strtok(extensions, "/");
        while(token != NULL){
            append(allowedExtensions, token, false);
            token = strtok(NULL, "/");
        }
        size_t len = strlen(folderPath);
        DIR* directory = opendir(folderPath);
        if (directory == NULL) {
            printf("Invalid directory specified.\n");
            continue;
        }
        //start walking...
        numberOfLines = 0;
        numberOfEmptyLines = 0;
        numberOfFiles = 0;
        walk(folderPath);
        printf("\nFiles scanned: %d\n", numberOfFiles);
        printf("Total number of lines: %d\n", numberOfLines);
        printf("Total number of blank lines: %d\n", numberOfEmptyLines);
        printf("Total number of lines, excluding blank lines: %d\n\n", numberOfLines-numberOfEmptyLines);
        if(argc == 3){
            break;
        }
    }
    return 0;
}