#include <iostream>
#include <string>
using namespace std;

struct version
{
    int timestamp;
    version *next;
    bool *usedblocks;
};
struct MyFile
{
    string name;
    string content;
    MyFile(string name,string content)
    {
        this->name=name;
        this->content=content;
    }
};
struct backupfile
{
    string name;
    backupfile *next;
    version *vhead;
};
class DataProtection
{
    char *memory;
    int no_of_blocks;
    int block_size;

    bool *freemem;
    int free_blocks;

    int hash_size;
    backupfile **list;

    int time_stamp;
public:
    DataProtection(int mem_size,int block_size)
    {
        no_of_blocks=mem_size/block_size;
        this->block_size=block_size;
        memory = new char[mem_size];

        free_blocks=no_of_blocks;
        freemem= new bool[no_of_blocks];
        for(int i=0;i<no_of_blocks;i++)
            freemem[i]=true;
        hash_size=10;
        list = new backupfile*[hash_size];
        for(int i=0;i<hash_size;i++)
            list[i]=NULL;
    }
    void backup(MyFile);
    int hash(string);
    void timestamp(int val)
    {
        time_stamp=val;
    }
    void displayVirtualDisk()
    {
        int i;
        for(i=0;i<no_of_blocks * block_size;i++)
            cout<<memory[i]<<" ";
        cout<<"\n\n";
    }
    void restore(string,int);
    void displaycatalog();
};
int DataProtection::hash(string str)
{
        int len=str.size(),i,sum;
        sum=0;
        for(i=0;i<len;i++)
            sum+=str[i]-'0';
        return sum%hash_size;
}
void DataProtection::backup(MyFile f)
{
    int max_size=f.content.size();
    if(max_size>block_size *free_blocks)
    {
        cout<<"Stack Overflow, Memory Overrun\n Exiting...\n";
        return;
    }
    version *curr_ver = new version;
    curr_ver->timestamp=time_stamp;
    curr_ver->next=NULL;
    curr_ver->usedblocks = new bool[no_of_blocks];
    for(int i=0;i<no_of_blocks;i++)
        curr_ver->usedblocks[i]=false;
    int i,j,k=0;
    for(i=0;i<no_of_blocks && k<max_size;i++)
    {
        if(freemem[i])
        {
            freemem[i]=false;
            free_blocks--;
            curr_ver->usedblocks[i]=true;
            for(j=i*block_size;j<(i+1)*block_size;j++)
            {
                if(k<max_size)
                {
                    memory[j]=f.content[k++];
                }
                else
                {
                    memory[j]='\0';
                }
            }
        }
    }
    int hashindex = hash(f.name);
    backupfile *curr=list[hashindex];
    while(curr)
    {
        if(curr->name==f.name)
        {
            version *temp=curr->vhead;
            curr->vhead=curr_ver;
            curr_ver->next=temp;
            break;
        }
        curr=curr->next;
    }
    if(!curr)
    {
        backupfile * newfile = new backupfile;
        newfile->name=f.name;

        backupfile *tempfile=list[hashindex];
        list[hashindex]=newfile;
        newfile->next=tempfile;
        newfile->vhead=curr_ver;
    }
}
void DataProtection::restore(string filename,int time)
{
        int hashindex=hash(filename);
        backupfile *curr=list[hashindex];
        string result="";
        while(curr)
        {
            if(curr->name==filename)
            {
                    version *temp=curr->vhead;
                    while(temp)
                    {
                        if(temp->timestamp=time)
                        {
                            int i;
                            for(i=0;i<no_of_blocks;i++)
                            {
                                if(temp->usedblocks[i])
                                {
                                    int j;
                                    for(j=i*block_size;j<(i+1)*block_size;j++)
                                    {
                                        if(memory[j]!='\0')
                                            result+=memory[j];
                                        else
                                            break;
                                    }
                                }
                            }
                        }
                        temp=temp->next;
                    }
            }
            curr=curr->next;
        }
        cout<<result<<"\n";
}
void DataProtection::displaycatalog()
{
    int i;
    for(i=0;i<hash_size;i++)
    {
        if(list[i])
        {
            cout<<i<<"\n";
            backupfile *curr=list[i];
            while(curr)
            {
                cout<<"File Name: "<<curr->name<<"\n";
                version *head=curr->vhead;
                while(head)
                {
                    cout<<"Version No: "<<head->timestamp<<"\n";
                    cout<<"Used Blocks:\n";
                    for(int j=0;j<no_of_blocks;j++)
                        if(head->usedblocks[j])
                            cout<<j<<" ";
                    cout<<"\n";
                    head=head->next;
                }
                curr=curr->next;
            }
        }
    }
}
int main()
{
    MyFile f1("NewFile1","File Name: NewFile1 Date: 02/07/2016");
    MyFile f2("NewFile2","File Name: NewFile1 Date: 02/07/2016");
    DataProtection dp(256,16);
    dp.timestamp(2);
    dp.backup(f1);
    dp.displayVirtualDisk();
    dp.restore("NewFile1",2);
    dp.displaycatalog();
    return 0;
}
