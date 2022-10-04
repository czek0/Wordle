/* Wordle
 * A one file Terminal Game inspired by NyTime's Wordle
 * Author: Francesca / Czek0.git
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>


/* Default Game Values */
#define LENGTH 5
#define MAX 6
#define DICT "/usr/share/dict/words"


typedef enum ErrorMsg { 
    SYMBOL_ER,
    DICT_ER, 
    CMDLINE_ER, 
} ErrorMsg;

/* print_error()
 * -------------
 * Print errors in game.
 *
 * message: an errror enum msg code.
 */
void print_error(ErrorMsg message) {
    switch (message) {
	case SYMBOL_ER:
	    fprintf(stdout, "Words must contain only letters - try again.\n");
	    break;
	case DICT_ER:
	    fprintf(stdout, "Word not found in the dictionary - try again.\n");
	    break;
	case CMDLINE_ER:
	    fprintf(stderr, "Usage: wordle [-len word-length]"
	    " [-max max-guesses] [dictionary]\n");
	    break;				
    }
    fflush(stdout);
    fflush(stderr);
}

/* read_file()
 * -----------
 * Reads a dictionary and scans the file for words of game word length.
 * Puts valid dictionary words into a 2D array.
 *
 * dict: is the dictionary location or dictionary file
 * len: is the length of the only words allowed in the array
 *
 * Returns: an array of words to be used in the game where the size of
 * the dictionary is stored in the 0 position
 */
char** read_file(char* dict, int n) {
    FILE* fp = fopen(dict, "r");
    int numElements = 1; // will be used to organise array, leave room for var
    char** gameWords = malloc(sizeof(char*) * 1);

    // iterate through each line of dictionary using fscan
    char arr[100][100]; // temperary storage
    int j; // the return value of scanf to check EOF
    while (j = fscanf(fp, "%s", arr[1]), j > 0 || j != EOF) {
	int errorPresent = 0; // no errors in this word
	// check each char (i) if in alphabet
	for (int i = 0; i < strlen(arr[1]); i++) {
	    if (!isalpha(arr[1][i])) {
		errorPresent = 1; // there is an error in this word
		break;	
	    }
	}
	// if word has no errors and is of correct length, store
	if (!errorPresent && ((int)strlen(arr[1]) == n)) {
	    gameWords = realloc(gameWords, sizeof(char*) * (numElements + 1)); 
	    gameWords[numElements] = malloc(sizeof(char) * n + 1);
	    strcpy(gameWords[numElements], arr[1]); //store the numElements
	    numElements++; 	//increase the num of elements in the gameWords
	}
    }
    // lastly, store the numElements in the first position
    gameWords[0] = malloc(sizeof(char) * 6);
    sprintf(gameWords[0], "%d",numElements);
    fclose(fp);

    return gameWords;
}

/* check_guess()
 * -------------
 * checks if guess is the following :
 * 1. right length
 * 2. contains letters A to Z (upper of lower case)
 * 	
 * Returns: 1 if guess is a valid attempt, 0 if not
 * Errors: will print error if any of the checks are violated.
 */
int check_guess(char* guess, int n) {
    // check length
    int length = (int)strlen(guess);
    if (length != n) {
	fprintf(stdout, "Words must be %d letters long - try again.\n", n);
	fflush(stdout);
	return 0;
    }
 
    // check if in alphabet	
    for (int i = 0; i < n; i++) {
	char c = (guess[i]);
	if (!isalpha(c)) {
	    print_error(SYMBOL_ER);
	    return 0;
	}
    }
    return 1;
}

/* word_in_dict()
 * --------------
 * Checks if a word is in the dictionary. 
 * 
 * word: is the word to be checked
 * dict: is the dictionary to be compared
 *
 * Returns: 1 if in dictionary, 0 if not.
 * Errors: if the word is not found, an error will be printed
 */
int word_in_dict(char* word, char** dictionary) {
    int inDict = 0; 
    int dictLen = atoi(dictionary[0]);

    // iterate through dictionary
    for (int i = 1; i < dictLen; i++) {
	if (strcasecmp(word, dictionary[i]) == 0) {
	    inDict = 1; // yes it is in dictionary
	    break;
	} 
    }
    // throw error if not  in dictionary
    if (!inDict) {
	print_error(DICT_ER);
    }	
    return inDict;
}
	
/* get_len()
 * ---------
 * Gets game word length (n).
 * Assumes that check_args passed.
 *
 * argc: argc from main 
 * argv: argv from main
 *
 * Returns: the length of word (n)
 */
int get_len(int argc, char*  argv[]) {
    int n = LENGTH; // default set

    for (int i = 0; i < argc; i++) {
	if (!strcmp(argv[i], "-len") && i != (argc - 1)) {
	    n = atoi(argv[i + 1]); 
	}
    }

    return n;
}

/* get_max()
 * ---------
 * Gets game maximum guess (m).
 *
 * argc: argc from main 
 * argv: argv from main
 *
 * Returns: the max of guesses (m)
 */
int get_max(int argc, char* argv[]) {
    int m = MAX; // default set
	
    for (int i = 0; i < argc; i++) {
	if (!strcmp(argv[i], "-max") && (i != argc - 1)) { 
	    m = atoi(argv[i + 1]); 
	}
    }

    return m;
}

/* get_dict()
 * ----------
 * checks the validity of last argument and assigns dictionary location 
 *
 * argc: argc from main 
 * argv: argv from main
 *
 * Returns: a string where the dictionary is located
 * Errors: if the file name does not exist or cannot be opened
 */
char* get_dict(int argc, char* argv[]) {
    char* dict = DICT; // default set
    FILE* testFile; 

    // check that dictionary is not -len or -max value
    if (argc > 1 && strcmp(argv[argc - 2], "-len") 
	    && strcmp(argv[argc - 2], "-max")) { 
	dict = strdup(argv[argc - 1]); // copy last arg to dict
	testFile = fopen(dict, "r"); // check that dict can be opened
	if (!testFile) {
	    fprintf(stderr, "wordle: dictionary file"
	    	    " \"%s\" cannot be opened\n", dict);
	    exit(2);
	}
	fclose(testFile);
    }

    return dict;
}

/*
 * get_random_word()
 * -----------------
 * returns a random word from a char* pointer
 *
 * dict: char** dictionary to get random word from
 * n: size of the word
 * Returns: a random word of the length of the game
 * Errors: if no word of that length exists
 */
char* get_random_word(char** dict, int n) {
    int size = atoi(dict[0]);
    int randomPosition = 0;
    
    srand(time(0));
    for (int i = 0; i <= 1; i++) {
	randomPosition = rand()%(size-0) + 0;
    }
    return dict[randomPosition];
    
}

/* echo_hint ()
 * −−−−−−−−−−−−
 * Compares guess to answer and prints a string of hints
 *
 * guess: the user input guess
 * answerWord: the game answer
 * n: the size of the word
 */
void echo_hint(char* guess, char* answerWord, int n) {
    char* a = strdup(answerWord); // copy of answer for testing 
    char* g = strdup(guess); // copy of guess for testing
    char modifyWord[n + 1];
    
    // make every letter in both guess and answerWord lowercase
    for (int x = 0; x <= n; x++) {
	g[x] = tolower(g[x]);
	a[x] = tolower(a[x]);
    }
    // reset the modfiy word
    for (int p = 0; p <= n; p++) {
	modifyWord[p] = '-';
    }
    modifyWord[n] = '\0';

    // find pairs and remove from from a and g to ignore
    // then hint pairs in modifWord
    for (int j = 0; j <= n; j++) {
	if (a[j] == g[j]) {
	    modifyWord[j] = toupper(g[j]);
	    a[j] = '-';
	    g[j] = '-';
	}
    }
	
    // find chars from guess in answer
    // hint them in modifyWord
    // remove from from a to ignore for double cases
    for (int i = 0; i <= n; i++) {
	for (int j = 0; j <= n; j++) {
	    if (g[i] == a[j] && g[i] != '-') {
		modifyWord[i] = tolower(g[i]);
		a[j] = '-';
		break;
	    }
	}
    }
    
    // print string of hints
    fprintf(stdout, "%s\n", modifyWord);		
    fflush(stdout);
}
	
/* check_args()
 * −−−−−−−−−−−−
 * Checks arguments passed to main for valid arguments.
 * The following are invalid args and will throw errors:
 * 1. argc is more than 6
 * 2. count of -len and -max arguments
 * 3. order of -len and -max arguments
 * 4. -len and -max are valid integers
 * 5. dictionary is valid
 * argc: argc from main
 * argv: argv from main
	 *
	 * Errors: will throw command line error
	 */
void check_args(int argc, char* argv[]) {
    int lenCount = 0;
    int maxCount = 0;	
    int errorPresent = 0;
    // more than 5 arguments passed
    errorPresent += argc > 6 ? 1 : 0; 
    // check count of -len and -max argument
    for (int i = 0; i < argc; i++) {
	lenCount += !strcmp(argv[i], "-len") ? 1 : 0;
	maxCount += !strcmp(argv[i], "-max") ? 1 : 0;
	errorPresent += !strcmp(argv[i], "") ? 1 : 0;
			
        if (argv[i][0] == '-' && strcmp(argv[i], "-len") 
		&& strcmp(argv[i], "-max")) {
	    errorPresent++;
    	}
    }
    errorPresent += (lenCount > 1 || maxCount > 1) ? 1 : 0;
    // if one check if dictionary and not -len -max
    if (argc == 2) {
	errorPresent += argv[1][0] == '-' ? 1 : 0;
    }
    // check that 1 is -len or -max and 2 is between 3 to 9 inclusive
    if (argc >= 3) {
        if (strcmp(argv[1], "-len") && strcmp(argv[1], "-max")) {
	    errorPresent++;
        } else if (atoi(argv[2]) < 3 || atoi(argv[2]) > 9 
		|| strlen(argv[2]) != 1) {
	    errorPresent++;
        }
    } 
    // if three, check arguments, and 3 is a dictionary
    if (argc == 4) {
        if (!strcmp(argv[3], "-len") || !strcmp(argv[3], "-max")) {
	    errorPresent++;
        }
    }
    // if four, check b twice for 1,2,3,4 and 5 is dictionary
    if (argc >= 5) {
        if (strcmp(argv[3], "-len") && strcmp(argv[3], "-max")) {
	    errorPresent++;
        } else if (atoi(argv[4]) < 3 || atoi(argv[4]) > 9 
		|| strlen(argv[4]) != 1) {
	    errorPresent++;
        }
    } 
    if (errorPresent) {
	print_error(CMDLINE_ER);
	exit(1);
    }
}

/* game_over()
 * −−−−−−−−−−−
 * Prints the answer to stderr and exits.
 *
 * answer: the get_random_word() game answer.
 */
void game_over(char* answer) {
    fprintf(stderr, "Bad luck - the word is \"%s\".\n", answer);
    fflush(stderr);
    exit(3);
}

/* prompt()
 * −−−−−−−−
 * Prints a prompt message for the user.
 * 
 * n: the size of the word
 * m: the number of guesses
 *
 */
void prompt(int n, int m) {
    if (m == 1) {
	fprintf(stdout, "Enter a %d letter word (last attempt):\n", n);
    } else {	
	fprintf(stdout, 
		"Enter a %d letter word (%d attempts remaining):\n", n, m);
    }
    fflush(stdout);
}
 
int main(int argc, char* argv[]) {
    check_args(argc, argv);
    int m = get_max(argc, argv);
    int n = get_len(argc, argv);
    char** dictionary = read_file(get_dict(argc, argv), n);
    char* answer = get_random_word(dictionary, n);
    char* guess;	

    fprintf(stdout, "Welcome to Wordle!\n");
    for (int i = m; i > 0; i--) { 	
    	prompt(n, i);
	char input[52] = "\0";
	char* line;
	line = fgets(input, 52, stdin);
	if ((input[strlen(input) - 1]) == '\n') {
       	    (input[strlen(input) - 1]) = '\0';
	}
	if (feof(stdin) && line == NULL)  {
	    game_over(answer);
	}
	guess = input;
	while (!check_guess(guess, n) || !word_in_dict(guess, dictionary)) {
	    prompt(n, i);
	    fflush(stdout);
	    fgets(input, 52, stdin);
	    if (feof(stdin)) {
		game_over(answer);
	    }
	    if ((input[strlen(input) - 1]) == '\n') {
       		input[strlen(input) - 1] = '\0';
	    }			
	    guess = input;	
	}	
	if (strcasecmp(guess, answer) == 0) {	
	    fprintf(stdout, "Correct!\n");
	    exit(0);
	}
	echo_hint(guess, answer, n);
    }
    game_over(answer);	
    for (int i = 0; i < atoi(dictionary[0]); i++) {
	free(dictionary[i]);
    }
    free(dictionary);	
    free(answer);
    return 0;
}
