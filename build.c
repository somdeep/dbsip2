#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "random.c"



int binary(int32_t * arr,int start,int end,int32_t target);


int main(int argc, char* argv[])
{
	if(argc<4)
	{
		printf("\nArguments format : keys probes fanout");
		exit(0);
	}

	int32_t keys=atoi(argv[1]);//storing number of keys obtained as argument
	int32_t probes=atoi(argv[2]);//storing number of probes also obtained as arguments
	
	int32_t levels=argc-3;
	int32_t i=0;
	int32_t j=0;
	int32_t fanout[levels];//array stores fanout for each level
	int32_t lookfor = 10;
	int32_t foundindex;


	
	while(i<levels)
	{
		fanout[i]=atoi(argv[i+3]);	
		if(fanout[i]>17 || fanout[i]<2)
		{
			printf("Fanout values must be between 2 and 17\n");
			exit(0);
		}
		printf("%d\n",fanout[i] );
		i++;

	}


	int32_t numkey=levels-1;

	printf("Passed k:%d p:%d levels:%d\n",keys,probes,levels);

	int32_t *levelArray[levels];
	int32_t arraySize[levels];
	int32_t prev=1;
	int32_t treesize=0;
	int32_t current[levels];//points to most recently filled entry in each level
	int32_t nodesize[levels];//store extent of node currently filled at level under consideration
	//at any point of time, only one node is being filled on each level, this stores the extent to which that node is filled
	int32_t nodecount[levels];//store number of nodes at each level
	void* memorypt;

	//now building array of arrays
	for(i=0;i<levels;i++)
	{
		arraySize[i]=prev*(fanout[i]-1);
		treesize+=arraySize[i];
		//allocate size
		posix_memalign(&memorypt,16,sizeof(int32_t)*arraySize[i]);
		levelArray[i]=(int32_t *)memorypt;
		current[i]=0;
		nodesize[i]=0;
		nodecount[i]=
		prev=arraySize[i]+arraySize[i]/(fanout[i]-1);
		printf("Level %d maximum size : %d\n",i,arraySize[i]);
	}

	if(keys>treesize)
	{	
		printf("Too many keys\n");
		exit(1);
	}


	//calling in functions from program for obtaining random numbers
	rand32_t *gen = rand32_init(time(NULL));
	size_t n = keys;
	int32_t *a = generate_sorted_unique(n, gen);
	


	int32_t* value;
	//check for insert
	for (i = 0; i <levels ; i++)
	{
		value=levelArray[i];
		for (j = 0; j < arraySize[i]; j++)
		{
			*value=INT32_MAX;
			value++;
		}
	}


	printf("Total (maximum) number of keys possible : %d\n",treesize);
	printf("\n\nInserting and then printing tree\n\n");



	//insertion : building index

	int l=levels-1;//this holds the current level, always starts with the last level
	printf("levels : %d\n",levels );
	for (i = 0; i < keys; ++i)
	{	
		l=levels-1;//at the start of each iteration, we begin from the last level, and attempt to fill any available spaces
		while((fanout[l]-1) <= nodesize[l])
		{
			nodesize[l]=0;
			l--;
			if(l<0)
			{
				printf("\nToo many keys in insertion");
				exit(0);
			}
		}

		value=levelArray[l];
		value=value+(current[l]);

		*value=*a;
		//*value=i;
		a++;
		// printf("%d  %d\n",value,*value);
		current[l]++;
		nodesize[l]++;		
	}


	for (i = 0; i<levels; ++i)
	{
		value=levelArray[i];
		printf("Level %d -----> ",i);
		for (j = 0; j < arraySize[i]; ++j)
		{
			printf(" %d ", *value);
			value++;
		}
		printf("\n\n");
	}

	if(*levelArray[0] == INT32_MAX)
	{
		printf("Too few keys, empty root\n");
		exit(0);
	}


	free(gen);
	gen = rand32_init(time(NULL));
	n=probes;
	int32_t *b= generate(n,gen);
	int32_t *bholder=b;

	//PROBE LOOP - start of the probes being carried out

	int32_t result[levels];
	int32_t resultcount=0;
	i=0;
	int32_t part=0,traversed=0;
	int32_t target=*b;
	//int32_t target=;
	int32_t start=0;
	int32_t end=arraySize[i]-1;
	int32_t count[probes];
	printf("\nRanges : \n");


	for(j=0;j<n;j++)
	{
		resultcount=0;
		start=0;
		end=0;
		traversed=0;
		count[j]=0;
		//printf("New target : %d\n", target);
		for(i=0;i<levels;i++)
		{	
			if(i>0)
			{
				start=traversed*fanout[i-1]*(fanout[i]-1) + (fanout[i]-1)*part;
				end=start + fanout[i]-2;
				traversed=start/(fanout[i]-1);
			}
			part=1+binary(levelArray[i],start,end,target)-start;
			count[j]+=part+start;
			//printf("%d %d \n", );
			result[resultcount++]=part;
			// printf("Level %d ------>  %d\n",i,part);
		}
		target=*(++b);
		// for (int xyz = 0; xyz < resultcount; xyz++)
		// {
		// 	printf("Level %d ------>  %d\n",xyz,result[xyz]);//printing result array
		// }


	}

	b=bholder;
	for (i = 0; i < probes; ++i)
	{
		printf("Probe %d  : Value - %d : Range Identifier : %d\n",i,*b,count[i] );
		b++;
	}

	//free used memory
	free(gen);

	for (i = 0; i < levels; ++i)
	{
		free(levelArray[i]);
	}


	return(0);
       
}



int32_t binary(int32_t * arr,int32_t start,int32_t end,int32_t target)
{//module for binary search, always returns lower range which is in end
    if(start>end)
        return end;

    int32_t mid=(start+end)/2;

    if(target<=arr[mid])
        return binary(arr,start,mid-1,target);
    else
        return binary(arr,mid+1,end,target);    
        
}