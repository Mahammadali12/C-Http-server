#include <malloc.h>

struct node
{
	struct node* next;
	int value;
};

void free_list(struct node* head)
{
	if(head == NULL)
		return ;
	free_list(head->next);
	free(head);
}

void add_to_end(int v,struct node* head)
{
	struct node* temp=head;
	while(temp->next != NULL )
	{
		temp=temp->next;
	}
	temp->next = (struct node*) malloc(sizeof(struct node)); 
	temp = temp->next;
	temp->value = v;
	temp->next = NULL;
}

int listLength(struct node* head)
{
    int i=0;
    while (head != NULL)
    {
        i++;
        head = head->next;
    }
    return i;
}

void printList(struct node* head)
{
	while (head != NULL)
	{
		printf("%i ",head->value);
		head = head->next;
	}
	printf("\n");
	
}
