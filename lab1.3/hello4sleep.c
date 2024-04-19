#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>

void print_dyn(void){
        void (*func)(void);
        void* h =  dlopen("./libdynHello.so",RTLD_LAZY);
        if(!h){
                printf("dlopen failed: %s\n", dlerror());
                return;
        }

    	char c;
   		scanf("%c",&c);
   		while(c != 'd'){
        	sleep(1);
    	    scanf("%c",&c);
	    }

        func = dlsym(h,"hello");
        char* error = dlerror();
        if(error != NULL){
                printf("dlsym failed: %s", error);
                return;
        }

        scanf("%c",&c);
        while(c != 'd'){
            sleep(1);
            scanf("%c",&c);
        }


        func();
        dlclose(h);
}

int main (int argc, char* argv[])
{
	char c;
	scanf("%c",&c);
	while(c != 'd'){
		sleep(1);
		scanf("%c",&c);
	}
    print_dyn();
    return 0;
}
     
