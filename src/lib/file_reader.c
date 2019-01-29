#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#include ""
// #include "lib/counter.h"
// #include "lib/counter_container.h"

void Init_start(int ranks,int my_rank, double split_size, FILE ** file_to_read, int* file_partition, int * offset) {

	//ZH Notes: size_t can be unsigned int, long, or long long
	size_t starting_byte = 0;
	// Calculate the part that will be read by processes
	size_t partition[ranks];

	for(int i = 0; i < starting_byte; i++){
		starting_byte = my_rank * split_size;
		partition[i] = starting_byte;
	}
	// test
	// some mpi shit
	// if(my_rank > 1) {
	// 	starting_byte; // starting_byte init
	// }

    // Calculate the file to read and the remaining part of size already read by previous processes
    *file_partition = 0;

	// MPI_PrintIndented(my_rank, "file_to_read_index %d", *file_to_read_index);

	// Open the first file that my process must read
    //*file_to_read = openFile(filesContainer->files[*file_to_read_index]->path, "r");

		// ZH Notes: SEEK_SET moves file pointer position to the beginning of the file.
    // Begin reading from the the starting byte.
    fseek(*file_to_read, starting_byte, SEEK_SET);


		// ZH Query: what does offset do? why is it a pointer?
		// where is it initialised?
    *offset = 0;
	char current_char;
	if(starting_byte > 0) {

		current_char = fgetc(*file_to_read);

		// MPI_Print(my_rank, "file %d", file_to_read_index);
		// MPI_Print(my_rank, "starting_byte %f", starting_byte);
		// MPI_PrintIndented(my_rank, "start %c", current_char);

		// ZH Notes: islanum checks whether current_char is either a
		// decimal digit or an uppercase or lowercase letter.
		if(isalnum(current_char)) {

			// if it's a char it goes back up to space

			while(isalnum(current_char)) {

				(*offset)++;
				starting_byte--;

				if(starting_byte >= 0) {
					fseek(*file_to_read, starting_byte, SEEK_SET);
					current_char = fgetc(*file_to_read);
				}
				else {
					// back to the beginning of the file
					rewind(*file_to_read);
					break;
				}

			}

		}

	}
	// ZH Query: what does offset accomplish? I don't see it used anywhere?
}

void resetWordBuffer(char * word, int * word_index) {

	// ZH Query: why is (sizeof word[0]) multiplied by 100?

	// Clear current word and reset current word index
	memset(word, 0, 100 * (sizeof word[0]) );
	*word_index = 0;
}

void addWordToContainer(CounterContainer * entriesContainer, int * words_found, char * word, int * word_index) {

	CounterContainer_findAndAddWord(entriesContainer, word, 1);
	(*words_found)++;

	resetWordBuffer(word, word_index);

}

void startReader(int my_rank, double split_size, CounterContainer * entriesContainer) {

	FILE * 	file_to_read;
	int 	file_to_read_index;

	int     offset;
	double  already_read_by_me;
	int 	words_found;

	char 	current_char;

	char 	current_word[40];
	int 	current_word_index = 0;

	// Check Starting Point
	file_to_read = NULL;
	checkStartingPoint(my_rank, split_size, filesContainer, &file_to_read, &file_to_read_index, &offset);

    // Start reading
    already_read_by_me = 0;
    words_found        = 0;

	resetWordBuffer(current_word, &current_word_index);

    // Read my split size
    while(already_read_by_me < (split_size+offset) ) {

		current_char = fgetc(file_to_read);

		if(current_char == EOF) {

			if(current_word_index > 0) {
				// In this case i read a space
				// So i read a whole word

				// MPI_Print_To_File(log_file, my_rank, "Word readed: %s", current_word);
				addWordToContainer(entriesContainer, &words_found, current_word, &current_word_index);

			}

			// In this case the file is finished
			// So i need to read the next available file

			fclose(file_to_read);
			file_to_read_index++;

			// Check if all files was readed
			if(file_to_read_index >= filesContainer->num_files) {
				file_to_read = NULL;
				break;
			}

			// MPI_Print_To_File(log_file, my_rank, "Skip to file: %d", file_to_read_index);

			file_to_read = openFile(filesContainer->files[file_to_read_index]->path, "r");

			continue;
		}

		if( isalnum(current_char) ) {

			// In this case the current char is an alphanumeric character
			// So i need to add this to current word

			current_word[current_word_index] = current_char;
			current_word_index++;

		}
		else if( isspace(current_char) || iscntrl(current_char) ) {

			if(current_word_index > 0) {
				// In this case i read a space
				// So i read a whole word

				// MPI_Print_To_File(log_file, my_rank, "Word readed: %s", current_word);
				addWordToContainer(entriesContainer, &words_found, current_word, &current_word_index);

			}

		}

		// Anyway i must increase the part readed by me
		already_read_by_me++;

    }

    // MPI_PrintIndented(my_rank, "end %c", current_char);

	if(file_to_read != NULL) {

		current_char = fgetc(file_to_read);

		// MPI_PrintIndented(my_rank, "next end %c", current_char);

		if( (!isalnum(current_char)) ) {

			if(current_word_index > 0) {

				// In this case i read a space
				// So i read a whole word

				// MPI_Print_To_File(log_file, my_rank, "Final Word readed: %s", current_word);
				addWordToContainer(entriesContainer, &words_found, current_word, &current_word_index);

			}

		}

	}

	fprintf(log_file, "Size Readed: %f bytes\n", already_read_by_me);
    // MPI_Print(my_rank, "I read this size: %f", already_read_by_me);

    fprintf(log_file, "Words Found: %d\n\n", words_found);
    // MPI_Print(my_rank, "Words Found: %d", words_found);

    // CounterContainer_print(&entriesContainer);
    fprintf(log_file, "Local Histogram\n\n");
    CounterContainer_printToFile(entriesContainer, log_file);

}
