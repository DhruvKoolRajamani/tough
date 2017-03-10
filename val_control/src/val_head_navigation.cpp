#include <val_control/val_head_navigation.h>

const double degToRad = M_PI / 180;

HeadTrajectory::HeadTrajectory(ros::NodeHandle nh):nh_(nh), NUM_NECK_JOINTS(3)
{
  neckTrajPublisher =
          nh_.advertise<ihmc_msgs::NeckTrajectoryRosMessage>("/ihmc_ros/valkyrie/control/neck_trajectory",1,true);
    headTrajPublisher =
            nh_.advertise<ihmc_msgs::HeadTrajectoryRosMessage>("/ihmc_ros/valkyrie/control/head_trajectory",1,true);
}

HeadTrajectory::~HeadTrajectory()
{
}

void HeadTrajectory::appendNeckTrajectoryPoint(ihmc_msgs::NeckTrajectoryRosMessage &msg, float time, std::vector<float> pos)
{
  for (int i = 0; i < NUM_NECK_JOINTS; i++)
  {
      ihmc_msgs::TrajectoryPoint1DRosMessage p;
      ihmc_msgs::OneDoFJointTrajectoryRosMessage t;
      t.trajectory_points.clear();

      p.time = time;
      p.position = pos[i];
      p.velocity = 0;
      p.unique_id = HeadTrajectory::head_id;
      t.trajectory_points.push_back(p);
      t.unique_id = HeadTrajectory::head_id;
      msg.joint_trajectory_messages.push_back(t);
  }
}


void HeadTrajectory::moveHead(float roll, float pitch, float yaw, const float time)
{
  ihmc_msgs::HeadTrajectoryRosMessage msg;
  ihmc_msgs::SO3TrajectoryPointRosMessage data;

  roll = degToRad * roll;
  pitch = degToRad * pitch;
  yaw = degToRad * yaw;

  data.time = time;
  tf::Quaternion q;
  q.setRPY(roll, pitch, yaw);
  tf::quaternionTFToMsg(q, data.orientation);
  geometry_msgs::Vector3 v;
  v.x = 0.0;
  v.y = 0.0;
  v.z = 0.0;
  data.angular_velocity = v;

  HeadTrajectory::head_id--;
  msg.unique_id = HeadTrajectory::head_id;
  msg.execution_mode = msg.OVERRIDE;

  msg.taskspace_trajectory_points.clear();

  msg.taskspace_trajectory_points.push_back(data);

  // publish the message
  headTrajPublisher.publish(msg);
}

void HeadTrajectory::moveHead(const geometry_msgs::Quaternion &quaternion, const float time)
{
  ihmc_msgs::HeadTrajectoryRosMessage msg;
  ihmc_msgs::SO3TrajectoryPointRosMessage data;

  data.orientation = quaternion;

  geometry_msgs::Vector3 v;
  v.x = 0.0;
  v.y = 0.0;
  v.z = 0.0;
  data.angular_velocity = v;

  HeadTrajectory::head_id--;
  msg.unique_id = HeadTrajectory::head_id;
  msg.execution_mode = msg.OVERRIDE;

  msg.taskspace_trajectory_points.clear();

  msg.taskspace_trajectory_points.push_back(data);

  // publish the message
  headTrajPublisher.publish(msg);
}


void HeadTrajectory::moveHead(const std::vector<std::vector<float> > &trajectory_points, const float time)
{
  ihmc_msgs::HeadTrajectoryRosMessage msg;

  HeadTrajectory::head_id--;
  msg.unique_id = HeadTrajectory::head_id;
  msg.execution_mode = msg.OVERRIDE;

  msg.taskspace_trajectory_points.clear();

  for(int i = 0; i < trajectory_points.size(); i++)
  {
    ihmc_msgs::SO3TrajectoryPointRosMessage data;

    float roll = degToRad * trajectory_points[i][0];
    float pitch = degToRad * trajectory_points[i][1];
    float yaw = degToRad * trajectory_points[i][2];

    data.time = time;
    tf::Quaternion q;
    q.setRPY(roll, pitch, yaw);
    tf::quaternionTFToMsg(q, data.orientation);
    geometry_msgs::Vector3 v;
    v.x = 0.0;
    v.y = 0.0;
    v.z = 0.0;
    data.angular_velocity = v;

    msg.taskspace_trajectory_points.push_back(data);
  }
  // publish the message
  headTrajPublisher.publish(msg);
}

void HeadTrajectory::moveNeckJoints(const std::vector<std::vector<float> > &neck_pose, const float time)
{
  ihmc_msgs::NeckTrajectoryRosMessage msg;

  HeadTrajectory::head_id--;
  msg.unique_id = HeadTrajectory::head_id;

  // Add all neck trajectory points to the trajectory message
  for(int i = 0; i < neck_pose.size(); i++){
    if(neck_pose[i].size() != NUM_NECK_JOINTS)
      ROS_ERROR("Check number of trajectory points");
    appendNeckTrajectoryPoint(msg, time / neck_pose.size(), neck_pose[i]);
  }
  // publish the message
  neckTrajPublisher.publish(msg);
}

int HeadTrajectory::getNumNeckJoints() const
{
    return NUM_NECK_JOINTS;
}
