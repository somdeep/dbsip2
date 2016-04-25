#include <stdio.h>
#include <stdint.h>

int binary(int arr[],int start,int end,int target);
int main()

{
    
    printf("\nHello World");
    int32_t arr[]={1,2,5,6};
    int32_t *a=&arr[0];
    printf("%d\n",sizeof(arr)/sizeof(int) );
    int32_t result=binary(a,0,sizeof(arr)/sizeof(int)-1,-5)+1;
    printf("\nResult : %d",result);
    return 0;

}





// int32_t binary(int32_t * arr,int32_t start,int32_t end,int32_t target)
// {
//     if(start>end)
//         return end;


//     int32_t mid=(start+end)/2;


//     if(arr[mid] == target)
//         return mid;


//     else if(target<arr[mid])
//         return binary(arr,start,mid-1,target);
//     else
//         return binary(arr,mid+1,end,target);
        
    
        
// }


int32_t binary(int32_t * arr,int32_t start,int32_t end,int32_t target)
{
    if(start>end)
        return end;


    int32_t mid=(start+end)/2;


    // if(arr[mid] == target)
    //     return mid;


    if(target<=arr[mid])
        return binary(arr,start,mid-1,target);
    else
        return binary(arr,mid+1,end,target);
        
    
        
}