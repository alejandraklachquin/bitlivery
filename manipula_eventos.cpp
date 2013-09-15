struct event * malloc_flow(int client_id)
{
           struct event *temp_event1; 	// ponteiro para o novo no a inserir
    

    temp_event1 = (event *)malloc(1*sizeof(event));
        
    temp_event1->event_id = FLOW_EVENT;
    temp_event1->client_id = client_id;
    temp_event1->server_id = 0;
    temp_event1->time = INF;
    temp_event1->next = NULL;
    temp_event1->prev = NULL;
    
    //retorna endereco do evento de fluxo alocado    
    return    temp_event1;       
       
       
}

struct event * get_next(event *list)
{
       return list->next;
}

float get_time(event *temp)
{
      if(temp == NULL) return INF;
      return temp->time;
}
                        
int get_client_id(event *temp)
{
      
      return temp->client_id;
}

int get_server_id(event *temp)
{
      
      return temp->server_id;
}

int get_event_id(event *temp)
{
      
      return temp->event_id;
}
