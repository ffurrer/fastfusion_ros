/*
 * fastfusion_node.hpp
 *
 *  Created on: Sep 27, 2015
 *      Author: karrer
 */

#ifndef INCLUDE_FASTFUSION_NODE_HPP_
#define INCLUDE_FASTFUSION_NODE_HPP_

#include <string>
#include <sstream>
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include "camerautils/camerautils.hpp"
#include "online_fusion_ros.hpp"
#include <tf_conversions/tf_eigen.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include "fastfusion_node/online_fusion_ros.hpp"

#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <pcl/io/ply_io.h>
#include <pcl/features/integral_image_normal.h>
#include <vector>

class FastFusionWrapper {
public:
	FastFusionWrapper(void);
	~FastFusionWrapper(){};
	//-- Main Function
	void run(void);
protected:
	//-- Image Message Callback
	void imageCallback(const sensor_msgs::ImageConstPtr& msg_cam0, const sensor_msgs::ImageConstPtr& msg_cam1);
	void imageCallbackPico(const sensor_msgs::ImageConstPtr& msgDepth, const sensor_msgs::ImageConstPtr& msgConf,
			const sensor_msgs::ImageConstPtr& msgNoise);
	void imageCallbackPico(const sensor_msgs::ImageConstPtr& msgDepth, const sensor_msgs::ImageConstPtr& msgConf);
	void pclCallbackPico(const sensor_msgs::PointCloud2ConstPtr& msgPtCloud, const sensor_msgs::ImageConstPtr& msgNoise);
	void pclCallback(sensor_msgs::PointCloud2 pcl_msg);
	void getRGBImageFromRosMsg(const sensor_msgs::ImageConstPtr& msgRGB, cv::Mat *rgbImg);
	void getConfImageFromRosMsg(const sensor_msgs::ImageConstPtr& msgConf, cv::Mat *confImg);
	void getDepthImageFromRosMsg(const sensor_msgs::ImageConstPtr& msgDepth, cv::Mat *dephtImg);
	void getNoiseImageFromRosMsg(const sensor_msgs::ImageConstPtr& msgNoise, cv::Mat *noiseImg);
	void registerPointCloudCallback(const sensor_msgs::PointCloud2 pcl_msg);
	void depthImageCorrection(cv::Mat & imgDepth, cv::Mat * imgDepthCorrected);

	//-- Pose Message to eigen (Rotation Matrix + Translation)
	CameraInfo convertTFtoCameraInfo(const tf::Transform& transform);

	//-- ROS node handle
	ros::NodeHandle node_, nodeLocal_;
	ros::Time previous_ts_;
	cv::Mat intrinsic_, intrinsicRGB_;
	cv::Mat distCoeff_, distCoeffRGB_;
	cv::Mat depthCorrection_;
	double imageScale_;

	//-- Interface to fastfusion framework
	OnlineFusionROS onlinefusion_;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DEBUGGING
	int frameCounter_;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//-- Subscribers
	message_filters::Subscriber<sensor_msgs::Image> *subscriberRGB_;
	message_filters::Subscriber<sensor_msgs::Image> *subscriberDepth_;
	message_filters::Subscriber<sensor_msgs::Image> *subscriberNoise_;
	message_filters::Subscriber<sensor_msgs::Image> *subscriberConfidence_;
	message_filters::Subscriber<sensor_msgs::PointCloud2> *subscriberPointCloud_;

	//-- Message Synchronizer
	message_filters::TimeSynchronizer<sensor_msgs::PointCloud2, sensor_msgs::Image> *syncPointCloud_;
	message_filters::Synchronizer<message_filters::sync_policies::ApproximateTime<sensor_msgs::Image, sensor_msgs::Image,
			sensor_msgs::Image> > *syncNoise_;
	message_filters::Synchronizer<message_filters::sync_policies::ApproximateTime<sensor_msgs::Image,sensor_msgs::Image> > *sync_;

	//-- Transformations
	tf::TransformListener tfListener;
	void broadcastTFchain(ros::Time timestamp);
	tf::TransformBroadcaster tfBroadcaster_;
	tf::Transform tf_cam0_imu;
	tf::Transform tf_depth_cam0;				// When using ToF camera
	tf::Transform tf_rgb_cam0;				// When using Realsense
	tf::Transform tf_body_cam;				// Transfrom VICON-Body to imu
	Eigen::Matrix3d R_cam0_imu, R_depth_cam0, R_rgb_cam0, R_body_cam;
	Eigen::Vector3d t_cam0_imu, t_depth_cam0, t_rgb_cam0, t_body_cam;
	std::string world_id_;
	std::string cam_id_;

	bool use_pmd_, depth_noise_;
	bool testing_point_cloud_;
	pcl::PointCloud<pcl::PointXYZRGB> pointCloudFrame_;
	pcl::PointCloud<pcl::PointXYZRGB> pointCloudFrameTrans_;
};




#endif /* INCLUDE_FASTFUSION_NODE_HPP_ */
