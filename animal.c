
/*
 *Author: Grace Thompson
 *Date Started: 02/09/16
 *Last Modified: 02/12/16
 *File: animal.c
 *Description: this program generates an animal guessing game based off a binary tree.
 *If there is no file already that already exists for the game, the program begins the
 *game with one node containing a guess of 'cat.' From there, depending on the user's
 *responses, the program learns new animals and adds the new questions and animals
 *to the tree. If a file already exists, the program reads in the tree and generates
 *the tree to use for the game upon initialization. The user can continue
 *to play the game as long as they like until the user specifies to stop.
 
 *Reference: OSU 'Guess the Animal Extra Credit' page for Summer '11 found online
 * to aid in understanding writing and reading tree from file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

//utility procedure to store a string read in from the user
char * newStr (char * charBuffer) {
    char *p = (char *)malloc(1 + strlen(charBuffer));
    strcpy(p, charBuffer);
    return p;
}

struct Node {
    char *value;
    char type; //can be A for animal or Q for question
    struct Node *no;    //left node when answer is no
    struct Node *yes;   //right  node when answer is yes
};

struct ATree {
    struct Node *root;
    int count;
};

/*initialize  tree members to one node with the value 'cat'
 param: tree to be initialized
 pre: tree is not null
 post: tree size is 1, with root member field set to 'cat'
 */
void initTree(struct ATree *tree) {
    tree->root = malloc(sizeof(struct Node));
    tree->root->value = "cat";
    tree->root->type = 'A';
    tree->root->no = 0;
    tree->root->yes = 0;
    tree->count = 1;
}

/*function to read contents of file and build binary tree
 param: file must be successfully open
 pre: file contains proper order of tree elements
 post: binary tree is constructed and returned by its root node
 */
struct Node *readTree(FILE *f) {
    char c; //store current character read from file
    char buffer[100]; //stores input as its read from file
    char *stringFromFile;
    struct Node *fileTree = (struct Node *)malloc(sizeof(struct Node));   //start at root of tree
    
   // while ((c = getc(f)) != EOF) {   //read in next character to see if an animal node or a question node
    c = fgetc(f);
    if (c != EOF) {
        //c = fgetc(f);
        if (c == 'A') {
            //read in animal string
            fgets(buffer, 100, f);
            stringFromFile = newStr(buffer);
            fileTree->value = stringFromFile;
            fileTree->type = 'A';
            fileTree->no = 0;   //leaf node, so set children to null
            fileTree->yes = 0;
        } else {
            fgets(buffer, 100, f);
            stringFromFile = newStr(buffer);
            fileTree->value = stringFromFile;
            fileTree->type = 'Q';   //type is a question
            fileTree->no = readTree(f);     //read left child
            fileTree->yes = readTree(f);    //read right child
        }

    }
    return fileTree;
}

/*function to allocate memory for binary tree and, if file exists, get contents of 
 tree from the file. If no file exists, initialize a tree.
 param: none
 pre: none
 post: binary tree is created and ready for game use
 */
struct ATree* createTree() {
    struct ATree *tree = (struct ATree *)malloc(sizeof(struct ATree));
    assert(tree != 0);
    
    FILE *inputFile = fopen("animalGame.txt", "r");
    if (inputFile == NULL) {    //if no file can be found, initialize tree to start with 'cat'
        initTree(tree);
    } else {
        tree->root = readTree(inputFile);
    }
    
    return tree;
}

//free the nodes on the tree
void _freeTree(struct Node *node) {
    if (node != 0) {
        _freeTree(node->no);
        _freeTree(node->yes);
        free(node);
    }
}

//clear the nodes of the tree and set members to 0
void clearTree(struct ATree *tree) {
    _freeTree(tree->root);
    tree->root = 0;
    tree->count = 0;
}

//delete the tree
void deleteTree(struct ATree *tree) {
    clearTree(tree);
    free(tree);
}

//returns 1 if empty
int isEmptyTree(struct ATree *tree) {
    return (tree->count == 0);
}

//returns size of tree
int sizeTree(struct ATree *tree) {
    assert(tree != 0);
    return tree->count;
}

/*function to add a new question node and set animal children nodes.
 The current node (which used to contain an animal guess) becomes the new
 question node, and the old animal (which was a 'no'), becomes the new 'no'
 leaf node, while the new animal input by the user becomes the 'yes' leaf node.
 param: pointer to current node, the animal for response 'no', the new question,
 and the animal for the response 'yes.'
 pre: node is not null
 post: new question and guesses added to tree at current location
 */
void addQuestion(struct Node *current, char *noAnimal, char *newQuestion, char *yesAnimal) {
    //make new leaf nodes for the yes animal and the no animal
    struct Node *noLeaf = malloc(sizeof(struct Node));
    assert(noLeaf != 0);
    struct Node *yesLeaf = malloc(sizeof(struct Node));
    assert(yesLeaf != 0);
    
    //set Node values for the new yes and no guesses, leaf nodes have no children
    noLeaf->value = noAnimal;
    noLeaf->type = 'A';
    noLeaf->no = 0;
    noLeaf->yes = 0;
    
    yesLeaf->value = yesAnimal;
    yesLeaf->type = 'A';
    yesLeaf->no = 0;
    yesLeaf->yes= 0;
    
    current->value = newQuestion;
    current->type = 'Q';    //set current node to question type
    current->no = noLeaf;
    current->yes = yesLeaf;
}

/*function to ask a question
 param: current node
 pre: node is not null
 post: returns 'Y' if answer was yes, 'N' if answer was no
 */
char askQuestion(struct Node *current) {
    assert(current != 0); //node is not null
    char buffer[100];
    char answer;
    char *question = current->value;
    printf("%s", question); //ask the question
    fgets(buffer, 100, stdin);
    if (buffer[0] == 'Y' || buffer[0] == 'y') {
        answer = 'Y';   //answer was yes
    }else {
        answer = 'N';   //answer was no
    }
    return answer;
}

/*function to make a guess
 param: current node
 pre: node is not null
 post: returns 'Y' if answer was yes, 'N' if answer was no
 */
char guessAnimal(struct Node *current) {
    char buffer[100];   //store user response
    char answer;
    char question[] = "Is it a ";
    printf("%s%s?: ", question, current->value);
    fgets(buffer, 100, stdin);
    if (buffer[0] == 'Y' || buffer[0] == 'y') {
        answer = 'Y';   //answer was yes
    }else {
        answer = 'N';   //answer was no
    }
    return answer;
    
}

//response for when the answer is correct
void guessedRight() {
    printf("I guessed your answer correctly! \n");
    printf("Thanks for playing! \n");
}

/*function to get new question and animal if program couldn't guess correctly.
 Gets the animal the user was thinking of, and a question to distinguish that 
 animal from the last incorrect guess, and adds it to the tree.
 param: current node
 pre: node is not null
 post: new question and answers are added to tree
 */
void guessedWrong(struct Node *current) {
    char buffer[100];
    printf("You win, I couldn't guess your animal. \n");
    printf("\n");
    printf("What was your animal? ");
    fgets(buffer, 100, stdin);
    char *animal = newStr(buffer);
    char *oldAnimal = current->value;
    
    printf("\n");
    printf("What is a yes/no question that is true of %s but not of %s? ", animal, oldAnimal);
    fgets(buffer, 100, stdin);
    char *newQ = newStr(buffer);    //get new question from user
    addQuestion(current, oldAnimal, newQ, animal);      //add new question where the animal leaf node was
    printf("Okay, got it! \n");
    printf("\n");
    
}

/*function to set up game
 param: binary tree with questions and guesses
 pre: tree is not null
 post: none
 */
void playGame(struct ATree *tree) {
    assert(tree != 0);
    struct Node *current = tree->root;  //start game at root
    char answer;
    while (current != 0) {
        if (current->type == 'Q') { //if current node is a question
            answer = askQuestion(current);
            if (answer == 'Y') {    //move according to response
                current = current->yes;
            }else {
                current = current->no;
            }
        }else {
            answer = guessAnimal(current);  //if current node is an animal
            if (answer == 'Y') {    //guessed correct animal
                guessedRight();
                return;
            }else {
                guessedWrong(current);
                return;
            }
        }
    }
}



//print values in tree
void printNode(struct Node *current) {
    if (current == 0) {
        return;     //nothing to print
    }
    printf("(");
    printNode(current->no);
    printf("%s", current->value);
    printNode(current->yes);
    printf(")");
}

void printTree(struct ATree *tree) {
    if (tree == 0) {
        return;
    }
    printNode(tree->root);
}


//function introduces the program to the user
void introduction() {
    printf("Welcome to the Animal Guessing Game! \n");
    printf("I will ask you a series of questions to which you will answer yes or no. \n");
    printf("I am counting on you to help me expand my series of questions if I get it wrong.\n");
    printf("Let's begin!\n");
    printf("\n");
}


/*function to write tree to file after game ends
 param: current node, file to write to
 pre: file is successfully open, node is not null
 post: writes each node's value to file in preorder
 */
void writeToFile(struct Node *current, FILE *f) {
    if (current == 0) {
        return;
    }
    
    if (current->type == 'Q') { //mark as question
        putc('Q', f);
        fputs(current->value, f);
    }
    else if (current->type == 'A') {    //mark as animal guess
        putc('A', f);
        fputs(current->value, f);
    }
  
    //printf("%s copied to file \n", current->value);
    writeToFile(current->no, f);
    writeToFile(current->yes, f);

}


/*function to create animalGame.txt file
 param: binary tree with animal questions and guesses
 pre: tree is not null
 post: writes tree to file
 */
void createFile(struct ATree *tree) {
    struct Node *cur = tree->root;
    FILE *f;
    f = fopen("animalGame.txt", "w");
    assert(f != 0); //assert that file was accessed successfully
    writeToFile(cur, f);
    fclose(f);  //close file after finishing
}





int main(int argc, const char * argv[]) {
    char again = 'Y';
    struct ATree *myTree = createTree();
    char buffer[30];    //store user response to play again
    while (again == 'Y') {
        introduction();
        playGame(myTree);
        printf("\n");
        printf("Do you want to play again? \n");
        fgets(buffer, 30, stdin);
        again = toupper(buffer[0]);
    }

    
    createFile(myTree); //write game tree to file

    printf("tree written to file animalGame.txt \n");
    
    deleteTree(myTree); //free memory used for tree storage

    return 0;
}
