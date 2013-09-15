int process_flow_event(event *temp_event_events, event *events, client clients_vector[NUMBER_CLIENTS], int knowledge_servers[][KNOWLEDGE], int connections[][NUMBER_CLIENTS], float flow[][NUMBER_CLIENTS], int servers_state[NUMBER_SERVERS], int client_id, int *number_events )
{
     
     int event_id, i;
     double when, exp_paramenter;
     struct event *temp_event; 

    struct event **ant, **pont;
    ant = ( event **) malloc(sizeof ( event *));
    pont = ( event **) malloc(sizeof ( event *));
     
           event_id = FLOW_EVENT;

           if(DEBUG_LEVEL>3) printf("%f FLOW_EVENT %d\n", t, client_id);
                    
           //retornar nova lista de conexoes para cliente + atualizar o vetor de fluxos dele (own_connections[])
				   if(ALGORITHM == OVERHEAD )                   i = algoritmo_overhead(clients_vector,client_id, knowledge_servers, connections, flow, servers_state, event_id);
				   if(ALGORITHM == CONNLIMIT)                   i = algoritmo_connlimit(clients_vector,client_id, knowledge_servers, connections, flow, event_id);

				
                   if (i == 1) return 1;
                   if (i == 2)
                   {
                      if(DEBUG_LEVEL>3) printf( "%f NO_SERVER_AVAILABLE %d\n", t, client_id);
                      //já tem esse valor na verdade
                      clients_vector[client_id].flow_event_alloc=1;

                      //tirando da lista evento que foi processado (temp_event_events)
                      i = erase(events,temp_event_events);                 
                      if( i == 1 ) return 1;
                      (*number_events)--; 
                                
                      temp_event = clients_vector[client_id].flow_event;
                                // while(true)
                                // {          //gambiarra para que evento mudanca de fluxo nao ocorra antes de conexoes/desconexoes
                                //            pode ser usado para estabelecer limites do tempo que demora para percepcao do fluxo
                                //            ideia: estabelecer distancias entre servidores e clientes
                                //            estabelecer limites superiores e inferiores para o tempo necessario para perceber a mudanca de fluxo
                      while(true)
                      {                                //            dadas as distancias dos servidores que tiveram mudancas significativas de fluxo
                   
                             

                            exp_paramenter = (float)(1/(float)DELTA2);
                            if(clients_vector[client_id].last_connection_scheduled > t) when = clients_vector[client_id].last_connection_scheduled+gera_aleatorio(exp_paramenter);
                            if(clients_vector[client_id].last_connection_scheduled <= t) when = t+gera_aleatorio(exp_paramenter);  

                                         //if( when > clients_vector[j].last_connection_scheduled ) break;
                                         //IDEIA: cliente ira perceber mudanca de fluxo depois de um tempo pelo menos FLOW_PERCEPTION vezes maior que "o maior tempo atual para estabelecer uma conexao"
                                         //if( (when-t) >= FLOW_PERCEPTION*(clients_vector[j].last_connection_scheduled - t)  ) break;
                              //}
                              
                              if( when < t )
                              {
                                  printf("ERROR: %f ERROR_TIME_EVENT - FLOW_EVENT %d %f\n", t, client_id, when);
                                  return 1;
                               
                              }
                              busca_tempo (events, when, ant, pont);
                              if(*pont==NULL) break;
                        }              
                      recoloca(events,FLOW_EVENT,when,0,client_id,temp_event);
                      
                      if(DEBUG_LEVEL>3) printf( "%f PUSH FLOW_EVENT %d %f\n", t, client_id, when);
                      
                      (*number_events)++;     
                       
                       return 0;

                   }
                   
                   for(i=0;i<NUMBER_SERVERS;i++)
                   {
                                             //vetor connections[] possui as conexoes sob perspectiva dos servidores  - conexoes que estao de fato estabelecidas - considerando protocolo tcp, cliente sabe quando a conexao foi estabelecida
                                             //caso  own_connections[] de client_id estiver diferente apos algortimo de escolha de conexoes, enviar pedido de des/conexao para servidor
                                             if ( clients_vector[client_id].own_connections[i] != clients_vector[client_id].own_connections_old[i] )
                                             {
                                                  if(clients_vector[client_id].own_connections[i]==1)
                                                  {
                                                    while(true)
                                                    {
                                                          //criando evento de conexao para cliente no vetor de eventos
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
                                                      insere(events,CONNECTION_EVENT,when,i,client_id);
                                                      (*number_events)++;
                                                      if(DEBUG_LEVEL>3) printf("%f PUSH CONNECTION_EVENT %d %d %f\n", t, i, client_id, when);
                                                      //atualizando tempo do ultimo evento de conexao
                                                      if( when > clients_vector[client_id].last_connection_scheduled ) clients_vector[client_id].last_connection_scheduled = when;

                                                  }
                                                  else
                                                  {
                                                    while(true)
                                                    {
                                                      
                                                          //criando evento de desconexao para cliente no vetor de eventos
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
                                                      insere(events,DISCONNECTION_EVENT,when,i,client_id);
                                                      (*number_events)++;
                                                      if(DEBUG_LEVEL>3) printf("%f PUSH DISCONNECTION_EVENT %d %d %f\n", t, i, client_id, when);
                                                      //atualizando tempo do ultimo evento de conexao
                                                      if( when > clients_vector[client_id].last_connection_scheduled ) clients_vector[client_id].last_connection_scheduled = when;
                                                  }
                                             }
                                            
                                            //evento de mudanca de fluxo foi processado
                                            //setar variavel flow_event_alloc em zero, indicando que nao ha mais evento de mudanca de fluco alocado no vetor de eventos para este cliente
                                            // no caso return 2 do bitlivery, outro evento de sluxo foi alocado, então a variavel permanece em 1
                                             
                   }
                   
                   clients_vector[client_id].flow_event_alloc=0;
                   
                   //tirando da lista evento que foi processado (temp_event_events)
                   i = erase(events,temp_event_events);                 
                   if( i == 1 ) return 1;
		               (*number_events)--;    
                   
                   return 0;
     
}


