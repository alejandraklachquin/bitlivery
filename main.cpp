int main(int argc, char *argv[]) {
    
        
    int i, j, k, teste;
    int number_events, number, servers_up, clients_up;
    int event_id, client_id, server_id;
    double when, exp_paramenter;

    struct event **ant, **pont;
    
    struct client clients_vector[NUMBER_CLIENTS]; //vetor de instancias de clientes
    struct event *exits; //lista de saidas, duplamente enc. ordenada no tempo
    struct event *events; //lista de eventos: des/conexao, fluxo, duplamente enc. ordenada no tempo
    struct event *temp_event_exits, *temp_event_events; //ponteiro para primeiro evento a ser processados de cada lista
    
    int clients_state[NUMBER_CLIENTS];
    int servers_state[NUMBER_SERVERS];
	int knowledge_temp[NUMBER_SERVERS];
    int servers_togo_down[NUMBER_SERVERS];
	int knowledge_servers[NUMBER_CLIENTS][KNOWLEDGE];
    int connections[NUMBER_SERVERS][NUMBER_CLIENTS]; //1 se servidor i estiver contado a cliente j
    int servers_number_connections[NUMBER_SERVERS]; //numero de conexoes estabelecidas por cada servidor
    int clients_number_connections[NUMBER_CLIENTS]; //numero de conexoes estabelecidas por cada cliente
    
    char randstate[256];
        
    float flow[NUMBER_SERVERS][NUMBER_CLIENTS]; //(X) fluxo atual do servidor i para cliente j
    long double data[NUMBER_SERVERS][NUMBER_CLIENTS]; //(B) total de dados transmitidos ate o tempo t. do servidor i para cliente j
    long double clients_flow[NUMBER_CLIENTS]; //fluxo real total alocado 
    long double servers_flow[NUMBER_SERVERS]; 
    

    long double ic_clients_flow[NUMBER_CLIENTS];  
    long double dp_clients_flow[NUMBER_CLIENTS]; 
    long double ic_servers_flow[NUMBER_SERVERS];
    long double dp_servers_flow[NUMBER_SERVERS];
    long double ic_system_flow;  
    long double dp_system_flow;
     
    long double  ic_servers_number_connections[NUMBER_SERVERS];
    long double  ic_clients_number_connections[NUMBER_CLIENTS]; 
    long double  dp_servers_number_connections[NUMBER_SERVERS];
    long double  dp_clients_number_connections[NUMBER_CLIENTS]; 
        
    if(initstate(time(NULL), randstate, 256)==NULL) return 1;
    srandom( (int)time(NULL) );

    
    printf("SETTINGS NUMBER_SERVERS %d \nSETTINGS NUMBER_CLIENTS %d \nSETTINGS KNOWLEDGE %d\nSETTINGS CONNECTION_COST %d\nSETTINGS CAP_SERVER %d\nSETTINGS CAP_CLIENT %d\nSETTINGS ALGORITHM OVERHEAD\nSETTINGS DELTA1 %d\nSETTINGS DELTA2 %d\nSETTINGS DELTA3 %d\n", NUMBER_SERVERS, NUMBER_CLIENTS, KNOWLEDGE, CONNECTION_COST,CAP_SERVER_1,CAP_CLIENT_1, DELTA1, DELTA2, DELTA3 );
    flag_estatisticas=0;
    
    exits = NULL;
    events = NULL;

    ant = ( event **) malloc(sizeof ( event *));
    pont = ( event **) malloc(sizeof ( event *));
    
    //Criando no cabeca para lista de eventos 'events' que contem eventos de fluxo, conexao e desconexao
    if (( events = ( event *) malloc(sizeof( event )) ) == NULL )
    {
        printf("ERROR: Nao foi possivel alocar memoria para events\n");
        return 1;
    }
    events->event_id = -1;
    events->time = 0;
    events->next = NULL;
    events->prev = NULL;
    
    //Criando no cabeca para lista de eventos 'exits' que contem eventos de saida
    if (( exits = ( event *) malloc(sizeof( event)) ) == NULL )
    {
        printf("ERROR: Nao foi possivel alocar memoria para exits\n");
        return 1;
    }
    exits->event_id = -1;
    exits->time = 0;
    exits->next = NULL;
    exits->prev = NULL;    
    

   //Gerando capacidades
   if(RANDOM_CAP) {
	set_servers_capacite_random();     
  	set_clients_capacite_random();}
   else{
	set_servers_capacite();     
  	set_clients_capacite();}
      
    
    for(i=0; i<NUMBER_SERVERS ; i++)
    {       
        servers_number_connections[i]=0;
        servers_state[i]=1;
        servers_flow[i]=0;
		knowledge_temp[i]=NUMBER_CLIENTS;  
        global_nsamples_s[i]=0; 
        servers_togo_down[i]=0;
        for(j=0; j<NUMBER_CLIENTS; j++)
        {
           if(i==0) //para realizar esta operacao apenas uma vez
           {    
    		    //setando custo de conexao
    	        Servers_connection_cost[j] = (((float)(CONNECTION_COST))/100)*Clients_cap[j];
            	//if(DEBUG_LEVEL>1) printf("SETTINGS Client %d connection cost: %f\n", j,Servers_connection_cost[j] );

                //setando valores iniciais dos clientes
                clients_vector[j].client_id=j;
                clients_number_connections[j]=0;
                clients_vector[j].band_filled = 0; 
                clients_vector[j].last_connection_scheduled = 0;
		        clients_flow[j] = 0;
  
            }
            global_nsamples_c[j]=0;
           //setando valores iniciais
            flow[i][j] = 0;
            data[i][j] = 0;
            connections[i][j] = 0;
            clients_state[j]=0;
            clients_vector[j].own_flow[i]=0;
            clients_vector[j].own_connections[i]=0;
            clients_vector[j].own_connections_old[i]=0;   
            
 
            mean_c[j]=0;
            mean_s[i]=0;
            var_c[j]=0;
            var_s[i]=0;
            mean_clients_flow[j] =0;  
            mean_servers_flow[i] =0;
            mean_system_flow =0;
            mean_servers_number_connections[i]=0; 
            mean_clients_number_connections[j]=0;  
                  
        }
        
    }


    if(DEBUG_LEVEL>1) printf("\n");

    //INICIANDO CRIACAO E PROCESSAMENTO DE EVENTOS
    
    t = 0;
    t_last = 0;
    t_print = 0;
    t_last_fi = 0;
    number_events = 0;
    nsamples = 0;
    mean_system_flow=0;
    servers_up = NUMBER_SERVERS;
    clients_up = 0;

    if(DEBUG_LEVEL>1) printf( "%f BEGIN\n", t);

    //Criando eventos de saida e entrada de servidores
    if(NUMBER_SERVERS_DOWN > 0) 
    {
        for(i=0 ; i<NUMBER_SERVERS_DOWN ; i++)
        {   
        
             while(true)
            {
                number = rand() % NUMBER_SERVERS;
                if (servers_togo_down[number]==0) break;
            }
            
            servers_togo_down[number] = 1;

            while(true)
            {
                 exp_paramenter = (float)(1/(float)DELTA1);
                 when = TIME_SERVERS_DOWN + gera_aleatorio(exp_paramenter);
                 //printf("TENTATIVA_SERVER_EXIT_EVENT %d  %f\n", number, when);
                 if( when < t )
                 {
                     printf("ERROR: %f ERROR_TIME_EVENT - SERVER_EXIT_EVENT %d %f\n", t, number, when);
                     return 1;
                     
                 }
                 busca_tempo (events, when, ant, pont);
                 if(*pont==NULL) break;
            }
            if(DEBUG_LEVEL>0)  printf("PUSH SERVER_EXIT_EVENT %d  %f\n", number, when);
            insere(events,SERVER_EXIT_EVENT,when,number,0);
            number_events++;
            while(true)
            {
                 exp_paramenter = (float)(1/(float)DELTA1);
                 when = TIME_SERVERS_UP + gera_aleatorio(exp_paramenter);
                 if( when < t )
                 {
                     printf("ERROR: %f ERROR_TIME_EVENT - SERVER_ARRIVAL_EVENT %d %f\n", t, number, when);
                     return 1;
                     
                 }
                 busca_tempo (events, when, ant, pont);
                 if(*pont==NULL) break;
            }
            if(DEBUG_LEVEL>0)  printf("SERVER_ARRIVAL_EVENT %d  %f\n", number, when);
            insere(events,SERVER_ARRIVAL_EVENT,when,number,0);
            number_events++;

        }
    }



    //Criando eventos de chegada, saida e fluxo
    for(j=0; j<NUMBER_CLIENTS ; j++)
    {                                
             //criando evento de chegada para cliente no vetor de eventos
            while(true)
            {
                //printf("PUSH exp_paramenter %f %d\n", exp_paramenter, j);
                exp_paramenter = (float)(1/(float)DELTA1);
                
                teste=j/50;
                //printf("PUSH teste %d %d\n", teste, j); 
                teste=500*teste;
                //printf("PUSH teste %d %d\n", teste, j);  
                
                when = t+gera_aleatorio(exp_paramenter)+teste;
                //printf("PUSH exp_paramenter %f %d\n", when, j);
                
                if( when < t )
                {
                     printf("ERROR: %f ERROR_TIME_EVENT - CLIENT_ARRIVAL_EVENT %d %f\n", t, j, when);
                     return 1;
                     
                 }
                 busca_tempo (events, when, ant, pont);
                 if(*pont==NULL) break;
            }
             insere(events,CLIENT_ARRIVAL_EVENT,when,0,j);

             number_events++;
             if(DEBUG_LEVEL>3) printf("%f PUSH CLIENT_ARRIVAL_EVENT %d %f\n", t, j, when);
             
             //criando evento de saida para cliente no vetor de saidas e passando endereÃ§o para o cliente
             when=INF;             
             clients_vector[j].exit_event = insere(exits,CLIENT_EXIT_EVENT,when,0,j); 
             number_events++;
             if(DEBUG_LEVEL>3) printf("%f PUSH CLIENT_EXIT_EVENT %i %f\n", t, j,when);
             
             //criando evento de fluxo para cliente e passando endereÃ§o para o cliente
             when = INF;
             clients_vector[j].flow_event = malloc_flow(j); 
             clients_vector[j].flow_event_alloc=0;

             if(KNOWLEDGE < NUMBER_SERVERS ) {
        			for(i=0; i<KNOWLEDGE ; i++)
        			{
        				while(true)
        				{
        					number = rand() % NUMBER_SERVERS;
        					if (knowledge_temp[number]!=j) break;
        				}
        				knowledge_servers[j][i]=number;
        				knowledge_temp[number] = j;
        			}
             }
             else{
                for(i=0; i<KNOWLEDGE ; i++){
                    knowledge_servers[j][i]=i;

                }


             }
    }    
    
    
    //Processando a lista de eventoS     
    temp_event_events = get_next(events);
    temp_event_exits = get_next(exits);
    
    if(temp_event_events == NULL)
    {
                  printf("ERROR: Nao ha eventos para comecar a simulacao\n");
                  return 1;                           
    }

    t_print++;
    
    while( number_events )
    {

           if(DEBUG_LEVEL>3) printf( "\n%f REMAINING EVENTS TO PROCESS %d\n", t, number_events);
           
           if(DEBUG_LEVEL>3) listar(exits);
           if(DEBUG_LEVEL>3) listar(events);
           if(DEBUG_LEVEL>3) printf("\n\n");
           
           if(get_time(temp_event_events) >= get_time(temp_event_exits)) //verificando qual evento ocorre primeiro: events ou exits
           {
                //EXIT_EVENT OCORRE PRIMEIRO
                //puts("1 entrando saida cliente de case main");
                t= get_time(temp_event_exits); 
                event_id = get_event_id(temp_event_exits);
                client_id = get_client_id(temp_event_exits);
                //server_id = get_server_id(temp_event_exits);
                

                if(event_id==CLIENT_EXIT_EVENT)
                {
                    i = process_exit_event(temp_event_exits, exits, events, clients_vector, connections, clients_state, servers_state, &servers_up, &number_events, CLIENT_EXIT_EVENT, client_id , &clients_up);                    
                    if( i == 1)                         return 1;
                }                                                                        
                                    
           }
           else{
                
               //EVENTO DA LISTA EVENTS OCORRE PRIMEIRO QUE UMA SAIDA 

               t = get_time(temp_event_events);
               event_id = get_event_id(temp_event_events);
               client_id = get_client_id(temp_event_events);
               server_id = get_server_id(temp_event_events);
               
               switch(event_id)
               {
                               case CLIENT_ARRIVAL_EVENT:
                                    
                                    i = process_arrival_event(temp_event_events, events, clients_vector, knowledge_servers, connections, flow, clients_state, servers_state, &servers_up, &number_events, CLIENT_ARRIVAL_EVENT, client_id, &clients_up);
                                    if( i == 1) return 1;
                                    break;

                                case SERVER_ARRIVAL_EVENT:
                                    
                                    i = process_arrival_event(temp_event_events, events, clients_vector, knowledge_servers, connections, flow, clients_state, servers_state, &servers_up, &number_events, SERVER_ARRIVAL_EVENT, server_id, &clients_up );
                                    if( i == 1) return 1;
                                    break;

                                    
                               case FLOW_EVENT:
                                    
                                    i = process_flow_event(temp_event_events, events, clients_vector, knowledge_servers, connections, flow,servers_state, client_id, &number_events);
                                    if( i == 1) return 1;
                                    break;
                                    
                               case CONNECTION_EVENT:
                                    
                                    i = process_connection_event(temp_event_events, events, exits, clients_vector, connections, servers_number_connections, clients_number_connections, flow, clients_flow, servers_flow, data, clients_state, servers_state, event_id, server_id, client_id, &number_events, clients_up);     
                                    if( i == 1) return 1;
                                    break;
                                    
                               case DISCONNECTION_EVENT:
                                                                        
                                    i = process_connection_event(temp_event_events, events, exits, clients_vector, connections, servers_number_connections, clients_number_connections, flow, clients_flow, servers_flow, data, clients_state, servers_state, event_id, server_id, client_id, &number_events,clients_up);     
                                    if( i == 1) return 1;
                                    break;

                                case SERVER_EXIT_EVENT:
                
                                    i = process_exit_event(temp_event_events, exits, events, clients_vector, connections, clients_state, servers_state, &servers_up, &number_events, SERVER_EXIT_EVENT, server_id, &clients_up );
                                    if( i == 1) return 1;
                                    break;
                                

               
               }

        }//END else event time < exit time
    
      //atualizando evento de saida mais proximo               
       temp_event_exits = get_next(exits);
       temp_event_events = get_next(events);
       
    }//END while - ENQUANTO HOUVER EVENTOS 
    
    if(DEBUG_LEVEL>1) printf( "%f END\n", t);


	for(j=0;j<NUMBER_CLIENTS;j++)
    {
        //dp_clients_number_connections[j] = sqrt(var_clients_number_connections[j]);
        //ic_clients_number_connections[j] = ((1.96*dp_clients_number_connections[j])/sqrt(nsamples));
        //dp_clients_flow[j] = sqrt(var_clients_flow[j]);
        //ic_clients_flow[j] = ((1.96*dp_clients_flow[j])/sqrt(nsamples));


        when = client_exit_time[j]-client_arrive_time[j];
        printf( "CLIENT_DOWNLOAD_TIME %d %f\n", j, when);

		mean_clients_number_connections[j] +=  oldmean_clients_number_connections[j]*(t-t_last_fi);
		mean_clients_flow[j] +=  oldmean_clients_flow[j]*(t-t_last_fi);
		mean_clients_number_connections[j] = mean_clients_number_connections[j]/(t-t_inicio_stats);        
		mean_clients_flow[j] = mean_clients_flow[j]/(t-t_inicio_stats);
        printf( "CLIENT_CONNECTION_STATISTIC %d %Lf\n", j, mean_clients_number_connections[j]);
        printf( "CLIENT_FLOW_STATISTIC %d %Lf\n", j, mean_clients_flow[j]);
        
        
        if(DEBUG_LEVEL>2) {
			mean_c[j]+=oldmean_c[j]*(t-t_last_fi);
			mean_c[j] = mean_c[j]/(t-t_inicio_stats);
			
        	var_c[j]+=oldvar_c[j]*(t-t_last_fi);
			var_c[j] = var_c[j]/(t-t_inicio_stats);
				
			printf( "CLIENT_FLOW_MAT %d %Lf %Lf\n", j, mean_c[j], var_c[j]);
			}
    }
	for(i=0; i< NUMBER_SERVERS ; i++)
	{
        //dp_servers_number_connections[i] = sqrt(var_servers_number_connections[i]);
        //ic_servers_number_connections[i] = ((1.96*dp_servers_number_connections[i])/sqrt(nsamples));
        //dp_servers_flow[i] = sqrt(var_servers_flow[i]);
        //ic_servers_flow[i] = ((1.96*dp_servers_flow[i])/sqrt(nsamples));
        
		mean_servers_number_connections[i] += oldmean_servers_number_connections[i]*(t-t_last_fi);
        mean_servers_flow[i] += oldmean_servers_flow[i]*(t-t_last_fi);
		mean_servers_number_connections[i] = mean_servers_number_connections[i]/(t-t_inicio_stats);        
		mean_servers_flow[i] = mean_servers_flow[i]/(t-t_inicio_stats);
		
		printf( "SERVER_CONNECTION_STATISTIC %d %Lf\n", i, mean_servers_number_connections[i]);
		printf( "SERVER_FLOW_STATISTIC %d %Lf\n", i, mean_servers_flow[i]);
        
        
        if(DEBUG_LEVEL>2) {
			mean_s[i]+=oldmean_s[i]*(t-t_last_fi);
			mean_s[i] = mean_s[i]/(t-t_inicio_stats);
			
			var_s[i]+=oldvar_s[i]*(t-t_last_fi);
			var_s[i] = var_s[i]/(t-t_inicio_stats);
			printf( "SERVER_FLOW_MAT %d %Lf %Lf\n",  i, mean_s[i], var_s[i]);			
		}
	}
	
    //dp_system_flow = sqrt(var_system_flow);
    //ic_system_flow = ((1.96*dp_system_flow)/sqrt(nsamples));
    mean_system_flow += oldmean_system_flow*(t-t_last_fi);
	mean_system_flow = mean_system_flow/(t-t_inicio_stats);
    printf( "SYSTEM_FLOW_STATISTIC 0 %Lf\n", mean_system_flow);        

    
    return 0;
    
}//END main
