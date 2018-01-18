#include <sys/defs.h>
#include <string.h>
#include <sys/kprintf.h>
#include <sys/virmem.h>
#include <sys/phymem.h>
#include <sys/gdt.h>
#include <sys/process.h>
#include<sys/tarfs.h>
#include <sys/initializeIDT.h>
#include <sys/keyboardHandler.h>
FILE VFS[100];
int fd=2;
uint64_t cwd=0;
extern char command_buffer[100];
extern volatile int done_read;
extern int buf_count;
extern struct task *current;
extern struct task *next;

uint64_t pow(int x,int k)
{
	if(k==0)
	  return 1; 
	return x*(pow(x,k-1));
}


uint64_t  octal_decimal(int octal)
{
	int res=0,k=0;
	while(octal>0)
	{
		int rem=octal%10;
		octal=octal/10;
		res=res+(rem)*pow(8,k++);
	}
	return res;
}

uint64_t getcwd(uint64_t buf)
{
	char *input_buffer=(char *)buf;
	int len =strlen(VFS[cwd].name);
	int i;
	for(i=0;i<len;i++)
	   input_buffer[i]=VFS[cwd].name[i];
	input_buffer[i]='\0';
	return (uint64_t)input_buffer;
}

uint64_t chdir_old(uint64_t buf)
{
//        char * ls_dir = (char *) buf;
//        kprintf("ls_dir given :%s\n", ls_dir);
        int fd_file=-1, fd_alt=-1;
        char *filename=(char *)buf;
        char tempDir[80];
        if((filename[0]==' ') | (filename[0]=='\0'))
        {       
                fd_file = 0;
        }
        else if(filename[0]=='/')
        {   
            int i=0;
            for(i=1;i<strlen(filename);i++)
            {   
                tempDir[i-1]=filename[i];
            }
            tempDir[i]='\0';
            fd_alt = open((uint64_t)tempDir);
            tempDir[i-1]='/';
            tempDir[i]='\0';
            fd_file = open((uint64_t)tempDir);
	    fd_file = -1;
        }
        else if(filename[0]=='.')
        {   
            if(filename[1]=='.')
            {   
                int new_fd1 = next->fd_table[cwd].parent;
                int j=0;
                strcpy(next->fd_table[new_fd1].name, tempDir);
                for(j = strlen(next->fd_table[new_fd1].name);j<(strlen(next->fd_table[new_fd1].name)+strlen(filename)-3); j++)
                {
                    tempDir[j]=filename[j-strlen(next->fd_table[new_fd1].name)+3];
                }
                tempDir[j]='\0';
                fd_alt = open((uint64_t)tempDir);
                tempDir[j]='/';
                tempDir[j+1]='\0';
//                kprintf("tempDir :%s\n",tempDir);
                fd_file = open((uint64_t)tempDir);
            }
            if(filename[1]=='/')
            {
                int j=0;
                strcpy(next->fd_table[cwd].name, tempDir);
                for(j = strlen(next->fd_table[cwd].name);j<(strlen(next->fd_table[cwd].name)+strlen(filename)-2); j++)
                {
                    tempDir[j]=filename[j-strlen(next->fd_table[cwd].name)+2];
                }
                tempDir[j]='\0';
                fd_alt = open((uint64_t)tempDir);
                tempDir[j]='/';
                tempDir[j+1]='\0';
                fd_file = open((uint64_t)tempDir);
            }
        }
        else
        {
                int j=0;
                strcpy(next->fd_table[cwd].name, tempDir);
                for(j = strlen(next->fd_table[cwd].name);j<(strlen(next->fd_table[cwd].name)+strlen(filename)); j++)
                {
                        tempDir[j]=filename[j-strlen(next->fd_table[cwd].name)];
                }
                tempDir[j]='\0';
                fd_alt = open((uint64_t)tempDir);
                if(tempDir[j-1]!='/')
                {
                        tempDir[j]='/';
                }
                tempDir[j+1]='\0';
                fd_file = open((uint64_t)tempDir);
        }
        if(fd_file!=-1)
	{
                cwd = fd_file;
                kprintf("The new directory is :%s \n", next->fd_table[cwd].name);
                return 1;
        }
        else if(fd_alt!=-1)
        {
                cwd = fd_alt;
                kprintf("%d \n", next->fd_table[cwd].name);
        }
        else
        {
                kprintf("ERROR: No such Directory\n");
                return 0;
        }
        return 1;
}

uint64_t chdir(uint64_t path)
{
	char *input_buf=(char *)path;
	int k=strlen(input_buf);
	char buf[k];
	int dirFound = 0;
	int positionOfSlash=-1;
	for(int i=0; i<strlen(input_buf);i++)
	{
		if(input_buf[i]=='/')
			{ positionOfSlash = i;
			break; }
	}
	for(int i=(positionOfSlash+1); i<strlen(input_buf);i++)
        {
		buf[i-(positionOfSlash+1)] = input_buf[i];
	}
	if(((input_buf[0]!='.')&(input_buf[0]!='/'))&(positionOfSlash!=-1))
	{
		for(int i=0; i<positionOfSlash;i++)
        	{       
                	buf[i] = input_buf[i];
        	}
	}
	//kprintf("Path given to chdir is :%s\n",input_buf);
	if((input_buf[0]==' ') & (input_buf[1]==' '))
	{
		cwd = 0;
		dirFound = 1;
	}
	if((input_buf[0]=='.') & (input_buf[1]=='/'))
	{
		if((input_buf[2]=='.') & (input_buf[3]=='.'))
		{
			for(int i=5;i<strlen(input_buf);i++)
			{
				buf[i-5]=input_buf[i];
			}
			int fdNew = VFS[cwd].parent;
			cwd = fdNew;
		}
                char str1[k-2];
                int d;
                for(d=2;input_buf[d]!='\0';d++){ str1[d-2]=input_buf[d]; }
                str1[d]='\0';
                int h1 = strlen(VFS[cwd].name);
                char tempName[h1+k];
                strcpy(VFS[cwd].name, tempName);
                int j1;
                for(j1=0;str1[j1]!='\0';j1++)
                {
                        tempName[h1+j1]=str1[j1];
                }
                tempName[h1+k-2]='/';
		for(int j=0; j<100; j++)
		{
                        char *strName = VFS[j].name;
			if(!strcmp(strName, tempName))
			{
				cwd=j;
				dirFound = 1;
				break;
			}
		}
	}
	else if((input_buf[0]=='.') & (input_buf[1]=='.'))
        {
	    if(input_buf[2]=='/')
	    {
                int fdNew = VFS[cwd].parent;
                cwd = fdNew;
		dirFound = 1;
		char str1[k-3];
		int d;
		for(d=3;input_buf[d]!='\0';d++){ str1[d-3]=input_buf[d]; }
		str1[d]='\0';
		int h1 = strlen(VFS[cwd].name);
		char tempName[h1+k-2];
		strcpy(VFS[cwd].name, tempName);
		int j1;
        	for(j1=0;str1[j1]!='\0';j1++)
        	{
                	tempName[h1+j1]=str1[j1];
        	}
		tempName[h1+k-3]='\0';
		if(tempName[h1+k-4]!='/'){ tempName[h1+k-3]='/'; tempName[h1+k-2]='\0'; }
		if(strlen(buf)!=0)
		{
			for(int j=0; j<7; j++)
                	{
				char *strName = VFS[j].name;
				if(!strcmp(strName, tempName))
				{
					cwd=j;
                                        dirFound = 1;
                                        break;
				}
                        }
		}
		else if(buf[0] == ' ') { dirFound = 1; }
		else { dirFound = 1; }
	    }
	    else
	    {
		int fdNew = VFS[cwd].parent;
                cwd = fdNew;
		dirFound = 1;
	    }
        }
	else if((input_buf[0]=='/'))
	{
		cwd=0;

                char str1[k-1];
                int d;
                for(d=1;input_buf[d]!='\0';d++){ str1[d-1]=input_buf[d]; }
                str1[d]='\0';
                int h1 = strlen(VFS[cwd].name);
                char tempName[h1+k];
                strcpy(VFS[cwd].name, tempName);
                int j1;
                for(j1=0;str1[j1]!='\0';j1++)
                {
                        tempName[h1+j1]=str1[j1];
                }
                tempName[h1+k-1]='/';
		if(strlen(buf)!=0)
                {
                        for(int j=0; j<100; j++)
                        {
                                char *strName = VFS[j].name;
				if(!strcmp(strName, tempName))
				{   
                                        //cwd=j;
					dirFound = 1;
                                        break;
                                }
                        }
                }
		else { dirFound = 1; }
		kprintf("Directory not accessible \n");
	}
	else
	{

                char str1[k];
                int d;
                for(d=0;input_buf[d]!='\0';d++){ str1[d]=input_buf[d]; }
                str1[d]='\0';
                int h1 = strlen(VFS[cwd].name);
                char tempName[h1+k];
                strcpy(VFS[cwd].name, tempName);
                int j1;
                for(j1=0;str1[j1]!='\0';j1++)
                {               
                        tempName[h1+j1]=str1[j1];
                }
		if(tempName[h1+k-1]!='/')
		{
                	tempName[h1+k]='/';
		}

		if(strlen(buf)!=0)
                {
                        for(int j=0; j<100; j++)
                        {
                                char *strName = VFS[j].name;
				if(!strcmp(strName, tempName) && (VFS[j].parent == cwd))
				{
                                        cwd=j;
					dirFound = 1;
                                        break;
                                }
                        }
                }
	}
	if(dirFound==0)
	{
		kprintf("Directory not found\n");
		return -1;
	}
	//kprintf("Current dir fd is :%d\n", cwd);
        //kprintf("Current dir path is :%s\n", VFS[cwd].name);
	return 1;
}

int get_parent(char  *filename)
{
	int len=strlen(filename);
	char  buf[len+1];
	int i=0;
	while(filename[i]!='/')
	{
		buf[i]=filename[i];
		i++;
	}
	buf[i]=filename[i];
	buf[++i]='\0';
	for(i=2;i<=fd;i++)
	{
        	if(!strcmp(buf,VFS[i].name))
                		return i;
	}
	return -1;
}
uint64_t readdir(uint64_t buf)
{
        int fd_file=-1, fd_alt=-1;
        char *filename=(char *)buf;
        char tempDir[80];
        if((filename[0]==' ') | (filename[0]=='\0'))
        {
                fd_file = cwd;
                int i;
                for(i=0;i<=fd;i++)
                {
                        if(current->fd_table[i].parent==fd_file)// &&  i!=next->fd_table[i].parent)
                        {
                                char buf[100];
                                int j=0;
                                for(j=0; j<strlen(current->fd_table[i].name); j++)
                                { buf[j] = current->fd_table[i].name[j]; }
                                buf[j] = '\0';
                                kprintf("%s \n", buf);
                        }
                }
                return 1;
        }
        if(filename[0]=='/')
        {
            int i=0;
            for(i=1;i<strlen(filename);i++)
            {
                tempDir[i-1]=filename[i];
            }
            tempDir[i]='\0';
            fd_alt = open((uint64_t)tempDir);
            tempDir[i-1]='/';
            tempDir[i]='\0';
            fd_file = open((uint64_t)tempDir);
	    fd_file = -1;
        }
        else if(filename[0]=='.')
        {
            if(filename[1]=='.')
            {
                int new_fd1 = current->fd_table[cwd].parent;
                int j=0;
                strcpy(current->fd_table[new_fd1].name, tempDir);
                for(j = strlen(current->fd_table[new_fd1].name);j<(strlen(current->fd_table[new_fd1].name)+strlen(filename)-3); j++)
                {
                    tempDir[j]=filename[j-strlen(current->fd_table[new_fd1].name)+3];
                }
                tempDir[j]='\0';
                fd_alt = open((uint64_t)tempDir);
                tempDir[j]='/';
                tempDir[j+1]='\0';
                fd_file = open((uint64_t)tempDir);
            }
            if(filename[1]=='/')
            {
                int j=0;
                strcpy(current->fd_table[cwd].name, tempDir);
                for(j = strlen(current->fd_table[cwd].name);j<(strlen(current->fd_table[cwd].name)+strlen(filename)-2); j++)
                {
                    tempDir[j]=filename[j-strlen(current->fd_table[cwd].name)+2];
                }
                tempDir[j]='\0';
                fd_alt = open((uint64_t)tempDir);
                tempDir[j]='/';
                tempDir[j+1]='\0';
                fd_file = open((uint64_t)tempDir);
            }
        }
        else
        {
            int j=0;
            strcpy(current->fd_table[cwd].name, tempDir);
            for(j = strlen(current->fd_table[cwd].name);j<(strlen(current->fd_table[cwd].name)+strlen(filename)); j++)
            {
                tempDir[j]=filename[j-strlen(current->fd_table[cwd].name)];
            }
            tempDir[j]='\0';
            fd_alt = open((uint64_t)tempDir);
            if(tempDir[j-1]!='/')
            {   
                tempDir[j]='/';
            }
            tempDir[j+1]='\0';
            fd_file = open((uint64_t)tempDir);
        }
        if(fd_file!=-1)
        {
                int i, v = 0;
                for(i=0;i<=fd;i++)
                {
                        if(current->fd_table[i].parent==fd_file)// &&  i!=next->fd_table[i].parent)
                        {
                                char buf[100];
                                int j=0;
                                for(j=0; j<strlen(current->fd_table[i].name); j++)
                                { buf[j] = current->fd_table[i].name[j]; }
                                buf[j] = '\0';
                                if(i!=0) { kprintf("%s \n", buf); }
                                v = 1;
                        }
                }
                if(v==0)
                {
                        kprintf("%s \n", current->fd_table[fd_file].name);
                }
        }
        else if(fd_alt!=-1)
        {
                kprintf("%s \n", current->fd_table[fd_alt].name);
        }
        else
        {
                kprintf("Directory / File not found\n");
                return 0;
        }
        return 1;
}

void init_tarfs()
{
	struct posix_header_ustar * start=(struct posix_header_ustar * )&_binary_tarfs_start;
	int offset=0;
	uint64_t size = 0;
	while(strlen(start->name)!=0)
	{
		size=octal_decimal(stoi(start->size));
		VFS[fd].size=octal_decimal(stoi(start->size));
		strcpy(start->name,VFS[fd].name);
		strcpy(start->typeflag,VFS[fd].typeflag);
		VFS[fd].fd=fd;
		VFS[fd].f_address=((uint64_t)(&_binary_tarfs_start) +offset + 512);
		VFS[fd].parent=get_parent(VFS[fd].name);
		VFS[fd].referenceCount = 0;
		if(fd==VFS[fd].parent)
			VFS[fd].parent = 0;
 		if(size==0)
                	offset=offset+512;
		else if(size%512==0)
                		offset=offset +size+512;
		else
                	offset=offset + (size/512)*512 + 512+512;
		start=(struct posix_header_ustar * )(&_binary_tarfs_start+offset);
		fd++;
	}
}
uint64_t open(uint64_t file)
{
int i;
char * filename=(char *)file;
if(!strcmp(filename,"."))
{
  current->fd_table[cwd].reference_count +=1;
  return cwd;
}
for(i=2;i<=fd;i++)
{
        if(!strcmp(filename,current->fd_table[i].name))
        {
                current->fd_table[i].reference_count +=1;
                return i;
        }
}
return -1;
}
uint64_t read(uint64_t fd,uint64_t  buf,uint64_t length)
{
                if(fd==0)
                {
                        int i;
                        char *input_buffer=(char *)buf;
                        while(done_read!=1){}
                        done_read=0;
                        int bytes_read=buf_count;
                        buf_count=0;
                        for(i=0;i<bytes_read;i++)
                        input_buffer[i]=command_buffer[i];
                        input_buffer[i]='\0';
                        return bytes_read;
                }
                else
                {
                        char *file_start=(char *)current->fd_table[fd].f_address;
			length=1024;
			memcpy((void *) buf, (void *) file_start, length);
                                return current->fd_table[fd].size;

                }
}
uint64_t read_cat(uint64_t fd,uint64_t  buf,uint64_t length)
{       
        char *file_start=(char *)current->fd_table[fd].f_address;
        length=1024; 
        memcpy((void *) buf, (void *) file_start, length);
        char *temp;
        temp = strtok(file_start, "\n");
        while(temp != NULL)
        {       
                kprintf("%s \n",temp);
                temp = strtok(NULL, "\n");
        }
        return current->fd_table[fd].size;
}
uint64_t write(uint64_t  buf,uint64_t length)
{
		char *out_buf=(char *)buf;
		kprintf("\n %s \n",out_buf);
                return length;
}

uint64_t cat(uint64_t path)
{
        int fd_file=-1;
        char *filename=(char *)path;
        char fileContent[1024];
//        kprintf("Filename in cat: %s\n",filename);

        char tempDir[80];
        if(filename[0]=='/')
        {
            int i=0;
            for(i=1;i<strlen(filename);i++)
            {
                tempDir[i-1]=filename[i];
            }
            tempDir[i]='\0';
            fd_file = open((uint64_t)tempDir);
        }
        else if(filename[0]=='.')
        {
            if(filename[1]=='.')
            {
                int new_fd1 = current->fd_table[cwd].parent;
                int j=0;
                strcpy(current->fd_table[new_fd1].name, tempDir);
                for(j = strlen(current->fd_table[new_fd1].name);j<(strlen(current->fd_table[new_fd1].name)+strlen(filename)-3); j++)
                {
                    tempDir[j]=filename[j-strlen(current->fd_table[new_fd1].name)+3];
                }
                tempDir[j]='\0';
                fd_file = open((uint64_t)tempDir);
            }
            if(filename[1]=='/')
            {
                int j=0;
                strcpy(current->fd_table[cwd].name, tempDir);
                for(j = strlen(current->fd_table[cwd].name);j<(strlen(current->fd_table[cwd].name)+strlen(filename)-2); j++)
                {
                    tempDir[j]=filename[j-strlen(current->fd_table[cwd].name)+2];
                }
                tempDir[j]='\0';
                fd_file = open((uint64_t)tempDir);
            }
        }
        else
        {
            int j=0;
            strcpy(current->fd_table[cwd].name, tempDir);
            for(j = strlen(current->fd_table[cwd].name);j<(strlen(current->fd_table[cwd].name)+strlen(filename)); j++)
            {
                tempDir[j]=filename[j-strlen(current->fd_table[cwd].name)];
            }
            tempDir[j]='\0';
            fd_file = open((uint64_t)tempDir);
        }

        if(fd_file!=-1)
        {
                read_cat(fd_file, (uint64_t)fileContent, 1024);
        }
        else
        {
                kprintf("File not found\n");
                return 0;
        }
        return 0;
}
