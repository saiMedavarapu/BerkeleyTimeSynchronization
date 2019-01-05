//********************************************************************
//	Name : Sai Medavarapu
//********************************************************************


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>


int clockValues(int rank, int i, int root, int buff[], int param[], int proc1[], int root_clock[],  int root_hr, int root_min,char str[], int param1[], char str1[], int size)
{
    if(rank == i -1 )
    {
        strcpy(str,&buff[0]);
        param[0]=str[0] -'0';
        param[1]=str[1] - '0';
        param[2]=str[3] - '0';
        param[3]=str[4] - '0';
        // The time for the root clock is saved at this position
        if(rank ==root)
        {
            strncpy(proc1,str,20);
            root_clock[0]=atoi(&proc1[0]);
            root_clock[1]=atoi(&proc1[1]);
            root_clock[3]=atoi(&proc1[3]);
            root_clock[4]=atoi(&proc1[4]);
            root_hr=root_clock[0]*60;
            root_min=root_clock[3]+root_hr;
        }
    }
    // This part scans the threshold value from the file
    else if(i==size+1 && rank == root)
    {
        strcpy(str,&buff[0]);
        strncpy(str1,str,20);
        param1[0]=str[0] - '0';
        param1[1]=str[1] - '0';
        param1[2]=str[3] - '0';
        param1[3]=str[4] - '0';
    }
    return 0;
}


//*********************** Main Function ***************************
int main(int argc, char *argv[])//start of main
{
    FILE *fp, *fp1;
    int lines=0;
    float Pr,P2,P3,P4,P5;
    char buff[80];
    char str[80];
    int param[80];
    int start=0;
    int entrycount;
    int count=0,rank,size;
    int j,k=0;
    int i=0;
    int param1[4];
    int root;
    char proc1[10];
    char str1[20];
    int root_clock[10];
    char buff1[80];
    int root_hr;
    int root_min;
    MPI_Status  Stat,Stat1;
    MPI_Request  req;
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    fp = fopen(argv[1],"r");/// Fopen function is being called for opening the file
    fp1 = fopen(argv[1],"r");
    int z=0;
    // This loop is used to abort the program if the user chooses wrong no of processors
    while(!feof(fp1))
    {
        fscanf(fp1,"%s",buff);
        z++;
    }
    printf("%d",z);
    if( size < (z-2) || size > (z-2) )
    {
        printf("Please give correct no of processors");
        MPI_Abort(MPI_COMM_WORLD,0); // used to abort the program
        exit(0);// exits the loop
    }
    else
    {
        // This while loop is used to read the file line by line untill the end of the file
        while(i<(size+2))
        {
            fscanf(fp,"%s",buff);// This functions used to read the file and save the strings nto the character array buff
            if( i == 0 )
            {
                strcpy(str,&buff[0]);
                root=atoi(str);//This part is used for getting the rank of the main processors mentioned in the file.
            }
            
            else
            {
                //int clockValues(rank, i, str, buff, param,root, proc1, root_clock, root_hr,root_min, str,  param1,  str1 , size);
                //I tried this without commenting but it gave me a wierd error of non termination. I have commented it.

                //This function is used to read the clock values given in file and these are stored by individual processors later.
                if(rank == i -1 )
                {
                    strcpy(str,&buff[0]);
                    param[0]=str[0] -'0';
                    param[1]=str[1] - '0';
                    param[2]=str[3] - '0';
                    param[3]=str[4] - '0';
                    // The time for the root clock is saved at this position
                    if(rank ==root)
                    {
                        strncpy(proc1,str,20);
                        root_clock[0]=atoi(&proc1[0]);
                        root_clock[1]=atoi(&proc1[1]);
                        root_clock[3]=atoi(&proc1[3]);
                        root_clock[4]=atoi(&proc1[4]);
                        root_hr=root_clock[0]*60;
                        root_min=root_clock[3]+root_hr;
                    }
                }
                // This part scans the threshold value from the file
                else if(i==size+1 && rank == root)
                {
                    strcpy(str,&buff[0]);
                    strncpy(str1,str,20);
                    param1[0]=str[0] - '0';
                    param1[1]=str[1] - '0';
                    param1[2]=str[3] - '0';
                    param1[3]=str[4] - '0';
                }
            }
            i++;
        }
        fclose(fp);
    }
    // If there is a problem in opening the file then this loop is used to for error handling.
    if(fp == NULL)
    {
        printf("not found");
        MPI_Abort(MPI_COMM_WORLD,0);
        exit(0);// exits the loop.
    }
    // This part is handled by the main processor mentioned in the file.
    if(rank==root)
    {
        int diff_time,ack;
        int my_min,my_hr;
        int avg_time;
        int inp_time[size];
        int r[size];
        int i;
        float set_time = 0;
        int count = 0;
        int flag[size];
        float actual_time;
        my_hr=(param1[0]*10+param1[1])*60;
        my_min=(param1[2]*10+param1[3])+my_hr;
        printf("I am process with rank %d acting as the coordinator process \n", root);
        // The main processor bradcasts its time to all processors so as to caliculate their difference with respect to the main clocks time.
        MPI_Bcast(proc1,4,MPI_INT,root,MPI_COMM_WORLD);
        for(i=0;i<size;i++)
        {
            if(i != root)
            {
                //timeDifferentials(diff_time, Stat, inp_time, r, avg_time, my_min, set_time, count, i);
                // This part is used to recevive the time diffrentials sent from each processor present.
                MPI_Recv(&diff_time, 1, MPI_INT, MPI_ANY_SOURCE, 55, MPI_COMM_WORLD, &Stat);
                inp_time[i]=diff_time;
                r[i] = Stat.MPI_SOURCE;
                avg_time = inp_time[i];
                int diff1_time=abs(avg_time);
                if(diff1_time < my_min)
                {
                    set_time = set_time + avg_time;
                    count++;
                    diff1_time = 0;
                }
                // if the value is greater than the threshold value then that is ignored.
                else
                {
                    printf( "Coordinator Process is ignoring time differential value of %d from process %d \n", diff_time, i);
                }
            }
        }
        // here the average time value if caliculated for the processors which satisfy the threshold condition.
        set_time = set_time/(count+1);
        printf("Time differential average is %f \n", set_time);
        for(i=0;i<size;i++)
        {
            // In this loop the difference for each clock with respect to the average time is caliculated and later sent to respective clocks.
            if(i != root)
            {
                actual_time = set_time - inp_time[i];
                printf("Coordinator process is sending the clock adjustment value of %f to process %d \n", actual_time, r[i]);
                MPI_Send(&actual_time, 1, MPI_FLOAT, r[i], 65, MPI_COMM_WORLD);
            }
        }
        float new_time = root_min + set_time;
        int new1_time = new_time/60;
        int fraction_time = new_time - new1_time;
        //fraction_time = fraction_time*60;
        printf("Adjusted local time at process %d is %s", rank, proc1); // the final adjusted values by each clock are being printed by the root processor.
        
    }
    // This part of the program is used by all the other processors declared by the used during the run time.
    else
    {
        int my_array[10],i,diff_array[10],hours,tot_min, my_hr,my_min,diff_time;
        char proc1[10];
        float actual_time;
        my_array[0]=param[0];
        my_array[1]=param[1];
        my_array[2]=param[2];
        my_array[3]=param[3];
        MPI_Bcast(proc1,4,MPI_INT,root,MPI_COMM_WORLD);// Receiving the broadcast from the root.
        char str1[20];
        printf( "Coordinator process is sending time %d \n", root);
        printf( "Process %d has received time %s \n", rank, proc1); // The main clock time is being received and saved into an array.
        diff_array[0]=proc1[0] - '0';
        diff_array[1]=proc1[1] - '0';
        diff_array[2]=proc1[3] - '0';
        diff_array[3]=proc1[4] - '0';
        hours=(diff_array[0]*10+diff_array[1])*60;
        tot_min=(diff_array[2]*10+diff_array[3])+hours;
        my_hr=(my_array[0]*10+my_array[1])*60;
        my_min=(my_array[2]*10+my_array[3])+my_hr;
        diff_time =	my_min - tot_min;// The difference withrespect to the main clock is caliculated and sent back to the root processor
        printf( "Process %d is sending time differential value of %d to process %d\n", rank, diff_time, root);
        MPI_Send(&diff_time, 1, MPI_INT, root, 55, MPI_COMM_WORLD);
        MPI_Recv(&actual_time,1,MPI_FLOAT,root,65,MPI_COMM_WORLD,&Stat1);// Here the value to which each clock need to add and adjust is being received.
        printf("Process %d has received the clock adjustment value of %f \n", rank, actual_time);
        float new_time = my_min + actual_time;// each clock is adjusting its value by using the value sent from the mian clock
        int new1_time = new_time/60;
        int fraction_time = new_time - new1_time;
        // fraction_time = fraction_time*60;
        //printf("Adjusted local time at process %d is %s \n", rank, proc1);
    }
    MPI_Finalize();
    return 0;
}
