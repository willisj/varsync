#pragma once 
#include <stdint.h>

#define VSROLE_SERVER 	1,1
#define VSROLE_CLIENT 	1,0
#define VSROLE_SENDER 	2,1
#define VSROLE_RECEIVER	2,0
#define MAX_PACKETSIZE 1024

struct __attribute__((__packed__)) variable_handler{
	uint32_t id; 
	uint32_t var_size;
	void * var_ptr;
	struct variable_handler * next;
};

struct sync_list{
	struct variable_handler * head;
	uint8_t role;
};

int  getbit( int input, int bit, int value);
int  setbit( int input, int bit, int value);
struct sync_list* init_list(uint8_t role, const char * ip, unsigned int port);
int add_var_handler(  struct sync_list * list, struct variable_handler* handle);
void * update_thread(void* list);
void deinit_list(struct sync_list*);
