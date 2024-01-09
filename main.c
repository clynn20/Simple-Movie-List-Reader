#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

struct movie{
	char* title;
	int year;
	char* lang[5];
	double rating_val;
	struct movie* next;
};

struct movie* create_movie(char*);
struct movie* process_file(char*);
void print_year(struct movie*);
void print_high_rate(struct movie*);
void print_language(struct movie*);
void print_list(struct movie*);
void print_menu();
void free_mem(struct movie*);


//parse the current line and create a movie struct with the data in this line 
struct movie* create_movie(char* curr_line){
	struct movie* curr_movie = malloc(sizeof(struct movie));
	char* save_ptr;
	char* save_ptr_lang;

	// the first token is title
	char* token = strtok_r(curr_line,",",&save_ptr);
	curr_movie->title = calloc(strlen(token)+1, sizeof(char));
	strcpy(curr_movie->title,token);

	//the next token is year
	token = strtok_r(NULL,",",&save_ptr);
	// change string into int 
	curr_movie->year = atoi(token);

	// the next token is language
	token = strtok_r(NULL,",",&save_ptr);
	
	// parse the language information 
	char* lang_token = strtok_r(token,"[]",&save_ptr_lang);
	// first token in language information
	lang_token = strtok_r(lang_token,";",&save_ptr_lang);

	for(int i=0;i<5;i++){
		curr_movie->lang[i] = NULL;
	}

	int i=0;
	while(lang_token){
		curr_movie->lang[i] = calloc(strlen(lang_token)+1,sizeof(char));
		strcpy(curr_movie->lang[i],lang_token);
		// next token in language information
		lang_token = strtok_r(NULL,";",&save_ptr_lang);
		i++;
	}

	// the last token is rating value
	token = strtok_r(NULL,"\n",&save_ptr);
	// change string into double 
	curr_movie->rating_val = strtod(token,NULL);

	curr_movie->next = NULL;
	return curr_movie;
}

// process the file and create movie link list 
struct movie* process_file(char* filename){
	FILE* readfile = fopen(filename,"r");
	if(readfile==NULL){
		printf("Could not open %s\n",filename);
		exit(1);
	} 
	char* curr_line = NULL;
	size_t len = 0;
	ssize_t nread;
	int num = 0;
	// create movie list link head and tail
	struct movie* head = NULL;
	struct movie* tail = NULL;

	// skip first line 
	getline(&curr_line,&len,readfile);
	// read the file line by line
	while((nread = getline(&curr_line,&len,readfile))!=-1){
		struct movie* new_movie = create_movie(curr_line);
		if(head==NULL){
			head = new_movie;
			tail = new_movie;
		}
		else{
			tail->next = new_movie;
			tail = new_movie;
		}
		//update the number of movie 
		num = num+1;

	}
	printf("Processed file %s and parse data for %d movies\n",filename,num);
	free(curr_line);
	fclose(readfile);
	return head;
}


// print the movie title from the given year
void print_year(struct movie* list){
	int year, exist=0;
	printf("Enter the year for which you want to see movies: ");
	scanf("%d",&year);
	while (list != NULL){
		if(list->year == year){
			exist = 1;
			printf("%s\n",list->title);
		}
		list = list->next; 
	}
	if(exist!=1){
		printf("No data about movies released in %d\n",year);
	}
}

// print the highest rating value movie in each year
void print_high_rate(struct movie* list){
	// array for check if the specific year has been chosen or not
	// index 0 for year 1900, index 121 stand for year 2021
	int mark[122] = {0};
	while(list!=NULL){
		// let current high rating movie be the first movie in the list
		struct movie* high = list;
		// let compare movie be the movie that next to the current high rating movie
		struct movie* compare = list->next;
		while(compare != NULL){
			if(high->year == compare->year){
				if(compare->rating_val > high->rating_val){
					high = compare;
				}
			}
			compare = compare->next;
		}
		// found the highest movies and mark the choosen times of that specific year
		mark[(high->year)-1900]++;
		// compares all the other movies till the end
		if(compare == NULL){
			// check if the specific year has been only choosen once, more than one means that year had been choosen already.
			if(mark[(high->year)-1900]==1){
				printf("%d	%.1f	%s\n",high->year, high->rating_val, high->title);
			}
		}
		// update the next start of comparison
		list = list->next;
	}
}

// print the movies which had the given language.
void print_language(struct movie* list){
	char language[21];
	printf("Enter the language for which you want to see movies: ");
	scanf("%s",language);
	int exist = 0;
	// compare the user input language with the movie list lang[]
	while(list!=NULL){
		for(int i=0;i<5;i++){
			if(list->lang[i]!=NULL){
				if(strcmp(list->lang[i],language)==0){
					printf("%d %s\n",list->year,list->title);
					exist = 1;
				}
			}
		}
		list = list->next;
	}
	if(exist!=1){
		printf("No data about movies released in %s\n",language);
	}

}

// [debug use only] print the movie list data
void print_list(struct movie* list){
	while(list!=NULL){
		printf("%s %d ", list->title, list->year);
		for(int i=0;i<5;i++){
			if(list->lang[i]!=NULL){
				printf("%s ",list->lang[i]);
			}
		}
		printf("%.1f\n",list->rating_val);
		list = list->next;
	}
}

// menu instruction for user
void print_menu(){
	printf("\n");
	printf("1. Show movies released in the specified year\n");
	printf("2. Show highest rated movie for each year\n");
	printf("3. Show the title and year of release of all movies in a specific language\n");
	printf("4. Exit from the program\n");
	printf("\n");
}

// free the allocate memory
void free_mem(struct movie* list){
	while(list!=NULL){
		struct movie* temp = list->next;
		free(list->title);
		for(int i=0;i<5;i++){
			free(list->lang[i]);
		}
		free(list);
		list = temp;
	}
}

int main(int argc, char* argv[]){
	int option=0;
	struct movie* list = process_file(argv[1]);
	//print_list(list); 
	do{
		do{
			print_menu();
			printf("Enter a choice from 1 to 4: ");
    		scanf("%d", &option);
			if(option!=1 && option!=2 && option!=3 && option!= 4){
				printf("You didn't enter the correct choice, please try again\n");
			}
		}while(option!=1 && option!=2 && option!=3 && option!=4);
		switch(option){
			case 1:
				 print_year(list);
				 break;
			case 2:
				 print_high_rate(list);
				 break;
			case 3:
				 print_language(list);
				 break;
			case 4:
				 free_mem(list);
				 exit(0);
		}
	}while(option!=4);
	return 0;
}

