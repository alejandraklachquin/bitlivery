int algoritmo_overhead(client clients_vector[NUMBER_CLIENTS], int client_id, int knowledge_servers[][KNOWLEDGE], int connections[][NUMBER_CLIENTS], float flow[][NUMBER_CLIENTS], int servers_state[NUMBER_SERVERS], int event_id)
{
     int min_flow_id, max_flow_id;
     int i;
     int number;
     int connections_attempt;
     float min_flow, total_band_used, max_flow;
     float cost, max_cost;
     int number_connections, number_active_connections, number_connections_plus, servers_dropped;
     int number_servers_online;

     connections_attempt = 0;
     number_connections = 0;
     number_active_connections = 0;
	   servers_dropped = 0;
     number_servers_online=0;
     
     max_flow_id = 0;
     max_flow = 0;

     min_flow_id = 0;
     min_flow = Clients_cap[client_id];
     total_band_used = 0;
     
          if(DEBUG_LEVEL>3) printf("%f ALGORITMO OVERHEAD - INCIO %d\n", t, client_id);  


          for(i=0 ; i< KNOWLEDGE ; i++)
          {
              number = knowledge_servers[client_id][i];
              if(servers_state[number]==1)
              {
                    number_servers_online++;
              }

          }


          if(event_id == CLIENT_ARRIVAL_EVENT)
          {
              //escolher aleatoriamente 1 servidore e setar own_connections[]=1 para servidor escolhido
              while(true)
              {
                    i = rand() % KNOWLEDGE;
                    number = knowledge_servers[client_id][i];
                    if( (connections[number][client_id] == 0 ) && (clients_vector[client_id].own_connections[number] == 0) && (servers_state[number]==1))
                     {
                         clients_vector[client_id].own_connections[number] = 1;
                         if(DEBUG_LEVEL>3) printf("%f SERVER_CHOSEN %d %d\n", t, number, client_id);  
                         break;                             
                     }
                     else {

                      connections_attempt++;
                     } 
                     if((connections_attempt == MAX_CONNECTIONS_ATTEMPT) || (number_servers_online==0) )
                     {
                        if(DEBUG_LEVEL>3) printf("%f ALGORITMO OVERHEAD - FIM %d\n", t, client_id);  
                        return 2;

                     }           
                }                        
                                     
          }
          else
          {               

              
               //atualizar vetor de fluxos 
               for(i=0; i<NUMBER_SERVERS; i++)
               {
                      //guardando como era o vetor de conexoes (nem todas as conexoes estao estabelecidas pois o pedido de conexao pode nao ter sido processado ainda)
                      clients_vector[client_id].own_connections_old[i] = clients_vector[client_id].own_connections[i];  
                      clients_vector[client_id].own_flow[i]=flow[i][client_id];  
                      
                      if(clients_vector[client_id].own_connections[i] == 1){
                                                                      
                            number_connections++; //numero de conexoes estabelecidas + as que irao se concretizar (pedido de conexao ja enviado) - as que serao desfeitas
                      }
                      
                      if( (connections[i][client_id] == 1)  ) 
                      {
						              number_active_connections++;
                          //procurando menor fluxo recebido
                          //utiliza connections pq o cliente sabe se a conexao ja foi estabelecida ou nao - TCP 3wayhandshake 
                          //caso contrario, o cliente consideraria servidores cuja conexao ainda nÃ£o foi estabelecida, e por isso ainda possuem fluxo zero
                          if(flow[i][client_id]<min_flow)
                          {
								              min_flow = flow[i][client_id];
                            	min_flow_id = i;
                          }
                          if(flow[i][client_id]>max_flow)
                          {
                              max_flow = flow[i][client_id];
                              max_flow_id = i;
                          }
                          if( flow[i][client_id] < Servers_connection_cost[client_id] )
                          {
            								//fluxo recebido pelo cliente eh menor que o overhead gasto com a conexao
            								//servidor recebera pedido de desconexao
            								clients_vector[client_id].own_connections[i] = 0;
            								servers_dropped++;
            								if(DEBUG_LEVEL>3) printf("%f SERVER_DROPPED %d %d\n", t, i, client_id);
                            
                          }
                      }
               }
               
           //para clientes que nao estao com a banda saturada:
	         if( clients_vector[client_id].band_filled == 0 )
           {
          				if ((number_connections < number_servers_online) && (number_connections==number_active_connections))
          				{
          					   while(true)
                    	{
	                         //escolher um servidor aleatoriamente 
                            i = rand() % KNOWLEDGE;
                            number = knowledge_servers[client_id][i];
                            if( (connections[number][client_id] == 0 ) && (clients_vector[client_id].own_connections[number] == 0) && (servers_state[number]==1))
                             {
                                 clients_vector[client_id].own_connections[number] = 1;
                                 if(DEBUG_LEVEL>3) printf("%f SERVER_CHOSEN %d %d\n", t, number, client_id);  
                                 break;                             
                             } 
                             else{

                                connections_attempt++;
                             } 
                             if( connections_attempt == MAX_CONNECTIONS_ATTEMPT )
                             {
                                if(DEBUG_LEVEL>3) printf("%f ALGORITMO OVERHEAD - FIM %d\n", t, client_id);  
                                return 2;

                             }            
                   			}
          				}
                  if(servers_dropped == number_servers_online)
                  { 
                      clients_vector[client_id].own_connections[max_flow_id] = 1;
                  }  
            }
            else{
                  //banda do cliente esta totalmente preenchida
                  if( (servers_dropped == 0) && (number_active_connections>1) )
                  {   
                      //jogar fora servidor de menor fluxo
                      clients_vector[client_id].own_connections[min_flow_id] = 0;
                      if(DEBUG_LEVEL>3) printf("%f SERVER_DROPPED %d %d\n", t, min_flow_id, client_id);
                      
                  }     
            }     
      }//end else - caso !ARRIVAL_EVENT
                         if(DEBUG_LEVEL>3) printf("%f ALGORITMO OVERHEAD - FIM %d\n", t, client_id);  


    
    return 0;
}
