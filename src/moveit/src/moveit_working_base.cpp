#include "moveit_interface.h"

void Moveit_Interface::setStartState(const std::vector<double>* start,  moveit::planning_interface::MoveGroup move_group, const robot_state::JointModelGroup *joint_model_group)
{
	//	If a different start state should be considered instead of the current state of the robot, this function sets that state. 
	robot_state::RobotState start_state(*move_group.getCurrentState());	//just to initialize

	start_state.setJointGroupPositions(joint_model_group, *start);

	move_group.setStartState(start_state); ROS_INFO("49\n");

	return;
}

void Moveit_Interface::setGoalState(std::vector<double>* goal, moveit::planning_interface::MoveGroup move_group, const robot_state::JointModelGroup *joint_model_group)
{	ROS_INFO("Inside\n");
	//TODO: Add goal tolerance
	std::vector<double>::iterator it = goal->begin();
	ROS_INFO("%f\n", *it);
	geometry_msgs::Pose goal_pose;
	goal_pose.position.x = *it; ++it;
	goal_pose.position.y = *it; ++it;
	goal_pose.position.z = *it; ++it;
	tf::Quaternion q; 
	q.setRPY(*it, *(it+1), *(it+2));
	goal_pose.orientation.w = q.getW();
	goal_pose.orientation.x = q.getX();
	goal_pose.orientation.y = q.getY();
	goal_pose.orientation.z = q.getZ(); ROS_INFO("66\n");
	move_group.setPoseTarget(goal_pose); ROS_INFO("67\n");

	//
	return;

}

void Moveit_Interface::parseMotionPrims()
{
	//Parse the two different heuristics and store them in a 2D array
	//the primitive file would be a rosparam and can be called using getParam()
}

void Moveit_Interface::getSuccessors()
{

}

void Moveit_Interface::applyPrim()
{

}

bool Moveit_Interface::isValidSuccessor(std::vector<double> child, const robot_state::JointModelGroup *joint_model_group)
{
  	robot_model_loader::RobotModelLoader robot_model_loader("robot_description");
	robot_model::RobotModelPtr kinematic_model = robot_model_loader.getModel();
	planning_scene::PlanningScene planning_scene(kinematic_model);

	collision_detection::CollisionRequest collision_request;
	collision_detection::CollisionResult collision_result;

	collision_detection::AllowedCollisionMatrix acm = planning_scene.getAllowedCollisionMatrix();
	
	robot_state::RobotState copied_state = planning_scene.getCurrentState();
	copied_state.setJointGroupPositions(joint_model_group, child);

	collision_result.clear();
	planning_scene.checkCollision(collision_request, collision_result, copied_state, acm);

	return collision_result.collision;
}

std::vector<double> Moveit_Interface::getCurrentState(moveit::planning_interface::MoveGroup move_group, const robot_state::JointModelGroup *joint_model_group)
{
	moveit::core::RobotStatePtr current_state = move_group.getCurrentState();
  
  // Next get the current set of joint values for the group.
  	std::vector<double> joint_group_positions;
  	current_state->copyJointGroupPositions(joint_model_group, joint_group_positions);

  	return joint_group_positions;
}



int main(int argc, char **argv)
{ ROS_INFO("Hi\n");
  ros::init(argc, argv, "moveit_interface");
  ros::NodeHandle nh;
  ros::AsyncSpinner spinner(1);
  spinner.start();
  ROS_INFO("84\n");
  //To let RViz start up
  sleep(10.0);
  ROS_INFO("87\n");

  ros::Publisher display_publisher = nh.advertise<moveit_msgs::DisplayTrajectory>("/move_group/display_planned_path", 1, true);
  moveit_msgs::DisplayTrajectory display_trajectory;
  // static const std::string PLANNING_GROUP = "whole_body";
  
  // moveit::planning_interface::MoveGroup move_group(PLANNING_GROUP);

  	static const std::string PLANNING_GROUP = "whole_body";

    moveit::planning_interface::MoveGroup move_group(PLANNING_GROUP);

    moveit::planning_interface::PlanningSceneInterface planning_scene_interface;

    const robot_state::JointModelGroup *joint_model_group =
    move_group.getCurrentState()->getJointModelGroup(PLANNING_GROUP);

  // Raw pointers are frequently used to refer to the planning group for improved performance.
 //  	geometry_msgs::Pose target_pose1;
	// target_pose1.orientation.w = 1.0;
	// target_pose1.position.x = 0.28;
	// target_pose1.position.y = -0.7;
	// target_pose1.position.z = 1.0;
	// move_group.setPoseTarget(target_pose1);

	// Now, we call the planner to compute the plan
	// and visualize it.
	// Note that we are just planning, not asking move_group 
	// to actually move the robot.
	moveit::planning_interface::MoveGroup::Plan my_plan;
	// bool success = move_group.plan(my_plan);
	// if(!success)
	// {
	// 	throw 0;
	// }
    //JOINT SPACE GOAL
    std::vector<double> group_variable_values;
	move_group.getCurrentState()->copyJointGroupPositions(move_group.getCurrentState()->getRobotModel()->getJointModelGroup(move_group.getName()), group_variable_values);

	// Now, let's modify one of the joints, plan to the new joint
	// space goal and visualize the plan.
	group_variable_values[0] = -1.0;
	group_variable_values[3] = -1.0;  

	move_group.setJointValueTarget(group_variable_values);
	bool success = move_group.plan(my_plan);
	ROS_INFO("Visualizing plan 2 (joint space goal) %s",success?"":"FAILED");
  /* Sleep to give Rviz time to visualize the plan. */
	sleep(5.0);
	

  	//ADDING TABLE TO ENVIRONMENT
    moveit_msgs::CollisionObject collision_object;
  	collision_object.header.frame_id = move_group.getPlanningFrame();

  /* The id of the object is used to identify it. */
  	collision_object.id = "box1";

  /* Define a box to add to the world. */
	shape_msgs::SolidPrimitive primitive;
	primitive.type = primitive.BOX;
	primitive.dimensions.resize(3);
	primitive.dimensions[0] = 2;
	primitive.dimensions[1] = 4;
	primitive.dimensions[2] = 0.4;

	/* A pose for the box (specified relative to frame_id) */
	geometry_msgs::Pose box_pose;
	box_pose.orientation.w = 1.0;
	box_pose.position.x =  3;
	box_pose.position.y =  0;
	box_pose.position.z =  1;

	collision_object.primitives.push_back(primitive);
	collision_object.primitive_poses.push_back(box_pose);
	collision_object.operation = collision_object.ADD;

	std::vector<moveit_msgs::CollisionObject> collision_objects;  
	collision_objects.push_back(collision_object);  

	// Now, let's add the collision object into the world
	ROS_INFO("Add an object into the world");  
	planning_scene_interface.addCollisionObjects(collision_objects);
  
  /* Sleep so we have time to see the object in RViz */
  	sleep(20.0);
  	//////////////////////////
  	// std::vector<std::string> object_ids;
  	// object_ids.push_back(collision_object.id);  
  	// planning_scene_interface.removeCollisionObjects(object_ids);

  // 	Moveit_Interface interface;
  // 	ROS_INFO("98\n");
  // 	std::vector<double> goal_state = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  // 	const std::vector<double> start_state = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  // interface.setGoalState(&goal_state, move_group, joint_model_group);
  // //Testing
  // moveit::planning_interface::MoveGroup::Plan my_plan;
  // bool success = move_group.plan(my_plan);

  // ROS_INFO_NAMED("tutorial", "Visualizing plan 2 (joint space goal) %s", success ? "" : "FAILED");
  // sleep(5.0);
  // interface.setStartState(&start_state, move_group, joint_model_group);
  // //Testing
  // success = move_group.plan(my_plan);

  // ROS_INFO_NAMED("tutorial", "Visualizing plan 2 (joint space goal) %s", success ? "" : "FAILED");
  // sleep(5.0);

  ROS_INFO("101\n");
  ros::shutdown();
  ROS_INFO("103\n");

  return 0;
}

