#include <dlfcn.h>
#include <stdio.h>

void print_dyn(void){
	void (*func)(void);
	void* h =  dlopen("./libdynHello.so",RTLD_LAZY);
	if(!h){
		printf("dlopen failed: %s\n", dlerror());
		return;
	}

	func = dlsym(h,"hello");
	char* error = dlerror();
	if(error != NULL){
		printf("dlsym failed: %s", error);
		return;
	}

	func();
	dlclose(h);
}

int main (void)
{
	print_dyn();
  	return 0;
}
