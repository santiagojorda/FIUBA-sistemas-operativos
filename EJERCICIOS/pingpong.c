

int main(int argc, char const *argv[])
{

    int pelota = 0;
    pid_t pid = fork();
    int fd_padre[2], fd_hijo[2];

    if (pid < 0){
        exit("error");
    } else if (pid == 0){ 


    } else {
        pelota++;
        write(pelota)

    }

    return 0;
}
