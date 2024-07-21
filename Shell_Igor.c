#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>


typedef struct jobs{
  int id;
  char *name;
  int flag; // 0 - finished, 1 - working in bg, 2 - stopped 
} jobs;


typedef struct node{
    char **str;
    char ch;
    struct node *next;
}list;

char *input(){
  printf("My Shell %s> ", getcwd(NULL, 0));
  int bufsize = 20, i = 0;
  char* buffer = (char*) calloc(bufsize, sizeof(char));
  char ch = getchar();
  int flag = 0, flag1 = 0;
  while(1) {
    if (i >= bufsize) {
      bufsize *= 2;
      buffer = realloc(buffer, bufsize);
    }
    if (ch == '\n'){
      if ((flag % 2) == 0 && (flag1 % 2) == 0) {
        break;
      } else{
        printf("> ");
      }
    }
    if (ch == EOF){
      if (i == 0){
        break;
      }
      clearerr(stdin);
      continue;
    }
    if (ch == '"'){
      flag++;
    }
    if(ch == '\'') {
      flag1++;
    }
    buffer[i] = ch;
    i++;
    ch = getchar();
  }
  if (i == 0){
    return NULL;
  }
  buffer[i] = '\0';
  //printf("str = %s, %ld\n", buffer, strlen(buffer));
  return buffer;
}
 
list* create_node(char** x, char c){
	//printf("First time\n");
	//list *p = NULL;
	//p = (list*) realloc(p, sizeof(list));
    list *p = (list*) calloc(1, sizeof(list));
    //printf("Go\n");
    if (p == NULL){
  		perror("calloc");
	   	_exit(EXIT_FAILURE);
	  }
    p->str = x;
    p->ch = c;
    p->next=NULL;
    return p;
}
void push_back(list **l, char** x, char c){
    
    if (*l==NULL){
	    //printf("Normal Here\n");
	    (*l)=create_node(x, c);
        //printf("Normal part\n");
        return;
    }
    list *p=*l;
    while(p->next!=NULL){
        p=p->next;
    }
    p->next=create_node(x, c);
}

void print_list(list *l){
    if(l==NULL){
        return;
    }
    else{
		for (int i =0;; i++){
			if (l->str[i] == NULL)
				break;
			else
				puts(l->str[i]);
		}
        print_list(l->next);
    }
}


void del_list(list **l){
    if((*l) == NULL){
		free(*l);
		return;
    }
    else{
		for (int i =0;; i++){
			if ((*l)->str[i] == NULL){
				free((*l)->str[i]);
				break;
			}
			free((*l)->str[i]);
		}
		free((*l)->str);
        del_list(&(*l)->next);
        free(*l);
    }
}
list *list_parse (char **str1, int len2){
	list *l = NULL;
	int i = 0, j = 0; 
	int flag = 0; // флаг для проверки стоят ли 2 команды рядом
	char c; 
	while(j < len2){
		flag = 0;
		if (strcmp(str1[j],"||") == 0){
			c = '1';
		} else if (strcmp(str1[j],"&&") == 0){
			c = '2';
		} else if (strcmp(str1[j],"|") == 0){
			c = '|';
		} else if (strcmp(str1[j],"&") == 0){
			printf("c = &\n");
			c = '&';
		} else if (strcmp(str1[j],";") == 0){
			c = ';';
		} else if (strcmp(str1[j], ">") == 0){
			c = '>';
		} else if (strcmp(str1[j], "<") == 0){
			c = '<';
		} else{
			flag = 1; // если сейчас команда ( не разделитель)
			c = '0';
		}
		if ((!flag && j < len2-1) || j == len2-1) { 
			int flag1 = 0; // нужно ли учитывать последнюю строку ( если разделитель, то не надо, если команда - надо)
			if (flag == 0){
				flag1 = 1;
			}
			if (i == j && j != len2 - 1){ // случай двух подряд идущих разделителей
				_exit(0);
			}
			char **mas = (char**) calloc( (j-i) + 2, sizeof(char*)); // +1 за счёт первой команды, +1 за счёт NULL
			if (mas == NULL){
				perror("calloc");
				exit(EXIT_FAILURE); 
			}
			int range = j-i - flag1;
			for (int u = 0; u <= range; u++){
				int len_new = strlen(str1[u+i]);
				mas[u] = (char*) calloc(len_new+1, sizeof(char));
				strcpy(mas[u], str1[u+i]);
			}
			mas[range+1] = (char*)NULL;
			push_back(&l, mas, c);
			//
			i = j + 1;
		} 
		j++;
	}
	return l;
}

char** parsing(char *buffer, int *n) {
  int wordcount = 2, wordsize = 10, i = 0, j = 0, k = 0;
  
  char** new_buf = (char**) calloc(wordcount,  sizeof(char*));
  for (int m = 0; m < wordcount ; m++){
    new_buf[m] = (char*)calloc(wordsize, sizeof(char));
  }
  int f1 = 0;//f1 = 0(вне кавычек), f1 = 1(в кавычках)
  int f2 = 1;//f2 = 0(вне слова), f2 = 1(в слове)
  while(buffer[i] != '\0') {
    if(j >= wordcount - 2) {
      wordcount++;
      new_buf = realloc(new_buf, wordcount*sizeof(char*));
      new_buf[wordcount-1] = calloc(wordsize, sizeof(char));
    }
    if (k >= wordsize){
      wordsize *= 2;
      new_buf[j] = realloc(new_buf[j], wordsize*sizeof(char*));
    }
    switch(buffer[i]) {
      case '"':
        if (f1 == 0) {
          f1 = 1;
          f2 = 1;
        }
        else {
          f1 = 0;
        }
        
        break;
      case ' ':
        if(f1 == 1) {
          break;
        }
        else{
          if (f2 == 0){
            i++;
            break;
          }
          new_buf[j][k] = '\0';
          j++, k=0;
          i++;
          f2 = 0;
        }
        break;
      case '|':
        if (buffer[i+1] == '|'){
			if (f2 == 1){
				new_buf[j][k] = '\0';
				j++, k=0;
				f2 = 0;
			}
			new_buf[j][k++] = buffer[i++];
			new_buf[j][k] = buffer[i++];
			j++, k = 0;
		}
		else{
			new_buf[j][k] = buffer[i++];
			j++, k = 0;		
		}
		break;
      case '&':
		if (buffer[i+1] == '&'){
			if (f2 == 1){
				new_buf[j][k] = '\0';
				j++, k=0;
				f2 = 0;
			}
			new_buf[j][k++] = buffer[i++];
			new_buf[j][k] = buffer[i++];
			j++, k = 0;	
		} else{
			j++, k = 0;	
			new_buf[j][k] = buffer[i++];
			j++, k = 0;	
		}
			
		break;	
      case '>':
      case '<':
        if(f1 == 1) {
          f2 = 1;
          break;
        }
        else {
          if(f2 == 1) {
            new_buf[j][k] = '\0';
            j++;
            k = 0;
          }
          new_buf[j][k++] = buffer[i++];
          new_buf[j][k] = '\0';
          j++;
          k = 0;
        }
        break;
      default:
        f2 = 1;
        break;
    }
    if (f2 != 0){
      new_buf[j][k++] = buffer[i++];
    }
  }
  j++;
  (*n) = j;
  new_buf[j] = NULL;
  return new_buf;
}

list* listlen( list *l, int *num){ // ls -l | wc 
  list *p = l;
  int len = 1;
  while( p != NULL){
    if (p -> str == NULL){
      (*num) = len;
      return NULL;
    }
    if (p->ch != '|'){
      break;
    }
    len++;
    p = p->next;
  }
  (*num) = len;
  return p;
}

list* do_conveyer(list **lst, jobs *shell_jobs, int *count_jobs) {
    int cmd_num;
    list *p = listlen(*lst, &cmd_num);
    //printf("cmd_num = %d\n", cmd_num);
	int fd[cmd_num-1][2];
	int pid;
	list *ls = *lst;
	for(int i = 0; i < cmd_num; i++, ls = ls->next) {
	  if (i != cmd_num - 1) {
		if (pipe(fd[i]) == -1){
		  perror("pipe");
		  _exit(EXIT_FAILURE);
		}
	  }
	  pid = fork();
	  if(pid == -1) {
		perror("fork");
		_exit(EXIT_FAILURE); 
	  }
	  if(pid == 0) {
		if(i != 0) {
		  if (dup2(fd[i - 1][0], STDIN_FILENO) == -1) {
			perror("dup2");
			exit(EXIT_FAILURE);
		  }
		  close(fd[i - 1][0]);
		}
		if (i != cmd_num - 1) {
			if (dup2(fd[i][1], STDOUT_FILENO) == -1) {
			    perror("dup2");
			    exit(EXIT_FAILURE);
		    }
		    close(fd[i][0]);
		    close(fd[i][1]);
		}
		execvp(ls -> str[0], ls -> str);
		perror("execvp");
		exit(EXIT_FAILURE);
	  }
	  if(i != 0) {
		close(fd[i - 1][0]);
	  }
	  if(i !=  cmd_num) {
		close(fd[i][1]);
	  }
	}
	for(int i = 0; i < cmd_num; i++) {
	  wait(NULL);
	}

  	return p;    
}

void func_jobs(jobs *shell_jobs, int count_jobs) {
   for(int i = 0; i < count_jobs; i++) {
     printf("[%d] %s %d ", i, shell_jobs[i].name, shell_jobs[i].id);
     switch(shell_jobs[i].flag) {
      case 0: printf("finished\n"); break;
      case 1: printf("working in bg\n"); break;
      case 2: printf("stopped\n"); break;
      default: printf("something else\n");
     }
   }
}


void func_kill(list *lst,jobs* shell_jobs,int count_jobs){
  if (lst -> str[1] == NULL){
    printf("Error kill\n");
    return;
  } else if (strcmp(lst -> str[1], "-l") == 0){
    printf(" 1) SIGHUP   2) SIGINT   3) SIGQUIT   4) SIGILL   5) SIGTRAP\n");
    printf("6) SIGABRT   7) SIGBUS   8) SIGFPE   9) SIGKILL  10) SIGUSR1\n");
    printf("11) SIGSEGV  12) SIGUSR2  13) SIGPIPE  14) SIGALRM  15) SIGTERM\n");
    printf("16) SIGSTKFLT  17) SIGCHLD  18) SIGCONT  19) SIGSTOP  20) SIGTSTP\n");
  } else{
    int num = atoi(lst -> str[1]);
    if (num >= count_jobs){
      printf("No such process\n");
    } else{
      if (lst -> str[2] == NULL){
        kill(shell_jobs[num].id, SIGTERM); // сигнал завершения процесса(только просьба)
      } else{
        int sig = atoi(lst -> str[2]);
        if (1 <= sig && sig <= 64){
          printf("pid = %d, sig = %d\n", shell_jobs[num].id, sig);
          kill(shell_jobs[num].id, sig);
        }
      }
    }
  }
}



void update(jobs* shell_jobs, int count_jobs){
	int status;
	for(int i = 0; i < count_jobs; i++) {
		if(shell_jobs[i].flag == 1) {
			pid_t pid = waitpid(shell_jobs[i].id, &status, WNOHANG | WUNTRACED); // ждет остановлленный
			if (pid == 0)
				continue;
			if (WIFEXITED(status) == 1  || WIFSIGNALED(status) == 1){
				shell_jobs[i].flag = 0;
			} else if(WIFSTOPPED(status) == 1){
				shell_jobs[i].flag = 2;
			}
		} else if (shell_jobs[i].flag == 2){
			pid_t pid = waitpid(shell_jobs[i].id, &status, WNOHANG | WCONTINUED);
			if (pid == 0)
				continue;
			if (WIFEXITED(status) == 1  || WIFSIGNALED(status) == 1){
				shell_jobs[i].flag = 0;  
			} else if (WIFCONTINUED(status) == 1){
				shell_jobs[i].flag = 1;
			}
		}
	}
}

int comands (list *lst, jobs *shell_jobs, int count_jobs){
  //exit
  if(strcmp(lst -> str[0], "exit") == 0) {
      exit(0);
  }
  // cd
  else if(strcmp(lst -> str[0],"cd") == 0) {
    char *home = getenv("HOME");
    if(lst -> str[1] == NULL) {
      if(chdir(home) == -1) {
        perror("cd");
      }
    }
    else {
      if(chdir(lst -> str[1]) == -1) {
        perror("cd");
      }
    }
  }
  //pwd
  else if(strcmp(lst -> str[0],"pwd") == 0) {
    int len_path = 50;
    char *buf = (char*) calloc(len_path, sizeof(char));
    buf = getcwd(buf, len_path);
    printf("%s\n", buf);
  }
  // jobs
  else if (strcmp(lst -> str[0], "jobs") == 0){
    func_jobs(shell_jobs, count_jobs);
  }
  //help
  else if(strcmp(lst -> str[0],"help") == 0) {
    printf("shells' helper\n");
  }
  //kill
  else if (strcmp(lst -> str[0], "kill") == 0){
    func_kill(lst, shell_jobs, count_jobs);
  } 
  // Ничего не выполнилось
  else{
    return 0;
  }
  return 1;
}

int execution_simple(list **lst,jobs *shell_jobs, int *count_jobs) {
  if (comands(*lst, shell_jobs, *count_jobs) == 0) { 
    
    pid_t pid;
    pid = fork();
    if (pid == 0){
      signal(SIGTSTP, SIG_DFL);
      signal(SIGINT, SIG_DFL);
      signal(SIGQUIT, SIG_DFL);
      execvp((*lst) -> str[0], (*lst)->str);
      exit(1);
    } else if (pid == -1){
		perror("fork");
    }  else{
		shell_jobs[(*count_jobs)].id = pid;
		shell_jobs[(*count_jobs)].name = (*lst)->str[0];
	}
      
    if ((*lst) -> ch != '&') {    
		int status;
		waitpid(pid, &status, WUNTRACED);
		if (WIFEXITED(status) == 1)
			shell_jobs[(*count_jobs)].flag = 0;
		else {
			if(WIFSTOPPED(status) == 1) {
				shell_jobs[(*count_jobs)].flag = 2;       
				(*count_jobs)++;
			}
		}
    }  else{
		shell_jobs[(*count_jobs)].flag = 1;
	}
    (*count_jobs)++;
    return 0;
  }
  return 1;
}

void func2(list *lst, jobs *shell_jobs, int *count_jobs) {    // wc < file.c
	char* str1 = lst->next->str[0];
	if (str1 != NULL){
		pid_t pid;
		pid = fork();
		if (pid == 0){
			signal(SIGTSTP, SIG_DFL);
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
			int fd = open(str1, O_RDONLY ); // O_WRONLY, O_CREAT | 0666
			if (fd == -1){
				char *str = str1+1;
				str[strlen(str)-1] = '\0';
				fd = open(str, O_RDONLY);
				if (fd == -1){
					perror("open");
					exit(1);
				}
			}
			
			if (dup2(fd, STDIN_FILENO) == -1){
				perror("dup2q"); // STDOUT_FILENO
				exit(1);
			}
			execvp(lst -> str[0], lst->str); // ls -l
			exit(1);
		}
		int status;
		waitpid(pid, &status, WUNTRACED);
		if (WIFEXITED(status) == 1)
			shell_jobs[*count_jobs].flag = 0;
		else {
			if(WIFSTOPPED(status) == 1) {
				shell_jobs[*count_jobs].flag = 2;       
				count_jobs++;
			} else{
			shell_jobs[*count_jobs].flag = 1;
			}
		}
	}
}


void func3(list *lst, jobs *shell_jobs, int *count_jobs) {    // ls >  file.c
	char* str1 = lst->next->str[0];
	if (str1 != NULL){
		pid_t pid;
		pid = fork();
		if (pid == 0){
			signal(SIGTSTP, SIG_DFL);
			signal(SIGINT, SIG_DFL);
			signal(SIGQUIT, SIG_DFL);
			int fd = open(str1, O_CREAT | O_TRUNC | O_WRONLY, 0666); 
			if (fd == -1){
				char *str = str1+1;
				str[strlen(str)-1] = '\0';
				fd = open(str, O_CREAT | O_TRUNC | O_WRONLY, 0666);
				if (fd == -1){
					perror("open");
					exit(1);
				}
			}
			printf("Ok\n");
			if (dup2(fd, STDOUT_FILENO) == -1){
				perror("dup2q");
				exit(1);
			}
			execvp(lst -> str[0], lst->str); // ls -l
			exit(1);
		}
		int status;
		waitpid(pid, &status, WUNTRACED);
		if (WIFEXITED(status) == 1)
			shell_jobs[*count_jobs].flag = 0;
		else {
			if(WIFSTOPPED(status) == 1) {
				shell_jobs[*count_jobs].flag = 2;       
				count_jobs++;
			} else{
			shell_jobs[*count_jobs].flag = 1;
			}
		}
	}
}



 void comands2(list **lst, jobs *shell_jobs, int *count_jobs){
	if (*lst == NULL){
		return;
	}

	if ((*lst)->ch == ';'){
		execution_simple(lst, shell_jobs, count_jobs);
		comands2( &(*lst)->next, shell_jobs, count_jobs);
	} if ((*lst)->ch == '&'){ 
		execution_simple(lst, shell_jobs, count_jobs);
		comands2(&((*lst)->next), shell_jobs, count_jobs);
	} else if ((*lst)->ch == '|'){// |
		list *p = do_conveyer(lst, shell_jobs, count_jobs);
		//printf("cmd_name, %s, %c\n", p->str[0], p->ch);
		comands2(&p->next, shell_jobs, count_jobs);	
	} 
   else if ((*lst)->ch == '<'){
		func2(*lst, shell_jobs, count_jobs);
		
	} else if ((*lst)->ch == '>'){
		func3(*lst, shell_jobs, count_jobs);
	
     // дописать перенаправление в другую сторону
    }else if ((*lst)->ch == '1'){ // ||
		int must_do = execution_simple(lst, shell_jobs, count_jobs);
		if (!must_do){
			comands2(&(*lst)->next, shell_jobs, count_jobs);
		}
	} else if ((*lst)->ch == '2'){ // &&
		int must_do = execution_simple(lst, shell_jobs, count_jobs);
		if (must_do){
			comands2(&(*lst)->next, shell_jobs, count_jobs);
		}
	}else{
		execution_simple(lst, shell_jobs, count_jobs);
	}
}


int main() {
	jobs shell_jobs[256] = {0};
	int count_jobs = 0;
	signal(SIGTSTP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	while(1) {
		int n = 0;
		char *buffer = input();
		if (buffer == NULL){
			continue;
		}
		char **new_buf = parsing(buffer, &n); //парсинг строки
		/*    for (int i = 0 ; new_buf[i] != NULL; i++){
		printf("s[%d] = %s\n", i, new_buf[i]);
		}*/
		//printf("count = %d\n", n);
		/*for (int i = 0; new_buf[i] != NULL; i++){
			puts(new_buf[i]);
		}*/
		list *lst = list_parse(new_buf, n);  
		update(shell_jobs, count_jobs);
		comands2(&lst, shell_jobs, &count_jobs);
 
	}

  return 0;
}
