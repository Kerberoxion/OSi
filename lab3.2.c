#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

void print_dyn(char* name, char* filename){
    void (*func)(char*);
    void* h =  dlopen("./libfunc.so",RTLD_LAZY);
    if(!h){
        printf("dlopen failed: %s\n", dlerror());
        return;
    }

    func = dlsym(h,name);
    char* error = dlerror();
    if(error != NULL){
        printf("dlsym failed: %s", error);
        return;
    }

    func(filename);
    dlclose(h);
}


int main(int argc, char* argv[]) {
    int len = strlen(argv[0]);
    char func[100] = "";
    for(int i = 2;i < len; i++ ){
        func[i-2] = argv[0][i];
    }
    
    print_dyn(func, argv[1]);   
    
    return 0;
}

