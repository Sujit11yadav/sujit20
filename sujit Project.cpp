#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define BRIDGE_DIRECTION_NORTH 0x01
#define BRIDGE_DIRECTION_SOUTH 0x02
typedef struct bridge
{
    int cars;
    int direction;
    pthread_mutex_t m;
    pthread_cond_t e;
} bridge_t;

static bridge_t shared_bridge={.cars=0,.direction=BRIDGE_DIRECTION_SOUTH,.m=PTHREAD_MUTEX_INITIALIZER,.e=PTHREAD_COND_INITIALIZER
};
static void arrive(bridge_t *bridge,int direction)
{
    pthread_mutex_lock(&bridge->m);
    while (bridge->cars>0&&(bridge->cars>5||bridge->direction!=direction))
	pthread_cond_wait(&bridge->e,&bridge->m);
    if (bridge->cars==0)
        bridge->direction=direction;
    bridge->cars++;
    if(bridge->direction==1)
    printf("crossing south to north\t num of cars on bridge=%d \n",bridge->cars);
    else
    printf("crossing north to south\t num of cars on bridge=%d \n",bridge->cars);
    sleep(1);
    pthread_mutex_unlock(&bridge->m);
}
static void leave(bridge_t *bridge)
{
    pthread_mutex_lock(&bridge->m);
    bridge->cars--;
    pthread_cond_signal(&bridge->e);
    pthread_mutex_unlock(&bridge->m);
}
static void drive(bridge_t *bridge,int direction)
{
    arrive(bridge, direction);
    leave(bridge);
}
static void* north(void *data)
{
    drive((bridge_t *)data,BRIDGE_DIRECTION_NORTH);
    return NULL;
}
static void* south(void *data)
{
    drive((bridge_t *) data,BRIDGE_DIRECTION_SOUTH);
    return NULL;
}
static int run(int a, int b)
{
    int i,n=a+b;
    pthread_t thread[n];
    for(i=0;i<n;i++)
    if(pthread_create(&thread[i],NULL,i<a?north:south,&shared_bridge))
    {
        printf("thread creation failed\n");
        return EXIT_FAILURE;
    }
    for(i=0;i<n;i++)
        if(thread[i])
            pthread_join(thread[i], NULL);
    return EXIT_SUCCESS;
}
int main(int argc,char **argv)
{
    int c,a=0,b=0;
    printf("Enter the South and North cars:");
    scanf("%d %d",&a,&b);
    if(b<=0){
    	printf("number of cars going north must be>0\n");
        exit(EXIT_FAILURE);
        }
    else if(a<=0)
	 {
      printf("number of cars going south must be>0\n");                        
        exit(EXIT_FAILURE);
        }
    return run(a,b);
}

