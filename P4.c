#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <grp.h>

int main(int argc, char *argv[]){
	FILE *fp;			//file
	char *buffer;		//used for string
	struct stat st;
	struct passwd *pw=getpwuid(getuid());	//passwd struct
	struct group *grp;	//group struct
	int uid;			//will hold user id
	int gid;			//will hold group id
	int i;				//loop variable
	char *path;	
	char *home;

	char *homedir = pw->pw_dir;	//gets home directory
	for(i=1; i<argc; i++){
		int length1= (int)strlen(homedir);	//get length of homedir
		int length2= (int)strlen(argv[i]);	//get length of argv[i]
		int maxLength = length1+length2+1; 	//add the two together

		if(buffer[0]=='/'){
			path = (char*)malloc(maxLength*sizeof(char));
			strcat(path, homedir);
			strcat(path,"/");
			strcat(path, argv[i]);
			stat(path, &st);		
		}else{
			stat(argv[i], &st);
		}
		
		buffer = argv[i];
		uid = getuid();
		gid = getgid();
		grp = getgrgid(st.st_gid);



		if(buffer[0]=='/'){ //directory passed.

			//user D-R-W-X
            printf((S_ISDIR(st.st_mode)) ? "d" :"-");
            printf((st.st_mode & S_IRUSR) ? "r":"-");
            printf((st.st_mode & S_IWUSR) ? "w":"-");
            printf((st.st_mode & S_IXUSR) ? "x":"-");

            //group R-W-X
            printf((st.st_mode & S_IRGRP) ? "r":"-");
            printf((st.st_mode & S_IWGRP) ? "w":"-");
            printf((st.st_mode & S_IXGRP) ? "x":"-");
                        
            //other R-W-X         
            printf((st.st_mode & S_IROTH) ? "r":"-");
            printf((st.st_mode & S_IWOTH) ? "w":"-");
            printf((st.st_mode & S_IXOTH) ? "x":"-");       

            //number of hard links
            printf(".  %d", (int)st.st_nlink);           
            printf(" %s ", pw->pw_name);
            printf("%s ", grp->gr_name);
            printf("%s\n ", argv[i]);

		}else{ //file passed in not a direct
			//user D-R-W-X
			printf((S_ISDIR(st.st_mode)) ? "d" :"-");
			printf((st.st_mode & S_IRUSR) ? "r":"-");
			printf((st.st_mode & S_IWUSR) ? "w":"-");
			printf((st.st_mode & S_IXUSR) ? "x":"-");

			//group R-W-X
			printf((st.st_mode & S_IRGRP) ? "r":"-");
			printf((st.st_mode & S_IWGRP) ? "w":"-");
			printf((st.st_mode & S_IXGRP) ? "x":"-");
			
			//other R-W-X
			printf((st.st_mode & S_IROTH) ? "r":"-");
			printf((st.st_mode & S_IWOTH) ? "w":"-");
			printf((st.st_mode & S_IXOTH) ? "x":"-");	

			//number of hard links
			printf(".  %d", (int)st.st_nlink);
			printf(" %s ", pw->pw_name);
			printf("%s ", grp->gr_name);
			printf("%s\n ", argv[i]);
		}

	}

}
