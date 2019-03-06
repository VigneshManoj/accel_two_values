#include <iostream>
#include <fstream>
#include "ros/ros.h"
#include <math.h>
#include "std_msgs/String.h"
#include <tf/transform_listener.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/WrenchStamped.h>
#include <accel_two_values/Num5.h>

int i=0,j=1;

//To get both the acceleration values and write this value into file
struct input_two_accel
{
    float accelx_wrench;
    float accely_wrench;
    float accelz_wrench;
    float accelx_wrench_ins;
    float accely_wrench_ins;
    float accelz_wrench_ins;
    float pos_x;
    float pos_y;
    float pos_z;
};

//To extract the wrench data from the wrench node (get the acceleration values of tip)
void call_wrench_func(accel_two_values::Num5 custom_var1)
{
    float accelx,accely,accelz;
    input_two_accel var1;
    // int velocity_updated_ ,wheel_odom_updated_;
    var1.accelx_wrench=custom_var1.body_wrench.wrench.force.x;
    var1.accely_wrench=custom_var1.body_wrench.wrench.force.y;
    var1.accelz_wrench=custom_var1.body_wrench.wrench.force.z;
    //std::cout<<"\nThe accel value in call_wrench function is "<<var1.accelx_wrench<<" "<<var1.accely_wrench<<" "<<var1.accelz_wrench<<std::endl;

}

//To extract the wrench data from the wrench_insertion node (get the acceleration values of insertion link)
void call_wrench_ins_func(accel_two_values::Num5 custom_var2)
{
    float accelx,accely,accelz;
    input_two_accel var2;
    //int velocity_updated_ ,wheel_odom_updated_;
    var2.accelx_wrench_ins=custom_var2.body_wrench.wrench.force.x;
    var2.accely_wrench_ins=custom_var2.body_wrench.wrench.force.y;
    var2.accelz_wrench_ins=custom_var2.body_wrench.wrench.force.z;
    //std::cout<<"\nThe accel value in call_wrench_ins function is "<<var2.accelx_wrench_ins<<" "<<var2.accely_wrench_ins<<" "<<var2.accelz_wrench_ins<<std::endl;

}

//To write the data into the file
void write_accel_val( input_two_accel custom_var)
{


      std::ofstream myfile;
      ros::Time t;
      t=ros::Time::now();
      //std::cout<<"\nThe accel value in write_accel function is "<<custom_var.accelx_wrench_ins<<" "<<custom_var.accely_wrench_ins<<" "<<custom_var.accelz_wrench_ins<<std::endl;
      // std::cout<<"\n The position value in write_accel function is "<<custom_var.pos_x<<" "<<custom_var.pos_y<<" "<<custom_var.pos_z<<std::endl;
      myfile.open ("/home/aimlabx/Desktop/accel_two_values.csv",std::ios_base::app);
      if(i>10)
      {
        myfile<<j<<",accel x value,"<<custom_var.accelx_wrench_ins<<",accel y value,"<<custom_var.accely_wrench_ins<<",accel z value,"<<custom_var.accelz_wrench_ins<<",pos_x,"<<custom_var.pos_x<<",pos_y,"<<custom_var.pos_y<<",pos_z,"<<custom_var.pos_z<<",time,"<<t<<"\n";
        j++;
      }
      else i++;
      myfile.close();

}

input_two_accel accel_calc_func()
{

    //Give the rotation matrix values between customtf and insertion link
    float rot_matrix[3][3]={{1,0,0},{0,1,0},{0,0,1}};
    input_two_accel variable_name;
    //Assigning the position x,y,z value using Ax=T*Ay
    //Here Ax=customtf and Ay=insertion_link
    variable_name.pos_x=(variable_name.accelx_wrench - (variable_name.accelx_wrench_ins * rot_matrix[0][0]) - (variable_name.accely_wrench_ins * rot_matrix[0][1]) - (variable_name.accelz_wrench_ins * rot_matrix[0][2]));
    variable_name.pos_y=(variable_name.accely_wrench - (variable_name.accelx_wrench_ins * rot_matrix[1][0]) - (variable_name.accely_wrench_ins * rot_matrix[1][1]) - (variable_name.accelz_wrench_ins * rot_matrix[1][2]));
    variable_name.pos_z=(variable_name.accelz_wrench - (variable_name.accelx_wrench_ins * rot_matrix[2][0]) - (variable_name.accely_wrench_ins * rot_matrix[2][1]) - (variable_name.accelz_wrench_ins * rot_matrix[2][2]));
    std::cout<<"\nThe accel value in accel_calc function is "<<variable_name.accelx_wrench_ins<<" "<<variable_name.accely_wrench_ins<<" "<<variable_name.accelz_wrench_ins<<std::endl;
    std::cout<<"\n The position value accel_calc function is "<<variable_name.pos_x<<" "<<variable_name.pos_y<<" "<<variable_name.pos_z<<std::endl;
    return (variable_name);
}

int main(int argc, char **argv)
{

      ros::init(argc, argv, "listener_accel_two");
      ros::NodeHandle n;
      //Structure variable
      input_two_accel var3;
      //Subscribing to both the customtf and insertion topics
      ros::Subscriber sub1 = n.subscribe("/wrench_visualization/wrench_insertion", 1, call_wrench_ins_func);
      ros::Subscriber sub2 = n.subscribe("/wrench_visualization/wrench", 1, call_wrench_func);
      ros::Rate rate(1000);
      //Write into the file
      while(ros::ok())
      {
            var3=accel_calc_func();
            write_accel_val(var3);
            ros::spinOnce();
            rate.sleep();
      }
      return 0;
}
