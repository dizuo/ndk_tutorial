
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hash-app.h"

void hash_test(int num);

int main(int argn, char* argv[])
{
	hash_test(20);

    printf("\n any key pressed to exit...\n");
    getchar();

    return 0;
}


// =========================================================

typedef struct item
{
	char buffer[100];
	int no;
} item_t;

void dump_item(void* ptr)
{
	item_t* pitem = (item_t*)ptr;
	printf("{%s,%d}\t", pitem->buffer, pitem->no);
}

int make_key(void* ptr)
{
	item_t* pitem = (item_t*)ptr;
	
	return pitem->no;
}

int item_cmp(void* p1, void* p2)
{
	item_t* pleft = (item_t*)p1;
	item_t* pright = (item_t*)p2;

	return strcmp(pleft->buffer, pright->buffer);
}

void hash_test(int num)
{
	srand( time(NULL) );

	hs_tb_t* table = hash_create(make_key, item_cmp);

	hash_reserve(table, 136);

	item_t* parr = (item_t*)malloc(sizeof(item_t) * num);

	for (int i = 0; i < num; i++)
	{
		int val = rand() % 1379;
		sprintf(parr[i].buffer, "hash_test-%d", val);
		parr[i].no = val;

		hash_insert(table, parr + i, parr + i);
	}

	hash_print(table, dump_item);

	// query test case
	for (int try = 0; try < 20; try++)
	{
		item_t key;
		key.no = rand() % 1379;
		sprintf(key.buffer, "hash_test-%d", key.no);
		item_t* pres = (item_t*)hash_query(table, &key);
		if (pres)
		{
			printf("query res = ");
			dump_item(pres);
		}
		else
		{
			printf("%s not found ", key.buffer);
		}
		printf("\n");
	}

	// erase node.
	srand( time(NULL) );
	for (int i = 0; i < num; i++)
	{
		item_t key;
		key.no = rand() % 1379;
		sprintf(key.buffer, "hash_test-%d", key.no);

		item_t* pres = hash_erase(table, &key);
		if (pres)
		{
			printf("erase node = ");
			dump_item(pres);
		}
		else
		{
			printf("%s not found", key.buffer);
		}
		printf("\n");
	}

	hash_destroy(table);
	table = 0;

	free(parr);

}