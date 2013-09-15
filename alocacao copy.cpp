int alocacao(client clients_vector[NUMBER_CLIENTS], int clients_state[NUMBER_CLIENTS], int servers_state[NUMBER_SERVERS],  int connections[][NUMBER_CLIENTS], float flow[][NUMBER_CLIENTS], long double clients_flow[NUMBER_CLIENTS], long double servers_flow[NUMBER_SERVERS], int clients_number_connections[NUMBER_CLIENTS], int servers_number_connections[NUMBER_SERVERS])
{
      
    int i, j, k, keep;  
    int servers_number_connections_temp[NUMBER_SERVERS]; //possui o numero de conexoes que o servidor possui com clientes que nao tem a banda totalmente preenchida ainda
    long double mean_flow;
    long double clients_cap_rem[NUMBER_CLIENTS]; //inicia com o total da capacidade do cliente, Ã© subtraido o custo das conexoes, e a medida que os servidores alocam banda, vai sendo decrementado (indica quanta banda livre o cliente tem para receber mais fluxo)
    long double servers_cap_rem[NUMBER_SERVERS]; //inicia com o total da capacidade do servidor, Ã© subtraido a medida que a banda vai sendo alocada (ou seja, indica quanta banda o servidor ainda possui para distribuir)
    long double clients_cap_filled[NUMBER_CLIENTS]; //inicia zerada e indica quanta banda jÃ¡ foi preenchida, os custos de conexao preenchem a banda
    long double servers_cap_filled[NUMBER_SERVERS]; //indica quanta banda jÃ¡ foi distribuida pelo servidor
    long double servers_flow_test[NUMBER_SERVERS], clients_flow_test[NUMBER_CLIENTS];
    long double system_flow;

     int nsamples_s[NUMBER_SERVERS], nsamples_c[NUMBER_CLIENTS];
     long double oldmean_s_temp[NUMBER_SERVERS], mean_s_temp[NUMBER_SERVERS], var_s_temp[NUMBER_SERVERS];
     long double oldmean_c_temp[NUMBER_CLIENTS], mean_c_temp[NUMBER_CLIENTS], var_c_temp[NUMBER_CLIENTS];                    
                        
    
	
	
    for(i=0;i<NUMBER_SERVERS;i++)
    {
                                 servers_number_connections_temp[i] =  servers_number_connections[i]; //inicialmente todos os clientes conectados aos servidores ainda tem banda disponivel. A medida que a banda dos clientes vai sendo saturada, esta variavel eh decrementada.
                                 servers_cap_rem[i] = Servers_cap[i]; //servidores comecam com toda a capacidade para distribuir
                                 servers_cap_filled[i] = 0;
                                 servers_flow_test[i] = 0;
                                 mean_s_temp[i]=0;
                                 var_s_temp[i]=0;
                                 for(j=0; j< NUMBER_CLIENTS ; j++)
                                 {
                                          nsamples_c[j]=0;
                                          nsamples_s[i]=0;  
                                          flow[i][j]=0;
                                          mean_c_temp[j]=0;
                                          var_c_temp[j]=0;
                                          if(i==0){ //atribuicoes feitas so no primeiro laco
					      					                    clients_flow_test[j]=0;	
                                              clients_cap_filled[j] = 0;
                                              clients_vector[j].band_filled =0; 
                                              clients_cap_rem[j] = Clients_cap[j];
                                          }
                                 
								 		  if(connections[i][j] == 1)
								 		  {
												clients_cap_rem[j] -= Servers_connection_cost[j]; //substraimos da capacidade livre do cliente o custo das conexoes ativas
												clients_cap_filled[j] += Servers_connection_cost[j];
					  			 		  }
    							 }
    }
     
    keep = 1;    
      
    while(keep)
    { 
        //Este loop eh executado atÃ© que a distribuicao do fluxo nao mude mais
        keep = 0;
        
        for(i=0;i<NUMBER_SERVERS;i++) //FOR#1
        {
              if(servers_number_connections_temp[i] == 0) continue; //caso o servidor nao esteja conectado a nenhum cliente ou todos seus clientes ja estejam com a banda saturada, pular para o proximo servidor
              
              mean_flow = floor(servers_cap_rem[i]/servers_number_connections_temp[i]);
              
              if( mean_flow > 1 ) //se o servidor tem banda para distribuir
              {
                  for(j=0; j< NUMBER_CLIENTS ; j++) //FOR#2
                  {                            
                           if( connections[i][j] == 1 ) //se cliente e servidor estao conectados
                           {
                               if(clients_vector[j].band_filled != 1) //se ainda ha banda para preecher nesse cliente
                               {
                                                if(clients_cap_rem[j] > mean_flow ){
                                                             //se o fluxo medio que o servidor esta reservando eh menor que a banda livre do cliente, aloca o fluxo medio
                                                             flow[i][j] += mean_flow;
                                                             
                                                             //mean_flow foi gasto na banda do cliente e do servidor
                                                             servers_cap_rem[i] -= mean_flow;
                                                             clients_cap_rem[j] -= mean_flow;
                                                             
                                                             //banda sendo utilizada pelo cliente ; quanto o servidor ja alocou
                                                             servers_cap_filled[i] += mean_flow;
                                                             clients_cap_filled[j] += mean_flow; }
                                                    else{    
                                                             //neste caso, o cliente nao tem banda suficiente para absorver todo o fluxo que estava disponivel nesta rodada, alocar so ate aonde ele tinha disponivel                                         
                                                             flow[i][j] += clients_cap_rem[j];
                                                             //o servidor sÃ³ gastou parte do fluxo esperado, o cliente nao tem mais capacidade de receber fluxo
                                                             servers_cap_rem[i] -= clients_cap_rem[j];                                                     
                                                             
                                                             //atualizando quanto de banda cliente esta utilizando no total, que eh == a sua capacidade
                                                             //atualizando quanto o servidor ja alocou de banda
                                                             servers_cap_filled[i] += clients_cap_rem[j];
                                                             clients_cap_filled[j] += clients_cap_rem[j];
                                                          
                                                             clients_cap_rem[j] = 0; //o cliente nao tem mais banda livre
                                                             clients_vector[j].band_filled =1; //a banda do cliente esta saturada
                                                             
                                                             //atualizando numero de clientes conectados e com banda disponivel para cada servidor
                                                             for(k=0;k<NUMBER_SERVERS;k++)
                                                             {                            //como o cliente nao tem mais banda livre, nos proximos loops os servidores nao precisarao mais considerar este cliente (nao sera necessario reservar uma fatia da banda livre do servidor nas proximas iteracoes do while)
                                                                                          if(connections[k][j] == 1) servers_number_connections_temp[k]--;
                                                             }
                                                             
                                                             //if(DEBUG_LEVEL>3) printf("%f CLIENT_BAND_FILLED %d (confirmando id) %d (filled =1) %d\n", t, j, clients_vector[j].client_id, clients_vector[j].band_filled);
                                   
                                                         }
     
                                                    //houve alocacao de banda, talvez ainda haja clientes com banda livre e servidores com banda disponivel para alocar
                                                    keep = 1;                   
                                                    if(DEBUG_LEVEL > 4 ) printf( "TEMP_FLOW_ALOCCATED %d %d %Lf\n", i, j, flow[i][j]);                   
                                                         
                               }
                               
                               if( ( (flow[i][j]+Servers_connection_cost[i]) >  ( Clients_cap[j]) )   || ( flow[i][j] >  Servers_cap[i] ) || (servers_cap_filled[i] > Servers_cap[i]) || (clients_cap_filled[j] > Clients_cap[j]) )
                               {
                                   
                                   printf( "ERROR: %f ERROR_FLOW_ALLOCATION %d %d %f %f %Lf\n", t, i, j, Servers_cap[i],  Clients_cap[j], flow[i][j]);
                                   return 1;    
                               }
                           }               
                  }
              }     
        }
        
    }
        
        
    
	for(j=0;j<NUMBER_CLIENTS;j++)
	{
		 for(i=0; i< NUMBER_SERVERS ; i++)
		 {
	
			      servers_flow_test[i]+=flow[i][j];
			      clients_flow_test[j]+=flow[i][j];
		          if( ((flow[i][j] != 0) && (connections[i][j] == 0)) || (flow[i][j] < 0) || (servers_flow_test[i] > Servers_cap[i]) || (clients_flow_test[j] > Clients_cap[j]) )
		          {
		              printf( "ERROR: %f ERROR_FLOW_ALLOCATION - Server: %d Capacitie: %f Total_flow: %Lf Client: %d Capacitie: %f Total_flow: %Lf Connected:%d Flow: %Lf\n", t, i ,Servers_cap[i], servers_flow_test[i], j, Clients_cap[j], clients_flow_test[j], connections[i][j], flow[i][j]);
		              return 1;
		          }
		          servers_flow[i] = servers_flow_test[i];
		          if(DEBUG_LEVEL > 3 )  
		          { 
		             if (flow[i][j] != 0) printf("%f FLOW_ALOCCATED %d %d %Lf\n", t, i, j, flow[i][j]);
		            
		          }
		          if( DEBUG_LEVEL > 2)
		          {
		                if(flow[i][j] != 0)
		                {
		                    nsamples_s[i]++;
		                    nsamples_c[j]++;
		                        if( nsamples_s[i] == 1 )
                            {
                                mean_s_temp[i]=flow[i][j];
                            }
                            else{
                                    oldmean_s_temp[i]=mean_s_temp[i];
                                    mean_s_temp[i] = (( mean_s_temp[i] *(nsamples_s[i]-1))+flow[i][j])/nsamples_s[i];
                                    var_s_temp[i] =  ((1.0 - (1.0/(nsamples_s[i]-1))) * var_s_temp[i]) + (nsamples_s[i]) * ( (mean_s_temp[i] - oldmean_s_temp[i])*(mean_s_temp[i] - oldmean_s_temp[i]) );
                            }
                          
                            if( nsamples_c[j] == 1 )
                            {
                                mean_c_temp[j]=flow[i][j];
                            }
                            else{
                                    oldmean_c_temp[j]=mean_c_temp[j];
                                    mean_c_temp[j] = (( mean_c_temp[j] *(nsamples_c[j]-1))+flow[i][j])/nsamples_c[j];
                                    var_c_temp[j] =  ((1.0 - (1.0/(nsamples_c[j]-1))) * var_c_temp[j]) + (nsamples_c[j]) * ( (mean_c_temp[j] - oldmean_c_temp[j])*(mean_c_temp[j] - oldmean_c_temp[j]) );
                            }
                            
                            
                        }
                  }
		          

		 }
		 clients_flow[j] = clients_flow_test[j];
                 
	}    
	
	    
	//variavel auxiliar para calcular fluxo medio por conexao
	nsamples++;

	
        system_flow = 0;
     	   for(j=0;j<NUMBER_CLIENTS;j++)
         {
            if(clients_number_connections[j]>0)
            {
  	              system_flow += clients_flow[j];
  	              if(global_nsamples_c[j] == 0)
                  {
                     global_nsamples_c[j]++;
                     oldmean_clients_number_connections[j]=clients_number_connections[j];
                     oldmean_clients_flow[j]=clients_flow[j];
                  }
                  else
                  {
                     global_nsamples_c[j]++;

            				 mean_clients_number_connections[j] +=  oldmean_clients_number_connections[j]*(t-t_last_fi);
            				 oldmean_clients_number_connections[j]=clients_number_connections[j];
            			
            				 mean_clients_flow[j] +=  oldmean_clients_flow[j]*(t-t_last_fi);
            				 oldmean_clients_flow[j]=clients_flow[j];
    				
                  }    
            }

         }        
         for(i=0;i<NUMBER_SERVERS;i++)
         {
            if(servers_state[i]==1)
            {
                if(global_nsamples_s[i] == 0)
                {
                  global_nsamples_s[i]++;
                   oldmean_servers_number_connections[i] = servers_number_connections[i];
                   oldmean_servers_flow[i] = servers_flow[i];
                }
                else
                {
                  global_nsamples_s[i]++;
                  mean_servers_number_connections[i] += oldmean_servers_number_connections[i]*(t-t_last_fi);
                  oldmean_servers_number_connections[i] = servers_number_connections[i]; 

                  mean_servers_flow[i] += oldmean_servers_flow[i]*(t-t_last_fi);
                  oldmean_servers_flow[i] = servers_flow[i];
                }
            }
         }
         if(nsamples == 1)
         {
              oldmean_system_flow=system_flow;
         }
         else
         {
             mean_system_flow += oldmean_system_flow*(t-t_last_fi);
             oldmean_system_flow=system_flow;
         }
// LDBL_MAX
	
    if(DEBUG_LEVEL > 2 )  
    {
     /***************     CALCULANDO MEDIA AMOSTRAL TEMPORAL DA { MEDIA e sua VARIANCIA } DO FLUXO: ponderado no tempo!!!  ***************/
    for(j=0;j<NUMBER_CLIENTS;j++)
	  {
      if(clients_number_connections[j]>0)
      {
      	   if( global_nsamples_c[j] == 1 )
           {
              oldmean_c[j]=mean_c_temp[j];
              oldvar_c[j] = var_c_temp[j]; 
           }
          else{
              //oldmean_c[j]=mean_c[j];
              //mean_c[j] = ((( mean_c[j] *(nsamples-1))*t_last_fi)+(mean_c_temp[j]*(t-t_last_fi))/t;

              mean_c[j]+=oldmean_c[j]*(t-t_last_fi);
              oldmean_c[j]=mean_c_temp[j];
              //oldvar_c[j]=var_c[j];
              //var_c[j] = ((( var_c[j] *(nsamples-1))*t_last_fi)+(var_c_temp[j]*(t-t_last_fi))/t;
              var_c[j]+=oldvar_c[j]*(t-t_last_fi);
              oldvar_c[j]=var_c_temp[j];
      	    }
            if ((var_c[j] < 0) || (mean_c[j]<0)) {
              printf("%f PARCIAIS_MEDIA_VAR_TEMPORAL_CLIENT %d %Lf %Lf\n", t, j, mean_c[j], var_c[j]);
            }
            //printf("%f PARCIAIS_MEDIA_VAR_TEMPORAL_CLIENT %d %Lf %Lf\n", t, j, mean_c[j], var_c[j]);  
        }	
    }    
    for(i=0; i< NUMBER_SERVERS ; i++)
    {
      if(servers_state[i]==1){
    	    if( global_nsamples_s[i] == 1 )
             {
                oldmean_s[i]=mean_s_temp[i];
                oldvar_s[i] = var_s_temp[i]; 
             }
             else{
                //oldmean_c[j]=mean_c[j];
                //mean_c[j] = ((( mean_c[j] *(nsamples-1))*t_last_fi)+(mean_c_temp[j]*(t-t_last_fi))/t;
                mean_s[i]+=oldmean_s[i]*(t-t_last_fi);
                oldmean_s[i]=mean_s_temp[i];

                //oldvar_c[j]=var_c[j];
                //var_c[j] = ((( var_c[j] *(nsamples-1))*t_last_fi)+(var_c_temp[j]*(t-t_last_fi))/t;
                var_s[i]+=oldvar_s[i]*(t-t_last_fi);
                oldvar_s[i]=var_s_temp[i];

                if ((var_s[i] < 0) || (mean_s[i]<0)) {
              printf("%f PARCIAIS_MEDIA_VAR_TEMPORAL_SERVER %d %Lf %Lf\n", t, i, mean_s[i], var_s[i]);
            }
    	    }
       //printf("%f PARCIAIS_MEDIA_VAR_TEMPORAL_SERVER %d %Lf %Lf\n", t, j, mean_s[i], var_s[i]);
       }  
	}
}
    t_last_fi = t;
		                      
    if(DEBUG_LEVEL > 3 )   
    {
       printf("\n");
       
    }
    return 0;
     
}

