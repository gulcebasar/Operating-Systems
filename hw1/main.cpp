//
//  main.cpp
//  sw
//
//  Created by Gulce Basar on 10/24/14.
//  Copyright (c) 2014 Gulce Basar. All rights reserved.
//
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>

using namespace std;

struct thrFuncParam // Parameters for the threads functions
{
    string fileNames[10];
    string directory;
    string proName;
    string type;
    int proCounter;
    int thrCounter;
};

void *ThreadFunc(void *prm) // the function threads will execute
{
    
    struct thrFuncParam *param;
    param = (struct thrFuncParam *) prm; // get the inputs
    
    string line, lineCont[3], name;
    ifstream file;
    double avg, total=0, lineCounter=0;
    
    string thrName = "Thread" + to_string(param->proCounter+1) + "." + to_string(param->thrCounter+1); // set the threads name
    
    name = param->directory + "/" + param->fileNames[param->thrCounter]; // set the files name
    file.open(name.c_str());
    
    while (getline(file, line)) // reads the file line by line
    {
        istringstream iss(line);
        iss >> lineCont[0] >> lineCont[1] >> lineCont[2]; // turns line into an array
        
        lineCounter++;
        total = total + atoi(lineCont[param->proCounter].c_str());
        //proCounter is the index of the grade process will read
        //adds the needed number to the total
    }
    avg = total / lineCounter;
    cout << thrName << " " << param->type << " average for " << param->fileNames[param->thrCounter] << " is:" << avg << endl;

    pthread_exit(NULL);
}


int main(int argc, const char * argv[])
{
    string directory, proName = "Master", type;
    DIR *dir;
    struct dirent *dirp;
    int pid, fileNum=0, proCounter = 0;

    
    cout << "Please enter the folder name with the path:";
    cin >> directory;
    
    cout << proName << ": Start" << endl;
    
    type="Project";
    pid = fork(); // Slave1 ( proCounter=0 pid=0 )
    
    if( pid !=0 )
    {
        waitpid (pid,NULL,NULL); // waits for slave1 to end
        type="Midterm";
        proCounter=1;
        pid = fork(); // Slave2 ( proCounter=1 pid=0 )
    }
    
    if( pid !=0 )
    {
        waitpid (pid,NULL,NULL);// waits for slave2 to end
        type="Final";
        proCounter=2;
        pid = fork(); // Slave3 ( proCounter=2 pid=0 )
    }
    
    // All slave processes have a different proCounter
    // proCounter is the index of the grade process will read
    
    if ( pid == 0 ) // Slave processes
    {
        string fileNames[10], thrName;
        int tid;
        
        cout << proName << ": " << type << " averages" << endl;
        
        proName= "Slave" + to_string(proCounter+1); // sets the process's name
        
        dir = opendir(directory.c_str());
        if(dir == NULL )
        {
            cout << "Error opening " << directory << endl;
            return 1;
        }
        
        while ((dirp = readdir(dir))) // reads the inputs of the directory
        {
            if( dirp->d_name != NULL )
            {
                if( dirp->d_name[0] != '.' ) // skips unnecessary files
                {
                    fileNames[fileNum] = dirp->d_name; // adds files name to the fileNames array
                    fileNum++; // increases file number
                }
            }
        }
        
        thrFuncParam param; // crreates the struct to send the inputs to the thread function
        
        // sets the values of the struct
        for(int i=0; i<fileNum ; i++ )
        {
            param.fileNames[i] = fileNames[i];
        }
        param.directory = directory;
        param.proName =  proName;
        param.type = type;
        param.proCounter = proCounter;
        
        pthread_t threads[fileNum]; //thread array as the size of file number
        
        for(int i=0; i<fileNum; i++ ){
            param.thrCounter=i; // sets threadConter to i
            // thrCounter is the index of the file it will read
            
            tid = pthread_create(&threads[i], NULL, ThreadFunc, (void *)&param); // creating threads
            if (tid){
                cout << "Error createing thread" << endl;
                exit(-1);
            }
            tid = pthread_join(threads[i], 0); // make threads wait for eachother
        }
        
        cout << proName << ": Done" << endl;
        closedir(dir);
    } // end of slave processes
    
    waitpid (pid,NULL,NULL); // waits for slave3 to end
    
    if( pid != 0 ) // master process
        cout << proName << ": Finish" << endl;
    
    return 0;
}

















