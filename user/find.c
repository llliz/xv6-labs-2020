#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *filename) {
    int fd;
    char buf[512], *p;
    struct stat st;
    struct dirent de;

    if ((fd = open(path, 0))< 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    //参数检查，必须为目录参数
    if (st.type != T_DIR) {
        fprintf(2, "usage: find <directory> <filename>\n");
        return;
    }

    //路径长度检查
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
        fprintf(2, "find: path too long\n");
        return;
    }

    //将路径字符串复制到buf，p指针指向最后一个'/'之后
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    //循环读取目录项
    while (read(fd, &de, sizeof de) == sizeof de) {
        //若当前目录项不包含信息，跳过
        if (de.inum == 0) {
            continue;
        }
        //把目录项的名称添加到path后面
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        //读取目录项的文件结构信息，失败则跳过
        if(stat(buf, &st) < 0) {
            printf("cannot stat %s\n", buf);
            continue;
        }

        /*若目录项为目录文件且不是“.”或“..”，递归查询filename；
          若目录项不是目录文件且名称与filename相同，打印出来*/
        if (st.type == T_DIR && strcmp(".", p) != 0 && strcmp("..", p) != 0) {
            find(buf, filename);
        }else if (strcmp(p, filename) == 0) {
            printf("%s\n", buf);
        }
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2,"usage: find <directory> <filename>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}