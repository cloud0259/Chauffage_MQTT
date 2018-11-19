/*************************************************/
/*************CHAUFFE*****************************/
/*************************************************/
void chauffe()
{
  Serial.print("temp "); //Ecrit temp dans le terminal
  Serial.println(t); //Ecrit la temperature dans le terminal
  Serial.print("Cons :"); //Ecrit cons dans le terminal
  Serial.println(c); //Ecrit la consigne dans le terminal
  if(t > 0.0) //Si la température est supérieur a 0 (évite les erreurs.
  {
    if(t <= c -hysteresis) //Si la temperature est inférieur a la consigne - l'hysteresis
    {
      client.publish("relay","1"); //Publie la demande de chauffage a 1
    }
    else if(t > c) //Sinon si la temperature est supérieur a la consigne
    {
      client.publish("relay","0"); //Publie l'arrêt du chauffage.
    }
  }
}


