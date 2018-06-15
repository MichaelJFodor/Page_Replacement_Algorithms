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

// This struct uses a bit member variable to be able to track which page has reached the front
struct Page
{
    int bit;
    int value;
    struct Page *next;
};

struct Page *head = NULL;
struct Page *tail = NULL;
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
struct Page* create_list(int value, int bit)
{
    struct Page *ptr = (struct Page*) malloc(sizeof(struct Page));
    if (ptr == NULL)
    {
        fprintf(stderr, "Page creation failed!");
        return NULL;
    }
    ptr->value = value;
    ptr->bit = bit;
    tail = ptr;
    tail->next = tail;
    return ptr;
}



// insert page to tail
struct Page* insert_tail(int value, int bit)
{
    if(head == NULL)
    {
        return (create_list(value, 0));
    }

    struct Page *ptr = (struct Page*)malloc(sizeof(struct Page));
    if (ptr == NULL)
    {
        fprintf(stderr, "Page creation failed!");
        return NULL;
    }
    ptr->bit = bit;
    ptr->value = value;
    ptr->next =  tail->next;
    tail->next = ptr;
    tail = ptr;
    return ptr;
}

// insert page to head
struct Page* insert_front(int value, int bit)
{
    if(head == NULL)
    {
        return (create_list(value, 0));
    }

    struct Page *ptr = (struct Page*)malloc(sizeof(struct Page));
    if (ptr == NULL)
    {
        fprintf(stderr, "Page creation failed!");
        return NULL;
    }
    ptr->bit = bit;
    ptr->value = value;
    ptr->next = tail->next;
    
    tail->next = ptr;
    return tail;
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
        if(delet == tail)
            tail = previous;
        if (delet == head)
            head = delet->next;
    }

    free(delet);
    delet = NULL;
    return 0;
}


// pre: a table size and a page request
// post: TRUE or FALSE -> TRUE if page fault, FALSE if no page fault
int clock(int table_size, int page_request)
{
    struct Page *ptr = NULL;
    struct Page *previous = NULL;

    ptr = search_list(page_request, &previous);
    int count = 0;
    // page fault
    if (ptr == NULL)
    {
        ptr = head;
        while (ptr != NULL)
        {
            ptr = ptr->next;
            count++;
        }
	ptr = head;

        // if bit is 1, change it to 0 and move to end of cache
        // if bit is 0, swap page
        if (count >= table_size)
        {
            ptr = head;
            while(count >= table_size)
	    {
		// repeat until bit is 0
                while (ptr->bit == 1)
                {
                    // iterate and set flag bit to 0
		    ptr->bit = 0;
		    ptr = ptr->next;
                }
                // flag bit is now 0 so we swap
		int val = ptr->value;
            	ptr = head;
                remove_list(val);
                // insert new page at tail with bit 0
                insert_front(page_request, 1);
		--count;
	    }
            
        }
        else
        {
	    ptr = head;
            insert_front(page_request, 1);
        }

	return 1;
    }
    // page is in cache so we keep the bit a 1
    else
    {
        ptr->bit = 1;
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
        if(clock(table_size,page_request)) 
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
