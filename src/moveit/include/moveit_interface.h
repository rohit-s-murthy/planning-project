#ifndef MOVEIT_INTERFACE_H
#define MOVEIT_INTERFACE_H

#include <moveit/move_group_interface/move_group.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>

#include <moveit_msgs/DisplayRobotState.h>
#include <moveit_msgs/DisplayTrajectory.h>

#include <moveit_msgs/AttachedCollisionObject.h>
#include <moveit_msgs/CollisionObject.h>

#include <moveit/robot_model_loader/robot_model_loader.h>
#include <moveit/planning_scene/planning_scene.h>

#include <vector>
#include <iostream>
#include <geometry_msgs/Quaternion.h>
#include <ros/ros.h>
#include <string>
#include <tf/transform_datatypes.h>

class Moveit_Interface{
	
	// ros::Publisher pub;
	// ros::Subscriber sub;
	// ros::nodeHandle n;

	// Moveit_Interface()
	// {
		
	// }
public:
	void setStartState(const std::vector<double>* start, moveit::planning_interface::MoveGroup move_group, const robot_state::JointModelGroup *joint_model_group);

	void setGoalState(std::vector<double>* goal, moveit::planning_interface::MoveGroup move_group, const robot_state::JointModelGroup *joint_model_group);

	void parseMotionPrims();

	void getSuccessors();

	void applyPrim();

	bool isValidSuccessor(std::vector<double> child, const robot_state::JointModelGroup *joint_model_group);

	std::vector<double> getCurrentState(moveit::planning_interface::MoveGroup move_group, const robot_state::JointModelGroup *joint_model_group);

};

#endif
