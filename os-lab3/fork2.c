#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

// 计算 f(x) (阶乘)
long long compute_fx(int x) {
    long long result = 1;
    for (int i = 2; i <= x; i++) {
        result *= i;
    }
    return result;
}

// 计算 f(y) (类斐波那契)
long long compute_fy(int y) {
    if (y == 1 || y == 2) return 1;
    long long a = 1, b = 1, c;
    for (int i = 3; i <= y; i++) {
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}

int main() {
    int x, y;
    printf("Enter x and y: ");
    scanf("%d %d", &x, &y);

    int pipe_fx[2], pipe_fy[2];
    if (pipe(pipe_fx) == -1 || pipe(pipe_fy) == -1) {
        perror("pipe error");
        exit(EXIT_FAILURE);
    }

    pid_t pid_fx = fork();
    if (pid_fx < 0) {
        perror("fork error");
        exit(EXIT_FAILURE);
    } else if (pid_fx == 0) {

        close(pipe_fx[0]);  
        long long result = compute_fx(x);
        write(pipe_fx[1], &result, sizeof(result));
        close(pipe_fx[1]);
        exit(EXIT_SUCCESS);
    }

    pid_t pid_fy = fork();
    if (pid_fy < 0) {
        perror("fork error");
        exit(EXIT_FAILURE);
    } else if (pid_fy == 0) {
        close(pipe_fy[0]); 
        long long result = compute_fy(y);
        write(pipe_fy[1], &result, sizeof(result));
        close(pipe_fy[1]);
        exit(EXIT_SUCCESS);
    }

    close(pipe_fx[1]); 
    close(pipe_fy[1]);

    long long fx, fy;
    read(pipe_fx[0], &fx, sizeof(fx));
    read(pipe_fy[0], &fy, sizeof(fy));

    printf("f(x,y) = %lld + %lld = %lld\n", fx, fy, fx + fy);

    waitpid(pid_fx, NULL, 0);
    waitpid(pid_fy, NULL, 0);

    return 0;
}