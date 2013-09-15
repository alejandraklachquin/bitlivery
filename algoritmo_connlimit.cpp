int algoritmo_connlimit(client clients_vector[NUMBER_CLIENTS], int client_id, int knowledge_servers[][KNOWLEDGE], int connections[][NUMBER_CLIENTS], long double flow[][NUMBER_CLIENTS], int event_id)
{
     int min_flow_id;
     int i;
     int number;
     float min_flow, total_band_used;
     float cost, max_cost;
     int number_connections, number_connections_plus;
     number_connections = 0;
     
     min_flow_id = 0;
     min_flow = Clients_cap[client_id];
     total_band_used = 0;
     
     if(CONNECTION_LIMIT == 1) //tirando no caso de chegada, somente 1 servidor Ã© removido e outro eh posto no lugar - numero de servidores conectados eh mantido
     {
          if(event_id == CLIENT_ARRIVAL_EVENT)
          {
              //escolher aleatoriamente SET_SIZE servidores e setar own_connections[]=1 para servidores escolhidos
              for(i=0; i < INITIAL_SET_SIZE; i++)        
              {
	   				   i = rand() % KNOWLEDGE;
		 			   number = knowledge_servers[client_id][i];
                       if(clients_vector[client_id].own_connections[number] == 0){
                            clients_vector[client_id].own_connections[number]=1;
                            if(DEBUG_LEVEL>3) printf("%f SERVER_CHOSEN %d %d\n", t, number, client_id);                            
                       }
                       else{
                            i--;
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
                                                                      
                            number_connections++; //numero de conexoes estabelecidas + as que irao se concretizar (pedido de conexao ja enviado)
                      }
                      
                      if( (connections[i][client_id] == 1)  ) 
                      {
                          //procurando menor fluxo recebido
                          //utiliza connections pq o cliente sabe se a conexao ja foi estabelecida ou nao - TCP 3wayhandshake 
                          //caso contrario, o cliente consideraria servidores cuja conexao ainda nÃ£o foi estabelecida, e por isso ainda possuem fluxo zero
                          
                          if(flow[i][client_id]<min_flow)
                          {
                              min_flow = flow[i][client_id];
                              min_flow_id = i;
                          }
                      }
               }
               
               //para clientes que nao estao com a banda saturada:
	       if( clients_vector[client_id].band_filled == 0 )
               {
                   max_cost = Clients_cap[client_id]*(((float)(MAX_BAND_COST))/100) ;
                   number_connections_plus = number_connections + 1;
                   cost = number_connections_plus*Servers_connection_cost[client_id] ;
                   if( (number_connections < (SET_SIZE && KNOWLEDGE)) && ( cost < max_cost))
                   {        
                    
                        //o cliente nao esta com a banda totalmente preenchida, tem direito a pelo menos mais uma conexao, e nao ira saturar a banda somente com o custo das conexoes
                        //number_connections eh o numero de conexoes estabelecidas mais os servidores que possuem um pedido de coxexao na lista de evento a partir daquele cliente
                        //o cliente pode estabelecer ate SET_SIZE conexoes. Realizar mais uma conexao para tentar garantir uma banda saturada (se vericarmos que nao eram necessarias tanta conexoes, podemos descartar o pior servidor posteriomente) 
                        while(true)
                        {
                             //escolher mais um servidor aleatoriamente 
	             			i = rand() % KNOWLEDGE;
				 			number = knowledge_servers[client_id][i];
				            if( connections[number][client_id] == 0)
                             {
                                 clients_vector[client_id].own_connections[number] = 1;
                                 if(DEBUG_LEVEL>3) printf("%f SERVER_CHOSEN %d %d\n", t, number, client_id);  
                                 break;                             
                             }
                        }
               			
           	       }
               	   else
               	   {
               			//nao esta com banda saturada, esta com set_size conexoes ou vai saturar a banda so com conexoes
               			//algoritmo simples: troca servidor que envia menor fluxo por outro qualquer (lembrando que o servidor de menor fluxo ja possui uma conexao estabelecida, ou seja, jÃ¡ foi execurado algortimo de alocacao daquele servidor para aquele cliente)
                    	while(true)
                   	{
             				i = rand() % KNOWLEDGE;
			 				number = knowledge_servers[client_id][i];
                            if( connections[number][client_id] == 0)
                             {
                                 clients_vector[client_id].own_connections[number] = 1;
                                 clients_vector[client_id].own_connections[min_flow_id] = 0;
                                 if(DEBUG_LEVEL>3) printf("%f SERVER_DROPPED %d %d\n", t, min_flow_id, client_id);
                                 if(DEBUG_LEVEL>3) printf("%f SERVER_CHOSEN %d %d\n", t, number, client_id);  
                                 break;                             
                             }
                   	}	
	               			
	            }
               	}    
               else{
                    //banda do cliente esta totalmente preenchida
                    if( (DROP_SERVERS == 1) && (number_connections>1) )
                    {   
                        //jogar fora servidor de menor fluxo caso cliente esteja conectado a mais de um servidor, para verificar se eh possivel manter a banda saturada com menos conexoes (conexoes custam banda)
                        clients_vector[client_id].own_connections[min_flow_id] = 0;
                        if(DEBUG_LEVEL>3) printf("%f SERVER_DROPPED %d %d\n", t, min_flow_id, client_id);
                        
                    }
                    else{
                         //manter todos os servidores (ou pq so temos um servidor conectado ou pq nao vamos utilizar a politica de descarte)
                         if(DEBUG_LEVEL>3) printf("%f ALL_SERVERS_KEEPED %d\n", t, client_id);}
                    
                    }
          }//end else - caso !ARRIVAL_EVENT
                    
     }//end if  CONNECTION_LIMIT==1
    
    return 0;
}
