
int process_arrival_event(event *temp_event_events, event *events, client clients_vector[NUMBER_CLIENTS], int knowledge_servers[][KNOWLEDGE], int connections[][NUMBER_CLIENTS], float flow[][NUMBER_CLIENTS], int clients_state[NUMBER_CLIENTS], int servers_state[NUMBER_SERVERS], int *servers_up, int *number_events, int event_id, int end_host, int *clients_up)
{
    int client_id, server_id, i;
    float when,exp_paramenter;
    struct event *temp_event; 

    struct event **ant, **pont;
    ant = ( event **) malloc(sizeof ( event *));
    pont = ( event **) malloc(sizeof ( event *));
               

    if(event_id==SERVER_ARRIVAL_EVENT)
    {
          server_id = end_host;
          //if(DEBUG_LEVEL>6) printf("%f SERVER_ARRIVAL_EVENT %d\n", t, server_id);
          servers_state[server_id]=1;
          (*servers_up)++;
          server_arrive_time[server_id]=t;
          if(DEBUG_LEVEL>1) printf("%f SERVER_ARRIVAL_EVENT %d\n", server_arrive_time[server_id], server_id);

        
    }
    else if(event_id==CLIENT_ARRIVAL_EVENT)
    {
          client_id = end_host;
          clients_state[client_id]=1;
          (*clients_up)++;
          
          client_arrive_time[client_id]=t;

               //if(DEBUG_LEVEL>0) printf("%f CLIENT_ARRIVAL_EVENT %d\n", t, client_id);
               if(DEBUG_LEVEL>1) printf("%f CLIENT_ARRIVAL_EVENT %d\n", client_arrive_time[client_id], client_id);
                   
               //escolher a quais servidores conectar 
			   if(ALGORITHM == OVERHEAD )                   i = algoritmo_overhead(clients_vector,client_id, knowledge_servers, connections, flow, servers_state, event_id);
			   if(ALGORITHM == CONNLIMIT)                   i = algoritmo_connlimit(clients_vector,client_id, knowledge_servers, connections, flow, event_id);
               if( i == 1 ) return 1;
               
               for(i=0;i<NUMBER_SERVERS;i++)
               {                            
                                         //enviar eventos de conexao para servidores escolhidos
                                         //isto eh, posicoes que possuem valor 1 no vetor own_connections[] do cliente que entrou (client_id)
                                         if ( clients_vector[client_id].own_connections[i] == 1 )
                                         {
                                              while(true)
                                              {
                                                    exp_paramenter = (float)(1/(float)DELTA3);
                                                    when = t+gera_aleatorio(exp_paramenter);
                                                    if( when < t )
                                                    {
                                                       printf("ERROR: %f ERROR_TIME_EVENT - CONNECTION_EVENT %d %d %f\n", t, i, client_id, when);
                                                       return 1;
                                                       
                                                    }
                                                   busca_tempo (events, when, ant, pont);
                                                   if(*pont==NULL) break;
                                              }
                                              //criando evento de conexao para cliente no vetor de eventos

                                              insere(events,CONNECTION_EVENT,when,i,client_id);
                                              (*number_events)++;
                                              if(DEBUG_LEVEL>3) printf("%f PUSH CONNECTION_EVENT %d %d %f\n", t, i, client_id, when);
                                              //atualizando tempo do ultimo evento de conexao
                                              if( when > clients_vector[client_id].last_connection_scheduled ) clients_vector[client_id].last_connection_scheduled = when;
                                              
                                         }
               }    
    
    }

               //tirando da lista evento que foi processado (temp_event_events)
    i = erase(events,temp_event_events); 
    if( i == 1) return 1;                
    (*number_events)--;


               
               return 0;
}

