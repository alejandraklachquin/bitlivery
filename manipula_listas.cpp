//remove evento da lista de eventos, mas nao desaloca a memoria
void remove_event(event *event_toremove)
{
     struct event *prev, *next;
     
     prev = event_toremove->prev;
     next = event_toremove->next;
     
     prev->next=next;
     if(next != NULL) next->prev=prev;
     
}

//reinsere evento na lista de eventos
void recoloca ( event *list, int event_id, float time, int server_id, int client_id, event *fix_event ) 
{ 
    struct event *pont, *ant;
    
    ant = list;
    pont = list->next;
    
    //procura posicao correta
    while(pont)
    {
         if( pont->time < time ) 
         {   ant=pont;
             pont=pont->next;}
         else{ 
              break;} 
                
    }
    
    fix_event->time = time;
    
    fix_event->next = (ant)->next;
    fix_event->prev = (ant);
    (ant)->next = fix_event;
    if((fix_event->next) != NULL) (fix_event->next)->prev = fix_event;
           
}
  
 void busca_tempo ( event *events, float when, event **ant, event **ponte) {

    struct event *ptr;

    *ant = events; // aponta no anterior 
    *ponte = NULL;  // aponta no procurado, se nao achar retorna nulo 
    ptr = events->next; // aponta para candidato 

    while (ptr)
    {  
        if ((ptr->time) < when) 
        { // ainda nao chegou no no
            *ant = ptr;
             ptr = ptr->next;
        }
        else 
        {       
               //a principio enocontramos o evento, ou no caso de insercao, a posicao a ser inserido       
               if (ptr->time == when) 
               { 
                
                      *ponte = ptr; // achou evento
                      
               }
               //CASO DE INSERCAO: (ou evento deveria estar na lista e nao esta - ERROR)
               ptr = NULL; //esta deveria ser a posicao do evento
               //no caso de insercao, "ant" fica apontando para evento que ira antecer novo evento
               //laco eh quebrado aqui pois ptr recebe valor null
               
        }
    }
}

// funcao usada para busca de eventos lista "events" - ou para encontrar posicao de insercao
void busca ( event *events, float when, int server_id, int client_id, event **ant, event **ponte) {

    struct event *ptr;

    *ant = events; // aponta no anterior 
	*ponte = NULL;  // aponta no procurado, se nao achar retorna nulo 
    ptr = events->next; // aponta para candidato 

    while (ptr)
    {  
        if ((ptr->time) < when) 
        { // ainda nao chegou no no
            *ant = ptr;
             ptr = ptr->next;
        }
        else 
        {       
               //a principio enocontramos o evento, ou no caso de insercao, a posicao a ser inserido       
               if (ptr->time == when) 
               {
                  if((client_id == ptr->client_id) && (server_id == ptr->server_id))
                  {
                      *ponte = ptr; // achou evento
                      break;
                  }
                  else {
                       //temos um evento com mesmo horario, mas nao eh o evento que buscamos
                       *ant = ptr;
                       ptr = ptr->next;
                       continue;}
               }
               //CASO DE INSERCAO: (ou evento deveria estar na lista e nao esta - ERROR)
               ptr = NULL; //esta deveria ser a posicao do evento
               //no caso de insercao, "ant" fica apontando para evento que ira antecer novo evento
               //laco eh quebrado aqui pois ptr recebe valor null
               
        }
    }
}

// funcao usada para busca de eventos lista "exits" - ou para encontrar posicao de insercao
//equivalente a funcao "busca"
void busca2 ( event *events, int who,  event **ant, event **ponte) {

    event *ptr;

    *ant = events; 
	  *ponte = NULL;  
    ptr = events->next; 

    while (ptr)
    {  
        if ((ptr->client_id) != who) 
        { // ainda nao chegou no no 
            *ant = ptr;
             ptr = ptr->next;
        }
        else 
        {             
               if (ptr->client_id == who) 
               {
                  *ponte = ptr; // achou 
                  break;
               }
               ptr = NULL; // nao estava na lista - ou: posicao para insercao, "ant" aponta para evento que o antecede
               //laco eh rompido aqui
        }
    }
}
    
event * insere ( event *list, int event_id, float time, int server_id, int client_id ) 
{
    
    struct event *temp_event1, 	// ponteiro para o novo no a inserir
    	**ant,      // ponteiro para ponteiro na lista - vai apontar para evento que ja existe na lista e antecede imediatamente novo evento 
        **pont;     // ponteiro para ponteiro, auxilia busca
    
    ant = ( event **) malloc(sizeof ( event *));
    pont = ( event **) malloc(sizeof ( event *));
    temp_event1 = (event *)malloc(1*sizeof(event));
        
        
    if(event_id==CLIENT_EXIT_EVENT){
                             busca2 (list, client_id, ant, pont);}
    else{
         busca (list, time, server_id, client_id, ant, pont);} 

    temp_event1->event_id = event_id;
    temp_event1->client_id = client_id;
    temp_event1->server_id = server_id;
    temp_event1->time = time;
    temp_event1->next = (*ant)->next;
    temp_event1->prev = (*ant);
    (*ant)->next = temp_event1;
    if(temp_event1->next!=NULL){
        (temp_event1->next)->prev = temp_event1;}
        
     return    temp_event1;        
}

//remove e desaloca evento de listas - eventos de fluxo somente sao removidos da lista
int erase ( event *list, event * event_remove )
{     

     
    int event_id, client_id, server_id;
    struct event *pont, *ant;
    float time;
    
    event_id = event_remove->event_id;
    client_id = event_remove->client_id;
    server_id = event_remove->server_id;
    time = event_remove->time;

    ant = list;
    pont = list->next;
    
    //buscando evento utilizando posicao da memoria como base de comparacao
    while(pont)
    {
         if( pont == event_remove ) 
         {break;}   
         else
         {   ant=pont;
             pont=pont->next;}      
                
    }
    
    //arrumando ponteiros
    if (pont) {
        (ant)->next = (pont)->next;
        if(((pont)->next) != NULL) ((pont)->next)->prev = (pont)->prev;
        if(event_id!=FLOW_EVENT) free(pont);
     }
     else 
     {
          
           if(event_id == CLIENT_EXIT_EVENT){
           printf("DELETE_ERROR CLIENT_EXIT_EVENT | Time: %f | Client id: %d \n", time, client_id);}
           else{
           if(event_id == SERVER_EXIT_EVENT){
           printf("DELETE_ERROR SERVER_EXIT_EVENT | Time: %f | Server id: %d \n", time, server_id);} 
           if(event_id == FLOW_EVENT){
           printf("DELETE_ERROR FLOW_EVENT | Time: %f | Server id: %d | Client id: %d \n", time, server_id, client_id);}
           else if(event_id == DISCONNECTION_EVENT){
           printf("DELETE_ERROR DISCONNECTION_EVENT | Time: %f | Server id: %d | Client id: %d \n", time, server_id, client_id);}
           else if(event_id == CONNECTION_EVENT){
           printf("DELETE_ERROR CONNECTION_EVENT | Time: %f | Server id: %d | Client id: %d \n", time, server_id, client_id);
           if(event_id == SERVER_ARRIVAL_EVENT){
           printf("DELETE_ERROR SERVER_ARRIVAL_EVENT | Time: %f | Server id: %d \n", time, server_id);}
           if(event_id == CLIENT_ARRIVAL_EVENT){
           printf("DELETE_ERROR CLIENT_ARRIVAL_EVENT | Time: %f | Server id: %d \n", time, client_id);}
         }
           }
	   return 1;
     }
     return 0;
}    

void listar (struct event *ptlista) {

     
     struct event *pont;

     pont = ptlista->next;
     
     while (pont) 
     {
           if(pont->event_id == CLIENT_EXIT_EVENT){
                  printf("QUEUE_CLIENT_EXIT_EVENT          | Time: %f | Client id: %d \n", pont->time, pont->client_id);
            }
            else{
                 if(pont->event_id == CLIENT_ARRIVAL_EVENT){
                 printf("QUEUE_CLIENT_ARRIVAL_EVENT       | Time: %f | Client id: %d \n", pont->time, pont->client_id);}
                 if(pont->event_id == SERVER_ARRIVAL_EVENT){
                 printf("QUEUE_SERVER_ARRIVAL_EVENT       | Time: %f | Server id: %d \n", pont->time, pont->server_id);}
                 if(pont->event_id == SERVER_EXIT_EVENT){
                 printf("QUEUE_SERVER_EXIT_EVENT          | Time: %f | Server id: %d \n", pont->time, pont->server_id);}
                 if(pont->event_id == FLOW_EVENT){
                 printf("QUEUE_FLOW_EVENT                 | Time: %f | Client id: %d \n", pont->time, pont->client_id);}
                 else if(pont->event_id == DISCONNECTION_EVENT){
                 printf("QUEUE_DISCONNECTION_EVENT        | Time: %f | Server id: %d | Client id: %d \n", pont->time, pont->server_id, pont->client_id);}
                 else if(pont->event_id == CONNECTION_EVENT){
                 printf("QUEUE_CONNECTION_EVENT           | Time: %f | Server id: %d | Client id: %d \n", pont->time, pont->server_id, pont->client_id);}
           }
           pont = pont->next;      
     }
     //puts("saindo da impressão das listas");
     
}

void print_system_state(int t_print, int clients_number_connections[NUMBER_CLIENTS], int servers_number_connections[NUMBER_SERVERS], long double client_flow[NUMBER_CLIENTS], long double server_flow[NUMBER_SERVERS])
{

	int i, j;
	long double flow_accumulator, system_total_flow;

	system_total_flow = 0;

	for(j=0;j<NUMBER_CLIENTS;j++)
	{
		 system_total_flow += client_flow[j];
 		 printf("%d CLIENT_STATE %d %d %Lf\n", t_print, j, clients_number_connections[j], client_flow[j]);
	}        
	for(j=0;j<NUMBER_SERVERS;j++)
	{
		 //system_total_flow += client_flow[j];
 		 printf("%d SERVER_STATE %d %d %Lf\n", t_print, j, servers_number_connections[j], server_flow[j]);
	}
	printf("%d SYSTEM_STATE %Lf\n", t_print, system_total_flow );

}
