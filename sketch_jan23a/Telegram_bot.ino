//https://kit.alexgyver.ru/tutorials/fastbot/
//Функция обработчик Telegram - бота  ----------------------------------------------------------
void newMsg(FB_msg& msg) {
  // выводим ID чата, имя юзера и текст сообщения
  Serial.print(msg.chatID);     // ID чата 
  Serial.print(", ");
  Serial.print(msg.username);   // логин
  Serial.print(", ");
  Serial.println(msg.text);     // текст
  // отправить сообщение обратно
  //bot.sendMessage(msg.text, msg.chatID);  

  if (msg.text == "Пошел на хуй"){bot.sendMessage("Сам пошел, пидор", msg.chatID); }
  if (msg.text == "/get_ip") {bot.sendMessage(getIP(), msg.chatID);} 
}
