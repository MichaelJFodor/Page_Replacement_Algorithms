#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

int parse_arguments(int argc, char *argv[])
{
    int table_size;
    if (argc == 2 && (table_size = atoi(argv[1])))
    {
          return table_size;
    }

    fprintf(stderr, "Wrong arguments, Pass table_size as an argument\n");
    exit(-1);
}

struct Page
{
    int value;
    struct Page *next;
};

struct Page *head = NULL;
struct Page *current = NULL;

// search list for a specific page
struct Page* search_list(int value, struct Page **previous)
{
    struct Page *ptr = head;
    struct Page *temp = NULL;
    while (ptr != NULL)
    {
        if (ptr->value == value)
        {
	     if(previous)
            	*previous = temp;

	     return ptr;
        }
        else
        {
            temp = ptr;
            ptr = ptr->next;
        }
    }

    return NULL;
}

// create list a list
struct Page* create_list(int value)
{
    struct Page *ptr = (struct Page*) malloc(sizeof(struct Page));
    if (ptr == NULL)
    {
        fprintf(stderr, "Page creation failed!");
        return NULL;
    }
    ptr->value = value;
    ptr->next = NULL;
    head = current = ptr;
    return ptr;
}

// insert page to front
struct Page* insert_front(int value)
{
    if(head == NULL)
    {
        return (create_list(value));
    }

    struct Page *ptr = (struct Page*)malloc(sizeof(struct Page));
    if (ptr == NULL)
    {
        fprintf(stderr, "Page creation failed!");
        return NULL;
    }
    ptr->value = value;
    ptr->next = head;
    head = ptr;
    return ptr;
}


// insert page to tail
struct Page* insert_tail(int value)
{
    if(head == NULL)
    {
        return (create_list(value));
    }

    struct Page *ptr = (struct Page*)malloc(sizeof(struct Page));
    if (ptr == NULL)
    {
        fprintf(stderr, "Page creation failed!");
        return NULL;
    }
    ptr->value = value;
    ptr->next = NULL;
    current->next = ptr;
    current = ptr;
    return ptr;
}

// search list for a specific page and delete it
int remove_list(int value)
{
    struct Page *delet = NULL;
    struct Page *previous = NULL;

    delet = search_list(value, &previous);

    if(delet == NULL)
        return -1;

    else
    {
        if(previous != NULL)
            previous->next = delet->next;
        if(delet == current)
            current = previous;
        if (delet == head)
            head = delet->next;
    }

    free(delet);
    delet = NULL;
    return 0;
}




// pre: a table size and a page request
// post: TRUE or FALSE -> TRUE if page fault, FALSE if no page fault
int fifo(int table_size, int page_request)
{
    struct Page *ptr = NULL;
    struct Page *previous = NULL; 
    int count = 0;
    ptr = search_list(page_request, &previous);
   
    // page fault
    if (ptr == NULL)
    {
        ptr = head;
	// Track count of size
        while (ptr != NULL)
        {
            ptr = ptr->next;
            count++;
        }

        if (count == table_size)
        {
            ptr = head;
	    insert_tail(page_request);
            remove_list(ptr->value);
            
        }
        // Insert at back
        else
        {
            insert_tail(page_request);
        }
	
	return 1;
    }

    return 0;
}



int main(int argc, char *argv[])
{
    int table_size = parse_arguments(argc, argv);
    int page_request;
    int num_requests = 0;
    int num_misses = 0;

    int *page_table = (int *) malloc(sizeof(int)*table_size);
    char *input = NULL;
    size_t input_allocated = 0;
    ssize_t bytes_read;



    while ((bytes_read = getline(&input, &input_allocated, stdin)) != -1)
    {
        page_request = atoi(input);
        num_requests++;
        if (page_request == 0)
              continue;

	// Here is where we make use of the P.R.A.
	// The P.R.A. will return 1 if there is a page fault, helping us count misses
        if(fifo(table_size,page_request)) 
        {
              printf("Page %d caused a page fault\n", page_request);
              num_misses++;
        }
    }

    // number of hits = num_requests-num_misses
    // hit rate = hits/requests
    printf("Hit rate: %f\n", (num_requests-num_misses)/(double)num_requests);


    free(input);
    free(page_table);
    return 0;
}
