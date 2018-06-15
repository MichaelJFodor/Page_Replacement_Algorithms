#include <stdio.h>
#include <stdlib.h>

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
    int dirty;
    int reference;
    struct Page *next;
};


struct Page *head = NULL;
struct Page *current = NULL; // current - last page in list

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


// create a linked list
struct Page* create_list(int value)
{
    struct Page *ptr = (struct Page*) malloc(sizeof(struct Page));
    if (ptr == NULL)
    {
        fprintf(stderr, "Could not create the page!");
        return NULL;
    }
    ptr->reference = 1;
    ptr->dirty = 0;
    ptr->value = value;
    ptr->next = NULL;
    head = current = ptr;
    return ptr;
}

// insert page to head
struct Page* insert_front(int value)
{
    if(head == NULL)
    {
        return (create_list(value));
    }

    struct Page *ptr = (struct Page*)malloc(sizeof(struct Page));
    if (ptr == NULL)
    {
        fprintf(stderr, "Could not create the Page!");
        return NULL;
    }

    ptr->reference = 1;
    ptr->dirty = 0;
    ptr->value = value;
    ptr->next = head;
    head = ptr;
    
    return ptr;
}


// search list for a specific page and delete it
int remove_list(int value)
{
    struct Page *delet = NULL;
    struct Page *previous = NULL;

    delet = search_list(value, &previous);

    delet->reference = 0;
    delet->dirty = 0;
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

void check_class(int page_request)
{
    struct Page *ptr = head;
    struct Page *previous = NULL;
    
    while(ptr != NULL)
    {
	if(ptr->reference == 0 && ptr->dirty == 0)
	{
		remove_list(ptr->value);
		return;	
	}	
	ptr = ptr->next;
    }
    ptr = head;
    while(ptr != NULL)
    {
	if(ptr->reference == 0 && ptr->dirty == 1)
	{
		remove_list(ptr->value);
		return;	
	}	
	
	ptr = ptr->next;
    }
    ptr = head;
    while(ptr != NULL)
    {
	if(ptr->reference == 1 && ptr->dirty == 0)
	{
		remove_list(ptr->value);
		return;	
	}	
	ptr = ptr->next;
    }
    ptr = head;
    while(ptr != NULL)
    {
	if(ptr->reference == 1 && ptr->dirty == 1)
	{
		remove_list(ptr->value);
		return;	
	}	
	ptr = ptr->next;
    }
}

void reset_refs()
{
    struct Page *ptr = head;
    while(ptr != NULL)
    {
	ptr-> reference = 0;
	ptr = ptr->next;
    }
}

// pre: a table size and a page request
// post: TRUE or FALSE -> TRUE if page fault, FALSE if no page fault
int nru(int table_size, int page_request, int timer)
{
    struct Page *ptr = NULL;
    struct Page *previous = NULL;
    ptr = search_list(page_request, &previous);
    int count = 0;

    if(timer == 0)
    {
	reset_refs();
    }

    // PAGE FAULT
    if(ptr == NULL)
    {
	ptr = head;
	while(ptr != NULL)
	{
		ptr = ptr->next;
		count++;
	}

	if(count >= table_size)
	{
		while(count >= table_size)
		{
			check_class(page_request);
			--count;
		}
	}
	else
	{
		insert_front(page_request);	// ref bit = 1
	}

	return 1;
    }
    // Page is in the list
    else
    {
	ptr = search_list(page_request, &previous);
	ptr->dirty = 1; // change to dirty bit = 1
    }

	return 0;
}

int main(int argc, char*argv[])
{
    int table_size = parse_arguments(argc, argv);
    int page_request;
    int num_requests = 0;
    int num_misses = 0;

    int *page_table = (int *) malloc(sizeof(int)*table_size);
    char *input = NULL;
    size_t input_allocated = 0;
    ssize_t bytes_read;

    // Every "reset_size" pages, we will reset the refrence bit to 0
    int reset_size = 12345;
    int reset_timer = reset_size;

    while ((bytes_read = getline(&input, &input_allocated, stdin)) != -1)
    {
        page_request = atoi(input);
        num_requests++;
        if (page_request == 0)
              continue;

	// Here is where we make use of the P.R.A.
	// The P.R.A. will return 1 if there is a page fault, helping us count misses
        if(nru(table_size,page_request,reset_timer--)) 
        {
              printf("Page %d caused a page fault\n", page_request);
              num_misses++;
        }
    	if(reset_timer <= 0)
		reset_timer = reset_size;
    }

    // number of hits = num_requests-num_misses
    // hit rate = hits/requests
    printf("Hit rate: %f\n", (num_requests-num_misses)/(double)num_requests);


    free(input);
    free(page_table);
    return 0;	





	return 0;
}










