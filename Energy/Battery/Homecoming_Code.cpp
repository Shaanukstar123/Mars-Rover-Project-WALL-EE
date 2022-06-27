{\rtf1\ansi\ansicpg1252\cocoartf2636
\cocoatextscaling0\cocoaplatform0{\fonttbl\f0\fswiss\fcharset0 Helvetica;}
{\colortbl;\red255\green255\blue255;}
{\*\expandedcolortbl;;}
\paperw11900\paperh16840\margl1440\margr1440\vieww11520\viewh8400\viewkind0
\pard\tx566\tx1133\tx1700\tx2267\tx2834\tx3401\tx3968\tx4535\tx5102\tx5669\tx6236\tx6803\pardirnatural\partightenfactor0

\f0\fs24 \cf0 #include <iostream>\
#include <cmath>\
\
int main() \{\
  std::cout << "mAh usage of device: ";\
  int b; //Rover's average battery usage\
  double ttdeath, rt, time_to_return, s; //time until rover runs out of battery, runtime of rover, time required to return to charging station, speed of rover\
  double x, y, l_path, battery_charge, tmp; // x and y coordinates, length of the indepent vectors that form the vector to x,y, battery's current charge, temporary value\
  std::cin >> b;\
  std::cout << "m/s speed of rover: ";\
  std::cin >> s;\
  std::cout << "Run-time in seconds: ";\
  std::cin >> rt;\
  std::cout << "Coordinate x: ";\
  std::cin >> x;\
  std::cout << "Coordinate y: ";\
  std::cin >> y;\
\
  // full run-time of rover = (6000 / current usage per hour), i.e. 6000mAh/bmAh\
  // battery can provide b mAh for 6000/b hours i.e. b mAh for (6000/b)/(60^2) seconds\
  // hence, after the rover has run for (6000/b)/(60^2), the battery is depleted\
  // rover uses b/(3600) mA per second\
\
  tmp = 6000/b;\
  tmp = (6000/b)*60*60;\
  battery_charge = ((tmp - rt)/tmp)*100; //Battery charge found as the time remaining divided by the total time that was available\
  std::cout << "Approximate battery charge: ";\
  std::cout << battery_charge;\
  std::cout << "%" << std::endl;\
  ttdeath = tmp - rt;\
  std::cout << "Approximate time until death: ";\
  std::cout << ttdeath << std::endl;\
  l_path = x + y;\
  time_to_return = l_path/s; //Finds time required to return to charging station\
  std::cout << "Time to return: " << time_to_return << std::endl;\
  if(ttdeath < 1.5*time_to_return)\{\
    std::cout << "WARNING: Return immediately" << std::endl; //Warns user to return is battery is about to run out, yet ensures the warning appears such that the rover has enough charge to return.\
  \}\
\}}
