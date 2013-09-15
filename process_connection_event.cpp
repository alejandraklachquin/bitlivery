int process_connection_event(event *temp_event_events, event *events, event *exits, client clients_vector[NUMBER_CLIENTS], int connections[][NUMBER_CLIENTS], int servers_number_connections[NUMBER_SERVERS], int clients_number_connections[NUMBER_CLIENTS], float flow[][NUMBER_CLIENTS], long double client_flow[NUMBER_CLIENTS], long double server_flow[NUMBER_SERVERS], long double data[][NUMBER_CLIENTS], int clients_state[NUMBER_CLIENTS], int servers_state[NUMBER_SERVERS], int event_id, int server_id, int client_id, int *number_events, int clients_up )
{
     int i,j, k;
     int change_flow;
     long double flow_accumulator, data_accumulator, data_accumulator_old, cost_accumulator;
     float when, epsilon, exp_paramenter;
     struct event *temp_event; 
     struct event **ant, **pont;


    ant = ( event **) malloc(sizeof ( event *));
    pont = ( event **) malloc(sizeof ( event *));

                    if (DEBUG_LEVEL > 3) 
                    {
                         if(event_id == CONNECTION_EVENT){
                                      printf("%f CONNECTION_EVENT %d %d\n", t, server_id, client_id);}
                         if(event_id == DISCONNECTION_EVENT){
                                      printf("%f DISCONNECTION_EVENT %d %d\n", t, server_id, client_id);}
                    }                    
                    
                    //atualiza dados recebidos de todos os clientes usando fluxo atual, t e t-ultima-att
                    for (j=0;j<NUMBER_CLIENTS;j++)
                    {
                        data_accumulator = 0; //iterador para total de dados recebidos - de todos os servidores
			            data_accumulator_old = 0;
                        for(i=0;i<NUMBER_SERVERS;i++)
                        {
						     data_accumulator_old += data[i][j];	
                                                     
						     data[i][j] += (t- t_last)*flow[i][j];

                             data_accumulator += data[i][j];

                             if( (data[i][j] < 0) || ((data_accumulator >  FILE_SIZE) && (clients_vector[j].exit_event != NULL)) || ((data_accumulator_old >  FILE_SIZE) && (clients_vector[j].exit_event != NULL)) )
                             {
                                printf("ERROR: %f ERROR_DATA_AMOUNT - %d %d %Lf %Lf %Lf \n", t, i, j, data[i][j], data_accumulator_old, data_accumulator);
	                            printf( "\n%f REMAINING EVENTS TO PROCESS %d\n", t, (*number_events));

    							listar(exits); listar(events); printf("\n");

    							printf( "CLIENT %d DATA_VECTOR:\n", j);
    							for(k=0; k<NUMBER_SERVERS; k++)
    							{
    								printf( "DATA_AMOUNT_FROM_SERVER %d %Lf\n", k, data[k][j]);
    							}
    							printf("\nCLIENT %d FLOW_VECTOR:\n", j);
    							for(k=0; k<NUMBER_SERVERS; k++)
    							{
    								printf( "FLOW_AMOUNT_FROM_SERVER %d %f\n", k, flow[k][j]);
    							}
                                 //return 1;
                            }                          
                        }
                           
                    }
                    //atualiza t_last, marca quando foi a ultima atualizacao nos fluxos dos servidores
                    t_last = t;
                    
                    //atualizando vetor de conexoes dos servidores
        	   if(event_id == CONNECTION_EVENT){
                                if(servers_state[server_id] == 0)
                                {
                                            printf("ERROR: %f ERROR_CONNECTION_ATTEMPT - SERVER_DOWN %d %d\n", t, server_id, client_id);
                                            return 1; 

                                }
                                if (connections[server_id][client_id] != 0)
                                {
                                            printf("ERROR: %f ERROR_CONNECTION_ATTEMPT - ALREADY_CONNECTED %d %d\n", t, server_id, client_id);
                                            return 1;                                       
                                }
                                connections[server_id][client_id] = 1;
                                servers_number_connections[server_id]++;
                                clients_number_connections[client_id]++;
                }
        	    else{	
                                if (connections[server_id][client_id] != 1)
                                {
                                            printf("ERROR: %f ERROR_DISCONNECTION_ATTEMPT - NOT_CONNECTED %d %d\n", t, server_id, client_id);
                                            return 1;                                       
                                }
                                connections[server_id][client_id] = 0;
                                servers_number_connections[server_id]--;
                                clients_number_connections[client_id]--;
                               
                }

                 if(DEBUG_LEVEL>1) printf("%f SERVER_NUMBER_CONNECTIONS %d %d\n", t, server_id, servers_number_connections[server_id]);
                 if(DEBUG_LEVEL>1) printf("%f CLIENT_NUMBER_CONNECTIONS %d %d\n", t, client_id, clients_number_connections[client_id]);
                 
                //roda algoritmo de alocacao de banda para determinar novo flow
                //ja retorna com flow atualizado
                i = alocacao(clients_vector, clients_state, servers_state, connections, flow, client_flow, server_flow, clients_number_connections, servers_number_connections, clients_up);
                if( i == 1) return 1;
   
                    //reescalona eventos de saida e fluxo para os clientes usando matriz de dados e fluxo atual
                    for (j=0;j<NUMBER_CLIENTS;j++)
                    {
                        //alguns clientes ja podem ter saido, mas o servidor ainda nao recebeu a notificacao
                        //para esses clientes, nao eh necessario reescalonar evento de saida/fluxo, pois eles nao estao mais no sistema
                        //clientes que ja sairam do sistema possuem respectivos exit_event e flow_event com valor NULL (estrutara desses eventos foi desalocada)
                        if(clients_vector[j].exit_event!=NULL)
                        { 
                                                                
                            flow_accumulator = 0; //iterador para fluxo total recebido por um cliente - de todos os servidores
                            data_accumulator = 0; //iterador para total de dados recebidos - de todos os servidores
                            cost_accumulator = 0; //iterador para total do custo das conexoes
                            change_flow = 0;
                            for(i=0;i<NUMBER_SERVERS;i++)
                            {
                                                         flow_accumulator += flow[i][j];
                                                         data_accumulator += data[i][j];
                                                         if(connections[i][j] == 1) cost_accumulator += Servers_connection_cost[j];
                                                         //se o fluxo alocado pelo servidor tiver mudado
                                                         //isto eh, flow[][] (fluxo efetivo) passou a ter um valor diferente de own_flow[] (fluxo enxergado pelos clientes)
                                                         //verificar se diferenca Ã© perceptivel, caso seja, escalonar evento de mudanca de fluxo para respectivo cliente
                                                         if( flow[i][j] != clients_vector[j].own_flow[i] )
                                                         {
                                                             epsilon = flow[i][j]-clients_vector[j].own_flow[i];
                                                             if(epsilon < 0){
                                                                      epsilon = -epsilon;}                                                         
                                                             if( epsilon > (((float)VARIACAO)/100)*(clients_vector[j].own_flow[i]) ){
                                                                       change_flow = 1;}
                                                         }                       
                            }
                            
                            if( cost_accumulator < 0 )
                            {
                                printf("ERROR: %f ERROR_FLOW_ALLOCATION - NEGATIVE_COST %d %Lf\n", t, j, cost_accumulator);
                                return 1;
                            }
                            if( (flow_accumulator < 0) || (flow_accumulator > (Clients_cap[j]-cost_accumulator)) )
                            {
                                printf("ERROR: %f ERROR_TOTAL_FLOW_ALLOCATION %d %Lf %Lf\n", t, j, flow_accumulator, (Clients_cap[j]-cost_accumulator));
                                return 1;
                            }
                            if( (data_accumulator < 0) || (data_accumulator > FILE_SIZE) )
                            {
                                printf("ERROR: %f ERROR_DATA_AMOUNT %d %Lf\n", t, j, data_accumulator);
				
                				printf( "\n%f REMAINING EVENTS TO PROCESS %d\n", t, (*number_events));

                				listar(exits); listar(events); printf("\n");

                				printf( "CLIENT %d DATA_VECTOR:\n", j);
                				for(k=0; k<NUMBER_SERVERS; k++)
                				{
                					printf( "DATA_AMOUNT_FROM_SERVER %d %Lf\n", k, data[k][j]);
                				}
                				printf("\nCLIENT %d FLOW_VECTOR:\n", j);
                				for(k=0; k<NUMBER_SERVERS; k++)
                				{
                					printf( "FLOW_AMOUNT_FROM_SERVER %d %f\n", k, flow[k][j]);
                				}           
                                                //return 1;
                            }
                            
                            
                            if(DEBUG_LEVEL>3){
                                               
                                               if(clients_vector[j].band_filled) printf("%f BAND_FILLED %d %Lf\n", t, j, flow_accumulator);
                                               if(!clients_vector[j].band_filled) printf("%f TOTAL_FLOW %d %Lf\n", t, j, flow_accumulator);
                                               printf("%f DATA_AMOUNT %d %Lf\n", t, j, data_accumulator);
                            }
                            
                            //REESCALONAR EVENTO DE SAIDA BASEADO NO FLUXO ATUAL E DADOS JA RECEBIDOS
                            //tempo estimado para fim do download
                            //when = ((FILE_SIZE-data_accumulator)/flow_accumulator)+t;
                            if( flow_accumulator != 0)
                            {
                                when = ((((long double)FILE_SIZE)-data_accumulator)/flow_accumulator)+t;
                                //printf("%f ESTIMATED_TIME_EXIT %d %Lf\n", t, j, when);

				//if( ( ((when - t)*flow_accumulator)+data_accumulator ) > FILE_SIZE ) 
				//{ printf("ERROR: %f ERROR_TIME_EVENT - EXIT_EVENT - TRUNKED %d exit: %f data_accumulator: %Lf flow_accumulator: %Lf \n", t, j, when, data_accumulator, flow_accumulator); return 1;}
                            }
                            else {
                                 when = INF;}
                                 
                            if( when < t )
                            {
                                 printf("ERROR: %f ERROR_TIME_EVENT - EXIT_EVENT %d %f\n", t, j, when);
                                 return 1;
                                 
                            }
                            
                            temp_event = clients_vector[j].exit_event;
                            if(get_time(temp_event) != when)
                            {
                                //removendo antigo evento de saida                        
                                remove_event(temp_event);
                                //reinserindo
                                recoloca(exits,CLIENT_EXIT_EVENT,when,0,j,temp_event);
                                
                                if(DEBUG_LEVEL>3) printf("%f REPUSH CLIENT_EXIT_EVENT %d %f\n", t, j, when);
                                                      
                            }
                            
                            //reescalona eventos de mudanca de fluxo no tempo t + 2 para clientes que tiveram alguma mudanca significativa e ainda nao possuem evento mudanca de fluxo escalonado
                            if(change_flow && (clients_vector[j].flow_event_alloc==0))
                            {
                                
                                clients_vector[j].flow_event_alloc=1;
                                
                                temp_event = clients_vector[j].flow_event;
                                // while(true)
                                // {          //gambiarra para que evento mudanca de fluxo nao ocorra antes de conexoes/desconexoes
                                //            pode ser usado para estabelecer limites do tempo que demora para percepcao do fluxo
                                //            ideia: estabelecer distancias entre servidores e clientes
                                //            estabelecer limites superiores e inferiores para o tempo necessario para perceber a mudanca de fluxo
                                while(true)
                                {                                //            dadas as distancias dos servidores que tiveram mudancas significativas de fluxo
                					   
                                       

                                       exp_paramenter = (float)(1/(float)DELTA2);
                					   if(clients_vector[j].last_connection_scheduled > t) when = clients_vector[j].last_connection_scheduled+gera_aleatorio(exp_paramenter);
                					   if(clients_vector[j].last_connection_scheduled <= t) when = t+gera_aleatorio(exp_paramenter); 	

                                                   //if( when > clients_vector[j].last_connection_scheduled ) break;
                                                   //IDEIA: cliente ira perceber mudanca de fluxo depois de um tempo pelo menos FLOW_PERCEPTION vezes maior que "o maior tempo atual para estabelecer uma conexao"
                                                   //if( (when-t) >= FLOW_PERCEPTION*(clients_vector[j].last_connection_scheduled - t)  ) break;
                                        //}
                                        
                                        if( when < t )
                                        {
                                            printf("ERROR: %f ERROR_TIME_EVENT - FLOW_EVENT %d %f\n", t, j, when);
                                            return 1;
                                         
                                        }
                                        busca_tempo (events, when, ant, pont);
                                        if(*pont==NULL) break;
                                  }              
                                recoloca(events,FLOW_EVENT,when,0,j,temp_event);
                                
                                if(DEBUG_LEVEL>3) printf( "%f PUSH FLOW_EVENT %d %f\n", t, j, when);
                                
                                (*number_events)++;
                            
                            }
                        }//end if client exit != null  
                                
                    }
                    
                    //tirando da lista evento que foi processado (temp_event_events)
                    i = erase(events,temp_event_events);                 
                    if( i == 1 ) return 1;
		            (*number_events)--;
                    
                    return 0;     
}

