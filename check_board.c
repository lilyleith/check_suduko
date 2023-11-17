///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2021 Deb Deppeler
// Posting or sharing this file is prohibited, including any changes/additions.
//
// We have provided comments and structure for this program to help you get 
// started.  Later programs will not provide the same level of commenting,
// rather you will be expected to add same level of comments to your work.
// 09/20/2021 Revised to free memory allocated in get_board_size function.
// 01/24/2022 Revised to use pointers for CLAs
//
////////////////////////////////////////////////////////////////////////////////
// Main File:        check_board.c
// This File:        check_board.c
// Other Files:      None
// Semester:         CS 354 Fall 2022
// Instructor:       deppeler
//
// Author:           Lily Leith
// Email:            lmleith@wisc.edu
// CS login: 	     lleith
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   Fully acknowledge and credit all sources of help,
//                   including Peer Mentors, Instructors, and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   Avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of
//                   of any information you find.
////////////////////////////////////////////////////////////////////////////////
   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
     
char *DELIM = ",";  // commas ',' are a common delimiter character for data strings
     
/* COMPLETED (DO NOT EDIT):       
 * Read the first line of input file to get the size of that board.
 * 
 * PRE-CONDITION #1: file exists
 * PRE-CONDITION #2: first line of file contains valid non-zero integer value
 *
 * fptr: file pointer for the board's input file
 * size: a pointer to an int to store the size
 */
void get_board_size(FILE *fptr, int *size) {      
	char *line1 = NULL;
    	size_t len = 0;

    	if ( getline(&line1, &len, fptr) == -1 ) {
        	printf("Error reading the input file.\n");
		free(line1);
        	exit(1);
    	}

    	char *size_chars = NULL;
    	size_chars = strtok(line1, DELIM);
    	*size = atoi(size_chars);

	// free memory allocated for reading first link of file
	free(line1);
	line1 = NULL;
}

/* free_board function frees the memory allocated to board 
 * Pre-conditions: memory has been allocated to the int** passed using malloc()
 *
 * board: type int**, stores the values read in from the file argument
 * size: type int, the dimensions of the sudoku board
 *
 */
void free_board(int **board, int size) {
	// iterate through the array of pointers and free the subarrays
	// and set the pointers to NULL 
	for (int i = 0; i < size; i++) { 
		free(*(board + i));
		*(board + i) = NULL;
	}
	// free the board variable and set it to NULL
	free(board);
	board = NULL;
}

/* free_counts function frees the memory allocated to two different heap allocated arrays, 
 * both of the same size
 *
 * Pre-conditions: memory has been allocated to row_counts and col_counts using malloc()
 *
 * row_counts: type int**, used to store the counts of values in the rows of the board
 * col_counts: type int**, used to store the counts of values in the columns of the board
 * size: type int, specifies how many rows there are in the arrays
 */
void free_counts(int **row_counts, int **col_counts, int size) {
	// iterates through the arrays of pointers and frees the memory and sets the pointers to NULL
	for (int i = 0; i < size; i++) {
		free(*(row_counts + i));
		free(*(col_counts + i));
		*(row_counts + i) = NULL;
		*(col_counts + i) = NULL;
	}
	// frees the row_counts and col_counts and sets them to NULL
	free(row_counts);
	free(col_counts);
	row_counts = NULL;
	col_counts = NULL;
}


/* valid_board function is used to determine if the board is valid
 *
 * Preconditions: the values in the board were successfully saved into board variable 
 * Returns 1 if and only if the board is in a valid Sudoku board state.
 * Otherwise returns 0.
 * 
 * A valid row or column contains only blanks or the digits 1-size, 
 * with no duplicate digits, where size is the value 1 to 9.
 * 
 * board: heap allocated 2D array of integers 
 * size:  number of rows and columns in the board
 */
int valid_board(int **board, int size) {
    	// check if size given is in [1,9], if it's not, then return 0
    	if (size > 9 || size < 1) {
	    	return 0;
    	}
    	// allocate memory for row_counts and col_counts
    	// row_counts: stores how many times a value appears in each row
    	// col_counts: stores how many times a value appears in each column
	// both row_counts and col_counts are of size n x (n+1) to allow tallying of integers 
	// with values [0,n]
    	int **row_counts = malloc(sizeof(int*) * size);
    	int **col_counts = malloc(sizeof(int*) * size);
    	// free memory allocated and return 0 if malloc fails
    	if (row_counts == NULL || col_counts == NULL) {
	     	free(row_counts);
	    	free(col_counts);
	    	row_counts = NULL;
	    	col_counts = NULL;
	    	return 0;
    	}	
        // allocate memory for each pointer of size (n+1)
    	for (int i = 0; i < size; i++) {
	    	*(row_counts + i) = malloc(sizeof(int) * (size + 1));
	    	*(col_counts + i) = malloc(sizeof(int) * (size + 1));
		// free memory and return 0 if malloc fails
	    	if (*(row_counts + i) == NULL || *(col_counts + i) == NULL) {
			free_counts(row_counts,col_counts,size);
		    	return 0;
	    	}
		// set the initial values in the array as 0 
	    	for (int j = 0; j <= size; j++) {
		    	*(*(row_counts + i) + j) = 0;
		    	*(*(col_counts + i) + j) = 0;
	    	}
    	}
	
	// iterate through the values in board and set the corresponding value in row_counts and 
	// col_counts to 1
	// formula for row_count placement: upon reading an integer board[i][j] = k, we set 
	// row_counts[i][k] = 1
	// formula for col_count placement: upon reading an integer board[j][i] = k, we set
	// col_counts[i][k] = 1
    	for (int i = 0; i < size; i++) {   
    	    	for (int j = 0; j < size; j++) {
			int row_val = *(*(board + i) + j);
			int col_val = *(*(board + j) + i);
			// before setting the values in row_count and col_count, we check what the 
			// preexisting value is in the two arrays for this number
			int curr_row_count = *(*(row_counts + i) + row_val);
			int curr_col_count = *(*(col_counts + i) + col_val);
			// if either of curr_row_count or curr_col_count are 1, then either row_val
			// or col_val has already appeared in this row or column, so free the memory
			// in row_counts and col_counts and then return 0
			// do the same if the row_val or col_val is not within the bounds [0,size]
			if (curr_row_count == 1 || curr_col_count == 1 || row_val > size || col_val > size || row_val < 0 || col_val < 0) {
				free_counts(row_counts,col_counts,size);
				return 0;
			} else {
			// if these numbers have not appeared yet, then:
			// if the value is 0: no change, since duplicated 0s are valid
			// if the value is not 0: set the corresponding entry in row_val or col_val to 1
				if (row_val != 0) {
					*(*(row_counts + i) + row_val) = 1;
				} 	
				if (col_val != 0) {
					*(*(col_counts + i) + col_val) = 1;
				} 
			}
		 
    		}
    	}
    	// free allocated memory using free_counts() function
    	free_counts(row_counts, col_counts, size);
	// board is valid, return 1
    	return 1;   
}    

 
   
/* 
 * This program prints "valid" (without quotes) if the input file contains
 * a valid state of a Sudoku puzzle board wrt to rows and columns only.
 *
 * A single CLA which is the name of the file that contains board data 
 * is required.
 *
 * argc: the number of command line args (CLAs)
 * argv: the CLA strings, includes the program name
 */
int main( int argc, char **argv ) {              
     
    	// Check if number of command-line arguments (2) is correct and print
	// and error message and exit(0) if not
    	if (argc != 2) {
		printf("Usage: ./check_board <input_filename>\n");
		exit(1);
    	}

    	// Open the file and check if it opened successfully.
    	FILE *fp = fopen(*(argv + 1), "r");
    	if (fp == NULL) {
        	printf("Can't open file for reading.\n");
        	exit(1);
    	}

    	// Declare local variables.
    	int size;
	// call get_board_size to set size
    	get_board_size(fp, &size);
	// allocate memory to board using malloc
	// if malloc fails, then print "invalid" and exit(0)
    	int **board = malloc(sizeof(int *) * size);
    	if (board == NULL) {
	   	printf("invalid\n");
	   	free(board);
	    	board = NULL;
	    	exit(0);
    	}
	// iterate through the int* array and allocate memory of size n for each
	// if malloc fails, print "invalid", free the memory allocated using free_board()
	// and exit(0)
    	for (int i = 0; i < size; i++) {
		*(board + i) = malloc(sizeof(int) * size);
		if (*(board + i) == NULL) {
			printf("invalid\n");
			free_board(board,size);
			exit(0);
		}
    	}


    	// Read the rest of the file line by line.
    	// Tokenize each line wrt the delimiter character 
    	// and store the values in your 2D array.
    	char *line = NULL;
    	size_t len = 0;
    	char *token = NULL;
    	for (int i = 0; i < size; i++) {
		// if getline fails, print error message and exit(1)
        	if (getline(&line, &len, fp) == -1) {
            		printf("Error while reading line %i of the file.\n", i+2);
            		exit(1);
        	}
		// get the value
        	token = strtok(line, DELIM);
       		for (int j = 0; j < size; j++) {
			// store the value in appropriate entry of board
	    		*(*(board + i) + j) = atoi(token);
            		token = strtok(NULL, DELIM);
        	}
    	}

	// call valid_board function, print "valid" if return value is 1, and "invalid" if return
	// value is not 1
    	if (valid_board(board,size) == 1) {
	    	printf("valid\n");
    	} else {
	    	printf("invalid\n");
    	}
    	// free all heap allocated memory using free_board() for the board array and free() for the 
	// line array
    	free_board(board,size);
    	free(line);
    	line = NULL;
    	//Close the file.
    	if (fclose(fp) != 0) {
        	printf("Error while closing the file.\n");
        	exit(1);
    	} 
     
    	return 0;       
}       






// s22

