#include "history.h"
#include <string.h>

struct cmd_tagged_union _hist[HIST_SIZE];
int _cmd_counter = 0;


int _arr_index(int cmd_num) {
	return cmd_num % HIST_SIZE;
} 

int _cmd_in_memory(int cmd_num) {

	if(cmd_num > _cmd_counter) 
		return 0;

	if(cmd_num < 1)
		return 0;

	if(_cmd_counter - cmd_num >= 10)
		return 0;

	return 1;	
}
 

void hist_add(struct cmd_tagged_union cmd) {
	_cmd_counter++;	

	int cmd_ind = _arr_index(_cmd_counter); 

	_hist[cmd_ind] = cmd;
}


struct cmd_tagged_union *hist_fetch(int cmd_num) {
	
	if(_cmd_in_memory(cmd_num) == 0) 
		return 0; 

	return &_hist[_arr_index(cmd_num)];
}

