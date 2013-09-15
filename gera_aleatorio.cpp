//gera numero aleatorio com distribuicao exponencial 
double gera_aleatorio(double parametro)
{
      
      double number;
      
      while(true)
      {
          number = (double)random()/RAND_MAX;
          if(number != 0) break;
      }

      return (double)log(1-number)/(-parametro);
}
