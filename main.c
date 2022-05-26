#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>

#if defined(_WIN32)
    #define CLEAR_COMMAND "cls"
#else
    #define CLEAR_COMMAND "clear"
#endif

int *initialize_game(); //allocates and fills game board, returns a pointer to the board
void teardown(int *board); //free(board)

void print_values_debug(int *board, char cards[30][4]); //debugging function to print all cards face up (as strings)
void print_raw_values_debug(int *board); //debugging function to print all cards face up (as ints)

int accept_input(int* board, int user1, int user2); //validates inputs, returns 0 for invalid and 1 for valid, calls clear_buffer when an invalid input is detected
void clear_buffer(); //clears garbage strings from stdin

void print_board(int *board); //print the board face down, replaces empty spaces with "-"
int update_board(int *board, int user1, int user2); //checks for match and empties spaces when matched, returns 1 for match, 0 for miss
void print_values(char cards[30][4], int* board, int user1, int user2); //prints the user's selected spaces face up, prints "-" on empty spaces

int main(void) {

    //string literals, correlated to the cards raw value
    //theme: US cities
    char card_arr[30][4] = {
        "NYC",
        "ATL",
        "CHI",
        "SD",
        "LA",
        "DC",
        "MIA",
        "JAX",
        "PHI",
        "DAL",
        "HOU",
        "MIL",
        "OKC",
        "PHX",
        "SEA"
    };

    char play_again = 'y';
    while(play_again == 'y') { //if user says no when prompted to play again, the game will exit
        int* board = initialize_game();
        int user1, user2;
        int turn_number = 0, matches = 0;

        /*print_raw_values_debug(board);
        printf("\n");
        print_values_debug(board, card_arr);*/

        while(1) { //loops until user quits or wins
            printf("\n");
            print_board(board); //face down
            printf("\n");
            printf("turn: %d\nmatches: %d\n", turn_number, matches);
            printf("enter two spaces to match (0 to exit): ");
            unsigned short flag = 0; //this flag validates input
            while(flag == 0) {
                scanf("%d %d", &user1, &user2);
                flag = accept_input(board, user1, user2); //will return 1 for valid input or 0 for invalid input
            }

            //decrement users inputs to start at 0
            --user1;
            --user2;

            system(CLEAR_COMMAND); //buggy, but prevents some cheating

            printf("\n");
            matches += update_board(board, user1, user2); //successful match will return 1, miss will return 0
            print_values(card_arr, board, user1, user2); //show the user's selected cards face up
            printf("\n");
            if(matches >= 15) { //when all cards are matched
                printf("game over. you won in %d turns\n", turn_number);
                teardown(board);
                break;
            } else {
                ++turn_number;
            }
        }
        while(1) { //simple input validation on play_again
            printf("play again?(y/n): ");
            scanf(" %c", &play_again);
            if((play_again == 'y')||(play_again == 'n')) {
                clear_buffer();
                break;
            } else {
                printf("\ninvalid. enter 'y' or 'n'.\n");
                clear_buffer();
            }
        }
    }
    return 0;
}

int *initialize_game() {

    int* board = (int*)malloc(30 * sizeof(int)); //address/array to return
    if(board == NULL) { //should never happen, but just in case
        printf("there was a problem initializing the game...");
        exit(0);
    }
    srand(time(NULL));
    for(int i = 0; i < 30 ; board[i++] = -1);  //-1 implies a space that has no card, this fills board with "empty spaces"

    int cardnum = 1; //cardnum corresponds to card string array in main, this is the "raw value" of the card
    while(cardnum < 16) { //breaks after all cards are on the board, there are 30 spaces so there should be 15 matching cards
        for(size_t i = 0; i < 2; ++i) { //loop runs 2x to place matching cards
            size_t index = rand() % 30; //placing card at random index 0-29
            int flag_is_collision = 0;
            while(flag_is_collision == 0) { //indicates when our card placement was successful
                if(board[index] == -1) { //if this space is empty (-1 indicates a free space throughout the program)
                    board[index] = cardnum;
                    flag_is_collision = 1; //successful card placement :)
                } else { //this space contains a card, try the next one over
                    if(index == 29) { //don't go out of bounds!!
                        index = 0;
                    } else {
                        ++index;
                    }
                }
            }
        }
        ++cardnum;//at this point, two matching cards are at random indices, on to the next
    }
    return board; //returns pointer to game board
}

//free board from memory
void teardown(int *board) {
    free(board);
}

//prints all cards face up
void print_values_debug(int *board, char cards[30][4]) {
    for(size_t i = 0; i < 30; ++i) {
        printf("%4s   ", cards[board[i]-1]);
        if(i%5 == 4)
            printf("\n");
    }
}

//test function for printing all the raw values for the cards
void print_raw_values_debug(int *board) {
    for(size_t i = 0; i < 30; ++i) {
        printf("%4d   ", board[i]-1);
        if(i%5 == 4)
            printf("\n");
    }
}

int accept_input(int* board, int user1, int user2) {
    if((user1 == 0)||(user2 == 0)) { //user quit, exit game
        exit(0);
    } else if(user1 == user2) { //matching indexes
        printf("choose 2 different spaces.\n");
        printf("enter two spaces to match(0 to exit): ");
        clear_buffer();
        return 0;
    } else if(((user1 > 30)||(user2 > 30))||((user1 < 0)||(user2 < 0))) { //out of bounds
        printf("input(s) out of bounds!\n");
        printf("enter two spaces to match (0 to exit): ");
        clear_buffer();
        return 0;
    } else if((board[user1-1] == -1)||(board[user2-1] == -1)) { //empty space
        printf("choose a space that still contains a card.\n");
        printf("enter two spaces to match (0 to exit): ");
        clear_buffer();
        return 0;
    } else { //no problems, check the flag
        return 1;
    }
}

//removes extra nonsense from stdin if these was any
void clear_buffer(){
    int c;
    while ((c = getchar()) != EOF && c != '\n'); //eats away leftover garbage from stdin
}

//function to print entire board face down and replaces empty spaces with '-'
void print_board(int *board) {
    for(size_t i = 0; i < 30; ++i) { //for all board spaces
        if(board[i] != -1) { //if this space is NOT empty
            printf("%4d   ", i+1); //print the index + 1
        } else { //this space is empty, has been matched
            printf("%4c   ", '-');
        }
        if(i%5 == 4)//every 5 lines create new line
            printf("\n");
    }
}

//checks for match and empties spaces when matched, returns 1 for match, 0 for miss
int update_board(int* board, int user1, int user2) {
    if(board[user1] == board[user2]) { //if the spaces on the board match, make them empty
        board[user1] = -1;
        board[user2] = -1;
        return 1;
    } else { //no match
        return 0;
    }
}

//prints the user's selected spaces face up, prints "-" on empty spaces
void print_values(char cards[30][4], int* board, int user1, int user2) {
    //for all elements on board
    for(size_t i = 0; i < 30; ++i) {
        if(board[i] == -1) { //if this space has been matched, -1 indicates an empty space
            printf("%4c   ", '-');
        } else if(i == user1) {  //user selected space
            printf("%4s   ", cards[board[user1]-1]);
        } else if(i == user2) { //user selected space
            printf("%4s   ", cards[board[user2]-1]);
        } else {
            printf("%4d   ", i+1);  //not user chosen, not matched. default case
        }
        //every 5 spaces, new line
        if(i%5 == 4)
            printf("\n");
    }
}
