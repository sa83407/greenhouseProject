import processing.serial.*;
Serial port;
String[] list;

int textColor = color(0,0,0);
int white = color(255,255,255);
int belowTemp = color(135,206,235);



void setup() {

      //size of window
      size (1400, 800);
      //serial comm
      port = new Serial (this, "/dev/ttyACM0", 9600);
 
      createFont("FFScala", 200);
      
      smallBoxBackground();  
      
      
        //Labels ex.  "GH1" "GH2"
      //LH
      textSize(40);
      fill(textColor);
      text("LH", 100,29);
      
      //HH
      textSize(40);
      fill(textColor);
      text("HH", 380,29);
      
      //GH1
      textSize(40);
      fill(textColor);
      text("GH1", 660,29);
      
      //GH2
      textSize(40);
      fill(textColor);
      text("GH2", 940,29);
         
      //ISO1
      textSize(40);
      fill(textColor);
      text("ISO1", 1220,29);
      
      //ISO2
      textSize(40);
      fill(textColor);
      text("ISO2", 1220,220);
      
      //ISO4
      textSize(40);
      fill(textColor);
      text("ISO4", 1220,620);
      
      //GH7
      textSize(40);
      fill(textColor);
      text("GH4", 660,415);
      
      //GH8
      textSize(40);
      fill(textColor);
      text("GH3", 940,415);
            
      //ISO3
      textSize(40);
      fill(textColor);
      text("ISO3", 1220,420);
      
      //Counter
      textSize(20);
      fill(textColor);
      text("Counter", 100,450);
            
 }

void draw() {

          smallBoxBackground();



          if (port.available() > 0) {
                          String val = port.readString();
                          list = split(val, ',');
                          if(list.length > 19){
                          float temp = float(list[0]);
                          println(val);
                          println(list.length);
                          float temp2 = float(list[1]);
                          float temp3 = float(list[2]);
                          float temp4 = float(list[3]);
                          float temp5 = float(list[4]);
                          float temp6 = float(list[5]);
                          float temp7 = float(list[6]);
                          float temp8 = float(list[7]);
                          float temp9 = float(list[8]);
                          float temp10 = float(list[9]);
                          float hum1 = float(list[10]);
                          float hum2 = float(list[11]);
                          float hum3 = float(list[12]);
                          float hum4 = float(list[13]);
                          float hum5 = float(list[14]);
                          float hum6 = float(list[15]);
                          float hum7 = float(list[16]);
                          float hum8 = float(list[17]);
                          float hum9 = float(list[18]);
                          float hum10 = float(list[19]);
   

                              //Display values for temp/humidity
                               ////LH
                              textSize(150);
                              fill(textColor);
                              text((int)temp, 50,150);
                              
                              ////HH
                              textSize(150);
                              fill(textColor);
                              text((int)temp2, 330,150);
                              
                              //GH1
                              textSize(150);
                              fill(textColor);
                              text((int)temp3, 610,150);
                              
                              //GH2
                              textSize(150);
                              fill(textColor);
                              text((int)temp4, 890,150);
                              
                              //ISO1
                              textSize(100);
                              fill(textColor);
                              text((int)temp5, 1180,120);
                              
                               //ISO2
                              textSize(100);
                              fill(textColor);
                              text((int)temp6, 1180, 320);
                              
                              
                               //temp 7
                              textSize(100);
                              fill(textColor);
                              text((int)temp7,1180, 700);
                              
                              //temp 8
                              textSize(150);
                              fill(textColor);
                              text((int)temp8, 610, 550);
                              
                              //temp 9
                              textSize(150);
                              fill(textColor);
                              text((int)temp9, 890, 550);
                              
                              //iso3 temp
                              textSize(100);
                              fill(textColor);
                              text((int)temp10, 1170, 520);
                              
                              //hum 1 LH
                              textSize(60);
                              fill(textColor);
                              text((int)hum1, 100,300);
                              text("%", 180, 300);
                              
                              //hum 2 HH
                              textSize(60);
                              fill(textColor);
                              text((int)hum2, 380,300);
                              text("%", 460, 300);
                              
                              //hum 3 GH1
                              textSize(60);
                              fill(textColor);
                              text((int)hum3, 660,300);
                              text("%", 740, 300);
                              
                              //hum 4 GH2
                              textSize(60);
                              fill(textColor);
                              text((int)hum4, 940,300);
                              text("%", 1020, 300);
                              
                              //hum 5 ISO1
                              textSize(50);
                              fill(textColor);
                              text((int)hum5, 1220,170);
                              text("%", 1285, 170);
                              
                               //hum 6 iso2
                              textSize(50);
                              fill(textColor);
                              text((int)hum6, 1220, 370);
                              text("%", 1285, 370);
                              
                               //hum 7 ISO4
                              textSize(60);
                              fill(textColor);
                              text((int)hum7, 1220, 750);
                              text("%", 1290, 750);
                              
                              //hum 8 gh4
                              textSize(60);
                              fill(textColor);
                              text((int)hum8, 660, 700);
                              text("%", 740, 700);
                              
                              //hum 9 gh3
                              textSize(60);
                              fill(textColor);
                              text((int)hum9, 940, 700);
                              text("%", 1020, 700);
                              
                              //hum 10 iso3
                              textSize(50);
                              fill(textColor);
                              text((int)hum10, 1220, 570);
                              text("%", 1285, 570);
                              
                              //Counter
                              textSize(30);
                              fill(textColor);
                              text((int)hum10, 80,500);
      
            //if above temp
            if(temp > 100){
                    //display temp1
                    fill(183, 31, 31);
                    rect(20,30,250,350); //position
      
                    ////temp 1
                    textSize(150);
                    fill(textColor);
                    text((int)temp, 50,150);
                    //hum 1
                    textSize(60);
                    fill(textColor);
                    text((int)hum1, 100,300);
                    text("%", 180, 300);
      
            }
            if(temp2 > 100){
                     //display temp2
                     fill(183,31,31);
                     rect(300,30,250,350); //position
      
                    ////temp 2
                    textSize(150);
                    fill(textColor);
                    text((int)temp2, 330,150);
                    //hum 2
                    textSize(60);
                    fill(textColor);
                    text((int)hum2, 380,300);
                    text("%", 460, 300);
      
            }
            if(temp3 > 100){
                  //display temp3
                  fill(183,31,31);
                  rect(580,30,250,350); //position
      
                   //temp 3
                  textSize(150);
                  fill(textColor);
                  text((int)temp3, 610,150);
                  //hum 3
                  textSize(60);
                  fill(textColor);
                  text((int)hum3, 660,300);
                  text("%", 740, 300);
      
            }
            if(temp4 > 100){
                    //display temp4
                    fill(183,31,31);
                    rect(860,30,250,350); //position
                    
                    //temp 4
                    textSize(150);
                    fill(textColor);
                    text((int)temp4, 890,150);;
                    //hum 4
                    textSize(60);
                    fill(textColor);
                    text((int)hum4, 940,300);
                    text("%", 1020, 300);
      
            }
            if(temp5 > 100){
                     //display temp5
                    fill(183,31,31);
                    rect(1140,30,250,350); //position
      
                    //temp 5
                    textSize(100);
                    fill(textColor);
                    text((int)temp5, 1180,120);
                    //hum 5
                     textSize(50);
                      fill(textColor);
                      text((int)hum5, 1220,170);
                      text("%", 1285, 170);
      
            }
            if(temp6 > 100){
                    //display temp6
                    fill(183,31,31);
                    rect(1140,230,250,150); //position
      
                    //temp 6
                    textSize(100);
                    fill(textColor);
                    text((int)temp6, 1180, 320);
                    //hum 6
                    textSize(50);
                    fill(textColor);
                    text((int)hum6, 1220, 370);
                    text("%", 1285, 370);
      
            }
            if(temp7 > 100){
                   //display temp7
                  fill(183,31,31);
                  rect(1140,625,250,150);  //position
      
                  //temp 7
                  textSize(100);
                  fill(textColor);
                  text((int)temp7,1180, 700);
                  //hum 7
                  textSize(60);
                  fill(textColor);
                  text((int)hum7, 1220, 750);
                  text("%", 1290, 750);
      
            }
            if(temp8 > 100){
                   //display temp8
                   fill(183,31,31);
                   rect(580,420,250,350); //position
      
                   //temp 8
                  textSize(150);
                  fill(textColor);
                  text((int)temp8, 610, 550);
                  //hum 8
                  textSize(60);
                  fill(textColor);
                  text((int)hum8, 660, 700);
                  text("%", 740, 700);
      
            }
            if(temp9 > 100){
                   //display temp9
                  fill(183,31,31);
                  rect(860,420,250,350); //position
      
                  //temp 9
                  textSize(150);
                  fill(textColor);
                  text((int)temp9, 890, 550);
                  //hum 9
                  textSize(60);
                  fill(textColor);
                  text((int)hum9, 940, 700);
                  text("%", 1020, 700);
      
            }
            if(temp10 > 100){
                
                   //display temp10
                  fill(183,31,31);
                  rect(1140,430,250,150); //position n
      
                   //temp 10
                   textSize(100);
                    fill(textColor);
                    text((int)temp10, 1170, 520);
                   //hum 10
                  textSize(50);
                  fill(textColor);
                  text((int)hum10, 1220, 570);
                  text("%", 1285, 570);
      
            }
            
            
            //below 60
             if(temp < 60){
                    //display temp1
                    fill(belowTemp);
                    rect(20,30,250,350); //position
      
                    ////temp 1
                    textSize(150);
                    fill(textColor);
                    text((int)temp, 50,150);
                    //hum 1
                    textSize(60);
                    fill(textColor);
                    text((int)hum1, 100,300);
                    text("%", 180, 300);
      
            }
            if(temp2 < 60){
                     //display temp2
                     fill(belowTemp);
                     rect(300,30,250,350); //position
      
                    ////temp 2
                    textSize(150);
                    fill(textColor);
                    text((int)temp2, 330,150);
                    //hum 2
                    textSize(60);
                    fill(textColor);
                    text((int)hum2, 380,300);
                    text("%", 460, 300);
      
            }
            if(temp3 < 60){
                  //display temp3
                  fill(belowTemp);
                  rect(580,30,250,350); //position
      
                   //temp 3
                  textSize(150);
                  fill(textColor);
                  text((int)temp3, 610,150);
                  //hum 3
                  textSize(60);
                  fill(textColor);
                  text((int)hum3, 660,300);
                  text("%", 740, 300);
      
            }
            if(temp4 < 60){
                    //display temp4
                    fill(belowTemp);
                    rect(860,30,250,350); //position
                    
                    //temp 4
                    textSize(150);
                    fill(textColor);
                    text((int)temp4, 890,150);;
                    //hum 4
                    textSize(60);
                    fill(textColor);
                    text((int)hum4, 940,300);
                    text("%", 1020, 300);
      
            }
            if(temp5 < 60){
                     //display temp5
                    fill(belowTemp);
                    rect(1140,30,250,150); //position
      
                    //temp 5
                    textSize(100);
                    fill(textColor);
                    text((int)temp5, 1180,120);
                    //hum 5
                    textSize(50);
                    fill(textColor);
                    text((int)hum5, 1220,170);
                    text("%", 1285, 170);
      
            }
            if(temp6 < 60){
                    //display temp6
                    fill(belowTemp);
                    rect(1140,230,250,150); //position
      
                    //temp 6
                    textSize(100);
                    fill(textColor);
                    text((int)temp6, 1180, 320);
                    //hum 6
                    textSize(50);
                    fill(textColor);
                    text((int)hum6, 1220, 370);
                    text("%", 1285, 370);
      
            }
            if(temp7 < 60){
                   //display temp7
                  fill(belowTemp);
                  rect(1140,625,250,150);  //position
      
                  //temp 7
                  textSize(100);
                  fill(textColor);
                  text((int)temp7,1180, 700);
                  //hum 7
                  textSize(60);
                  fill(textColor);
                  text((int)hum7, 1220, 750);
                  text("%", 1290, 750);
      
            }
            if(temp8 < 60){
                   //display temp8
                   fill(belowTemp);
                   rect(580,420,250,350); //position
      
                   //temp 8
                  textSize(150);
                  fill(textColor);
                  text((int)temp8, 610, 550);
                  //hum 8
                  textSize(60);
                  fill(textColor);
                  text((int)hum8, 660, 700);
                  text("%", 740, 700);
      
            }
            if(temp9 < 60){
                   //display temp9
                  fill(belowTemp);
                  rect(860,420,250,350); //position
      
                  //temp 9
                  textSize(150);
                  fill(textColor);
                  text((int)temp9, 890, 550);
                  //hum 9
                  textSize(60);
                  fill(textColor);
                  text((int)hum9, 940, 700);
                  text("%", 1020, 700);
      
            }
            if(temp10 < 60){
                
                   //display temp10
                  fill(belowTemp);
                  rect(1140,430,250,150); //position n
      
                   //temp 10
                   textSize(100);
                    fill(textColor);
                    text((int)temp10, 1170, 520);
                   //hum 10
                  textSize(50);
                  fill(textColor);
                  text((int)hum10, 1220, 570);
                  text("%", 1285, 570);
      
            }
    }
}



 delay(5000);
}



void smallBoxBackground(){
          
            //display LH
              fill(white);
              rect(20,30,250,350); //position
              
              //display HH
              fill(white);
              rect(300,30,250,350); //position
              
              //display GH1
              fill(white);
              rect(580,30,250,350); //position
              
              //display GH2
              fill(white);
              rect(860,30,250,350); //position
              
              //display ISO1
              fill(white);
              rect(1140,30,250,150); //position
              
              //display ISO2
              fill(white);
              rect(1140,230,250,150); //position x,y,width,length
              
              //display iso4
              fill(white);
              rect(1140,625,250,150); //position
              
              //display temp8
              fill(white);
              rect(580,420,250,350); //position
              
              //display temp9
              fill(white);
              rect(860,420,250,350); //position
              
              //display iso3
              fill(white);
              rect(1140,430,250,150); //position    
              
              //display counter
              fill(white);
              rect(20,455,250,100); //position
      
}
