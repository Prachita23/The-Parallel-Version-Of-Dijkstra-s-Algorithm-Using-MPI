			#include <stdio.h>
			#include <mpi.h>
			#include <math.h>
			#include<limits.h>


			int num_nodes=6;
			 
			 
			int main (argc, argv)
			 int argc;
			 char *argv[];
			{
			 int rank, size,tag1,tag2,tag3,tag4,tag5,tag6,tag7,tag8;
			 int *found;
			 int i=0;
			 int j=0; 
			 tag1=1;
			 tag2=2;
			 tag3=3;
			 tag4=4;
			 tag5=5;
			 tag6=6;
			 tag7=7;
			 tag8=8;
			
			 MPI_Init (&argc, &argv); /* starts MPI */
			 
			 MPI_Comm_rank (MPI_COMM_WORLD, &rank); /* get current process id */
			 MPI_Comm_size (MPI_COMM_WORLD, &size); /* get number of processes */

			 if(rank == 0)
			{
				int  edge_weight[num_nodes][num_nodes];
				int dist[num_nodes];
				int SOURCE=0;
				int count=1;
				

				for(i=0;i<num_nodes;i++)
				{
					for(j=0;j<num_nodes;j++)
					{
						if(i==j)
							edge_weight[i][j]=0;
						else
							edge_weight[i][j]=999;
					}
				}
				edge_weight[0][1]=5;
				edge_weight[0][5]=7;
				edge_weight[1][2]=4;
				edge_weight[2][3]=3;
				edge_weight[2][4]=1;
				edge_weight[4][3]=2;
				edge_weight[5][4]=1;
			
				
			 
				/*Populate the distance and found array from source to all the nodes*/
				found= (int *) calloc(num_nodes, sizeof(int));
				for(i=0;i<num_nodes;i++){
					found[i]=0;
					dist[i]=edge_weight[SOURCE][i];
					
				}
				
				found[SOURCE]=1;
			
				int numberOfTasksPerProcess=num_nodes/size;  
				

			//The following array will contain the starting index(s) of the respective chunks(of the original array) assigned to various slave processes 
				int assignmentStartingIndex[size];
				int counter=0;
				int counter3=1;
				int counter4=1;
				int counter5=1;
				int leastPositions[size-1];
				int leastValues[size-1];
				int leastPositionPerProcess;
				int leastPositionValue;
				int found_leastValue;
				int foundleast[size-1];
				MPI_Status status;
				MPI_Status status1;
				MPI_Status status2;
				
				/*Computing the chunk to be processed by Master and storing it in a temporary array*/
				int temporayArrayMaster[numberOfTasksPerProcess];
					int foundMaster[numberOfTasksPerProcess];
					for(i=0;i<=numberOfTasksPerProcess-1;i++)
					{
						temporayArrayMaster[i]=dist[i];
						
						foundMaster[i]=found[i];
					}
					
				
				for(j=0;j<num_nodes;j++)
				{
					if(j%numberOfTasksPerProcess == 0)
					{
					assignmentStartingIndex[counter]=j;//Computing and saving the starting index of chunks assigned to slave processes
					counter=counter+1;
					}
				}//end of loop
				
				while(count<num_nodes)	{		
				printf("\n count=%d",count);
				//Sending array chunks to slave processes	
				for(j=1;j<size;j++)   //Since ranks of first and last slave processes are 1 and (size-1)
				{	
					int startIndex=assignmentStartingIndex[j];
					
					int endIndex;
					endIndex=startIndex + numberOfTasksPerProcess-1;
					
				printf("\n");
					int counter2=0;
					int l=0;
					for( l=startIndex;l<=endIndex;l++)
					{
					MPI_Send(&dist[l],1,MPI_INT,j,tag1,MPI_COMM_WORLD);
					MPI_Send(&found[l],1,MPI_INT,j,tag2,MPI_COMM_WORLD);
					
					}
				}
				
				/*Calculating the minimum value and its position of the Master Chunk*/
				
				for(i=0;i<=numberOfTasksPerProcess-1;i++)
					{
						foundMaster[i]=found[i];
						}
				int minPosition=chooseVertex(temporayArrayMaster,numberOfTasksPerProcess,foundMaster);
				int minMaster=dist[minPosition];
				int foundinMaster=found[minPosition];
				
				leastPositions[0]=minPosition;
				leastValues[0]= minMaster;
				foundleast[0]=foundinMaster;
				
				/*Receiving minimum value and its position from the respective slave processes and putting them in arrays*/
				for(i=1;i<size;i++)
				{
					MPI_Recv(&leastPositionPerProcess,1,MPI_INT,i,tag3,MPI_COMM_WORLD,&status);
					leastPositions[counter3]=leastPositionPerProcess;
					counter3++;
					
					MPI_Recv(&leastPositionValue,1,MPI_INT,i,tag4,MPI_COMM_WORLD,&status1);
					leastValues[counter4]=leastPositionValue;
					counter4++;
					
					MPI_Recv(&found_leastValue,1,MPI_INT,i,tag8,MPI_COMM_WORLD,&status2);
					foundleast[counter5]=found_leastValue;
					counter5++;
					
				}
				/*Finding global minimum from all the minimum values received from processes*/ 
				int minposition=findmin(leastValues,size,foundleast);
				int globalminposition=leastPositions[minposition];
				
				printf("globalminposition=%d",globalminposition);
				
				
				int globalmin=leastValues[minposition];
		
				found[globalminposition]=1; //Updating the node as 1(found) if it contains the global minimum
				
				count++;
				
				/*Sending the updated count value, global minimum calculated and corresponding edge received to all the slave processes*/
				for(i=1;i<size;i++){
				MPI_Send(&globalmin,1,MPI_INT,i,tag5,MPI_COMM_WORLD);
				
				MPI_Send(&edge_weight[globalminposition],num_nodes,MPI_INT,i,tag6,MPI_COMM_WORLD);
				
				MPI_Send(&count,1,MPI_INT,i,tag7,MPI_COMM_WORLD);
				}
				
				/* Updating the distance array if there is another shortest path available from the source to corresponding node*/
				for(i=0;i<numberOfTasksPerProcess;i++)
				{
					if(!found[i])
						temporayArrayMaster[i]= min(temporayArrayMaster[i],globalmin+edge_weight[globalminposition][i]);
						
						printf("\n Updated dist = %d for process=%d and count=%d",temporayArrayMaster[i],rank,count);
						
				}
			
				
				}
				
				}
				
			 
			else
			{
				int numberOfTasksPerProcess=num_nodes/size;  	
				int assignmentStartingIndex[size];  //Initializing the Array which will have the starting Index of the chunk for all slave processes	
				int counter=0,counter_found=0;
				int j=0;
				
				
				for(j=0;j<num_nodes;j++)
				{
					if(j%numberOfTasksPerProcess == 0)
					{
					assignmentStartingIndex[counter]=j;
					counter=counter+1;	
					}
				}//end of loop

				/*Calculating the start and end index of corresponding slave process*/
				int startIndex=	assignmentStartingIndex[rank];
			
				int endIndex;
					//In case the number of rows is not perfectly divisible by the number of processes
				
					endIndex=startIndex + numberOfTasksPerProcess-1;
					int numberOfNodesPerProcess= endIndex-startIndex+1;
					int temporaryArray[numberOfNodesPerProcess];  //Initializing the temporary Array which holds the chunk
					int found[numberOfNodesPerProcess];
					int i;
				
						
				//Code for receiving Data starts
				counter=0;
				
			
				for(i=startIndex;i<=endIndex;i++)
				{
				MPI_Status status;
				MPI_Recv(&temporaryArray[counter],1,MPI_INT,0,tag1,MPI_COMM_WORLD,&status); //Receive the array sent by the master for the particular slave
				
				
				counter=counter+1;
				printf("\n");
				}//end of loop
				
				int count=1;
				
			while(count<num_nodes){
				
				printf("\n count in slave =%d",count);
				for(i=startIndex;i<=endIndex;i++)
				{
				MPI_Status status22;
				MPI_Recv(&found[counter_found],1,MPI_INT,0,tag2,MPI_COMM_WORLD,&status22); //Receive the array sent by the master for the particular slave
				counter_found=counter_found+1;
				}//end of loop
				
				//Calculating the position of the minimum value in the array chunk held by the slave
				int minperProcess=chooseVertex(temporaryArray,numberOfNodesPerProcess,found);
				
				//Calculating the index of the minimum in the original ditance array
				int leastPositionPerProcess=minperProcess+startIndex;
				
				//Retrieving the value held at the position
				int leastPositionValue=temporaryArray[minperProcess];
				
				int found_leastValue=found[minperProcess];
				
				//Sending the values to Master
				MPI_Send(&leastPositionPerProcess,1,MPI_INT,0,tag3,MPI_COMM_WORLD);
				MPI_Send(&leastPositionValue,1,MPI_INT,0,tag4,MPI_COMM_WORLD);
				MPI_Send(&found_leastValue,1,MPI_INT,0,tag8,MPI_COMM_WORLD);
				
				int globalmin;
				MPI_Status status34;
				MPI_Status status35;
		
				MPI_Recv(&globalmin,1,MPI_INT,0,tag5,MPI_COMM_WORLD,&status34);
				int recvedge[num_nodes];
			
				//Receiving the edge
				MPI_Recv(&recvedge,num_nodes,MPI_INT,0,tag6,MPI_COMM_WORLD,&status35);
				
				int count1;
				MPI_Status status23;
				MPI_Recv(&count1,1,MPI_INT,0,tag7,MPI_COMM_WORLD,&status23);
				count=count1;
				
				
				//Update the distance array if there exists a path shorter than the distance from the source to a node
				for(i=0;i<numberOfNodesPerProcess;i++)
				{	
					if(!found[i])
						temporaryArray[i]= min(temporaryArray[i],globalmin+recvedge[startIndex+i]);
						
						printf("\n Updated dist = %d for process=%d",temporaryArray[i],rank);
						
				}
				
				}
			}
				
			 MPI_Finalize();
			return 0;

			}//end of 

			/*Choosing the vertex whose distance is minimum from the source for a process. Returning the position of the node*/
			int chooseVertex(int *dist, int n, int *found){   
				int i,tmp,leastPosition;
				int least=INT_MAX;
				for(i=0;i<n;i++)
				{
					tmp=dist[i];
					if(!found[i] && (tmp<=least))
					{
						least=tmp;
						leastPosition=i;
					}
				}
				printf("\n least position =%d",leastPosition);
				return leastPosition;
			}
			/*Calculating the global minimum distance from source to other nodes */
			int findmin( int *array,int size, int *foundleast ) {
				int i,tmp,globalminposition;
				int least=INT_MAX;
				for(i=0;i<size;i++)
				{
					tmp=array[i];
					
					if(!foundleast[i] &&(tmp<=least))
					{
						least=tmp;
						
						globalminposition=i;
					
					}
				}
				
				return globalminposition;
				
				
			}
			
			/*Calculating and returning the minimum*/
				int min(int a, int b)
				{int min;
					if(a<=b) 
						min=a;
					
					else
						min=b;
					return min;
				}