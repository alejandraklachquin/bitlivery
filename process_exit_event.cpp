int process_exit_event(event *temp_event_exits, event *exits, event *events, client clients_vector[NUMBER_CLIENTS], int connections[][NUMBER_CLIENTS], int clients_state[NUMBER_CLIENTS], int servers_state[NUMBER_SERVERS], int *servers_up, int *number_events, int event_id, int end_host, int *clients_up )
{
    int i,j, client_id, server_id;
    double when,exp_paramenter;
    struct event *temp_event, *temp_event2;
                
    struct event **ant, **pont;
    ant = ( event **) malloc(sizeof ( event *));
    pont = ( event **) malloc(sizeof ( event *));


    if(event_id==CLIENT_EXIT_EVENT)
    {

                client_id=end_host; 
                //if(DEBUG_LEVEL>0) printf("%f CLIENT_EXIT_EVENT %d\n", t, client_id);


                if(clients_state[client_id]==0) printf("ERROR: %f CLIENT_EXIT_EVENT %d\n", t, client_id);
                clients_state[client_id]=0;
                client_exit_time[client_id]=t;
                (*clients_up)--;
                if(DEBUG_LEVEL>3) printf("%f CLIENT_EXIT_EVENT %d\n", client_exit_time[client_id], client_id);


                //gerar eventos de desconexao com delta 3 para todos os servidores i conectados a client
                for(i=0;i<NUMBER_SERVERS;i++)
                {
                                          //cada cliente possui um vetor own_connections[]
                                          //se os dois valores sao iguais a 1, a conexao esta ativa e nao ha pedidos de desconexao ja alocados
                                          if((connections[i][client_id]==1) && (clients_vector[client_id].own_connections[i]==1))
                                          {
                                              //criando evento de desconexao para cliente no vetor de eventos
                                                while(true)
                                                {
                                                                    
                                                      exp_paramenter = (float)(1/(float)DELTA3);
                                                      when = t+gera_aleatorio(exp_paramenter);
                                                      if( when < t )
                                                      {
                                                         printf("ERROR: %f ERROR_TIME_EVENT - DISCONNECTION_EVENT %d %d %f\n", t, i, client_id, when);
                                                         return 1;
                                                         
                                                      }
                                                      busca_tempo (events, when, ant, pont);
                                                      if(*pont==NULL) break;
                                                 }
                                                  //if(DEBUG_LEVEL>0) printf( "\n%f 1 REMAINING EVENTS TO PROCESS %d\n", t, (*number_events));

                                                  insere(events,DISCONNECTION_EVENT,when,i,client_id);
                                                  (*number_events)++;
                                                  if(DEBUG_LEVEL>3) printf("%f PUSH DISCONNECTION_EVENT %d %d %f\n", t, i, client_id, when);
                                                  //if(DEBUG_LEVEL>0) printf( "\n%f 2 REMAINING EVENTS TO PROCESS %d\n", t, (*number_events));
                                                                                                                                
                                          }
                                          
                }

        

                //deletar evento de saida da lista de saidas
                i = erase(exits,temp_event_exits) ;
                if( i == 1 ) return 1;                
                (*number_events)--;
                //if(DEBUG_LEVEL>0) printf( "\n%f 3 REMAINING EVENTS TO PROCESS %d\n", t, (*number_events));
                //deletar evento de fluxo relativo ao cliente que saiu do sistema
                //passar endereco da estrura
                temp_event = clients_vector[client_id].flow_event;
                if(clients_vector[client_id].flow_event_alloc==1)
                {
                         //neste caso, o evento de fluxo estava alocado na lista de eventos
                         //tirar da lista de eventos, eventos de fluxo nao sao desalocados pela funcao erase
                        i = erase(events,temp_event);
                        if( i == 1 ) return 1;
                        (*number_events)--;
                        clients_vector[client_id].flow_event_alloc=0;
                        //puts("removendo evento de fluxo");
                                                                  
                }
                free(temp_event);
                
                clients_vector[client_id].flow_event=NULL;
                clients_vector[client_id].exit_event=NULL; 

                //puts("saindo de exit event");
                
                //verificar se ficou algum pedido de conexao pendente e cancelar
                temp_event = get_next(events);
                while(temp_event != NULL)
                {
                                 event_id = get_event_id(temp_event);
                                 i = get_client_id(temp_event);
                                 
                                 if( (event_id == CONNECTION_EVENT) && ( i == client_id) )
                                 {
                                     temp_event2 = temp_event;
                                     temp_event = get_next(temp_event);
                                     
                                     i = erase(events,temp_event2);
                                     if( i == 1 ) return 1;
                                     (*number_events)--;
                                     //puts("apagando evento de conexao");
                                 }
                                 else{
                                     temp_event = get_next(temp_event);}
                
                }
                                

        } 
        else if(event_id==SERVER_EXIT_EVENT)
        {

                server_id=end_host;
                if(servers_state[server_id]==0) printf("ERROR: %f SERVER_EXIT_EVENT %d\n", t, server_id);


                //if(DEBUG_LEVEL>0) printf("%f SERVER_EXIT_EVENT %d\n", t, server_id);
                (*servers_up)--;
                servers_state[server_id]=0;
                server_exit_time[server_id]=t;
                if(DEBUG_LEVEL>0) printf("%f SERVER_EXIT_EVENT %d\n", server_exit_time[server_id], server_id);



                //gerar eventos de desconexao com delta 3 para todos os servidores i conectados a client
                for(j=0;j<NUMBER_CLIENTS;j++)
                {
                                          //cada cliente possui um vetor own_connections[]
                                          //se os dois valores sao iguais a 1, a conexao esta ativa e nao ha pedidos de desconexao ja alocados
                                          if((connections[server_id][j]==1) && (clients_vector[j].own_connections[server_id]==1))
                                          {
                                              //criando evento de desconexao para cliente no vetor de eventos
                                                while(true)
                                                {  
                                                    exp_paramenter = (float)(1/(float)DELTA3);
                                                    when = t+gera_aleatorio(exp_paramenter);
                                                    if( when < t )
                                                    {
                                                       printf("ERROR: %f ERROR_TIME_EVENT - DISCONNECTION_EVENT %d %d %f\n", t, server_id, j, when);
                                                       return 1;
                                                       
                                                    }
                                                    busca_tempo (events, when, ant, pont);
                                                    if(*pont==NULL) break;
                                                }

                                              clients_vector[j].own_connections[server_id]=0;
                                              insere(events,DISCONNECTION_EVENT,when,server_id,j);
                                              (*number_events)++;
                                              if(DEBUG_LEVEL>0) printf("%f PUSH DISCONNECTION_EVENT %d %d %f\n", t, server_id, j, when);
                                              //atualizando tempo do ultimo evento de conexao
                                              if( when > clients_vector[j].last_connection_scheduled ) clients_vector[j].last_connection_scheduled = when;
                                                                              
                                          }
                                          
                }

                //deletar evento de saida da lista de saidas
                i = erase(events,temp_event_exits) ;
                if( i == 1 ) return 1;                
                (*number_events)--;

                //verificar se ficou algum pedido de conexao pendente e cancelar
                temp_event = get_next(events);
                while(temp_event != NULL)
                {
                                 event_id = get_event_id(temp_event);
                                 i = get_server_id(temp_event);
                                 //printf("%f PUSH DISCONNECTION_EVENT %d %d %f\n", t, server_id, j, when);

                                 if( (event_id == CONNECTION_EVENT) && ( i == server_id) )
                                 {
                                     temp_event2 = temp_event;
                                     temp_event = get_next(temp_event);
                                     
                                     i = erase(events,temp_event2);
                                     if( i == 1 ) return 1;
                                     (*number_events)--;
                                     
                                 }
                                 else{
                                     temp_event = get_next(temp_event);}
                
                }


        }
       return 0;   
    
}
